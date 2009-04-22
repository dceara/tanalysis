open Cil_types
open TaintTyping
open TaintGamma

(* Returns the taintedness for a lvalue. *)
(* For now we make the assumption that memory locations aren't used as lvalues. *)
let get_lvalue_taint env lvalue =
    match lvalue with
        | ((Var vinfo), _) -> Gamma.get_taint env vinfo.vid
        | _ -> U

(* Returns the taintedness of an expression according to the environment. *)
let rec do_expr env expr =
    match expr with
        | Const _ -> U
        | Lval lvalue -> get_lvalue_taint env lvalue
        | SizeOf _ -> U
        | SizeOfE s_expr -> do_expr env s_expr
        | SizeOfStr _ -> U
        | AlignOf _ -> U
        | AlignOfE _ -> U
        | UnOp (_, un_expr, _) -> do_expr env un_expr
        | BinOp (_, bin_expr1, bin_expr2, _) 
            -> Typing.combine_taint (do_expr env bin_expr1) (do_expr env bin_expr2)
        (* TODO: For now we consider all casts to be taints. Probably this isn't a *)
        (* good approach. *)
        | CastE _ -> T
        | AddrOf lvalue -> get_lvalue_taint env lvalue
        | StartOf lvalue -> get_lvalue_taint env lvalue
        | Info _ -> U

let do_null_expr env null_expr =
    match null_expr with
        | None -> U
        | Some expr -> do_expr env expr 

(* Changes the mapping for lvalue in the given environment according to cond_taint*)
(* and the assigned expression. *)
let do_assign env lvalue expr cond_taint =
    match (lvalue, cond_taint) with
        | ((Var vinfo, _), T) 
            -> 
                Gamma.set_taint env vinfo.vid T;
                env 
        | ((Var vinfo, _), _) 
            -> 
                let expr_taint = do_expr env expr in
                Gamma.set_taint env vinfo.vid (Typing.combine_taint expr_taint cond_taint);
                env
        | _ ->  env
        
(* Make the assumption that all the functions return a single value and have no*)
(* side effects. *)
let do_call env null_lval param_exprs cond_taint func func_envs =
    (* Local function used for finding the actual parameter passed for a *)
    (* formal one. *)
    let find_binding actuals formals dep =
        let i = List.fold_left
                    (fun idx f ->
                        if dep = f then idx
                        else idx + 1)
                    0
                    formals in
        List.nth actuals i
    in
    (* Local function used for instantiating all the formal parameter taints *)
    (* according to actual parameter taints. *)
    let instantiate_call env ret_taint actuals formals =
        List.fold_left 
            (fun t dep -> 
                let param_expr = find_binding actuals formals dep in
                let param_taint = do_expr env param_expr in
                Typing.combine_taint t param_taint)
            U
            ret_taint
    in
    match null_lval with
        | Some (Var vinfo, _)
            -> 
                let callee_env = Inthash.find func_envs func.svar.vid in
                let formals = func.sformals in
                (match Gamma.get_taint callee_env func.svar.vid with
                    | U -> Gamma.set_taint env vinfo.vid U
                    | T -> Gamma.set_taint env vinfo.vid T
                    | (G g) 
                        ->
                            Gamma.set_taint 
                                env 
                                vinfo.vid
                                (instantiate_call env g param_exprs formals));
                env  
        | _ -> env         

(* Changes the environment according to the instruction *)
let do_instr env instr cond_taint func func_envs =
    match instr with
        | (Set (lvalue, expr, _))
            -> do_assign env lvalue expr cond_taint
        | (Call (null_lval, cast_expr, param_exprs, _))
            (* Make the assumption that all the functions return a single value. *)
            -> do_call env null_lval param_exprs cond_taint func func_envs
        | _ -> env
    
(* Changes the return value in the environment according to the return expression *)
let do_return_instr env func null_expr cond_taint =
    Gamma.set_taint 
        env func.svar.vid 
        (Typing.combine_taint cond_taint (do_null_expr env null_expr));
    env
    
(* Only return the taintedness of the expression. The successors will be computed*)
(* later. *)
let do_if_instr env expr true_block false_block cond_taint =
    let expr_taint = do_expr env expr in 
    Typing.combine_taint expr_taint cond_taint
    
(* Only return the taintedness of the expression. The successors will be computed*)
(* later. *)
let do_switch_instr env expr cond_taint =
    let expr_taint = do_expr env expr in 
    Typing.combine_taint expr_taint cond_taint
    
(* TODO *)
let do_loop_instr env stmt_block stmt_continue stmt_break cond_taint =
    (env, cond_taint)
