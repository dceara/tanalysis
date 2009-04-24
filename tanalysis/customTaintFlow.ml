open Cil_types
open TaintTyping
open TaintGamma

module TaintComputer(Param:sig
                        (* The int key hash table that holds the environment *)
                        (* for each statement in the function. *)
                        val stmt_envs : statementsEnvironment 
                        val func : fundec         
                        val func_envs : functionEnvironment  
                        val fmt : Format.formatter
                        val debug : bool      
                        val info : bool     
                     end) = struct


    module SC = TaintInstructionComputer.InstrComputer(struct
                                                            let fmt = Param.fmt
                                                            let debug = Param.debug
                                                            let info = Param.info
                                                        end)

    (* This function should be removed. It's used only for debugging purposes. *)
    let print_envs () =
        SC.print () "\n\n%s\n" "[DEBUG] Printing all the envs:";
        Inthash.iter
            (fun key env ->
                SC.print () "Environmnet for sid: %d\n" key;
                SC.print_env () env;
                SC.print () "%s" "\n")
            Param.stmt_envs;
        SC.print () "%s" "\n\n"

    (* Tests if the old environment and the new environment are the same. *)
    let test_for_change old_ (new_, cond_taint) =
        match Gamma.compare old_ new_ with
            | false -> (true, new_, cond_taint)
            | true 
                -> 
                    match new_ with
                        | (true, _) -> (false, old_, cond_taint)
                        | (false, _) -> (true, new_, cond_taint)
    
    (* Applies the transformations done by a statement to a given environment. *)
    (* Params: *)
    (* stmt - the statement being analyzed *)
    (* new_env - the environment before the statement is analyzed *)
    (* cond_taint - the current condition taintedness stack *)
    (* Returns the new environment. *)
    let rec do_stmt stmt new_env cond_taint =
        let current_cond_taint = List.hd cond_taint in
        match stmt.skind with
            | (Instr instr) 
                -> 
                    let ret_env = 
                        SC.do_instr new_env instr current_cond_taint Param.func Param.func_envs in
                    if (Param.debug) then
                    (
                        SC.print () "[DEBUG] Instruction reached %s" "\n";
                        SC.print_env () ret_env
                    );
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
        (if (Param.info) then
            SC.print () "[INFO] Processing instruction %d from worklist\n" current_stmt.sid);
        (* For each predecessor, combine the results. If there aren't any preds *)
        (* then the statements' environment is returned. *)
        let (new_env, cond_taint) = 
            match List.length current_stmt.preds with
                | 0 ->
                    (Inthash.find Param.stmt_envs current_stmt.sid, cond_taint)
                | _ 
                    ->
                    let first_pred = List.hd current_stmt.preds in
                    let first_pred_id = first_pred.sid in  
                    (Gamma.copy
                        (List.fold_left
	                        (fun env pred_stmt ->
	                            let pred_env = Inthash.find Param.stmt_envs pred_stmt.sid in
	                            Typing.combine env pred_env)
	                        (Inthash.find Param.stmt_envs first_pred_id)    
	                        current_stmt.preds),
                    List.tl cond_taint)
        in
        let old_env = Gamma.copy (Inthash.find Param.stmt_envs current_stmt.sid) in
        let (changed, env, new_cond_taint) = 
            test_for_change old_env (do_stmt current_stmt new_env cond_taint) in
        match (changed, env) with
            | (false, _) 
                -> 
                    (* Fixed point reached for current statement. The successors *)
                    (* aren't added to the worklist. *)
                    (if (Param.debug) then
                        Printf.printf "[DEBUG] Fixed point reached for sid %d\n" current_stmt.sid);
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

    let create_initial_env () = 
        let initial_env = Gamma.create_env () in
        (List.iter
            (fun formal ->
                ignore (Typing.process_formal initial_env formal))
            Param.func.sformals);  
        (List.iter 
            (fun local ->
                ignore (Typing.process_local initial_env local))
            Param.func.slocals);
        initial_env
        
    (* Initialized the locals and formals in all the environments associated *)
    (* to the statements *)
    let init_environments initial_env =
        List.iter
            (fun stmt ->
                Inthash.add Param.stmt_envs stmt.sid (Gamma.copy initial_env))
            Param.func.sallstmts            

    (* Combines all the environments associated to return statements *)
    let combine_return_envs initial_env =
        let is_return stmt = 
            match stmt.skind with 
                | Return _ -> true 
                | _ -> false
        in    
        let stmts = Param.func.sallstmts in
        let ret_stmts = List.filter (fun stmt -> is_return stmt) stmts in
        match List.length ret_stmts with
            | 0 
                -> 
                    SC.print () "%s" "[WARNING] Function without return found!\n"; 
                    initial_env
            | _
                -> 
                    let first_ret_stmt = List.hd ret_stmts in
                    let first_env = Inthash.find Param.stmt_envs first_ret_stmt.sid in
                    List.fold_left 
                        (fun env stmt ->
                            Typing.combine env (Inthash.find Param.stmt_envs stmt.sid))
                    first_env
                    ret_stmts         

    (* This is the main entry point of the analysis. *)
    (* Params: *)
    (* worklist - the list of statements that will be computed. Initially this *)
    (* must hold only the starting statement *)
    let start worklist = 
        let initial_env = create_initial_env () in
        init_environments initial_env;
        compute (List.map (fun s -> (s, [U])) worklist);
        let final = combine_return_envs initial_env in
        SC.print_env () final
end

let run_custom_taint format f f_envs =
    let module TaintComputer = TaintComputer(struct
                                                let stmt_envs = (Inthash.create 1024)
                                                let func = f
                                                let func_envs = f_envs
                                                let fmt = format
                                                let debug = true
                                                let info = true
                                            end) in
    let start_stmt = List.hd f.sallstmts in
    TaintComputer.start [start_stmt]
