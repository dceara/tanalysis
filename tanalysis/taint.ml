open Cil_types
open Cilutil
open Cil
open Visitor
open SccCallgraph 
open Callgraph
open CustomTaintFlow

let option_enabled () = "taint-analysis.enabled"
let option_config_file () = "taint-analysis.config_file"

(* Register a new Frama-C option. *)
module Enabled =
    Cmdline.Dynamic.Register.False(struct let name = option_enabled () end)
        
(* Register config file sub option. *)
module ConfigFile =
    Cmdline.Dynamic.Register.EmptyString(struct let name = option_config_file () end)    

let get_config_name () =
    let config_file_option = option_config_file () in
         if Cmdline.Dynamic.Apply.String.is_set config_file_option then
            Cmdline.Dynamic.Apply.String.get config_file_option
         else
            "default.cfg"

let run_taint fmt globals =
    let computed_function_envs = Inthash.create 1024 in
    let (mappings, nodes, g, lst) = SccCallgraph.get_scc () in
    let get_function node = 
        let n = Hashtbl.find mappings node in
        let fname = nodeName n.cnInfo in
        try
            let GFun (func, _) = (List.find 
			                    (fun g -> 
			                        match g with
			                            | GFun (func, _) -> 
                                            func.svar.vname == fname
			                            | _ -> false)
			                    globals) in
            Some func
        with Not_found -> None
    in
    let rec next_func component =
        match SccCallgraph.get_next_call mappings nodes g component with
            | None -> ignore ()
            | Some (node, remaining) -> 
		        match get_function node with
		            | None -> next_func remaining
		            | Some func ->
                        let env = run_custom_taint fmt func computed_function_envs globals in
			                Inthash.add 
			                    computed_function_envs 
			                    func.svar.vid
                                env  
		                    
    in
    List.iter next_func lst;
    Format.fprintf fmt "%s\n" "Taint analysis done"             
    
    
    (* let fs = List.hd (globals.children) in
    match fs.func with
        | None -> ignore ()
        | Some func -> run_custom_taint fmt func (Inthash.create 1024) *)
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
let run fmt =
    if Enabled.get () then
        let file = Cil_state.file () in
        let globals = file.globals in 
        run_taint fmt globals
    
(* Extend the Frama-C command line. *)
let () =
      Options.add_plugin
       ~name:"taint-analysis" (* plug-in name *)
       ~descr:"Taint analysis plugin" (* plug-in description *)
       ["-taint-analysis", (* plug-in option *)
       Arg.Unit Enabled.on,
       ": Compute taint analysis";
       
       "-config-file",
       Arg.String (Cmdline.Dynamic.Apply.String.set (option_config_file () )),
       ": The config file used for setting the entry point and annotating functions";] 

(* Extend the Frama-C entry point (the "main" of Frama-C). *)
let () = Db.Main.extend run 
