FRAMAC_SHARE=$(shell frama-c -print-path)
PLUGIN_NAME=taint-analysis
CC=ocamlc
CFLAGS=-I $(FRAMAC_SHARE)/lib -c
PREFLAGS=-for-pack $(PLUGIN_NAME)
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
  vulnerableUtils \
	alias \
	taintPretty \
  vulnerablePretty \
  vulnerableVisitor \
	slicePretty \
	taintInstructionComputer \
	taintConfigHelper \
	customTaintFlow sccCallgraph \
	sccCallgraph \
	taintResults \
	metricComputer \
	minReadMetrics \
	maxReadMetrics \
	minTaintMetrics \
	maxTaintMetrics \
	metricPretty \
	taint 
	$(CC) $(PACKFLAGS) utils.cmo \
					   printer.cmo \
					   typeHelper.cmo \
					   taintGamma.cmo \
					   taintPrinter.cmo \
					   taintCFGPrinter.cmo \
					   taintTyping.cmo \
             vulnerableUtils.cmo \
					   alias.cmo \
					   taintPretty.cmo \
             vulnerablePretty.cmo \
             vulnerableVisitor.cmo \
					   slicePretty.cmo \
					   taintInstructionComputer.cmo \
					   taintConfigHelper.cmo \
					   customTaintFlow.cmo \
					   sccCallgraph.cmo \
					   taintResults.cmo \
					   metricComputer.cmo \
					   minReadMetrics.cmo \
					   maxReadMetrics.cmo \
					   minTaintMetrics.cmo \
					   maxTaintMetrics.cmo \
					   metricPretty.cmo \
					   taint.cmo 

xmlr: xmlr.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

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

vulnerableUtils: vulnerableUtils.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
  
alias: alias.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^	

taintPretty: taintPretty.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^	
	
slicePretty: slicePretty.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

vulnerablePretty: vulnerablePretty.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

vulnerableVisitor: vulnerableVisitor.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
  
taintInstructionComputer: taintInstructionComputer.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  
	
taintConfigHelper: taintConfigHelper.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  
	
customTaintFlow: customTaintFlow.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^  

taintResults: taintResults.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

metricComputer: metricComputer.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^

minReadMetrics: minReadMetrics.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
	
maxReadMetrics: maxReadMetrics.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
	
minTaintMetrics: minTaintMetrics.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
	
maxTaintMetrics: maxTaintMetrics.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
	
metricPretty: metricPretty.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
	
taint: taint.ml
	$(CC) $(PREFLAGS) $(CFLAGS) $^
    
install: build
	$(CP) $(PLUGIN_NAME).cmo $(FRAMAC_SHARE)/plugins 

uninstall:
	$(RM) $(FRAMAC_SHARE)/plugins/$(PLUGIN_NAME).cmo

clean:
	$(RM) *.cm* .depend frama_c_journal.ml


