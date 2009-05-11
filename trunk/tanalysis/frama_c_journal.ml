(* Frama-C journal generated at 09:41 the 11/05/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.print-final.set" true in
 let () = Cmdline.Files.add "tests/real_tests/huffman/arbre.c" in
 let () = Cmdline.Files.add "tests/real_tests/huffman/bfile.c" in
 let () = Cmdline.Files.add "tests/real_tests/huffman/fap.c" in
 let () = Cmdline.Files.add "tests/real_tests/huffman/Occ.c" in
 let () = Cmdline.Files.add "tests/real_tests/huffman/prog6.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)