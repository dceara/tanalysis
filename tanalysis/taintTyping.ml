open TaintGamma
open Utils
open Cil_types

(* Typing rules *)
module Typing = struct
    
    (* Applies the oplus operator between taint values. *)
    let combine_taint t1 t2 =
        match (t1, t2) with
            | (T, _) 
            | (_, T) -> T
            | ((G g), U) -> (G g)
            | (U, (G g)) -> (G g)
            | (U, U) -> U
            | ((G g1), (G g2)) -> (G (List.concat [g1; g2]))
    
    (* Literals are always untainted. *)
    let process_literal env _ =
        U
    
    (* Locals are initialized to tainted. *)
    let process_local env vinfo =
        Gamma.set_taint env vinfo T;
        env
    
    (* Formals are initialized to G. *)
    let process_formal env vinfo =
        let t = (G [vinfo]) in 
        Gamma.set_taint env vinfo t;
        env
    
    (* For an expression the return taint type is computed based on the variables*)
    (* used in the expression. *)
    let process_expression env vinfos =
        List.fold_left 
            (fun t el -> combine_taint t (Gamma.get_taint env el))
            U
            vinfos
    
    (* An assignment changes the taint value for a variable with regard to the *)
    (* expression on the rhs. *)
    (* Params: *)
    (* env - the current environment *)
    (* vinfo - the lvalue *)
    (* vinfos - the variables used in the rhs *)
    let process_assign env vinfo vinfos =
        let expr_taint = process_expression env vinfos in
        Gamma.set_taint env vinfo expr_taint;
        env 

    (* Processes an if statement. Combines the types for the assigned variables *)
    (* for both branches and with regard to the taintedness of the guard expression. *)
    (* Returns the new environment. *)
    (* Params: *)
    (* env - the current environment *)
    (* env_true - the environment after executing the true branch *)
    (* env_false - the environment after executing the false branch *)
    (* expr_vinfos - the variables used in the guard expression *)
    (* assigned_true - the variables assigned in the true branch *)
    (* assigned_false - the variables assigned in the false branch *)            
    let process_if env env_true env_false expr_vinfos assigned_true assigned_false =
        let expr_taint = process_expression env expr_vinfos in
        let assigned = Utils.reunite 
                            (fun vi1 vi2 -> vi1.vid = vi2.vid) 
                            assigned_true 
                            assigned_false in
        List.iter 
            (fun vinfo -> Gamma.set_taint 
                            env 
                            vinfo 
                            (combine_taint 
                                (Gamma.get_taint env_true vinfo) 
                                (combine_taint 
                                    (Gamma.get_taint env_false vinfo) 
                                    expr_taint)))
            assigned;
         env
        
    (* Processes a while statement. Combines the types for the assigned variables *)
    (* with regard to the taintedness of the guard expression. *)
    (* Params: *)
    (* env - the current environment *)
    (* env_loop - the environment after executing the loop *)
    (* expr_vinfos - the variables used in the guard expression *)
    (* assigned_loop - the list of variables assigned in the loop *)
    let process_while env env_loop expr_vinfos assigned_loop =
        let expr_taint = process_expression env expr_vinfos in
        List.iter 
            (fun vinfo -> Gamma.set_taint
                            env
                            vinfo
                            (combine_taint
                                expr_taint
                                (Gamma.get_taint env_loop vinfo)))
            assigned_loop;
        env
    
    (* Processes a function call. Instantiates the taint types for a previously *)
    (* computed function. *)
    (* Params: *)
    (* env - the current environment *)
    (* func - the fundec for the called function *)
    (* vinfo - the variable assigned by the function call: for now we consider that *)
    (* functions always return a value and do not have side effects *) 
    (* env_funv - the previously computed callee environment *)
    (* param_bindings - the list of bindings between formal and actual parameters for *)
    (* the callee *)
    let process_call env func vinfo env_func param_bindings =
        (* Local function used for finding the actual parameter passed for a *)
        (* formal one. *)
        let find_binding param_bindings formal_info =
            List.find
                (fun (Param ((Formal finfo), _)) 
                    -> formal_info.vid = finfo.vid) 
                param_bindings 
        in
        (* Local function used for instantiating all the formal parameter taints *)
        (* according to actual parameter taints. *)
        let instantiate_call env ret_taint param_bindings =
            List.fold_left 
                (fun t dep -> 
                    let (Param (_, (Actual ainfo))) = find_binding param_bindings dep in
                    combine_taint t (Gamma.get_taint env ainfo))
	            U
	            ret_taint
        in
        (match Gamma.get_taint env_func func.svar with
            | U -> Gamma.set_taint env vinfo U
            | T -> Gamma.set_taint env vinfo T
            | (G g) 
                -> Gamma.set_taint env vinfo (instantiate_call env g param_bindings));
        env
end
