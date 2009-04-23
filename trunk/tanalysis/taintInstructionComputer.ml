open Cil_types
open Cil
open TaintTyping
open TaintGamma


module InstrComputer(Param:sig
                        val fmt : Format.formatter
                        val debug : bool      
                        val info : bool     
                     end) = struct
    let print () = Format.fprintf Param.fmt
    
    let print_taint () = Gamma.pretty_print_taint Param.fmt
    
    let print_env () = Gamma.pretty_print Param.fmt
    
    (* Returns the taintedness for a lvalue. *)
    (* TODO: For now we make the assumption that memory locations aren't used as lvalues. *)
    let get_lvalue_taint env lvalue =
        (if Param.info then
            print () "[INFO] Getting lvalue taint %s" "\n"); 
        match lvalue with
            | ((Var vinfo), _) 
                -> 
                    let taint = Gamma.get_taint env vinfo.vid in
                    if Param.debug then (
                        print () "[DEBUG] Taint for lvalue %s: " vinfo.vname;
                        print_taint () taint);
                    taint 
            | _ -> 
                    if Param.debug then (
                        print () "[DEBUG] Taint for memory lvalue: %s" "\n";
                        print_taint () U);
                    U
    
    (* Returns the taintedness of an expression according to the environment. *)
    let rec do_expr env expr =
        let do_const () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for Constant is: ";
                print_taint () U);
            U
        in
        let do_lvalue lvalue =
            let taint = get_lvalue_taint env lvalue in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for Lvalue is: ";
                print_taint () taint);
            taint
        in
        let do_sizeOf () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for SizeOf is: ";
                print_taint () U);
            U
        in
        let do_sizeOfE expr =
            let taint = do_expr env expr in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for SizeOfE is: ";
                print_taint () taint);
            taint
        in
        let do_sizeOfStr () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for SizeOfStr is: ";
                print_taint () U);
            U
        in
        let do_alignOf () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for AlignOf is: ";
                print_taint () U);
            U
        in
        let do_alignOfE () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for AlignOfE is: ";
                print_taint () U);
            U
        in
        let do_unOp expr =
            let taint = do_expr env expr in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for UnOp is: ";
                print_taint () taint);
            taint
        in
        let do_binOp expr1 expr2 =
            let taint = Typing.combine_taint 
                            (do_expr env expr1) 
                            (do_expr env expr2) in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for BinOp is: ";
                print_taint () taint);
            taint
        in
        (* TODO: For now we consider all casts to be taints. Probably this isn't a *)
        (* good approach. *)
        let do_castE () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for CastE is: ";
                print_taint () T);
            T
        in
        let do_addrOf lvalue =
            let taint = get_lvalue_taint env lvalue in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for AddrOf is: ";
                print_taint () taint);
            taint
        in
        let do_startOf lvalue = 
            let taint = get_lvalue_taint env lvalue in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for StartOf is: ";
                print_taint () taint);
            taint
        in
        let do_info () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for Info is: ";
                print_taint () U);
            U
        in
        
        (if Param.info then
            print () "[INFO] Processing instruction %s" "\n");
        match expr with
            | Const _ -> do_const ()  
            | Lval lvalue -> do_lvalue lvalue                    
            | SizeOf _ -> do_sizeOf ()                    
            | SizeOfE s_expr -> do_sizeOfE s_expr                    
            | SizeOfStr _ -> do_sizeOfStr ()
            | AlignOf _ -> do_alignOf ()
            | AlignOfE _ -> do_alignOfE ()
            | UnOp (_, un_expr, _) -> do_unOp un_expr
            | BinOp (_, bin_expr1, bin_expr2, _)  -> do_binOp bin_expr1 bin_expr2
            | CastE _ -> do_castE ()
            | AddrOf lvalue -> do_addrOf lvalue
            | StartOf lvalue -> do_startOf lvalue
            | Info _ -> do_info ()
    
    let do_null_expr env null_expr =
        let do_null () =
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for Null expr is: ";
                print_taint () U);
            U
        in
        let do_not_null expr =
            let taint = do_expr env expr in
            if Param.debug then (
                print () "%s" "[DEBUG] Taint for NotNull expr is: ";
                print_taint () taint);
            taint
        in
        
        (if Param.info then
            print () "[INFO] Processing nullable expression %s" "\n");
        match null_expr with
            | None -> do_null ()
            | Some expr -> do_not_null expr 
    
    (* Changes the mapping for lvalue in the given environment according to cond_taint*)
    (* and the assigned expression. *)
    let do_assign env lvalue expr cond_taint =
        let do_assign_lvalue_tainted vinfo =
            (if Param.debug then
                print () "[DEBUG] Assigning T to %s\n" vinfo.vname);
            Gamma.set_taint env vinfo.vid T;
            env
        in
        let do_assign_lvalue vinfo expr =
            let expr_taint = do_expr env expr in
            if Param.debug then (
                print () "[DEBUG] Assigning to %s taint:" vinfo.vname;
                print_taint () expr_taint
            );
            Gamma.set_taint env vinfo.vid (Typing.combine_taint expr_taint cond_taint);
            env
        in
        (* TODO: For now we make the assumption that memory locations aren't used as lvalues. *)
        let do_assign_default () =
            if Param.debug then (
                print () "[DEBUG] Assigning a value to a memory location%s" "\n"
            );
            env
        in
        
        (if Param.info then
            print () "[INFO] Processing assign instruction %s" "\n");
        match (lvalue, cond_taint) with
            | ((Var vinfo, _), T) -> do_assign_lvalue_tainted vinfo                    
            | ((Var vinfo, _), _) -> do_assign_lvalue vinfo expr 
            | _ ->  do_assign_default ()
            
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
        let do_call_lval vinfo =
            let callee_env = Inthash.find func_envs func.svar.vid in
            let formals = func.sformals in
            let taint = 
            (match Gamma.get_taint callee_env func.svar.vid with
                | U -> Gamma.set_taint env vinfo.vid U; U
                | T -> Gamma.set_taint env vinfo.vid T; T
                | (G g) 
                    ->
                        let g_taint = (instantiate_call env g param_exprs formals) in 
                        Gamma.set_taint env vinfo.vid g_taint;
                        g_taint) in
            if Param.debug then (
                print () "[DEBUG] Assigning to %s taint value:" vinfo.vname;
                print_taint () taint
            );
            env
        in  
        let do_call_default () = 
            env 
        in
        
        (if Param.info then
            print () "[INFO] Processing call instruction %s" "\n");
        match null_lval with
            | Some (Var vinfo, _) -> do_call_lval vinfo 
            | _ -> do_call_default ()
    
    (* Changes the environment according to the instruction *)
    let do_instr env instr cond_taint func func_envs =
        (if Param.info then
            print () "[INFO] Processing instruction %s" "\n");
        match instr with
            | (Set (lvalue, expr, _))
                -> do_assign env lvalue expr cond_taint
            | (Call (null_lval, cast_expr, param_exprs, _))
                (* Make the assumption that all the functions return a single value. *)
                -> do_call env null_lval param_exprs cond_taint func func_envs
            | _ -> env
        
    (* Changes the return value in the environment according to the return expression *)
    let do_return_instr env func null_expr cond_taint =
        let fname = func.svar.vname in
        let fid = func.svar.vid in
        (if Param.info then
            print () "[INFO] Processing return instruction for function %s\n" fname);
        let old_taint = 
            (try 
                Gamma.get_taint env fid
            with Not_found -> U)
        in
        let new_taint = (Typing.combine_taint 
                            old_taint 
                            (Typing.combine_taint 
                                cond_taint 
                                (do_null_expr env null_expr))) in
        Gamma.set_taint env fid new_taint;
        if Param.debug then (
            print () "[DEBUG] Old taint for return in function %s:" fname;
            print_taint () old_taint;
            print () "[DEBUG] New taint for return in function %s:" fname;
            print_taint () new_taint
        );
        env
        
    (* Only return the taintedness of the expression. The successors will be computed*)
    (* later. *)
    let do_if_instr env expr true_block false_block cond_taint =
        (if Param.info then
            print () "[INFO] Processing if instruction %s" "\n");
        let expr_taint = do_expr env expr in 
        let new_cond_taint = Typing.combine_taint expr_taint cond_taint in
        if Param.debug then (
            print () "%s" "[DEBUG] Condition taint for if instruction:";
            print_taint () cond_taint;
            print () "%s" "[DEBUG] New condition taint for if instruction:";
            print_taint () new_cond_taint;
        );
        new_cond_taint
        
    (* Only return the taintedness of the expression. The successors will be computed*)
    (* later. *)
    let do_switch_instr env expr cond_taint =
        let expr_taint = do_expr env expr in 
        Typing.combine_taint expr_taint cond_taint
        
    (* TODO *)
    let do_loop_instr env stmt_block stmt_continue stmt_break cond_taint =
        (env, cond_taint)
end