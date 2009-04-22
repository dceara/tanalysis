open Cil_types
open TaintTyping
open TaintGamma

let translate_cond_taint is_cond_tainted = 
    match is_cond_tainted with
        | true -> T
        | false -> U

(* TODO *)
let do_assign env lvalue expr is_cond_tainted =
    env

(* Make the assumption that all the functions return a single value*)
(* and aren't called from inside an expression *)
let do_call env null_lval param_exprs is_cond_tainted =
    let cond_taint = translate_cond_taint is_cond_tainted in
    match null_lval with
        | Some (Var vinfo, _)
            -> (* TODO *)
            env
        | _ -> env         

(* TODO *)
let do_expr env expr =
    U

let do_null_expr env null_expr =
    match null_expr with
        | None -> U
        | Some expr -> do_expr env expr 

(* Changes the environment according to the instruction *)
let do_instr env instr is_cond_tainted =
    match instr with
        | (Set (lvalue, expr, _))
            -> do_assign env lvalue expr is_cond_tainted
        | (Call (null_lval, cast_expr, param_exprs, _))
            (* Make the assumption that all the functions return a single value*)
            (* and aren't called from inside an expression *)
            -> do_call env null_lval param_exprs is_cond_tainted
        | _ -> env
    
(* Changes the return value in the environment according to the return expression *)
let do_return_instr env func null_expr is_cond_tainted =
    let cond_taint = translate_cond_taint is_cond_tainted in
    Gamma.set_taint 
        env func.svar.vid 
        (Typing.combine_taint cond_taint (do_null_expr env null_expr));
    env
    
(* TODO *)
let do_if_instr env expr true_block false_block is_cond_tainted =
    (env, is_cond_tainted)
    
(* TODO *)
let do_switch_instr env expr is_cond_tainted =
    (env, is_cond_tainted)
    
(* TODO *)
let do_loop_instr env stmt_block stmt_continue stmt_break is_cond_tainted =
    (env, is_cond_tainted)
