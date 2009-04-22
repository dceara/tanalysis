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
    
    (* Combines two environments. Stores the result in the first parameter and *)
    (* returns it. *)
    let combine env1 env2 =
        Hashtbl.iter
            (fun id t1 ->
                let t2 = Hashtbl.find env2 id in
                Gamma.set_taint env1 id (combine_taint t1 t2)
            )
            env1;
        env1
    
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
