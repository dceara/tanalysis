open Cil_types
open Cilutil
open Cil
open Graph
open Callgraph

type call_type = {
    fname : string;
} 
 
module G = Imperative.Digraph.Abstract(struct type t = call_type end)

let init_g cg =
    let g = G.create () in 
    let (mappings, nodes) = 
        Hashtbl.fold
            (fun key cn (result_m, result_n) ->
                let n = {fname = nodeName cn.cnInfo;} in
                let v = G.V.create n in
                G.add_vertex g v;
                Hashtbl.add result_n n.fname v;
                Hashtbl.add result_m v n;
                (result_m, result_n))    
            cg 
            (Hashtbl.create 512, Hashtbl.create 512) in
    Hashtbl.iter
        (fun key cn ->
            let name = nodeName cn.cnInfo in
            let n1 = Hashtbl.find nodes name in
            Inthash.iter
                (fun key cn_callee ->
                    let callee = nodeName cn_callee.cnInfo in
                    let n2 = Hashtbl.find nodes callee in
                    G.add_edge g n1 n2
                    )
                cn.cnCallees
        ) cg;
    (mappings, nodes, g)

(* Returns (mappings, nodes, G.V.t list array in topological order.*)
(* nodes = (func_name, node) *)
(* mappings = (node, func_name *)
let scc cg =
    let (mappings, nodes, g) = init_g cg in
    let module SCC = Components.Make(G) in
    (mappings, nodes, SCC.scc_array g)

let top_iter cg = 
    let (mappings, nodes, g) = init_g cg in
    let module TOP = Topological.Make(G) in
    TOP.iter 
        (fun n ->
            let nn = Hashtbl.find mappings n in
            Printf.printf "debug = %s\n" nn.fname)
            g