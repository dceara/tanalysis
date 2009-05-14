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
        if Param.info then (
            P.print () "[INFO] Adding library function: %s\n" fname
        );
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
        if Param.info then (
            P.print () "[INFO] Added library function: %s\n" fname
        );
        Inthash.add (!Param.lib_func_hash_ref) funcdec.svar.vid funcdec

    let start () =
        add_function
            ("taint", TPtr (TVoid [], []), M_T)
            [("p1", TInt (IInt, []), M_U)];
        add_function 
            ("malloc", TPtr (TVoid [], []), M_G ["p1"]) 
            [("p1", TInt (IInt, []), M_U)];
        add_function 
            ("calloc", TPtr (TVoid [], []), M_G ["p1"; "p2"]) 
            [("p1", TInt (IInt, []), M_U);
             ("p2", TInt (IInt, []), M_U)];
        add_function 
            ("realloc", TPtr (TVoid [], []), M_G ["p1"; "p2"]) 
            [("p1", TPtr (TVoid [], []), M_G["p1"]);
             ("p2", TInt (IInt, []), M_U)];
        add_function
            ("feof", TInt (IInt, []), M_U)
            [("p1", TPtr (TVoid [], []), M_G ["p1"])];
        add_function
            ("printf", TInt (IInt, []), M_U)
            [("p_format", TVoid [], M_G ["p_format"])];
        add_function
            ("fprintf", TInt (IInt, []), M_U)
            [("stream", TPtr (TVoid [], []), M_G ["stream"]);
             ("p_format", TVoid [], M_G ["p_format"])];
        add_function
            ("fgetc", TInt (IInt, []), M_G ["stream"])
            [("stream", TPtr (TVoid [], []), M_G ["stream"])];
        add_function
            ("fputs", TInt (IInt, []), M_U)
            [("s", TPtr (TVoid [], []), M_G ["s"]);
             ("stream", TPtr (TVoid [], []), M_G ["stream"])];
        add_function
            ("free", TVoid [], M_U)
            [("ptr", TPtr( TVoid [], []), M_G ["ptr"])];
        add_function
            ("fputc", TInt (IInt, []), M_U)
            [("c", TInt (IChar, []), M_G ["c"]);
             ("stream", TPtr (TVoid [], []), M_G ["stream"])];
        (* TODO: handle variable argument count *)
        add_function
            ("fscanf", TInt (IInt, []), M_U)
            [("stream", TPtr (TVoid [], []), M_G ["stream"]);
             ("p_format", TVoid [], M_G ["p_format"]);
             ("p1", TPtr (TVoid [], []), M_T);
             ("p2", TPtr (TVoid [], []), M_T);
             ("p3", TPtr (TVoid [], []), M_T);
             ("p4", TPtr (TVoid [], []), M_T);
             ("p5", TPtr (TVoid [], []), M_T);
             ("p6", TPtr (TVoid [], []), M_T);
             ("p7", TPtr (TVoid [], []), M_T);
             ("p8", TPtr (TVoid [], []), M_T);
             ("p9", TPtr (TVoid [], []), M_T);
             ("p10", TPtr (TVoid [], []), M_T)];
        add_function
            ("sscanf", TInt (IInt, []), M_U)
            [("str", TPtr (TVoid [], []), M_G ["str"]);
             ("p_format", TVoid [], M_G ["p_format"]);
             ("p1", TPtr (TVoid [], []), M_T);
             ("p2", TPtr (TVoid [], []), M_T);
             ("p3", TPtr (TVoid [], []), M_T);
             ("p4", TPtr (TVoid [], []), M_T);
             ("p5", TPtr (TVoid [], []), M_T);
             ("p6", TPtr (TVoid [], []), M_T);
             ("p7", TPtr (TVoid [], []), M_T);
             ("p8", TPtr (TVoid [], []), M_T);
             ("p9", TPtr (TVoid [], []), M_T);
             ("p10", TPtr (TVoid [], []), M_T)];
        add_function
            ("fopen", TPtr (TVoid [], []), M_U)
            [("path", TPtr (TVoid [], []), M_G ["path"]);
             ("mode", TPtr (TVoid [], []), M_G ["mode"])];
        add_function
            ("freopen", TPtr (TVoid [], []), M_U)
            [("path", TPtr (TVoid [], []), M_G ["path"]);
             ("mode", TPtr (TVoid [], []), M_G ["mode"]);
             ("stream", TPtr (TVoid [], []), M_G ["stream"])];
        add_function
            ("exit", TVoid [], M_U)
            [("status", TInt (IInt, []), M_G ["status"])];
        add_function
            ("fclose", TInt (IInt, []), M_U)
            [("stream", TPtr (TVoid [], []), M_G ["stream"])];
        add_function
            ("strcmp", TInt (IInt, []), M_G ["s1";"s2"])
            [("s1", TPtr (TVoid [], []), M_G ["s1"]);
             ("s2", TPtr (TVoid [], []), M_G ["s2"])]; 
        add_function
            ("strncmp", TInt (IInt, []), M_G ["n"])
            [("s1", TPtr (TVoid [], []), M_G ["s1"]);
             ("s2", TPtr (TVoid [], []), M_G ["s2"]);
             ("n", TInt (IInt, []), M_U);];
        add_function
            ("strlen", TInt (IInt, []), M_G ["s1"])
            [("s1", TPtr (TVoid [], []), M_G ["s1"])]; 
        add_function
            ("strcpy", TPtr (TVoid [], []), M_G ["s2"])
            [("s1", TPtr (TVoid [], []), M_G ["s1"; "s2"]);
             ("s2", TPtr (TVoid [], []), M_G ["s2"])]; 
        add_function
            ("strncpy", TPtr (TVoid [], []), M_G ["n"])
            [("s1", TPtr (TVoid [], []), M_G ["s1"]);
             ("s2", TPtr (TVoid [], []), M_G ["s2"]);
             ("n", TInt (IInt, []), M_U)];
        add_function
            ("memcpy", TPtr (TVoid [], []), M_G ["n"])
            [("dest", TPtr (TVoid [], []), M_G["dest"]);
             ("src", TPtr (TVoid [], []), M_G["src"]);
             ("n", TInt (IInt, []), M_U)];
        add_function
            ("sprintf", TInt (IInt, []), M_U)
            [("dest", TPtr (TVoid [], []), M_G ["format"; "dest"]);
             ("format", TPtr (TVoid [], []), M_G ["format"])];
        add_function
            ("snprintf", TInt (IInt, []), M_U)
            [("dest", TPtr (TVoid [], []), M_G ["format"; "dest"; "size"]);
             ("size", TInt (IInt, []), M_U);
             ("format", TPtr (TVoid [], []), M_G ["format"])];     
        add_function
            ("strtol", TInt (IInt, []), M_G ["nptr"])
            [("nptr", TPtr (TVoid [], []), M_G ["nptr"]);
             ("endptr", TPtr (TVoid [], []), M_G ["endptr"]);
             ("base", TInt (IInt, []), M_U)];
        add_function
            ("perror", TVoid [], M_U)
            [("str", TPtr (TVoid [], []), M_G ["str"])];
        add_function
            ("pow", TFloat (FFloat, []), M_G ["p1"; "p2"])
            [("p1", TFloat (FFloat, []), M_U);
             ("p2", TFloat (FFloat, []), M_U)];
        add_function
            ("sqrt", TFloat (FFloat, []), M_G ["p1"])
            [("p1", TFloat (FFloat, []), M_U)];
        add_function
            ("acos", TFloat (FFloat, []), M_G ["p1"])
            [("p1", TFloat (FFloat, []), M_U)];
        add_function
            ("sin", TFloat (FFloat, []), M_G ["p1"])
            [("p1", TFloat (FFloat, []), M_U)];
        add_function
            ("abs", TInt (IInt, []), M_G ["p1"])
            [("p1", TInt (IInt, []), M_U)];
        add_function
            ("open", TInt (IInt, []), M_U)
            [("path", TPtr (TVoid [], []), M_G ["path"]);
             ("flags", TInt (IInt, []), M_U)]
             
end

let test format dbg inf config_fname func_envs glbls func_hash lib_func_hash =
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
    I.start ()
     