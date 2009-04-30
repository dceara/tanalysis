open Cil_types
open TaintGamma

module TaintComputer(Param:sig
                        (* The int key hash table that holds the environment *)
                        (* for each statement in the function. *)
                        val stmt_envs : statementsEnvironment 
                        val func : fundec         
                        val func_envs : functionEnvironment 
                        val globals : global list
                        val dom_tree : stmt option Inthash.t 
                        val fmt : Format.formatter
                        val debug : bool      
                        val info : bool     
                     end) = struct


    module SC = TaintInstructionComputer.InstrComputer(struct
                                                            let globals = Param.globals
                                                            let fmt = Param.fmt
                                                            let debug = Param.debug
                                                            let info = Param.info
                                                        end)
    module P = TaintPrinter.Printer(struct
                                        let fmt = Param.fmt
                                    end)      
    module CFGP = TaintCFGPrinter.Printer(struct
                                            let dom_tree = Param.dom_tree
                                            let fmt = Param.fmt
                                          end)

    module Typing = TaintTyping.Typing(struct
                                         let fmt = Param.fmt
                                         let debug = Param.debug
                                         let info = Param.info
                                        end)

    (* This function should be removed. It's used only for debugging purposes. *)
    let print_envs () =
        P.print () "\n\n%s\n" "[DEBUG] Printing all the envs:";
        Inthash.iter
            (fun key env ->
                P.print () "Environmnet for sid: %d\n" key;
                P.print_env () env;
                P.print () "%s" "\n")
            Param.stmt_envs;
        P.print () "%s" "\n\n"

    (* Tests if the old environment and the new environment are the same. *)
    let test_for_change old_ (new_, cond_stack) =
        match Gamma.compare old_ new_ with
            | false -> (true, new_, cond_stack)
            | true 
                -> 
                    match old_ with
                        | (true, _) -> (false, old_, cond_stack)
                        | (false, _) -> (true, new_, cond_stack)
    
    (* Applies the transformations done by a statement to a given environment. *)
    (* Params: *)
    (* stmt - the statement being analyzed *)
    (* new_env - the environment before the statement is analyzed *)
    (* cond_taint - the current condition taintedness stack *)
    (* Returns (the new environment, a boolean to say if the current cond taint should be used, *)
    (* the current_cond_taint). *)
    let rec do_stmt stmt new_env cond_taint =
        let current_cond_taint = Typing.combine_taint_list cond_taint in
        (* let current_cond_taint = List.hd cond_taint in *)
        match stmt.skind with
            | (Instr instr) 
                -> 
                    if (Param.debug) then (
                        P.print () "[DEBUG] Instruction reached%s" "\n";
                    );
                    let ret_env = 
                        SC.do_instr new_env instr current_cond_taint Param.func_envs in
                    (ret_env, Same)
            | (Return (null_expr, _))
                -> 
                    let ret_env = 
                        SC.do_return_instr new_env Param.func null_expr current_cond_taint Param.func_envs in
                    (ret_env, Same)
            | (If (expr, true_block, false_block, _))
                -> 
                    let new_cond_taint = 
                        SC.do_if_instr new_env expr true_block false_block current_cond_taint Param.func_envs in
                    (new_env, Push (stmt.sid, new_cond_taint))
            | (Switch (expr, _, _, _))
                -> 
                    let new_cond_taint = 
                        SC.do_switch_instr new_env expr current_cond_taint Param.func_envs in
                    (new_env, Push (stmt.sid, new_cond_taint))
            | (Loop (_, stmt_block, _, stmt_continue, stmt_break))
                -> 
                    let (ret_env, new_cond_taint) = 
                        SC.do_loop_instr new_env stmt_block stmt_continue stmt_break current_cond_taint in
                    (ret_env, Same)
            | (Block stmt_block)
                -> 
                    let ret_env = List.fold_left
                                    (fun env s 
                                        -> 
                                            let (new_env, _) = do_stmt s env cond_taint in 
                                            new_env)
                                    new_env
                                    stmt_block.bstmts in
                    (ret_env, Same) 
            | (Break _)
                ->  
                    (new_env, Pop)
            | _ -> 
                    (new_env, Same)
     
    let combine_cond_taint stmt nullable_old_stmt cond_taint =
        match List.length stmt.preds with
            | 1 -> cond_taint
            | _ ->
	        match nullable_old_stmt with
	            | None -> List.tl cond_taint
	            | Some old_stmt ->
                match old_stmt.skind with 
                    | Break _ -> cond_taint
                    | _ ->
			        match stmt.skind with
	                    | Loop _ -> 
				            (match Dominators.dominates Param.dom_tree stmt old_stmt with
				                | false -> cond_taint
				                | true ->List.tl cond_taint)
	                    | _ -> List.tl cond_taint
        
    (* Params: *)
    (* worklist - the list of pairs (statement, is_cond_tainted) that will be *)
    (* computed. cond_taint - the taintedness of the condition. Must be combined *)
    (* with all the assignments so that implicit data flows are covered. *)
    let rec compute worklist =
        (* print_envs (); *)
        match List.length worklist with
        (* Stop when the worklist is empty. *)
        | 0 -> ignore ()
        | _ ->
        let (current_stmt, nullable_old_stmt, cond_taint) = List.hd worklist in
        if (Param.info) then (
            P.print () "[INFO] Processing instruction %d from worklist\n" current_stmt.sid;
        );
        (* For each predecessor, combine the results. If there aren't any preds *)
        (* then the statements' environment is returned. *)
        let (new_env, cond_taint) = 
            match List.length current_stmt.preds with
                | 0 ->
                    let (_, curr_env) = Inthash.find Param.stmt_envs current_stmt.sid in
                    ((true, curr_env), cond_taint)
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
	                        (List.tl current_stmt.preds)),
                    combine_cond_taint current_stmt nullable_old_stmt cond_taint
                    )
        in
        let old_env = Gamma.copy (Inthash.find Param.stmt_envs current_stmt.sid) in
        let (changed, env, cond_stack) = 
            test_for_change old_env (do_stmt current_stmt new_env cond_taint) in
        match changed with
            | false 
                -> 
                    (* Fixed point reached for current statement. The successors *)
                    (* aren't added to the worklist. *)
                    (if (Param.debug) then
                        Printf.printf "[DEBUG] Fixed point reached for sid %d\n" current_stmt.sid);
                    Inthash.replace Param.stmt_envs current_stmt.sid env;
                    compute (List.tl worklist)
            | true
                -> 
                    (* We still haven't reached a fixed point for the statement. *)
                    (* Add the successors to the worklist. *)
                    Inthash.replace Param.stmt_envs current_stmt.sid env;
                    let mfun s =
                        match cond_stack with
                            | Same -> 
                                (s, (Some current_stmt), cond_taint)
                            | Push (tsid, tt) -> 
                                (s, (Some current_stmt), Typing.add_to_taint_list (tsid, tt) cond_taint)
                            | Pop -> 
                                (s, (Some current_stmt), List.tl cond_taint)
                    in
                    let new_list = (List.concat 
			                            [List.tl worklist;
			                             List.map 
			                                mfun 
			                                current_stmt.succs]) in
                    compute new_list
                        

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
                    P.print () "%s" "[WARNING] Function without return found!\n"; 
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
        (* CFGP.print_cfg Param.func *)
        let initial_env = create_initial_env () in
        init_environments initial_env;
        compute (List.map (fun s -> (s, None, [(0, U)])) worklist);
        let final = combine_return_envs initial_env in
        P.print () "\nEnvironment for function %s:\n" Param.func.svar.vname;
        P.print_env () final;
        final
end

(* Runs the analysis on a given function with regard to the already computed *)
(* environments. *)
(* Returns the new environment for f. *)
let run_custom_taint format dbg inf f f_envs gls =
    let tree = Dominators.computeIDom f in 
    let module TaintComputer = TaintComputer(struct
                                                let stmt_envs = (Inthash.create 1024)
                                                let func = f
                                                let func_envs = !f_envs
                                                let globals = gls
                                                let dom_tree = tree
                                                let fmt = format
                                                let debug = dbg
                                                let info = inf
                                            end) in
    let start_stmt = List.hd f.sallstmts in
    TaintComputer.start [start_stmt]

(* Runs the analysis for a non recursive function f. Adds the new environment for *)
(* f to the function environments. *)
let run_custom_taint_non_recursive format debug info f f_envs gls =
    let env = run_custom_taint format debug info f f_envs gls in
    Inthash.add !f_envs f.svar.vid env

(* Runs taint analysis on a given list of mutually recursive functions. *)
(* Params: *)
(* format - the format used for printing *)
(* f_list - the list of fundec for the mutually recursive functions *)
(* f_envs - the already computed function environments *)
(* gls - the list of globals in the program *)
let run_custom_taint_recursive format dbg inf f_list f_envs gls =
    let module Typing = TaintTyping.Typing(struct
                                         let fmt = format
                                         let debug = dbg
                                         let info = inf
                                        end) in
    (* Initializes an environment for a recursive function. That is: *)
    (* add a return value that is dependent on all the formals. *)
    let init_recursive_env f =
        let env = Gamma.create_env () in
        let taint = List.fold_left 
                        (fun t formal ->
                            Gamma.set_taint env formal.vid (G [formal]);
                            Typing.combine_taint t (G [formal]))
                        U
                        f.sformals in
        Gamma.set_taint env f.svar.vid taint; 
        env 
    in
    (* Compares two environments for the same recursive function. Compares the *)
    (* return value. If the return value didn't change true is returned. False, *)
    (* otherwise. *)
    let compare_recursive_env f old_env new_env =
        let old_f_taint = Gamma.get_taint old_env f.svar.vid in
        let new_f_taint = Gamma.get_taint new_env f.svar.vid in
        Gamma.compare_taint old_f_taint new_f_taint
    in
    (* Function that iterates until no more changes are made to the environments *)
    (* for the mutually recursive functions. *)
    let rec iterate () = 
        match List.fold_left
                (fun changed f ->
                    let old_env = Inthash.find !f_envs f.svar.vid in
                    let new_env = run_custom_taint format dbg inf f f_envs gls in
                    match compare_recursive_env f old_env new_env with
                        | false -> 
                            Inthash.replace !f_envs f.svar.vid new_env;
                            true
                        | true -> false)
                false
                f_list with
            | false -> ignore ()
            | true -> iterate () 
    in    
    
    List.iter 
        (fun f -> Inthash.add !f_envs f.svar.vid (init_recursive_env f)) 
        f_list;
    iterate ()
