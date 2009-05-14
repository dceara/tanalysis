/***********************************************************************
translations.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef TRANSLATIONS_H
#define TRANSLATIONS_H

#include "section.h"
#include "symboles.h"

typedef unsigned int TypeTranslationEDL;
/* R_ARM_PC24, R_ARM_ABS32, ... */

/**********************************************************************/
struct StructTranslationEDL {
   unsigned long adresse;
   struct StructSymboleEDL *symbole;
   TypeTranslationEDL type;
};
typedef struct StructTranslationEDL TranslationEDL;
/***********************************************************************
TranslationEDL :
- adresse   : adresse de l'emplacement a completer relative au debut de
              la section contenant cet emplacement
- symbole   : symbole reference par l'emplacement a completer
- type      : type de calcul a effectuer pour completer les emplacements
***********************************************************************/

/**********************************************************************/
struct StructTableTranslationEDL {
   TranslationEDL *translations;
   unsigned int nbTranslations;
   struct StructSectionEDL *tableSymboles;
   struct StructSectionEDL *sectionCode;
};
typedef struct StructTableTranslationEDL TableTranslationEDL;
/***********************************************************************
TableTranslationEDL :
- translations    : les informations de translation au format interne
                    (EDL)
- nbTranslations  : nombre d'informations de translation
- tableSymboles   : table de symboles associee a cette table de
                    translation
- sectionCode     : section de code que permet de completer cette table
                    de translation
***********************************************************************/

#include "translationsI.h"

#endif /* TRANSLATIONS_H */
