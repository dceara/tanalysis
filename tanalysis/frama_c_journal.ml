(* Frama-C journal generated at 16:06 the 07/06/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.min-taint-metric.set" true in
 let () = Cmdline.Files.add "tests/func_tests/49.read_metrics_basic_recursion.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)