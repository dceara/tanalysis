# linux | solaris | cygwin
PLATEFORME = linux

EDLBASEDIR = .

LIBELFDIR = $(EDLBASEDIR)
LIBELFINCDIR = $(LIBELFDIR)/include
LIBELFLIBDIR = $(LIBELFDIR)/lib/$(PLATEFORME)

EDLTESTDIR = $(EDLBASEDIR)/src/test

DEBUG = # -D DEBUG
GPROF = # -p
GCC = gcc $(GPROF) $(DEBUG)
CCOPTS = -g -Wall # -Werror

ARMELFDIR = /opt/gnu/arm
ARMELFBINDIR = $(ARMELFDIR)/bin

ARMELFAS = $(ARMELFBINDIR)/arm-elf-as
ARMELFGCC = $(ARMELFBINDIR)/arm-elf-gcc
ARMELFLD = $(ARMELFBINDIR)/arm-elf-ld

####################
# assemblage avec as
####################
# ARMELFASOPTS =                   # arm elf petit bout sans debug
# ARMELFASOPTS = -EB               # arm elf GROS BOUT sans debug
ARMELFASOPTS = -gdwarf2          # arm elf petit bout
# ARMELFASOPTS = -gdwarf2 -EB      # arm elf GROS BOUT

#####################
# assemblage avec gcc
#####################
# ARMELFASGCCOPTS =                            # arm elf petit bout sans debug
# ARMELFASGCCOPTS = -mbig-endian               # arm elf GROS BOUT sans debug
ARMELFASGCCOPTS = -Wa,-gdwarf2               # arm elf petit bout
# ARMELFASGCCOPTS = -mbig-endian -Wa,-gdwarf2  # arm elf GROS BOUT

######################
# compilation avec gcc
######################
# ARMELFGCCOPTS =                              # arm elf petit bout sans debug
# ARMELFGCCOPTS = -mbig-endian                 # arm elf GROS BOUT sans debug
ARMELFGCCOPTS = -g                           # arm elf petit bout
# ARMELFGCCOPTS = -g -mbig-endian              # arm elf GROS BOUT

##########################
# edition de liens avec ld
##########################
ARMELFLDOPTS =                   # arm elf petit bout
# ARMELFLDOPTS = -EB               # arm elf GROS BOUT

###########################
# edition de liens avec gcc
###########################
ARMELFLDGCCOPTS =                # arm elf petit bout
# ARMELFLDGCCOPTS = -mbig-endian   # arm elf GROS BOUT

##########################################
# commandes d'assemblage et de compilation
##########################################
VPATH = $(EDLTESTDIR)/edl/as:$(EDLTESTDIR)/edl/c:$(EDLTESTDIR)/divers

%.o: %.s
	$(ARMELFAS) $(ARMELFASOPTS) -o $@ $<

%.o: %.S
	$(ARMELFGCC) -c $(ARMELFGCCOPTS) -o $@ $<

essai_%.o: essai_%.c
	$(ARMELFGCC) -c $(ARMELFGCCOPTS) -o $@ $<

test_%.o: test_%.c
	$(GCC) -c $(CCOPTS) -I $(LIBELFINCDIR) $<

%.o: %.c %.h %I.h %P.h
	$(GCC) -c $(CCOPTS) -I $(LIBELFINCDIR) $<

%.o: %.c
	$(GCC) -c $(CCOPTS) -I $(LIBELFINCDIR) $<

CIBLE = test

all: $(CIBLE)

###################################
# fichiers binaires translatables
# des modules de l'editeur de liens
###################################
OBJETS = objet.o section.o chaines.o symboles.o \
         translations.o programme.o code.o vide.o autre.o iterateur.o \
			segments.o

############################################
# construction du programme editeur de liens
############################################
fusion: fusion.o $(OBJETS)
	$(GCC) -g -o $@ $^ -L $(LIBELFLIBDIR) -lelf

implantation: implantation.o $(OBJETS)
	$(GCC) -g -o $@ $^ -L $(LIBELFLIBDIR) -lelf

copie: copie.o $(OBJETS)
	$(GCC) -g -o $@ $^ -L $(LIBELFLIBDIR) -lelf

$(CIBLE): $(OBJETS)
	$(GCC) -g -o $@ $^ -L $(LIBELFLIBDIR) -lelf

######################################
# construction des objets du programme
######################################
objets: $(OBJETS)

########################################################################
# test fusion avec modules en langage d'assemblage
# deux fichiers de test au moins
########################################################################
OBJETS_TESTF_AS = fich1.o fich2.o

testfas: fusion $(OBJETS_TESTF_AS) .gdbinit
	$(ARMELFLD) $(ARMELFLDOPTS) -r -o resultat2.o $(OBJETS_TESTF_AS)
	./fusion -o resultat.o $(OBJETS_TESTF_AS)

########################################################################
# test fusion avec modules en langage C
# fusion par lui-meme du programme de fusion (architecture Intel =>
# l'ajustement des emplacements a completer n'est pas effectue)
########################################################################
OBJETS_TESTF_C = $(OBJETS)
# OBJETS_TESTF_C = common1.o common2.o

testfc: fusion $(OBJETS_TESTF_C) .gdbinit
	ld -r -o resultat2.o $(OBJETS_TESTF_C)
	./fusion -o resultat.o $(OBJETS_TESTF_C)

########################################################################
# test implantation avec modules en langage d'assemblage - un ou
# plusieurs fichiers de test dont un module principal (etiquette main)
########################################################################
OBJETS_TESTI_AS = fich3.o fich4.o
testias: TEXT_AS = 0x10000
testias: DATA_AS = 0x8000
testias: BSS_AS = 0x18000
testias: RODATA_AS = 0x28000
testias: START_AS = main
testias: DUP = # -x

