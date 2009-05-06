open Cil
open Cil_types
open TaintGamma

(* functionEnvironment *)
class print func_envs = object(self)
    inherit defaultCilPrinterClass as super

    method private pDifferences fmt stmt_envs env s =
        let old_envs = 
            List.map
                (fun pred -> Inthash.find stmt_envs pred.sid)
                s.preds in
        let result_env = 
            Gamma.get_differences env old_envs in
        super#pGlobal fmt (GText "");
        super#pGlobal fmt (GText (Format.sprintf "/*sid:%d*/" s.sid));
        Gamma.env_iter
            (fun vid taint ->
                let str_taint = Gamma.pretty_string_taint vid taint in
                super#pGlobal fmt (GText (Format.sprintf "/*%s*/" str_taint)))
            result_env;
       super#pGlobal fmt (GText "")

    method pAnnotatedStmt next fmt s =
        let current_func_vinfo = self#current_function in
        match current_func_vinfo with
            | None ->
                assert(false)
            | Some vinfo ->
		        let (_, stmt_envs) = Inthash.find func_envs vinfo.vid in
                let env = Inthash.find stmt_envs s.sid in
                super#pGlobal fmt (GText (Format.sprintf "/*sid:%d*/" s.sid));       
                super#pAnnotatedStmt next fmt s;
                self#pDifferences fmt stmt_envs env s
                
    initializer
        super#setPrintInstrTerminator ";\n"
    	
end