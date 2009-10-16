open Cil_types

(* The type of the detected vulnerability. *)
type vulnerability = FunctionConstraint of stmt * string | BufferIndex of stmt * string 