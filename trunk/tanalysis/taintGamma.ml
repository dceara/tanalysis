open Cil_types

type taintValue = T | U | G of varinfo list

type formalParam = Formal of varinfo

type actualParam = Actual of varinfo

type paramBinding = Param of formalParam * actualParam

type environment = (varinfo, taintValue) Hashtbl.t

type functionEnvironment = (varinfo, environment) Hashtbl.t

(* Gamma mapping *)
module Gamma = struct
    
    let create_env () = 
        Hashtbl.create 1000
    
    (* Returns the taint value or the delayed taint value if found, raises Not_found otherwise *)
    let get_taint env vinfo =
        Hashtbl.find env vinfo.vid
            
    let set_taint env vinfo taint =
        (try 
            ignore (Hashtbl.find env vinfo.vid);
            Hashtbl.remove env vinfo.vid
        with Not_found ->
            ignore ());
        Hashtbl.add env vinfo.vid taint

    (* Function for pretty printing an environment. Should be used for debugging *)
    (* purposes only. *)
    let pretty_print env =
        let pretty_print_taint taint = 
            (match taint with
                | T -> Printf.printf "\t%s\n" "Tainted"
                | U -> Printf.printf "\t%s\n" "Untainted"
                | (G g) 
                    -> 
                        Printf.printf "\t%s" "Generic: ";
                        List.iter 
                            (fun el -> Printf.printf "Gamma(%s), " el.vname)
                            g)
        in
        Printf.printf "%s\n" "========================================";
        Hashtbl.iter 
            (fun vinfo taint ->
                Printf.printf "\tSymname: %s\n" vinfo.vname;
                pretty_print_taint taint;
                Printf.printf "%s" "\n")
            env;
        Printf.printf "%s\n" "========================================";
end
