(* Frama-C journal generated at 07:43 the 18/05/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.info.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.do-results.set" true in
 let () = Cmdline.Files.add "tests/real_tests/Queue/file2.c" in
 let () = Cmdline.Files.add "tests/real_tests/Queue/simul_file2.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)