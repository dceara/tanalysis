open Cil_types
open Cilutil
open Cil
open Visitor

type scope = {
        mutable func : fundec option;
        mutable parent : scope option;
        mutable vars : varinfo list;
        mutable children : scope list;
    }

module Scope = 
struct 
    
    let get_in_current s name =
        if List.exists (fun e -> e.vname = name) s.vars then
            Some (List.find (fun e -> e.vname = name) s.vars)
        else
            None
            
    let rec get_var_by_name s name =
        match s with
        | None 
        -> 
            None
        | Some ss 
        -> 
            if List.exists (fun e -> e.vname = name) ss.vars then
                Some (List.find (fun e -> e.vname = name) ss.vars)
            else
                get_var_by_name ss.parent name
    
    let add_var s var =
        let v = get_in_current s var.vname in
        match v with
            | None -> s.vars <- var :: s.vars 
            | Some _ -> ignore ()
    
    let is_global s =
        s.parent = None
    
    let create_scope f p =
        let s = { func = f; parent = p; vars = []; children = [];} in
        (match p with
            | None -> ignore()
            | Some ps -> ps.children <- s :: ps.children);
        s
        
    let iter_current_scope s f =
        List.iter f s.vars
        
    let iter_children_scopes s f =
        List.iter f s.children
    
end

class visitor(fmt) = object (self)

    inherit Visitor.generic_frama_c_visitor 
        (Project.current ()) (inplace_visit ()) as super

    val mutable global = Scope.create_scope None None
    
    method get_global =
        global
    
    method vglob globdec =
        (match globdec with
        | GVarDecl (_, vinfo, _) (* may be a function proto or var decl *)
        ->
           (match vinfo.vtype with
            | TFun (_, _, _, _)
            -> ignore () 
            | _
            -> Scope.add_var global vinfo)
        | GVar (vinfo, initinfo, _)
        -> Scope.add_var global vinfo
        | _ 
        -> ignore ());
        DoChildren
    
    method vfunc funcdec =
        let var_list = funcdec.slocals in
        let s = (Scope.create_scope (Some funcdec) (Some global)) in
        List.iter (fun v -> Scope.add_var s v) var_list;
        DoChildren
end
