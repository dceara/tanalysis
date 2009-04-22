open Cil_types
open Cil
open Db_types
open Db
open Cilutil
open TaintGamma
open TaintTyping

(* obsolete *)
(*
type dependencyList = string list (* TODO add real dependency list types *)
*)

(* obsolete *)
(*
type taintLatticeValue = T | U | G of dependencyList 
*)

(* obsolete *)
(*
type taintLattice = Value of taintLatticeValue Inthash.t
*)

(* obsolete *)
(*
type statementsHash = SH of taintLattice Inthash.t
*)

(* obsolete *)
(*
(* TODO must combine dependencies for real and remove duplicates *)
let combine_dependency_lists d1 d2 =
    List.append d1 d2 
*)

(* obsolete *)
(* let compute_new_taint v1 v2 =
    match (v1, v2) with 
        | T, _
        | _, T -> T
        | (G deps) , U -> (G deps) 
        | U, (G deps) -> (G deps)
        | U, U -> U
        | (G deps1), (G deps2) -> (G (combine_dependency_lists deps1 deps2)) *)
             

let compute_initial_taint func =
    let env = Gamma.create_env () in
    List.iter
        (fun formal
            -> Gamma.set_taint env formal.vid (G [formal]))
        func.sformals;
    List.iter
        (fun local
            -> Gamma.set_taint env local.vid T)
        func.slocals;
    env

(* TODO: implement this function *)
let test_and_compute_new_taint old new_ =
    (false, new_)

(* returns true if the values are new *)
(* obsolete *)    
(* let test_and_compute_new_taint (Value old) (Value new_) =
    (Inthash.fold 
        (fun id v result ->
            let v2 = Inthash.find new_ id in
            let new_val = compute_new_taint v v2 in
            Inthash.replace new_ id new_val;
            if (v2 != v) then
                true
            else
                result)
        old false, new_) *)
  
(* obsolete *)  
module TaintComputer(Param:sig
	                    val current_values : statementsEnvironment
                        val idom_states_stack : environmentStack
	                 end) = struct

    let name = "taint-computer"
    let debug = ref true
    type t = environment
    module StmtStartData = struct
        type data = environment
	    
	    let values = 
            Param.current_values
        
        let clear () = Inthash.clear values
	    let mem = Inthash.mem values 
        let find = Inthash.find values
	    let replace = Inthash.replace values
	    let add = Inthash.add values
	    let iter f = Inthash.iter f values            
    end 

    let pretty fmt state = 
        Format.fprintf fmt "%s\n" "Printing current dataflow state";
        Gamma.pretty_print fmt state

    let copy (s:t) = s

    let computeFirstPredecessor _ values = values

    (** Combine an old values hash with a new one at the point before 's'.
        Simply 'join' the two states.
        Return None if the new state is already included in the old one
        to stop processing (fix point reached).
        TODO: we also have to combine the values from the immediate dominator
        for the two blocks (i.e. for if statements)
    *)
    let combinePredecessors stmt ~old new_ =
        let is_new, new_values = test_and_compute_new_taint old new_ in
        if is_new then (
            Some new_values
        ) else (
            (* debug is a reference.. that's why we use ! *)
            (if !debug then
                Format.printf "[taint-analysis] fixed point reached for stmt %d@." stmt.sid); 
            None
        )

    let doStmt _stmt _lattice =  
        Dataflow.SDefault

    let doInstr stmt _instr lattice =
        Dataflow.Done (lattice)

    let stmt_can_reach _ _ = true
    let filterStmt _stmt = true
    let doGuard _ _ _ = Dataflow.GDefault
end

(* Computes the taint analysis for a given function with regard to already *)
(* computed functions and library functions. *)
(* Params: *)
(* func - the fundec for the requested function *)
(* envs - the Hastbl of previously computed function environments *)
let compute_taint func envs =
    let module TaintComputer = TaintComputer(struct
                                                let current_values = (Inthash.create 512)
                                                let idom_states_stack = []
                                            end) in
    let module TaintCompute = Dataflow.ForwardsDataFlow(TaintComputer) in
    let module StartData = TaintComputer.StmtStartData in
    let start = List.hd func.sallstmts in
    Printf.printf "[taint-analysis] computing for function %s\n" func.svar.vname;
    let initial_var_taint = compute_initial_taint func in
    let stmts = func.sallstmts in
        List.iter 
            (fun s -> Inthash.add
                        StartData.values
                        s.sid
                        (Hashtbl.copy initial_var_taint))
            stmts;
        TaintCompute.compute [start];
        Printf.printf "[taint-analysis] done for function %s\n" func.svar.vname

