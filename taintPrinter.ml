open TaintGamma

module Printer(Param:sig
                        val fmt : Format.formatter  
                     end) = struct
    
    include Printer.Printer(struct 
                             let fmt = Param.fmt
                            end)
    
    let print_taint () = Gamma.pretty_print_taint Param.fmt
    
    let print_env () = Gamma.pretty_print Param.fmt
    
    let print_taint_list () = Gamma.pretty_print_taint_list Param.fmt

end
