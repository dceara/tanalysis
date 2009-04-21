open Cil_types

type taintValue = T | U | G of varinfo list

type formalParam = Formal of varinfo

type actualParam = Actual of varinfo

type paramBinding = Param of formalParam * actualParam

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
        
end

(* Gamma lib mapping *)
module GammaLib = struct
    include Gamma
end

