(* Frama-C journal generated at 05:57 the 04/05/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Cmdline.Files.add "tests/32.pointers_as_params_simple.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)