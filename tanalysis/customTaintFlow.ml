open Cil_types
open TaintTyping
open TaintGamma

module TaintComputer(Param:sig
                        (* The int key hashtable that holds the environment *)
                        (* for each statement in the function. *)
	                    val stmt_envs : statementsEnvironment                        
	                 end) = struct


    (* Tests if the old environment and the new environment are the same. *)
    let test_for_change old_ (new_, is_cond_tainted) =
        match Gamma.compare old_ new_ with
            | true -> (true, old_, is_cond_tainted)
            | false -> (false, new_, is_cond_tainted)

    (* Applies the transformations done by a statement to a given environment. *)
    (* Params: *)
    (* stmt - the statement being analyzed *)
    (* new_env - the environment before the statement is analyzed *)
    (* is_cond_tainted - taints all the assignments if true *)
    (* Returns the new environment. *)
    (* TODO *)
    let do_stmt stmt new_env is_cond_tainted =
        (new_env, false)
     
    (* Params: *)
    (* worklist - the list of statements that will be computed. *)
    (* is_cond_tainted - if true all the assignments in the block must become *)
    (* tainted so that implicit data flows are covered. *)
    let rec compute worklist is_cond_tainted =
        let current_stmt = List.hd worklist in
        (* Foreach predecessor, combine the results. If there aren't any preds *)
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
        let (changed, env, new_is_cond_tainted) = 
            test_for_change old_env (do_stmt current_stmt new_env is_cond_tainted) in
        match (changed, env) with
            | (false, _) 
                -> 
                    (* Fix point reached for current statement. The successors *)
                    (* aren't added to the worklist. *)
                    Inthash.replace Param.stmt_envs current_stmt.sid env;
                    compute 
                        (List.tl worklist) 
                        new_is_cond_tainted
            | (true, _)
                -> 
                    (* We still haven't reached a fixed point for the statement. *)
                    (* Add the successors to the worklist. *)
                    Inthash.replace Param.stmt_envs current_stmt.sid env;
                    compute 
                        (List.concat [List.tl worklist;current_stmt.succs]) 
                        new_is_cond_tainted

    (* This is the main entry point of the analysis. *)
    (* Params: *)
    (* worklist - the list of statements that will be computed. Initially this *)
    (* must hold only the starting statement *)
    let start worklist = 
        compute worklist false                              
end
