open Cil_types

type taintValue = T | U | G of varinfo list

type formalParam = Formal of varinfo

type actualParam = Actual of varinfo

type paramBinding = Param of formalParam * actualParam

(* the environment is a mapping between symbol ids and taint values *)
type environment = (int, taintValue) Hashtbl.t

type statementsEnvironment = environment Inthash.t

type environmentStack = environment list

type functionEnvironment = (varinfo, environment) Hashtbl.t

(* Gamma mapping *)
module Gamma = struct
    
    let create_env () = 
        Hashtbl.create 1000
    
    (* Returns the taint value or the delayed taint value if found, raises Not_found otherwise *)
    let get_taint env vid =
        Hashtbl.find env vid
            
    let set_taint env vid taint =
        (try 
            ignore (Hashtbl.find env vid);
            Hashtbl.remove env vid
        with Not_found ->
            ignore ());
        Hashtbl.add env vid taint
        
    let compare t1 t2 =
        match (t1, t2) with
            | (T, T) -> true
            | (U, U) -> true
            | ((G g1), (G g2)) 
                ->
                    List.for_all 
                        (fun vinfo1 ->
                            List.mem vinfo1 g2)
                        g1  
            | _ -> false     
        
    let compare env1 env2 = 
        Hashtbl.fold
            (fun id t1 eq ->
                let t2 = Hashtbl.find env2 id in
                match eq with
                    | false -> false
                    | _ -> compare t1 t2)
            env1
            true

    (* Function for pretty printing an environment. Should be used for debugging *)
    (* purposes only. *)
    let pretty_print fmt env =
        let pretty_print_taint taint = 
            (match taint with
                | T -> Format.fprintf fmt "\t%s\n" "Tainted"
                | U -> Format.fprintf fmt "\t%s\n" "Untainted"
                | (G g) 
                    -> 
                        Format.fprintf fmt "\t%s" "Generic: ";
                        List.iter 
                            (fun el -> Format.fprintf fmt "Gamma(%s), " el.vname)
                            g)
        in
        Format.fprintf fmt "%s\n" "========================================";
        Hashtbl.iter 
            (fun vid taint ->
                Format.fprintf fmt "\tSymname: %d\n" vid;
                pretty_print_taint taint;
                Format.fprintf fmt "%s" "\n")
            env;
        Format.fprintf fmt "%s\n" "========================================";
end
