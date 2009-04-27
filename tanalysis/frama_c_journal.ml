(* Frama-C journal generated at 08:03 the 27/04/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Cmdline.PrintShare.set true in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)