testias: implantation $(OBJETS_TESTI_AS) .gdbinit
	$(ARMELFLD) $(ARMELFLDOPTS) -r -o resultat.o $(OBJETS_TESTI_AS)
	$(ARMELFLD) $(ARMELFLDOPTS) -o resultat2 resultat.o -e $(START_AS) \
	            -Ttext $(TEXT_AS) -Tdata $(DATA_AS) -Tbss $(BSS_AS)
	./implantation -o resultat resultat.o -e $(START_AS) -t $(TEXT_AS) \
	               -d $(DATA_AS) -r $(RODATA_AS) -b $(BSS_AS) $(DUP)

########################################################################
# test implantation avec modules en langage C
# deux fichiers de test au moins : crt0.o + 1 module principal
# (procedure main)
# - les variables globales externes non initialisees sont
#   interdites (sauf si les symboles communs ont ete traites)
# - certaines fonctions de la libc (printf par ex.) ne
#   s'exécutent pas toujours correctement s'il reste des symboles
#   communs dans l'executable
########################################################################
CRT0 = crt0.o
OBJETS_TESTI_C = $(CRT0) essai_printf.o
testic: TEXT_C = 0x08000
testic: DATA_C = 0x18000
testic: BSS_C = 0x20000
testic: RODATA_C = 0x28000
testic: START_C = start
testic: DUP = -x

testic: implantation $(OBJETS_TESTI_C) .gdbinit
	$(ARMELFGCC) $(ARMELFLDGCCOPTS) -r -o resultat.o $(OBJETS_TESTI_C) \
                                   -nostdlib -lc -lgcc
	$(ARMELFGCC) $(ARMELFLDGCCOPTS) -o resultat2 resultat.o \
	             -Ttext $(TEXT_C) -Tdata $(DATA_C) -Tbss $(BSS_C) \
					 -nostdlib
	./implantation -o resultat resultat.o -e $(START_C) $(DUP) \
	               -t $(TEXT_C) -d $(DATA_C) -r $(RODATA_C) -b $(BSS_C)

########################################################################
# test copie de module en langage d'assemblage
# un fichier de test uniquement
########################################################################
OBJETS_TESTC_AS = fich3.o fich4.o
testcas: DUP = # -x

testcas: copie $(OBJETS_TESTC_AS) .gdbinit
	$(ARMELFLD) $(ARMELFLDOPTS) -r -o resultat.o $(OBJETS_TESTI_AS)
	./copie -o resultat resultat.o $(DUP)

########################################################################
# test copie de module en langage C
# un fichier de test uniquement
# deux fichiers de test : crt0.o + 1 module principal
# (procedure main)
########################################################################
CRT0 = crt0.o
OBJETS_TESTC_C = $(CRT0) essai_printf.o
testcc: TEXT_C = 0x08000
testcc: DATA_C = 0x18000
testcc: BSS_C = 0x20000
testcc: DUP = -x

testcc: copie $(OBJETS_TESTC_C) .gdbinit
	$(ARMELFGCC) $(ARMELFLDGCCOPTS) -r -o resultat.o $(OBJETS_TESTC_C) \
                                   -nostdlib -lc -lgcc
	./copie -o resultat resultat.o $(DUP)

#######################
# initialisation de gdb
#######################
.gdbinit : gdbinit
	cp gdbinit .gdbinit

# ######################################
# construction graphes d'appels statique
# ######################################
graphefusion: cleanall
	sh graphestatique.sh fusion > graphestatique.txt
	sh grapheinverse.sh fusion > grapheinverse.txt
	a2ps -M A4 -1 -l 95 -o graphestatique.ps graphestatique.txt
	a2ps -M A4 -1 -l 95 -o grapheinverse.ps grapheinverse.txt

grapheimplantation: cleanall
	sh graphestatique.sh implantation > graphestatique.txt
	sh grapheinverse.sh implantation > grapheinverse.txt
	a2ps -M A4 -1 -l 95 -o graphestatique.ps graphestatique.txt
	a2ps -M A4 -1 -l 95 -o grapheinverse.ps grapheinverse.txt

###################
# impression des .h
###################
a2psh:
	a2ps -M A4 -A fill -o a2psh.ps \
	     objet.h objetI.h objetP.h \
	     section.h sectionI.h sectionP.h \
	     chaines.h chainesI.h chainesP.h \
	     symboles.h symbolesI.h symbolesP.h \
	     translations.h translationsI.h translationsP.h \
	     code.h codeI.h codeP.h \
	     vide.h videI.h videP.h \
	     autre.h autreI.h autreP.h \
	     segments.h segmentsI.h segmentsP.h \
	     iterateur.h iterateurI.h iterateurP.h

###################
# impression des .c
###################
a2psc:
	a2ps -M A4 -o a2psc.ps \
	     fusion.c implantation.c objet.c section.c chaines.c symboles.c \
	     translations.c code.c vide.c autre.c segments.c iterateur.c

###########
# nettoyage
###########
clean: cleantest
	/bin/rm -f implantation fusion copie test_* *.out *.exe \
              .gdb_history .gdbinit *.o *.log *.ps *~

cleantest:
	/bin/rm -f $(OBJETS_TESTI_AS) $(OBJETS_TESTI_C) \
              $(OBJETS_TESTF_AS) $(OBJETS_TESTF_C) \
              resultat.o resultat2.o resultat resultat2
