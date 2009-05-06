open Cil_types
open Cilutil
open Cil
open Visitor
open SccCallgraph 
open Callgraph
open CustomTaintFlow

let option_enabled () = "taint-analysis.enabled"
let option_print_intermediate () = "taint-analysis.print-intermediate"
let option_print_source () = "taint-analysis.print-source"
let option_config_file () = "taint-analysis.config_file"

(* Register a new Frama-C option. *)
module Enabled =
    Cmdline.Dynamic.Register.False(struct let name = option_enabled () end)

(* Register a new Frama-C option. *)
module PrintIntermediateEnabled =
    Cmdline.Dynamic.Register.False(struct let name = option_print_intermediate () end)
    
(* Register a new Frama-C option. *)
module PrintSourceEnabled =
    Cmdline.Dynamic.Register.False(struct let name = option_print_source () end)
                        
(* Register config file sub option. *)
module ConfigFile =
    Cmdline.Dynamic.Register.EmptyString(struct let name = option_config_file () end)    

let get_config_name () =
    let config_file_option = option_config_file () in
         if Cmdline.Dynamic.Apply.String.is_set config_file_option then
            Cmdline.Dynamic.Apply.String.get config_file_option
         else
            "default.cfg"

let run_taint fmt debug info globals =
    
    let computed_function_envs = ref (Inthash.create 1024) in
    let perform_analysis print_intermediate =
	    let (mappings, nodes, g, lst) = SccCallgraph.get_scc () in
	    let get_function node = 
	        let n = Hashtbl.find mappings node in
	        let fname = nodeName n.cnInfo in
	        Utils.get_function_by_name globals fname
	    in
	    let rec next_func component =
	        match SccCallgraph.get_next_call mappings nodes g component with
	            | FuncNone -> ignore ()
	            | FuncNonRecursive (node, remaining) -> 
	                (match get_function node with
	                    | None -> next_func remaining
	                    | Some func ->
	                        run_custom_taint_non_recursive 
                                fmt 
                                debug 
                                info
                                print_intermediate 
                                func 
                                computed_function_envs 
                                globals)
	            | FuncRecursive node_list -> 
	                let func_list = List.fold_left 
	                                    (fun res node 
	                                        -> 
	                                            match get_function node with
	                                                | None -> res
	                                                | Some func -> List.concat [res;[func]]) 
	                                    []
	                                    node_list in
	                run_custom_taint_recursive 
                        fmt 
                        debug 
                        info 
                        print_intermediate
                        func_list 
                        computed_function_envs 
                        globals
	    in
	    List.iter next_func lst;
    in
    let print_results enabled =
        Format.fprintf fmt "\n\n%s\n" "Taint analysis done";
        if enabled then 
		    Cil.dumpFile (new TaintPretty.print !computed_function_envs) stdout "test" (Cil_state.file ())
    in
    
    perform_analysis (PrintIntermediateEnabled.get ());
    print_results (PrintSourceEnabled.get ())
    

let run fmt =
    if Enabled.get () then
        let file = Cil_state.file () in
        let globals = file.globals in 
        run_taint fmt false false globals
    
(* Extend the Frama-C command line. *)
let () =
      Options.add_plugin
       ~name:"taint-analysis" (* plug-in name *)
       ~descr:"Taint analysis plugin" (* plug-in description *)
       ["-taint-analysis", (* plug-in option *)
       Arg.Unit Enabled.on,
       ": Compute taint analysis.";
       
       "-print-intermediate", (* plug-in option *)
       Arg.Unit PrintIntermediateEnabled.on,
       ": Print the intermediate results for the taint analysis.";
    
       "-print-source", (* plug-in option *)
       Arg.Unit PrintSourceEnabled.on,
       ": Print the source file with comments about the taintedness of the variables.";
    
       "-config-file",
       Arg.String (Cmdline.Dynamic.Apply.String.set (option_config_file () )),
       ": The config file used for setting the entry point and annotating functions";] 

(* Extend the Frama-C entry point (the "main" of Frama-C). *)
let () = Db.Main.extend run 
