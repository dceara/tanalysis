(* Frama-C journal generated at 07:39 the 18/05/2009 *)

(* Running *)
let start () =
 let () = Journal.run () in
 let () = Cmdline.CppCommand.set "gcc -E -C -I. -I./include" in
 let () = Journal.apply "Cmdline.taint-analysis.do-results.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.enabled.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.info.set" true in
 let () = Journal.apply "Cmdline.taint-analysis.config_file.set" "../../../default.cfg" in
 let () = Cmdline.Files.add "autre.c" in
 let () = Cmdline.Files.add "chaines.c" in
 let () = Cmdline.Files.add "code.c" in
 let () = Cmdline.Files.add "fusion.c" in
 let () = Cmdline.Files.add "implantation.c" in
 let () = Cmdline.Files.add "iterateur.c" in
 let () = Cmdline.Files.add "objet.c" in
 let () = Cmdline.Files.add "programme.c" in
 let () = Cmdline.Files.add "section.c" in
 let () = Cmdline.Files.add "segments.c" in
 let () = Cmdline.Files.add "symboles.c" in
 let () = Cmdline.Files.add "translations.c" in
 let () = Cmdline.Files.add "vide.c" in
 let () = Cmdline.Files.add "wrapper.c" in
 let () = File.init_from_cmdline () in
 (* Finished *)
 Journal.finished ()

let () =
 try start ()
 with e -> Format.eprintf "Journal raised an exception: %s" (Printexc.to_string e)