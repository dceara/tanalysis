open Cil_types
open TaintGamma

type taintMetaValue = M_T | M_U | M_G of string list

module Initializer(Param:sig
                        val globals : global list
                        val func_hash_ref : ((string, fundec) Hashtbl.t) ref
                        val lib_func_hash_ref : fundec Inthash.t ref
                        val fmt : Format.formatter
                        val debug : bool      
                        val info : bool     
                        val config_file_name : string
                        val func_envs_ref : functionEnvironment ref
                   end) = struct

    module IC = TaintInstructionComputer.InstrComputer(struct
                                                            let globals = Param.globals
                                                            let func_hash = !Param.func_hash_ref
                                                            let fmt = Param.fmt
                                                            let debug = Param.debug
                                                            let info = Param.info
                                                        end)

    module P = TaintPrinter.Printer(struct
                                        let fmt = Param.fmt
                                        let debug = Param.debug
                                        let info = Param.info
                                    end)
    module Typing = TaintTyping.Typing(struct
                                         let fmt = Param.fmt
                                         let debug = Param.debug
                                         let info = Param.info
                                        end)
                                        
    let add_function (fname, ret_type, ret_taint) params =
        let get_param_meta_taint name =
            match  
	            List.find
	                (fun (pname, ptype, ptaint) -> 
	                    name == pname)
	                params with
            | (_, _, ptaint) -> ptaint
        in
        let get_taint funcdec meta_taint =
            let formals = funcdec.sformals in
            let get_formal_by_name name =
                List.find 
                    (fun formal -> 
                        formal.vname = name)
                    formals
            in
            match meta_taint with
                | M_T -> T
                | M_U -> U
                | (M_G m_g) 
                    ->
                        List.fold_left
                            (fun t vname -> 
                                Typing.combine_taint t (G [(get_formal_by_name vname)]))
                             U
                            m_g
        in
        P.print_info () "[INFO] Adding library function: %s\n" fname; 
        let param_types = List.map 
                            (fun (pname, ptype, ptaint) ->
                                (pname, ptype, []))
                            params in
        let param_types = 
            match List.length param_types with
                | 0 -> None
                | _ -> Some param_types in
        let fun_type = 
            TFun (ret_type, param_types, false, []) in         
        let funcdec = Cil.emptyFunction fname in
        Cil.setFunctionTypeMakeFormals funcdec fun_type;
        Hashtbl.add !Param.func_hash_ref funcdec.svar.vname funcdec;
        let env = IC.create_initial_env funcdec in
        Gamma.set_taint 
            env 
            (Typing.get_function_return_vid funcdec.svar)
            (get_taint funcdec ret_taint); 
        List.iter
            (fun formal ->
                Gamma.set_taint 
                    env 
                    formal.vid 
                    (get_taint funcdec (get_param_meta_taint formal.vname)))
            funcdec.sformals; 
        Inthash.add (!Param.func_envs_ref) funcdec.svar.vid (env, Inthash.create 1024);
        P.print_info () "[INFO] Added library function: %s\n" fname; 
        Inthash.add (!Param.lib_func_hash_ref) funcdec.svar.vid funcdec

    let init_from_file () =
        let strip_ws str =
            let str =
            try
                let index = String.index str '\n' in
                String.sub str 0 index
            with Not_found ->
                str in
            let str =
            try
                let index = String.index str '\r' in
                String.sub str 0 index
            with Not_found ->
                str in
            let str =
            try
                let index = String.index str ' ' in
                String.sub str 0 index
            with Not_found ->
                str in
            let str =
            try
                let index = String.index str '\t' in
                String.sub str 0 index
            with Not_found ->
                str in
            try
                let index = String.index str '#' in
                String.sub str 0 index
            with Not_found ->
                str
        in
        let read_str in_chan =
            let str = input_line in_chan in
            strip_ws str
        in
        let read_integer in_chan =
            let str = read_str in_chan in
            int_of_string str
        in
        let taint_from_str str =
            match str with
            | "U" -> M_U
            | "T" -> M_T
            | "G" -> M_G []
            | _ -> failwith "Invalid taint"
        in
        let type_from_str str =
            if str = "float" then TFloat (FFloat, [])
            else if str = "int" then TInt (IInt, [])
            else if str = "ptr" then TPtr (TVoid [], [])
            else if str = "void" then TVoid []
            else failwith "Invalid type"
        in
        let in_chan = open_in Param.config_file_name in
        let read_function () =
            ignore (read_str in_chan);
            ignore (read_str in_chan); (* +++++++++ *)
            let fname = read_str in_chan in
            let ret_type = type_from_str (read_str in_chan) in
            let ret_taint = ref (taint_from_str (read_str in_chan)) in
            let ret_deps_cnt = read_integer in_chan in
            for i = 1 to ret_deps_cnt do
                let ret_dep_name = read_str in_chan in
                match !ret_taint with
                | M_G l ->
                    ret_taint := M_G (ret_dep_name::l)
                | _ ->
                    failwith "Invalid taint for generic. RET_DEP"
            done;
            let param_cnt = read_integer in_chan in
            let params = ref [] in
            for i = 1 to param_cnt do
                ignore (read_str in_chan); (* ========= *)
                let param_name = read_str in_chan in
                let param_type = type_from_str (read_str in_chan) in
                let param_taint = ref (taint_from_str (read_str in_chan)) in
                let param_deps_cnt = read_integer in_chan in
                for j = 1 to param_deps_cnt do
                    let param_dep_name = read_str in_chan in
                    match !param_taint with
                    | M_G l ->
                        param_taint := M_G (param_dep_name::l)
                    | _ ->
                        failwith "Invalid taint for generic. PARAM_DEP"
                done;
                params := (param_name, param_type, !param_taint)::!params
            done;
            add_function
                (fname, ret_type, !ret_taint)
                !params
        in       
        
        let function_cnt = read_integer in_chan in
        for i = 1 to function_cnt do
            read_function ()
        done

end

let run format dbg inf config_fname func_envs glbls func_hash lib_func_hash =
    let module I = Initializer(struct 
                                let globals = glbls
                                let func_hash_ref = func_hash 
                                let lib_func_hash_ref = lib_func_hash
		                        let fmt = format
		                        let debug = dbg      
		                        let info = inf     
		                        let config_file_name = config_fname
		                        let func_envs_ref = func_envs
                               end) in
    I.init_from_file ()
    (* I.start () *)     