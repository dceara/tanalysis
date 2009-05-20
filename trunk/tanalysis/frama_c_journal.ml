(* Frama-C journal generated at 11:23 the 19/05/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.print-source.set" true in
 let () = Cmdline.Files.add "tests/real_tests/PicEditor/t1_serial.c" in
 let () = Cmdline.Files.add "tests/real_tests/PicEditor/common.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)