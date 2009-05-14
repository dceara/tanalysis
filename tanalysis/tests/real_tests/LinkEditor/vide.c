/***********************************************************************
vide.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdlib.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "vide.h"
#include "section.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "videP.h"
#ifdef AVEC_LIBEDL
#include "_videI.h"
#endif
#else
#include "upcallI.h"
#endif

/**********************************************************************/
void cloreSectionVide(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   Elf_Data *descripteurContenu;

   /* elaboration descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_buf = NULL;
   /* FIN COMPLETER FUSION IMPLANTATION */
}
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerSectionVide(SectionEDL *sectionSource,
                          SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   unsigned int tailleSource;
   unsigned int tailleCible;

   tailleSource = sectionSource->tailleContenuElf;
   tailleCible = sectionCible->tailleContenuElf;

   tailleCible = tailleCible + tailleSource;

   sectionCible->tailleContenuElf = tailleCible;
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void etendreSectionVide(SectionEDL *section, unsigned int delta,
                        unsigned int align,
                        unsigned long *tailleIntermediaire) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   unsigned long tailleAlignee;

   tailleAlignee = alignement(section->tailleContenuElf, align);
   section->tailleContenuElf = tailleAlignee + delta;

   if (tailleIntermediaire != NULL) {
      *tailleIntermediaire = tailleAlignee;
   }
}
/* FIN SUPPRIMER FUSION */
