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
    
    let combine_taint_list taints =
        List.fold_left
            (fun t taint -> combine_taint t taint)
            U
            taints
    
    (* Combines two environments. Stores the result in the first parameter and *)
    (* returns it. *)
    let combine env1 env2 =
        let _env1 = match env1 with (_, _env) -> _env in
        let _env2 = match env2 with (_, _env) -> _env in
        Hashtbl.iter
            (fun id t1 ->
                let t2 = Hashtbl.find _env2 id in
                Gamma.set_taint env1 id (combine_taint t1 t2)
            )
            _env1;
        (* TODO: check this *)
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
