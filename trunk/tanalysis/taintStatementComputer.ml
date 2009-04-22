open Cil_types
open TaintTyping
open TaintGamma

(* TODO *)
let do_instr env instr is_cond_tainted =
    env
    
(* TODO *)
let do_return_instr env null_expr is_cond_tainted =
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
