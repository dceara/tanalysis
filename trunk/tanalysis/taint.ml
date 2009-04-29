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
    let computed_function_envs = ref (Inthash.create 1024) in
    let (mappings, nodes, g, lst) = SccCallgraph.get_scc () in
    Printf.printf "%s\n" "[DEBUG] dbg1";
    let get_function node = 
        let n = Hashtbl.find mappings node in
        let fname = nodeName n.cnInfo in
        Utils.get_function_by_name globals fname
    in
    let rec next_func component =
        Printf.printf "%s\n" "[DEBUG] dbg2";
        match SccCallgraph.get_next_call mappings nodes g component with
            | FuncNone -> ignore ()
            | FuncNonRecursive (node, remaining) -> 
		        (match get_function node with
		            | None -> next_func remaining
		            | Some func ->
                        Printf.printf "[DEBUG] fname = %s\n" func.svar.vname;
                        run_custom_taint_non_recursive fmt func computed_function_envs globals)
            | FuncRecursive node_list -> 
                let func_list = List.fold_left 
                                    (fun res node 
                                        -> 
                                            match get_function node with
                                                | None -> res
                                                | Some func -> List.concat [res;[func]]) 
                                    []
                                    node_list in
                run_custom_taint_recursive fmt func_list computed_function_envs globals
    in
    List.iter next_func lst;
    Format.fprintf fmt "%s\n" "Taint analysis done"
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        
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
