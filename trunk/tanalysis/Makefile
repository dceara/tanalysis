FRAMAC_SHARE=$(shell frama-c -print-path)
PLUGIN_NAME=taint-analysis
CC=ocamlc
CFLAGS=-I $(FRAMAC_SHARE)/lib
PREFLAGS=-for-pack $(PLUGIN_NAME) -c
PACKFLAGS=-pack -o $(PLUGIN_NAME).cmo
CP=cp -f
RM=rm -f

build: pack

pack: utils \
    printer \
    typeHelper \
	taintGamma \
	taintPrinter \
    taintCFGPrinter \
	taintTyping \
	alias \
	taintInstructionComputer \
	customTaintFlow sccCallgraph \
	sccCallgraph \
	taint 
	$(CC) $(PACKFLAGS) utils.cmo \
	                   printer.cmo \
	                   typeHelper.cmo \
					   taintGamma.cmo \
                       taintPrinter.cmo \
					   taintCFGPrinter.cmo \
					   taintTyping.cmo \
					   alias.cmo \
					   taintInstructionComputer.cmo \
					   customTaintFlow.cmo \
					   sccCallgraph.cmo \
					   taint.cmo  

sccCallgraph: sccCallgraph.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
  
utils: utils.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

printer: printer.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
	
typeHelper: typeHelper.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^	
	
taintGamma: taintGamma.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
  
taintTyping: taintTyping.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  

taintPrinter: taintPrinter.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

taintCFGPrinter: taintCFGPrinter.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

alias: alias.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^	
	
taintInstructionComputer: taintInstructionComputer.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  
	
customTaintFlow: customTaintFlow.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  
	  
taint: taint.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
    
install: build
	$(CP) $(PLUGIN_NAME).cmo $(FRAMAC_SHARE)/plugins 

uninstall:
	$(RM) $(FRAMAC_SHARE)/plugins/$(PLUGIN_NAME).cmo

clean:
	$(RM) *.cm* .depend frama_c_journal.ml


