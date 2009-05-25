open Cil_types
open Cil
open TaintGamma

module ResultsComputer(Param:sig
                        val globals : global list     
                        val func_envs : functionEnvironment
                        val func_hash : (string, fundec) Hashtbl.t 
                        val fmt : Format.formatter
                        val debug : bool      
                        val info : bool     
                     end) = struct
    
    module SC = TaintInstructionComputer.InstrComputer(struct
                                                            let globals = Param.globals
                                                            let func_hash = Param.func_hash
                                                            let fmt = Param.fmt
                                                            let debug = Param.debug
                                                            let info = Param.info
                                                        end)
    
    module Typing = TaintTyping.Typing (struct
	                    let fmt = Param.fmt
	                    let debug = Param.debug      
	                    let info = Param.info     
	                 end)
                    
    module TG = TypeHelper.TypeGetter(struct
                                            let fmt = Param.fmt
                                            let debug = Param.debug
                                            let info = Param.info
                                        end)  

    module P = TaintPrinter.Printer(struct
                                        let fmt = Param.fmt
                                        let debug = Param.debug
                                        let info = Param.info
                                    end) 

    type func_frame = Frame of fundec
    type func_stack = func_frame list 

    let stmt_count = ref 0;;

    let inc_stmt_count () =
        stmt_count := !stmt_count + 1

    let taint_stmt_count = ref 0;;

    let taint_stmt_ids = ref (Inthash.create 1024);;

    let inc_taint_stmt_count stmt =
        let matches_vulnerable_scheme () =
            (* TODO match statement against schemes *)
            true
        in
        
        taint_stmt_count := !taint_stmt_count + 1;
        if matches_vulnerable_scheme () then
            Inthash.add !taint_stmt_ids stmt.sid stmt

    let check_tainted taint stmt = 
        match taint with 
            | G _ -> 
                P.print () "WARNING: G reached in results check. Sid: %d:" stmt.sid;
                P.print_taint () taint;
                true
            | _ -> 
                Gamma.compare_taint taint T

    let rec do_instr env_instance curr_stmt curr_func_stack func instr =
        let curr_stmt_env = Inthash.find env_instance curr_stmt.sid in
        let get_lval_vinfo lval =
            match lval with
                | (Var vinfo, _) -> vinfo
                | (Mem exp, _) -> Utils.extract_vinfo_from_ptr_expr exp
        in
        let do_assign lval =
            let vinfo = get_lval_vinfo lval in
                if (check_tainted
                        (Gamma.get_taint
                            curr_stmt_env
                            vinfo.vid)
                        curr_stmt) then (
                    inc_taint_stmt_count curr_stmt;
                    true
                ) else (
                    false
                )
        in
        let do_call null_lval call_expr param_exprs =
            let do_null_lval null_lval =
                match null_lval with
                    | None -> false
                    | Some lval -> do_assign lval
            in 
            let do_check_pointer_params func effect_added =
                if effect_added then (
                    ignore ()
                ) else (
                    let p_len = List.length param_exprs in
                    ignore (
                        List.fold_left 
                        (fun (idx, added) formal ->
                            if added || idx >= p_len then (idx + 1, added)
                            else
                                match TG.is_return_param formal.vtype with
                                | true ->
                                    (try 
                                        let p_expr = List.nth param_exprs idx in
		                                let expr_vinfo = Utils.extract_vinfo_from_ptr_expr p_expr in
		                                let taint = Gamma.get_taint curr_stmt_env expr_vinfo.vid in
                                        if check_tainted taint curr_stmt then (
                                            inc_taint_stmt_count curr_stmt;
                                            (idx + 1, true)
                                        ) else (
                                            (idx + 1, false)
                                        )
                                    with Not_found ->
                                        (idx + 1, false))
                                | false ->
                                    (idx + 1, false))
                        (0, false)
                        func.sformals) )
            in  
            let get_caller_env () =
                let init_env = 
	                match List.length curr_stmt.preds with
	                | 0 -> curr_stmt_env
	                | _ -> Inthash.find env_instance (List.hd curr_stmt.preds).sid in
                List.fold_left
                    (fun env pred ->
                        Typing.combine env (Inthash.find env_instance pred.sid))
                    init_env
                    curr_stmt.preds 
            in
            let rec do_check_recursive_call func_stack callee_func =
                 match func_stack with
                    | [] -> false
                    | (Frame f)::tl ->
                        if f.svar.vid == callee_func.svar.vid then true
                        else
                            do_check_recursive_call tl callee_func
            in
            let do_function_call callee_func =
                let caller_env = get_caller_env () in
                let p_len = List.length param_exprs in
                let (_, taint_instances) = 
                    List.fold_left
                        (fun (idx, instances) formal ->
                            if idx >= p_len then (idx + 1, instances)
                            else
                                let p_expr = List.nth param_exprs idx in
                                let f_taint = SC.do_expr caller_env p_expr [] Param.func_envs in
                                (idx + 1, (formal, f_taint)::instances))
                        (0, [])
                        callee_func.sformals in
                let taint_instances = 
                    List.fold_left
                        (fun instances global ->
                            let g_taint = Gamma.get_taint caller_env global.vid in
                            (global, g_taint)::instances)
                        taint_instances
                        (SC.list_global_vars () ()) in
                let (_, callee_env) = Inthash.find Param.func_envs callee_func.svar.vid in
                let callee_env = Typing.instantiate_func_env callee_env taint_instances in
                let new_worklist =
                    match List.length callee_func.sallstmts with
                        | 0 -> []
                        | _ -> [List.hd callee_func.sallstmts] in
                let new_visited_ref = ref (Inthash.create 1024) in
                match do_check_recursive_call curr_func_stack callee_func with
                    | false ->
                        P.print_info () "Analyzing results for function %s\n" callee_func.svar.vname;
                        compute callee_env 
                            ((Frame func)::curr_func_stack) 
                            (callee_func, new_visited_ref, new_worklist)
                    | true ->
                        ignore ()
            in
                        
            let effect_added = do_null_lval null_lval in
            let finfo = get_lval_vinfo (Utils.extract_lvalue_from_expr call_expr) in
            try 
	            let callee_func = Hashtbl.find Param.func_hash finfo.vname in
	            do_check_pointer_params callee_func effect_added;
	            do_function_call callee_func
            with Not_found ->
                ignore () 
        in
        
        inc_stmt_count ();
        match instr with
            | Set (lval, _, _) ->
                ignore (do_assign lval)        
            | Call (null_lval, call_expr, param_exprs, _) ->
                do_call null_lval call_expr param_exprs
            | _ -> ignore ()

    and do_return env_instance curr_stmt func null_expr =
        let curr_stmt_env = Inthash.find env_instance curr_stmt.sid in
        inc_stmt_count ();
        match null_expr with
            | None -> 
                ignore ()
            | Some _ -> 
                if (check_tainted 
                        (Gamma.get_taint 
                            curr_stmt_env 
                            (Typing.get_function_return_vid func.svar))
                        curr_stmt) then
                    inc_taint_stmt_count curr_stmt

    and do_stmt env_instance curr_func_stack func visited_ref curr_stmt =
        let visited = 
            try
                Inthash.find !visited_ref curr_stmt.sid
            with Not_found ->
                Inthash.add !visited_ref curr_stmt.sid true;
                false in
        if visited then (
            []
        ) else ( 
	        match curr_stmt.skind with
	            | Instr instr -> 
	                do_instr env_instance curr_stmt curr_func_stack func instr;
                    curr_stmt.succs
	            | Return (null_expr, _) -> 
	                do_return env_instance curr_stmt func null_expr;
                    curr_stmt.succs
	            | If _ -> 
	                curr_stmt.succs
	            | Switch _ ->
	                curr_stmt.succs
	            | Loop _ -> 
	                curr_stmt.succs
	            | Block block ->
	                List.fold_left 
	                    (fun succs s -> 
                            let new_succs = do_stmt env_instance curr_func_stack func visited_ref s in
                            List.append succs new_succs)
                        []
	                    block.bstmts
	            | _ -> 
	                curr_stmt.succs
        )
        
    and compute env_instance curr_func_stack (func, visited_ref, worklist) =
        match worklist with
        | [] -> 
            P.print_info () "Finished analyzing function %s\n" func.svar.vname;
            P.print_info () "%s" "Current stack: ";
            List.iter
                (fun (Frame f) ->
                    P.print_info () "%s, " f.svar.vname)
                (List.rev curr_func_stack);
            P.print_info () "%s" "\n";
            ignore ()
        | curr_stmt::rest -> 
            match do_stmt env_instance curr_func_stack func visited_ref curr_stmt with
            | [] ->
                compute env_instance
                    curr_func_stack
                        (func, visited_ref, rest)
            | succs ->
                compute env_instance 
                    curr_func_stack 
                        (func, visited_ref, List.append (rest) succs)
            
end

let get_main_func globals =
        (* TODO: remove hardcoding for entry point *)
        match List.fold_left
            (fun result g ->
                match result with
                    | Some _ -> result
                    | None -> 
                        match g with
                            | GFun (funcdec, _) when funcdec.svar.vname = "main" -> Some funcdec
                            | _ -> None)
            None
            globals with
        | None -> failwith "[FATAL]: TAINT RESULTS: Missing main function."
        | Some result -> result

let get_results format dbg inf f_envs f_hash gls =
    let module Computer = ResultsComputer(struct
					                        let globals = gls
					                        let func_envs = f_envs
					                        let func_hash = f_hash 
					                        let fmt = format
					                        let debug = dbg      
					                        let info = inf     
					                     end) in
    let main_func = get_main_func gls in
    let (_, main_instance) = Inthash.find f_envs main_func.svar.vid in
    let worklist = 
        match List.length main_func.sallstmts with
            | 0 -> []
            | _ -> [List.hd main_func.sallstmts] in
    let visited_ref = ref (Inthash.create 1024) in
    Printf.printf "%s\n" "Analyzing results for function main";
    Computer.compute main_instance [] (main_func, visited_ref, worklist);
    (!Computer.stmt_count, !Computer.taint_stmt_count)
