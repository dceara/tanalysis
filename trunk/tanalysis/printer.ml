module Printer(Param:sig
                        val fmt : Format.formatter  
                     end) = struct
                        
    let print () = Format.fprintf Param.fmt
           
end
