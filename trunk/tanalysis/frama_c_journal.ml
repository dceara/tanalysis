(* Frama-C journal generated at 09:11 the 22/07/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.print-final.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.main-function.set" "main" in
 let () = Cmdline.Files.add "tests/func_tests/52.main_arguments.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)