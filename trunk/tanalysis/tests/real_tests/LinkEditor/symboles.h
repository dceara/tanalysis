/***********************************************************************
symboles.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef SYMBOLES_H
#define SYMBOLES_H

#include "section.h"
#include "chaines.h"
/* DEBUT SUPPRIMER FUSION */

#define BSS_START "__bss_start__"
#define BSS_END "__bss_end__"
#define END "end"
/* FIN SUPPRIMER FUSION */

typedef unsigned int PorteeSymboleEDL;
/* STB_LOCAL, STB_GLOBAL, ... */

typedef unsigned int TypeSymboleEDL;
/* STT_SECTION, STT_NOTYPE, ... */

typedef unsigned int NatureDefinitionEDL;
/* SHN_* ou index de section de definition */

/**********************************************************************/
struct StructSymboleEDL {
   struct StructChaineEDL *nom;
   unsigned long valeur;
   unsigned int taille;
   PorteeSymboleEDL portee;
   TypeSymboleEDL type;
   NatureDefinitionEDL natureDefinition;
   struct StructSectionEDL *sectionDefinition;
   struct StructSymboleEDL *symboleImage;
   struct StructSectionEDL *tableSymbolesOrigine;
};
typedef struct StructSymboleEDL SymboleEDL;
/***********************************************************************
SymboleEDL :
- nom                   : nom associe au symbole.
                          Si le symbole est un symbole de section, son
                          nom est le nom de la section correspondante
- valeur                : valeur (relative ou absolue) du symbole
- taille                : taille associee au symbole
- portee                : portee du symbole
- type                  : type du symbole
- natureDefinition      : nature du symbole (symboles non translatables)
                          ou index dans la table des entetes de sections
                          de la section de definition du symbole
- sectionDefinition     : section de definition du symbole
- symboleImage          : symbole qui appartient a une autre table de
                          symboles et a ete construit d'apres ce
                          symbole-ci (relation fonctionnelle)
- tableSymbolesOrigine  : table de symboles d'ou est issu le symbole
                          (pour le traitement des symboles communs)
***********************************************************************/

/**********************************************************************/
struct StructTableSymbolesEDL {
   SymboleEDL **symbolesLocaux;
   SymboleEDL **symbolesGlobaux;
   unsigned int nbLocaux;
   unsigned int nbGlobaux;
   unsigned int nbMaxLocaux;
   unsigned int nbMaxGlobaux;
   struct StructSectionEDL *tableChaines;
};
typedef struct StructTableSymbolesEDL TableSymbolesEDL;
/***********************************************************************
TableSymbolesEDL :
- symbolesLocaux  : les symboles locaux au format interne (EDL)
- symbolesGlobaux : les symboles globaux au format interne (EDL)
- nbLocaux        : nombre effectif de symboles locaux
- nbGlobaux       : nombre effectif de symboles globaux
- nbMaxLocaux     : nombre maximum de symboles locaux
- nbMaxGlobaux    : nombre maximum de symboles globaux
- tableChaines    : table de chaines associee a cette table de symboles
***********************************************************************/

#include "symbolesI.h"

#endif /* SYMBOLES_H */
