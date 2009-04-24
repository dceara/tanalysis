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
	taintGamma \
	taintTyping \
	taintPrinter \
	taintCFGPrinter \
	taintInstructionComputer \
	customTaintFlow sccCallgraph \
	taintFlow \
	scopeBuilder \
	taint 
	$(CC) $(PACKFLAGS) utils.cmo \
					   taintGamma.cmo \
					   taintTyping.cmo \
					   taintPrinter.cmo \
					   taintCFGPrinter.cmo \
					   taintInstructionComputer.cmo \
					   customTaintFlow.cmo \
					   sccCallgraph.cmo \
					   taintFlow.cmo \
					   scopeBuilder.cmo \
					   taint.cmo  

sccCallgraph: sccCallgraph.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

taintFlow: taintFlow.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

scopeBuilder: scopeBuilder.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
  
utils: utils.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

taintGamma: taintGamma.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
  
taintTyping: taintTyping.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  

taintPrinter: taintPrinter.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

taintCFGPrinter: taintCFGPrinter.ml
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


