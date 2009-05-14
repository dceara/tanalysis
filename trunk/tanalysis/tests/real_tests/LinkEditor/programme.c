/***********************************************************************
programme.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdlib.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "programme.h"
#include "section.h"
#include "code.h"
#include "vide.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "programmeP.h"
#ifdef AVEC_LIBEDL
#include "_programmeI.h"
#endif
#else
#include "upcallI.h"
#endif

/**********************************************************************/
void ouvrirSectionProgramme(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->sectionProgramme =
          (SectionProgrammeEDL *)calloc(1, sizeof(SectionProgrammeEDL));
   section->sectionProgramme->decalageContenu = 0;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void creerSectionProgramme(SectionEDL *section,
                           unsigned int tailleMax) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->sectionProgramme =
          (SectionProgrammeEDL *)calloc(1, sizeof(SectionProgrammeEDL));
   section->sectionProgramme->decalageContenu = 0;

   if (section->type == SHT_PROGBITS) {
      creerSectionCode(section, tailleMax);
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void cloreSectionProgramme(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   Elf_Data *descripteurContenu;

   switch (section->type) {
      case SHT_PROGBITS :
         cloreSectionCode(section);
         break;
      case SHT_NOBITS :
         cloreSectionVide(section);
         break;
      default:
         break;
   }

   /* elaboration descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_type = ELF_T_BYTE;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void fermerSectionProgramme(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   /* liberation du contenu au format interne */
   if (section->sectionProgramme != NULL) {
      free(section->sectionProgramme);
      section->sectionProgramme = NULL;
   }

   /* liberation du contenu au format ELF */
   if (section->type == SHT_PROGBITS) {
      fermerSectionCode(section);
   }
}
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerSectionProgramme(SectionEDL *sectionSource,
                               SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   unsigned int tailleCible;

   /* alignement de la section fusionnee */
   tailleCible = alignement(sectionCible->tailleContenuElf,
                            sectionSource->alignement);
   sectionSource->sectionProgramme->decalageContenu = tailleCible;
   sectionCible->tailleContenuElf = tailleCible;

   switch (sectionSource->type) {
      case SHT_PROGBITS :
         fusionnerSectionCode(sectionSource, sectionCible);
         break;
      case SHT_NOBITS :
         fusionnerSectionVide(sectionSource, sectionCible);
         break;
      default:
         break;
   }
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
unsigned int tailleSectionProgramme(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   return section->tailleContenuElf;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}
