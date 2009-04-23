open Cil_types
open Cil

type taintValue = T | U | G of varinfo list

type formalParam = Formal of varinfo

type actualParam = Actual of varinfo

type paramBinding = Param of formalParam * actualParam

(* The environment is a mapping between symbol ids and taint values. *)
type environment = (int, taintValue) Hashtbl.t

type statementsEnvironment = environment Inthash.t

type environmentStack = environment list

(* The function environment is a mapping between a function id and it's *)
(* computed environment. *)
type functionEnvironment = environment Inthash.t

(* Gamma mapping *)
module Gamma = struct
    
    let create_env () = 
        Hashtbl.create 1024
    
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
    
    (* Compares two environments. Returns true if envs are equal. *)    
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
                            g;
                        Format.fprintf fmt "%s" "\n";)
        in
        Format.fprintf fmt "%s\n" "========================================";
        Hashtbl.iter 
            (fun vid taint ->
                let vinfo = varinfo_from_vid vid in
                Format.fprintf fmt "\tSymname: %s\n" vinfo.vname;
                pretty_print_taint taint;
                Format.fprintf fmt "%s" "\n")
            env;
        Format.fprintf fmt "%s\n" "========================================"
    
    let pretty_print_taint fmt taint =
        match taint with
            | T -> Format.fprintf fmt "%s" "Tainted\n"
            | U -> Format.fprintf fmt "%s" "Untainted\n"
            | (G _) -> Format.fprintf fmt "%s" "Generic\n"

end
