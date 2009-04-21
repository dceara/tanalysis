
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
