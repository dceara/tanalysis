/***********************************************************************
iterateur.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef ITERATEUR_H
#define ITERATEUR_H

#include "section.h"

/**********************************************************************/
typedef enum enumCategorieSection {
   /* sections allouables */
   INSTRUCTIONS,
   DONNEES_INITIALISEES,
   DONNEES_LECTURE_SEULE,
   DONNEES_NON_INITIALISEES,
   /* sections non allouables */
   DONNEES_NON_ALLOUABLES,
   TABLE_CHAINES,
   TABLE_SYMBOLES,
   TABLE_TRANSLATION
} CategorieSection;
/***********************************************************************
CategorieSection :
- INSTRUCTIONS             : section de code allouable et executable
- DONNEES_INITIALISEES     : section de code allouable, non executable
                             et modifiable
- DONNEES_LECTURE_SEULE    : section de code allouable, non executable
                             et non modifiable
- DONNEES_NON_INITIALISEES : section vide allouable
- DONNEES_NON_ALLOUABLES   : section de programme non allouable ou
                             section de type autre
- TABLE_CHAINES            : table de chaines
- TABLE_SYMBOLES           : table de symboles
- TABLE_TRANSLATION        : table de translation
***********************************************************************/

/**********************************************************************/
typedef struct StructNoeudSection {
   struct StructSectionEDL *section;
   struct StructNoeudSection *suivant;
} NoeudSection;
/**********************************************************************/

/**********************************************************************/
typedef struct StructIterateurObjet {
   NoeudSection *listesSections[TABLE_TRANSLATION + 1];
   NoeudSection *dernieresSections[TABLE_TRANSLATION + 1];
   NoeudSection *sectionCourante;
   unsigned int finSections;
   CategorieSection categorieCourante;
} IterateurObjet;
/**********************************************************************/

#include "iterateurI.h"

#endif /* ITERATEUR_H */
