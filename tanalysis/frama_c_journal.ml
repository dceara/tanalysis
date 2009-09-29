(* Frama-C journal generated at 08:11 the 29/09/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)