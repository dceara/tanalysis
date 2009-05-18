open Cil_types
open Cilutil
open Cil
open Visitor
open SccCallgraph 
open Callgraph
open CustomTaintFlow
open TaintResults

let option_enabled () = "taint-analysis.enabled"
let option_print_intermediate () = "taint-analysis.print-intermediate"
let option_print_final () = "taint-analysis.print-final"
let option_print_source () = "taint-analysis.print-source"
let option_do_results () = "taint-analysis.do-results"
let option_config_file () = "taint-analysis.config_file"
let option_debugging () = "taint-analysis.debug"
let option_info () = "taint-analysis.info"

let default_config_file () = "default.cfg"

(* Register a new Frama-C option. *)
module Enabled =
    Cmdline.Dynamic.Register.False(struct let name = option_enabled () end)

(* Register a new Frama-C option. *)
module PrintIntermediateEnabled =
    Cmdline.Dynamic.Register.False(struct let name = option_print_intermediate () end)
    
(* Register a new Frama-C option. *)
module PrintFinalEnabled =
    Cmdline.Dynamic.Register.False(struct let name = option_print_final () end)
    
(* Register a new Frama-C option. *)
module PrintSourceEnabled =
    Cmdline.Dynamic.Register.False(struct let name = option_print_source () end)
    
(* Register a new Frama-C option. *)
module DoResultsEnabled =
    Cmdline.Dynamic.Register.False(struct let name = option_do_results () end)

module Debugging = 
    Cmdline.Dynamic.Register.False(struct let name = option_debugging () end)

module Info = 
    Cmdline.Dynamic.Register.False(struct let name = option_info () end)
                                                                                                                                                
(* Register config file sub option. *)
module ConfigFile =
    Cmdline.Dynamic.Register.EmptyString(struct let name = option_config_file () end)

let run_taint fmt debug info config_file_name globals =
    let module P = TaintPrinter.Printer(struct
                                        let fmt = fmt
                                    end) in   
    let computed_function_envs = ref (Inthash.create 1024) in
    let func_hash = Hashtbl.create 1024 in
    let lib_func_hash = Inthash.create 1024 in
    List.iter
        (fun global ->
            match global with
                | GFun (funcdec, _) -> Hashtbl.add func_hash funcdec.svar.vname funcdec
                | _ -> ignore ())
        globals;
    let intialize_library_calls () =
        TaintLibraryHelper.run 
            fmt 
            debug 
            info
            config_file_name 
            computed_function_envs
            globals
            (ref func_hash)
            (ref lib_func_hash)
    in    
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
                                func_hash
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
                        func_hash
                        globals
        in
        List.iter next_func lst;
    in
    let print_results enabled =
        if enabled then (
            Inthash.iter
                (fun id (env, _) ->
                    try
	                    let _ = Inthash.find lib_func_hash id in
	                        ignore ()
                    with Not_found ->
	                    let vinfo = varinfo_from_vid id in
			            P.print () "\nEnvironment for function %s:\n" vinfo.vname;
			            P.print_env () env)
            (!computed_function_envs)
        )
    in
    let print_source enabled =
        if enabled then 
            Cil.dumpFile (new TaintPretty.print !computed_function_envs) stdout "test" (Cil_state.file ())
    in
    let do_results enabled =
        if enabled then
             match get_results 
	                fmt 
	                debug 
	                info 
	                !computed_function_envs 
	                func_hash 
	                globals with
            | (stmt_count, taint_stmt_count) ->
                P.print () "STMT_COUNT: %d TAINT_STMT_COUNT: %d\n" stmt_count taint_stmt_count
    in
    
    intialize_library_calls ();
    perform_analysis (PrintIntermediateEnabled.get ());
    print_results (PrintFinalEnabled.get ());
    print_source (PrintSourceEnabled.get ());
    do_results (DoResultsEnabled.get ())
    

let run fmt =
    if Enabled.get () then
        let file = Cil_state.file () in
        let globals = file.globals in 
        let config_file_name = 
            match ConfigFile.is_set () with
                | true -> ConfigFile.get ()
                | false -> default_config_file () in
        run_taint fmt (Debugging.get ()) (Info.get ()) config_file_name globals
        
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
    
       "-print-final", (* plug-in option *)
       Arg.Unit PrintFinalEnabled.on,
       ": Print the final results for the taint analysis.";
    
       "-print-source", (* plug-in option *)
       Arg.Unit PrintSourceEnabled.on,
       ": Print the source file with comments about the taintedness of the variables.";
    
       "-do-results", (* plug-in option *)
       Arg.Unit DoResultsEnabled.on,
       ": Compute a percentage of the tainted instructions.";
    
       "-taint-debug", (* plug-in option *)
       Arg.Unit Debugging.on,
       ": Turn debugging ON."; 
    
       "-taint-info", (* plug-in option *)
       Arg.Unit Info.on,
       ": Turn info ON.";
    
       "-config-file",
       Arg.String (Cmdline.Dynamic.Apply.String.set (option_config_file () )),
       ": The config file used for setting the entry point and annotating functions";] 

(* Extend the Frama-C entry point (the "main" of Frama-C). *)
let () = Db.Main.extend run 
