open Cil
open Cil_types
open Lexing
open VulnerableUtils

class visitor(vulnerable_statements, fmt, debug, info) = object (self)
  inherit Visitor.generic_frama_c_visitor 
        (Project.current ()) (inplace_visit ()) as super
  
  method private pVulnerability s =  
      let vulnerability = Inthash.find vulnerable_statements s.sid in
      let _pVulnerability (loc, _) =
        let module P = Printer.Printer(struct
                                  let fmt = fmt
                                  let debug = debug
                                  let info = info
                                end) in
        match vulnerability with
        | FunctionConstraint fname 
            -> P.print () 
                  "[%s:%i] Vulnerability: Function constraint doesn't hold for function '%s'\n" 
                  loc.pos_fname 
                  loc.pos_lnum fname
        | BufferIndex bufname
            -> P.print () 
                  "[%s:%i] Vulnerability: Buffer write with tainted index for buffer '%s'\n" 
                  loc.pos_fname 
                  loc.pos_lnum
                  bufname
      in
      match s.skind with
      | Instr i ->
        (match i with
        | Set (_, _, loc) -> _pVulnerability loc
        | Call (_, _, _, loc) -> _pVulnerability loc
        | _ -> ignore ()
        )
      | _ -> ignore ()
  
  method vstmt s =
    (match Inthash.mem vulnerable_statements s.sid with
    | true -> self#pVulnerability s
    | false -> ignore ());
    DoChildren

end
