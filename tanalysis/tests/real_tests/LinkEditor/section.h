/***********************************************************************
section.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef SECTION_H
#define SECTION_H

#include <elf.h>
#include <libelf/libelf.h>

#include "objet.h"
#include "chaines.h"
#include "symboles.h"
#include "translations.h"
#include "programme.h"

typedef unsigned int TypeSectionEDL;
/* SHT_PROGBITS, SHT_NOBITS, ... */

typedef unsigned int FlagsSectionEDL;
/* SHF_WRITE | SHF_ALLOC | ... */

/**********************************************************************/
struct StructSectionEDL {
   struct StructObjetEDL *objetParent;
   struct StructObjetEDL *parentInitial;
   struct StructChaineEDL *nom;
   TypeSectionEDL type;
   FlagsSectionEDL flags;
   unsigned int alignement;
   unsigned int tailleElement;
   unsigned long adresse;
   /* DEBUT CACHER FUSION */
   unsigned long position;
   /* FIN CACHER FUSION */
   struct StructTableChainesEDL *tableChaines;
   struct StructTableSymbolesEDL *tableSymboles;
   struct StructTableTranslationEDL *tableTranslation;
   struct StructSectionProgrammeEDL *sectionProgramme;
   struct StructSectionEDL *sectionImage;
   void *contenuElf;
   unsigned int tailleContenuElf;
   unsigned int indexElf;
   Elf_Scn *descripteurLibelf;
   Elf_Data *descripteurContenu;
   Elf32_Shdr *descripteurElf;
};
typedef struct StructSectionEDL SectionEDL;
/***********************************************************************
SectionEDL :
- objetParent        : objet auquel appartient la section
- parentInitial      : objet avec lequel est partagee la section
- nom                : nom de la section
- type               : type de la section : section de code, table de
                       chaines, etc.
- flags              : indicateurs booleens precisant la nature de la
                       section : modifiable, executable, etc.
- alignement         : contraite d'alignement de la section
- tailleElement      : taille des elements (au format ELF) de la section
- adresse            : adresse absolue en memoire de la section
// DEBUT SUPPRIMER FUSION //
- position           : position dans le fichier de la section
// FIN SUPPRIMER FUSION //
- tableChaines       : descripteur specifique d'une table de chaines
- tableSymboles      : descripteur specifique d'une table de symboles
- tableTranslation   : descripteur specifique d'une table de translation
- sectionProgramme   : descripteur specifique d'une section dans
                       laquelle sont definis des symboles (section de
                       code ou section vide)
- sectionImage       : section qui appartient a un autre objet et a ete
                       construite d'apres cette section-ci (relation
                       fonctionnelle)
- contenuElf         : contenu au format ELF de la section
- tailleContenuElf   : taille effective du contenu au format ELF de la
                       section (en nombre d'octets)
- indexElf           : index de la section dans la table des entetes de
                       section
- descripteurLibelf  : descripteur libelf de la section
- descripteurContenu : descripteur libelf du contenu de la section
- descripteurElf     : descripteur ELF de la section
***********************************************************************/

#include "sectionI.h"

#endif /* SECTION_H */
