open Cil_types
open Cilutil
open Cil

(* reunites two lists recursively *)
let rec reunite comparator l1 l2 =
    match List.length l1 with
        | 0 
            -> l2
        | _ -> 
            let fst = List.hd l1 in
            let last = List.tl l1 in
            try 
                ignore (List.find (comparator fst) l2);
                reunite (comparator) last l2
            with Not_found ->
                reunite (comparator) last (fst::l2)

let get_function_by_name globals fname =
    try
        let GFun (func, _) = (List.find 
                            (fun g -> 
                                match g with
                                    | GFun (func, _) -> 
                                        func.svar.vname == fname
                                    | _ -> false)
                            globals) in
        Some func
    with Not_found -> None

let get_fundec_by_id globals fid =
    let GFun (func, _) = List.find
                            (fun g ->
                                match g with
                                    | GFun (func, _) -> 
                                        func.svar.vid = fid
                                    | _ -> false)
                            globals in
    func
