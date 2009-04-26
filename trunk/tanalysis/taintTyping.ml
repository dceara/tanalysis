open TaintGamma
open Utils
open Cil_types

(* Typing rules *)
module Typing = struct
    
    (* Applies the oplus operator between taint values. *)
    let combine_taint t1 t2 =
        let taint_comparator g1 g2 =
            g1.vid = g2.vid
        in
        match (t1, t2) with
            | (T, _) 
            | (_, T) -> T
            | ((G g), U) -> (G g)
            | (U, (G g)) -> (G g)
            | (U, U) -> U
            | ((G g1), (G g2)) -> (G (reunite taint_comparator g1 g2))
    
    let combine_taint_list taint_list =
        List.fold_left
            (fun t (_, taint) -> combine_taint t taint)
            U
            taint_list
            
    let add_to_taint_list (sid, t) l =
        match List.filter (fun (id, _) -> id == sid) l with
            | [] -> (sid, t)::l
            | _ -> l
    
    (* Combines two environments. Stores the result in the first parameter and *)
    (* returns it. *)
    let combine (vis1, _env1) (vis2, _env2) =
        let order_visited_first (v1, e1) (v2, e2) =
            match v1 with 
                | true -> ((v1, e1), (v2, e2))
                | false -> ((v2, e2), (v1, e1))
        in
        
        let ((vis1, _env1), (vis2, _env2)) = 
            order_visited_first (vis1, _env1) (vis2, _env2) in
        (* if one of the envs wasn't visited, the data there isn't sane so we*)
        (* ignore it :) *)
        match vis2 with
            | false -> (true, _env1)
            | true ->
		        Hashtbl.iter
		            (fun id t1 ->
		                let t2 = Hashtbl.find _env2 id in
		                Gamma.set_taint (vis1, _env1) id (combine_taint t1 t2)
		            )
		            _env1;
		        (true, _env1)
    
    (* Locals are initialized to tainted. *)
    let process_local env vinfo =
        Gamma.set_taint env vinfo.vid T;
        env
    
    (* Formals are initialized to G. *)
    let process_formal env vinfo =
        let t = (G [vinfo]) in 
        Gamma.set_taint env vinfo.vid t;
        env
    
end
