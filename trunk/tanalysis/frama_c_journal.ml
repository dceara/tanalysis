(* Frama-C journal generated at 11:28 the 11/05/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.print-final.set" true in
 let () = Cmdline.Files.add "tests/real_tests/ArmAssembler/AnalyseSynt.c" in
 let () = Cmdline.Files.add "tests/real_tests/ArmAssembler/MachineLex.c" in
 let () = Cmdline.Files.add "tests/real_tests/ArmAssembler/Phase1.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)