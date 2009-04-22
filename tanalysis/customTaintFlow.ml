open Cil_types
open TaintTyping
open TaintGamma

module TaintComputer(Param:sig
                        (* The int key hash table that holds the environment *)
                        (* for each statement in the function. *)
                        val stmt_envs : statementsEnvironment 
                        val func : fundec         
                        val func_envs : functionEnvironment              
	                 end) = struct


    (* Tests if the old environment and the new environment are the same. *)
    let test_for_change old_ (new_, cond_taint) =
        match Gamma.compare old_ new_ with
            | true -> (true, old_, cond_taint)
            | false -> (false, new_, cond_taint)

    (* Applies the transformations done by a statement to a given environment. *)
    (* Params: *)
    (* stmt - the statement being analyzed *)
    (* new_env - the environment before the statement is analyzed *)
    (* cond_taint - the current condition taintedness stack *)
    (* Returns the new environment. *)
    let rec do_stmt stmt new_env cond_taint =
        let module SC = TaintStatementComputer in
        let current_cond_taint = List.hd cond_taint in
        match stmt.skind with
            | (Instr instr) 
                -> 
                    let ret_env = 
                        SC.do_instr new_env instr current_cond_taint Param.func Param.func_envs in
                    (ret_env, current_cond_taint)
            | (Return (null_expr, _))
                -> 
                    let ret_env = 
                        SC.do_return_instr new_env Param.func null_expr current_cond_taint in
                    (ret_env, current_cond_taint)
            | (If (expr, true_block, false_block, _))
                -> 
                    let new_cond_taint = 
                        SC.do_if_instr new_env expr true_block false_block current_cond_taint in
                    (new_env, new_cond_taint)
            | (Switch (expr, _, _, _))
                -> 
                    let new_cond_taint = 
                        SC.do_switch_instr new_env expr current_cond_taint in
                    (new_env, new_cond_taint)
            | (Loop (_, stmt_block, _, stmt_continue, stmt_break))
                -> 
                    let (ret_env, new_cond_taint) = 
                        SC.do_loop_instr new_env stmt_block stmt_continue stmt_break current_cond_taint in
                    (ret_env, new_cond_taint)
            | (Block stmt_block)
                -> 
                    let ret_env = List.fold_left
                                    (fun env s 
                                        -> 
                                            let (new_env, _) = do_stmt s env cond_taint in 
                                            new_env)
                                    new_env
                                    stmt_block.bstmts in
                    (ret_env, current_cond_taint) 
            | _ -> 
                    (new_env, current_cond_taint)
     
    (* Params: *)
    (* worklist - the list of pairs (statement, is_cond_tainted) that will be *)
    (* computed. cond_taint - the taintedness of the condition. Must be combined *)
    (* with all the assignments so that implicit data flows are covered. *)
    let rec compute worklist =
        match List.length worklist with
        (* Stop when the worklist is empty. *)
        | 0 -> ignore ()
        | _ ->
        let (current_stmt, cond_taint) = List.hd worklist in
        (* For each predecessor, combine the results. If there aren't any preds *)
        (* then the statements' environment is returned. *)
        let new_env = 
            match List.length current_stmt.preds with
                | 0 ->
                    Inthash.find Param.stmt_envs current_stmt.sid
                | _ 
                    ->
                    let first_pred = List.hd current_stmt.preds in
                    let first_pred_id = first_pred.sid in 
		            List.fold_left
		                (fun env pred_stmt ->
		                    let pred_env = Inthash.find Param.stmt_envs pred_stmt.sid in
		                    Typing.combine env pred_env)
                        (Inthash.find Param.stmt_envs first_pred_id)    
		                current_stmt.preds
        in
        let old_env = Hashtbl.copy (Inthash.find Param.stmt_envs current_stmt.sid) in
        let (changed, env, new_cond_taint) = 
            test_for_change old_env (do_stmt current_stmt new_env cond_taint) in
        match (changed, env) with
            | (false, _) 
                -> 
                    (* Fix point reached for current statement. The successors *)
                    (* aren't added to the worklist. *)
                    Inthash.replace Param.stmt_envs current_stmt.sid env;
                    compute 
                        (List.tl worklist) 
            | (true, _)
                -> 
                    (* We still haven't reached a fixed point for the statement. *)
                    (* Add the successors to the worklist. *)
                    Inthash.replace Param.stmt_envs current_stmt.sid env;
                    compute 
                        (List.concat 
                            [List.tl worklist;
                             List.map 
                                (fun s -> (s, new_cond_taint::cond_taint)) 
                                current_stmt.succs])

    (* This is the main entry point of the analysis. *)
    (* Params: *)
    (* worklist - the list of statements that will be computed. Initially this *)
    (* must hold only the starting statement *)
    let start worklist = 
        compute (List.map (fun s -> (s, [U])) worklist)   
end
