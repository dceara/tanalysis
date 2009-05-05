open Cil_types

module AliasAnalysis(Param:sig
                        val fmt : Format.formatter
                        val debug : bool
                        val info : bool
                    end) = struct
    
            
    module P = Printer.Printer(struct let fmt = Param.fmt end)

    let get_aliases vinfo =
        (if Param.info then 
            P.print () "[INFO] Returning alias for %s: %s.\n 
                        TODO: Not implemented yet.\n" vinfo.vname vinfo.vname);
        [vinfo]
end
 