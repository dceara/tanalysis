/***********************************************************************
code.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "code.h"
#include "section.h"
#include "programme.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "codeP.h"
#ifdef AVEC_LIBEDL
#include "_codeI.h"
#endif
#else
#include "upcallI.h"
#endif

/**********************************************************************/
void creerSectionCode(SectionEDL *section, unsigned int tailleMax) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   /* allocation de la memoire pour le contenu au format ELF */
   section->contenuElf = calloc(tailleMax, 1);
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void cloreSectionCode(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   Elf_Data *descripteurContenu;

   /* elaboration descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_buf = section->contenuElf;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void fermerSectionCode(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   /* liberation du contenu au format ELF */
   if (section->objetParent->mode == ELF_C_WRITE
       && section->parentInitial == NULL) {
      if (section->contenuElf != NULL) {
         free(section->contenuElf);
         section->contenuElf = NULL;
      }
   }
}
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerSectionCode(SectionEDL *sectionSource,
                          SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   unsigned int tailleSource;
   unsigned int tailleCible;
   char *contenuSource;
   char *contenuCible;
   unsigned int i;

   tailleSource = sectionSource->tailleContenuElf;
   tailleCible = sectionCible->tailleContenuElf;
   contenuSource = (char *)sectionSource->contenuElf;
   contenuCible = (char *)sectionCible->contenuElf;

   for (i = 0 ; i < tailleSource ; i++) {
      contenuCible[tailleCible + i] = contenuSource[i];
   }
   tailleCible += tailleSource;

   sectionCible->tailleContenuElf = tailleCible;
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */
