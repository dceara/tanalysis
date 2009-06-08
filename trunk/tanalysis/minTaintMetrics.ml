open Cil
open Cilutil
open Cil_types
open MetricComputer
open TaintGamma
open Kernel_function
open Db_types

(* Min Taint Metric Part *)
module MinTaintMetric(Param:sig
                        val func_envs : functionEnvironment
                     end) = struct
    type min_taint_dependent_var_metric = DepVar of int
    type min_taint_possible_taint_var_metric = PosTaint of int
    type min_taint_non_cyclic_metric = NonCyclic of 
                                        (min_taint_dependent_var_metric * 
                                         min_taint_possible_taint_var_metric)
    type min_taint_cyclic_metric = Cyclic of 
                                        (min_taint_dependent_var_metric * 
                                         min_taint_possible_taint_var_metric)
    type min_taint_metric = MinTaintMetric of 
                                        (min_taint_non_cyclic_metric * 
                                         min_taint_cyclic_metric)
    
    let get_func stmt = 
        let (_, k_func) = find_from_sid stmt.sid in
        let func = match k_func.fundec with
            | Definition (f, _) -> f
            | _ -> failwith "[ERROR][MIN_TAINT_METRIC] Missing function taint spec" in
        func    
    
    let get_stmt_env stmt =
        let func = get_func stmt in
        let (_, stmt_env) = Inthash.find Param.func_envs func.svar.vid in
        let env = Inthash.find stmt_env stmt.sid in
        env
        
    let get_func_env func =
        let (env, _) = Inthash.find Param.func_envs func.svar.vid in
        env
    
    let compare
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv1, PosTaint nc_pt1), 
             Cyclic (DepVar c_dv1, PosTaint c_pt1)))
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv2, PosTaint nc_pt2), 
             Cyclic (DepVar c_dv2, PosTaint c_pt2))) =
        (* TODO: remove REALLY UGLY comparisons *)
        if nc_dv1 - nc_dv2 = 0 then
            if nc_pt1 - nc_pt2 = 0 then
                if c_dv1 - c_dv2 = 0 then
                    c_pt1 - c_pt2
                else
                    c_dv1 - c_dv2
            else
                nc_pt1 - nc_pt2
        else
            nc_dv1 - nc_dv2
    
    let equal t1 t2 =
        if compare t1 t2 = 0 then true else false
        
    let default = (MinTaintMetric 
		            (NonCyclic (DepVar 0, PosTaint 0), 
		             Cyclic (DepVar 0, PosTaint 0)))

    let zero = default
    
    let add 
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv1, PosTaint nc_pt1), 
             Cyclic (DepVar c_dv1, PosTaint c_pt1)))
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv2, PosTaint nc_pt2), 
             Cyclic (DepVar c_dv2, PosTaint c_pt2))) =
        (MinTaintMetric 
            (NonCyclic (DepVar (nc_dv1 + nc_dv2), PosTaint (nc_pt1 + nc_pt2)), 
             Cyclic (DepVar (c_dv1 + c_dv2), PosTaint (c_pt1 + c_pt1)))) 

    let sub 
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv1, PosTaint nc_pt1), 
             Cyclic (DepVar c_dv1, PosTaint c_pt1)))
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv2, PosTaint nc_pt2), 
             Cyclic (DepVar c_dv2, PosTaint c_pt2))) =
        (MinTaintMetric 
            (NonCyclic (DepVar (nc_dv1 - nc_dv2), PosTaint (nc_pt1 - nc_pt2)), 
             Cyclic (DepVar (c_dv1 - c_dv2), PosTaint (c_pt1 - c_pt1))))
            
    let add_second_in_loop
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv1, PosTaint nc_pt1), 
             Cyclic (DepVar c_dv1, PosTaint c_pt1)))
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv2, PosTaint nc_pt2), 
             Cyclic _)) =
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv1, PosTaint nc_pt1), 
             Cyclic (DepVar (c_dv1 + nc_dv2), PosTaint (c_pt1 + nc_pt2))))

    let node_value stmt =
        try 
            (* In the case of an empty instruction block we have no environment for it so *)
            (* we return the default value. *)
            (* TODO: fix this *) 
	        let pred_envs = List.map (fun pred -> get_stmt_env pred) stmt.succs in
	        let env = get_stmt_env stmt in
	        let diff_env = Gamma.get_differences env pred_envs in
	        let pos_tainted_cnt = Gamma.get_possible_tainted_count diff_env in
	        (MinTaintMetric 
	            (NonCyclic (DepVar 0, PosTaint pos_tainted_cnt), 
	             Cyclic (DepVar 0, PosTaint 0)))
        with Not_found ->
            default

    let get_call_cost callee =
        let callee_env = get_func_env callee in
        let dep_var_cnt = Gamma.count_dependencies callee_env callee.sformals in
        (MinTaintMetric 
            (NonCyclic (DepVar dep_var_cnt, PosTaint 0), 
             Cyclic (DepVar 0, PosTaint 0)))

    let edge_value stmt_src stmt_dst =
        let dst_val = node_value stmt_dst in
        (* Add DepVar cost only if statements are in different functions and *)
        (* if the source is a function call. This check needs to be done *)
        (* because of splitting call nodes in the metric analysis. *)
        let src_func = get_func stmt_src in
        let dst_func = get_func stmt_dst in
        if src_func.svar.vid = dst_func.svar.vid then
            dst_val
        else
            add dst_val (get_call_cost dst_func)
        
    let print_value 
        (MinTaintMetric 
            (NonCyclic (DepVar nc_dv1, PosTaint nc_pt1), 
             Cyclic (DepVar c_dv1, PosTaint c_pt1))) =
        Printf.printf 
	        "(MinTaintMetric (NonCyclic (DepVar %d, PosTaint %d), Cyclic (DepVar %d, PosTaint %d)))\n" 
            nc_dv1 nc_pt1 c_dv1 c_pt1
    
    let get_path_bound_stmts f_hash = 
    (* TODO: remove hard-coding for main entry point *)
    let main_func = Hashtbl.find f_hash "main" in
    let start = 
        match main_func.sallstmts with
        | [] -> failwith "[ERROR][METRICS] No statements in main function"
        | start::_ -> start in
    (* TODO: Check for multiple return statements *)
    let end_stmts = 
        List.fold_left
            (fun res stmt ->
                match stmt.succs with
                | [] -> stmt :: res
                | _ -> res)
            []
            main_func.sallstmts in
    (start, List.hd end_stmts)  
