open Cil_types
open Cil
open Db_types
open Db
open Cilutil

module TaintBlockData = struct
     type data = Value of Cilutil.StmtSet.t | Top

  let hash = function Top -> 222222 | Value s -> Cilutil.StmtSet.cardinal s

  let pretty fmt d =
    match d with
    | Top ->
	Format.fprintf fmt "Top"
    | Value d ->
	Format.fprintf fmt "{%a}"
          (Cil.fprintfList ~sep:","
	     (fun fmt s -> Format.fprintf fmt "%d" s.sid))
          (StmtSet.elements d)

  let inter a b = match a,b with
    | Top,Top -> Top
    | Value v, Top | Top, Value v -> Value v
    | Value v, Value v' -> Value (StmtSet.inter v v')

  let equal a b = match a,b with
    | Top,Top -> true
    | Value _v, Top | Top, Value _v -> false
    | Value v, Value v' -> StmtSet.equal v v'

  let add v d = match d with
    | Top -> Top
    | Value d -> Value (StmtSet.add v d)

  let mem v = function
    | Top -> true
    | Value d -> StmtSet.mem v d

  let map f = function
    | Top -> Top
    | Value set -> Value (f set)
end

module Datatype = struct
  include Project.Datatype.Register
    (struct
       type t = TaintBlockData.data
       let map = TaintBlockData.map
       let copy = map Cil_datatype.StmtSet.copy
       let rehash = map Cil_datatype.StmtSet.rehash
       let name = "taint-datatype"
     end)
  let () = register_comparable ~hash ~equal ()
end


module TaintBlockStartData = Cil_computation.IntHashtbl
    (Datatype)
    (struct
       let name = "taint-start"
       let dependencies = [ Cil_state.self ]
       let size = 503
     end)

module TaintComputer = struct

  let name = "dominators"
  let debug = ref true
  type t = TaintBlockData.data
  module StmtStartData = TaintBlockStartData
  let pretty = TaintBlockData.pretty
  let copy (s:t) = s

  let computeFirstPredecessor _stmt _set = assert false

  let combinePredecessors stmt ~old new_ =
    let new_ = TaintBlockData.add stmt new_ in
    let set = TaintBlockData.inter old new_ in
    if TaintBlockData.equal set old then None else Some set

  let doStmt _stmt _set =  
    Dataflow.SDefault

  let doInstr stmt _instr set =
    Dataflow.Done (TaintBlockData.add stmt set)

  let stmt_can_reach _ _ = true
  let filterStmt _stmt = true
  let doGuard _ _ _ = Dataflow.GDefault
end

module TaintCompute = Dataflow.ForwardsDataFlow(TaintComputer)

let compute_taint funcdec =
  let start = List.hd funcdec.sallstmts in  
  try 
    let _ = TaintBlockStartData.find start.sid in
        Printf.printf "[dominators1] computed for function %s"
        funcdec.svar.vname;
      Cil.log "[dominators] computed for function %s"
        funcdec.svar.vname;
  with Not_found ->
    Printf.printf "[dominators2] computing for function %s" funcdec.svar.vname;
    Cil.log "[dominators] computing for function %s"
      funcdec.svar.vname;
    let f = funcdec in
    let stmts = f.sallstmts in
      List.iter (fun s -> TaintBlockStartData.add s.sid TaintBlockData.Top) stmts;
      TaintBlockStartData.replace start.sid (TaintBlockData.Value (StmtSet.singleton start));
      TaintCompute.compute [start];
      Printf.printf "[dominators3] done for function %s" funcdec.svar.vname;  
      Cil.log "[dominators] done for function %s"
        funcdec.svar.vname
