open Cil_types
open Cil
open TaintTyping
open TaintGamma


module InstrComputer(Param:sig
                        val globals : global list
                        val fmt : Format.formatter
                        val debug : bool      
                        val info : bool     
                     end) = struct
                                   
    module P = TaintPrinter.Printer(struct
                                        let fmt = Param.fmt
                                        let debug = Param.debug
                                        let info = Param.info
                                    end)      
    
    let find_fundec id =
        let GFun (func, _) = List.find
                                (fun g ->
                                    match g with
                                        | GFun (func, _) -> 
                                            func.svar.vid = id
                                        | _ -> false)
                                Param.globals in
        func             
    
    let extract_vinfo_from_ptr_expr expr =
        let rec _extract_vinfo_from_lval lvl =
            match lvl with
                | (Var vinfo, _) -> Some vinfo
                | (Mem exp, _) -> _extract_vinfo_from_ptr_expr exp
        and 
        _extract_vinfo_from_ptr_expr expr =
	        match expr with
	            | Const _ -> None
                | SizeOf _ -> None
                | SizeOfE _ -> None
                | SizeOfStr _ -> None
                | AlignOf _ -> None
                | AddrOf lvl -> _extract_vinfo_from_lval lvl
                | StartOf _ -> None
                | Lval lvl -> _extract_vinfo_from_lval lvl
                | UnOp (_, unop_expr, _) -> _extract_vinfo_from_ptr_expr unop_expr 
                | BinOp (_, expr1, expr2, _) 
                    -> 
                        let null_vinfo1 = _extract_vinfo_from_ptr_expr expr1 in
                        let null_vinfo2 = _extract_vinfo_from_ptr_expr expr2 in
                        (match null_vinfo1 with
                            | None -> null_vinfo2
                            | _ -> null_vinfo1)
                | _ -> None   
        in
        match _extract_vinfo_from_ptr_expr expr with
            | None -> raise Not_found
            | Some vinfo -> vinfo
    
    (* Returns the taintedness for a lvalue. *)
    (* Params: *)
    (* env - the current environment *)
    (* lvalue - the lvalue being analyzed *)
    (* param_exprs - contains the list of param expressions in the case that *)
    (* lvalue contains a function call *)
    (* func_envs - the environments for already computed functions *)    
    let rec get_lvalue_taint env lvalue param_exprs func_envs =
        (* Local function used for finding the actual parameter passed for a *)
        (* formal one. *)
        let find_binding actuals formals dep =
            let (i, _) = List.fold_left
                        (fun (idx, found) f ->
                            match found with
                                | true -> (idx, found)
                                | false ->
                                    if dep.vname == f.vname then (idx, true)
                                    else (idx + 1, false))
                        (0, false)
                        formals in
            List.nth actuals i
        in
        (* Local function used for instantiating all the formal parameter taints *)
        (* according to actual parameter taints. *)
        let instantiate_call env ret_taint actuals formals =
            List.fold_left 
                (fun t dep -> 
                    let param_expr = find_binding actuals formals dep in
                    let param_taint = do_expr env param_expr [] func_envs in
                    Typing.combine_taint t param_taint)
                U
                ret_taint
        in
        let get_lvalue_taint_vinfo vinfo =
            let taint = 
                try
                    Gamma.get_taint env vinfo.vid 
                with Not_found ->
                    (* Then the lvalue is actually a function call.. Stupid CIL :P *)
                    let func = find_fundec vinfo.vid in
                    let callee_env = Inthash.find func_envs vinfo.vid in
                    let formals = func.sformals in
                    match Gamma.get_taint callee_env func.svar.vid with
                        | U -> U
                        | T -> T
                        | (G g) 
                            ->
                                let g_taint = (instantiate_call env g param_exprs formals) in 
                                g_taint       
                    
            in
            if Param.debug then (
                P.print () "[DEBUG] Taint for lvalue %s: " vinfo.vname;
                P.print_taint () taint);
            taint
        in
        (* Gets the taint for a lvalue that is a memory location. I.e.: a pointer. *)
        let get_lvalue_taint_mem expr =
            let taint = do_expr env expr [] func_envs in
            if Param.debug then (
                P.print () "[DEBUG] Taint for memory lvalue: %s" "\n";
                P.print_taint () taint);
            taint
        in
        (if Param.info then
            P.print () "[INFO] Getting lvalue taint %s" "\n"); 
        match lvalue with
            | ((Var vinfo), _) -> get_lvalue_taint_vinfo vinfo
            | ((Mem exp), _) -> get_lvalue_taint_mem exp          
    and
    (* Returns the taintedness of an expression according to the environment. *)
    (* Params: *)
    (* env - the current environment *)
    (* expr - the expression to be analyzed *)
    (* param_exprs - contains the list of parameter expressions if the current *)
    (* expression contains a function call *)
    (* func_envs - the previously computed function environments *)
    do_expr env expr param_exprs func_envs =
        let do_const () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for Constant is: ";
                P.print_taint () U);
            U
        in
        let do_lvalue lvalue =
            let taint = get_lvalue_taint env lvalue param_exprs func_envs in
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for Lvalue is: ";
                P.print_taint () taint);
            taint
        in
        let do_sizeOf () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for SizeOf is: ";
                P.print_taint () U);
            U
        in
        let do_sizeOfE expr =
            let taint = do_expr env expr param_exprs func_envs in
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for SizeOfE is: ";
                P.print_taint () taint);
            taint
        in
        let do_sizeOfStr () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for SizeOfStr is: ";
                P.print_taint () U);
            U
        in
        let do_alignOf () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for AlignOf is: ";
                P.print_taint () U);
            U
        in
        let do_alignOfE () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for AlignOfE is: ";
                P.print_taint () U);
            U
        in
        let do_unOp expr =
            let taint = do_expr env expr param_exprs func_envs in
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for UnOp is: ";
                P.print_taint () taint);
            taint
        in
        let do_binOp expr1 expr2 =
            let taint = Typing.combine_taint 
                            (do_expr env expr1 param_exprs func_envs) 
                            (do_expr env expr2 param_exprs func_envs) in
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for BinOp is: ";
                P.print_taint () taint);
            taint
        in
        (* TODO: For now we consider all casts to be taints. Probably this isn't a *)
        (* good approach. *)
        let do_castE () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for CastE is: ";
                P.print_taint () T);
            T
        in
        let do_addrOf lvalue =
            let taint = get_lvalue_taint env lvalue param_exprs func_envs in
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for AddrOf is: ";
                P.print_taint () taint);
            taint
        in
        let do_startOf lvalue = 
            let taint = get_lvalue_taint env lvalue param_exprs func_envs in 
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for StartOf is: ";
                P.print_taint () taint);
            taint
        in
        let do_info () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for Info is: ";
                P.print_taint () U);
            U
        in
        
        (if Param.info then
            P.print () "[INFO] Processing instruction %s" "\n");
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
    
    (* Processes a nullable expression. If the expression is null U is returned, *)
    (* otherwise the taintedness of the expression is returned. *)
    (* Params: *)
    (* env - the current environment *)
    (* null_expr - a possibly null expression *)
    (* param_exprs - contains the list of parameter expressions when the null_expr *)
    (* has a function call *)
    (* func_envs - the previously computed function environments *)
    let do_null_expr env null_expr param_exprs func_envs =
        let do_null () =
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for Null expr is: ";
                P.print_taint () U);
            U
        in
        let do_not_null expr =
            let taint = do_expr env expr param_exprs func_envs in
            if Param.debug then (
                P.print () "%s" "[DEBUG] Taint for NotNull expr is: ";
                P.print_taint () taint);
            taint
        in
        
        (if Param.info then
            P.print () "[INFO] Processing nullable expression %s" "\n");
        match null_expr with
            | None -> do_null ()
            | Some expr -> do_not_null expr 
    
    (* Changes the mapping for lvalue in the given environment according to cond_taint*)
    (* and the assigned expression. *)
    (* Params: *)
    (* env - the current environment *)
    (* expr - the rvalue expression *)
    (* cond_taint - the current condition taint *)
    (* param_exprs - contains the list of parameter expressions when expr has a *)
    (* function call *)
    (* func_env - the previously computed function environments *)
    let do_assign env lvalue expr cond_taint param_exprs func_env =
        let do_assign_lvalue_tainted vinfo =
            (if Param.debug then
                P.print () "[DEBUG] Assigning T to %s\n" vinfo.vname);
            Gamma.set_taint env vinfo.vid T;
            env
        in
        let do_assign_lvalue vinfo expr =
            let expr_taint = do_expr env expr param_exprs func_env in
            if Param.debug then (
                P.print () "[DEBUG] Assigning to %s taint:" vinfo.vname;
                P.print_taint () expr_taint
            );
            Gamma.set_taint env vinfo.vid (Typing.combine_taint expr_taint cond_taint);
            env
        in
        let do_assign_lvalue_mem_tainted ptr_expr =
            let vinfo = extract_vinfo_from_ptr_expr ptr_expr in
            (if Param.debug then
                P.print () "[DEBUG] Assigning T to %s\n" vinfo.vname);
            Gamma.set_taint env vinfo.vid T;
            env
        in
        let do_assign_lvalue_mem ptr_expr expr =
            let vinfo = extract_vinfo_from_ptr_expr ptr_expr in
            let expr_taint = do_expr env expr param_exprs func_env in
            if Param.debug then (
                P.print () "[DEBUG] Assigning to %s taint:" vinfo.vname;
                P.print_taint () expr_taint
            );
            Gamma.set_taint env vinfo.vid (Typing.combine_taint expr_taint cond_taint);
            env
        in
        
        (if Param.info then
            P.print () "[INFO] Processing assign instruction %s" "\n");
        match (lvalue, cond_taint) with
            | ((Var vinfo, _), T) -> do_assign_lvalue_tainted vinfo                    
            | ((Var vinfo, _), _) -> do_assign_lvalue vinfo expr
            | ((Mem ptr_expr, _), T) -> do_assign_lvalue_mem_tainted ptr_expr 
            | ((Mem ptr_expr, _), _) -> do_assign_lvalue_mem ptr_expr expr   
            
    (* TODO: Makes the assumption that all the functions return a single value and have no *)
    (* side effects. *)
    (* Params: *)
    (* env - the current environment *)
    (* null_lval - the nullable lvalue *)
    (* cast_expr - the expression containing the call *)
    (* cond_taint - the current condition taint *)
    (* func_envs - the previously computed environments *)
    let do_call env null_lval cast_expr param_exprs cond_taint func_envs =
        let do_call_lval vinfo =
            let taint = do_expr env cast_expr param_exprs func_envs in
            let taint = Typing.combine_taint taint cond_taint in
            Gamma.set_taint env vinfo.vid taint;
            if Param.debug then (
                P.print () "[DEBUG] Assigning to %s taint value:" vinfo.vname;
                P.print_taint () taint
            );
            env
        in  
        let do_call_default () = 
            env 
        in
        
        (if Param.info then
            P.print () "[INFO] Processing call instruction %s" "\n");
        match null_lval with
            | Some (Var vinfo, _) -> do_call_lval vinfo 
            | _ -> do_call_default ()
    
    (* Changes the environment according to the instruction *)
    let do_instr env instr cond_taint func_envs =
        (if Param.info then
            P.print () "[INFO] Processing instruction %s" "\n");
        match instr with
            | (Set (lvalue, expr, _))
                -> do_assign env lvalue expr cond_taint [] func_envs
            | (Call (null_lval, cast_expr, param_exprs, _))
                (* Make the assumption that all the functions return a single value. *)
                -> do_call env null_lval cast_expr param_exprs cond_taint func_envs
            | _ -> env 
        
    (* Changes the return value in the environment according to the return expression *)
    let do_return_instr env func null_expr cond_taint func_envs =
        let fname = func.svar.vname in
        let fid = func.svar.vid in
        (if Param.info then
            P.print () "[INFO] Processing return instruction for function %s\n" fname);
        let old_taint = 
            (try 
                Gamma.get_taint env fid
            with Not_found -> U)
        in
        let new_taint = (Typing.combine_taint 
                            old_taint 
                            (Typing.combine_taint 
                                cond_taint 
                                (do_null_expr env null_expr [] func_envs))) in
        Gamma.set_taint env fid new_taint;
        if Param.debug then (
            P.print () "[DEBUG] Old taint for return in function %s:" fname;
            P.print_taint () old_taint;
            P.print () "[DEBUG] New taint for return in function %s:" fname;
            P.print_taint () new_taint
        );
        env
        
    (* Only return the taintedness of the expression. The successors will be computed*)
    (* later. *)
    let do_if_instr env expr true_block false_block cond_taint func_env =
        (if Param.info then
            P.print () "[INFO] Processing if instruction %s" "\n");
        let expr_taint = do_expr env expr [] func_env in 
        let new_cond_taint = Typing.combine_taint expr_taint cond_taint in
        if Param.debug then (
            P.print () "%s" "[DEBUG] Condition taint for if instruction:";
            P.print_taint () cond_taint;
            P.print () "%s" "[DEBUG] New condition taint for if instruction:";
            P.print_taint () new_cond_taint;
        );
        new_cond_taint
        
    (* Only return the taintedness of the expression. The successors will be computed*)
    (* later. *)
    let do_switch_instr env expr cond_taint func_env =
        (if Param.info then
            P.print () "[INFO] Processing switch instruction %s" "\n");
        let expr_taint = do_expr env expr [] func_env in 
        let new_cond_taint = Typing.combine_taint expr_taint cond_taint in
        if Param.debug then (
            P.print () "%s" "[DEBUG] Condition taint for if instruction:";
            P.print_taint () cond_taint;
            P.print () "%s" "[DEBUG] New condition taint for if instruction:";
            P.print_taint () new_cond_taint;
        );
        new_cond_taint
        
    (* TODO *)
    let do_loop_instr env stmt_block stmt_continue stmt_break cond_taint =
         (if Param.info then
            P.print () "[INFO] Processing loop instruction %s" "\n");
        (env, cond_taint)
end