end

let compute_min_taint_metrics format dbg inf f_hash func_envs =
    let funcs = Hashtbl.fold
                    (fun name func result ->
                        func::result)
                    f_hash 
                    [] in
    let module MTM = MinTaintMetric(struct
                                        let func_envs = func_envs
                                    end) in
    let module P = Printer.Printer(struct
	                                    let fmt = format
	                                    let debug = dbg
	                                    let info = inf
	                                end) in
    let module Computer = MetricComputer(struct
                        type t = MTM.min_taint_metric
                        let func_hash = f_hash
                        let fmt = format
                        let debug = dbg    
                        let info = inf
                        let value_comparator = MTM.compare
                        let value_equal = MTM.equal
                        let value_default = MTM.default
                        let value_zero = MTM.zero
                        let value_add = MTM.add  
                        let value_sub = MTM.sub
                        let value_add_second_in_loop = MTM.add_second_in_loop
                        let node_value = MTM.node_value
                        let edge_value = MTM.edge_value   
                        let print_value = MTM.print_value
                     end) in
    let graph = Computer.create_graph funcs in
    let (new_graph, removed_edges) = Computer.break_loops graph in
    let (start_stmt, end_stmt) = MTM.get_path_bound_stmts f_hash in
    let (shortest_path, weight) = Computer.get_best_path new_graph start_stmt end_stmt in
    let (path_node_list, weight) = Computer.add_removed_edges new_graph shortest_path weight removed_edges in
    MTM.print_value weight; 
    ignore ()
