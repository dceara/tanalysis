open TaintGamma

module Printer(Param:sig
                        val fmt : Format.formatter  
                     end) = struct
                        
    let print () = Format.fprintf Param.fmt
    
    let print_taint () = Gamma.pretty_print_taint Param.fmt
    
    let print_env () = Gamma.pretty_print Param.fmt

end
