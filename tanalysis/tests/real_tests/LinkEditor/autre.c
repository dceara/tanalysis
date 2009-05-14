/***********************************************************************
autre.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdlib.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "autre.h"
#include "section.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "autreP.h"
#ifdef AVEC_LIBEDL
#include "_autreI.h"
#endif
#else
#include "upcallI.h"
#endif

/**********************************************************************/
void creerAutreSection(struct StructSectionEDL *section,
                       unsigned int tailleMax) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->contenuElf = calloc(tailleMax, 1);
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void cloreAutreSection(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   Elf_Data *descripteurContenu;

   /* elaboration descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_buf = section->contenuElf;
   switch (section->type) {
      case SHT_DYNAMIC:
         descripteurContenu->d_type = ELF_T_DYN;
         break;
      case SHT_DYNSYM:
         descripteurContenu->d_type = ELF_T_SYM;
         break;
      case SHT_FINI_ARRAY:
      case SHT_INIT_ARRAY:
      case SHT_PREINIT_ARRAY:
         descripteurContenu->d_type = ELF_T_ADDR;
         break;
      case SHT_GROUP:
      case SHT_HASH:
         descripteurContenu->d_type = ELF_T_WORD;
         break;
      case SHT_NOTE:
#ifdef ELF_T_NOTE
         descripteurContenu->d_type = ELF_T_NOTE;
#else
         descripteurContenu->d_type = ELF_T_BYTE;
#endif
         break;
      case SHT_RELA:
         descripteurContenu->d_type = ELF_T_RELA;
         break;
      default:
         descripteurContenu->d_type = ELF_T_BYTE;
         break;
   }
}

/**********************************************************************/
void fermerAutreSection(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   if (section->objetParent->mode == ELF_C_WRITE
       && section->parentInitial == NULL) {
      if (section->contenuElf != NULL) {
         /* liberation du contenu au format ELF */
         free(section->contenuElf);
         section->contenuElf = NULL;
      }
   }
}
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerAutreSection(SectionEDL *sectionSource,
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

   tailleCible = alignement(tailleCible, sectionSource->alignement);
   for (i = 0 ; i < tailleSource ; i++) {
      contenuCible[tailleCible + i] = contenuSource[i];
   }
   tailleCible += tailleSource;

   sectionCible->tailleContenuElf = tailleCible;
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
unsigned int tailleAutreSection(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   return section->tailleContenuElf;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}
