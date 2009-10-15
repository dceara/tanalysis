open Cil
open Cil_types
open TaintGamma

class print vulnerable_statements = object(self)
    inherit defaultCilPrinterClass as super

    method pAnnotatedStmt next fmt s =
        match Inthash.mem vulnerable_statements s.sid with
        | false -> 
            super#pAnnotatedStmt next fmt s
        | true ->
            super#pGlobal fmt (GText "/* STAC: BAD */");
            super#pAnnotatedStmt next fmt s
end