/***********************************************************************
iterateurP.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

STATIC void ajouterSectionGlobal(struct StructSectionEDL *,
                                 CategorieSection);
/***********************************************************************
ajouterSectionGlobal(s, c) : ajoute la section s a la sequence des
sections de meme couple nom-type que s, dans la categorie c de
l'iterateur global.
- etat initial :
  . s est une section de categorie c et de couple nom-type nt
- etat final :
  . s a ete ajoutee a la fin de la sequence des sections de categorie c
    et de couple nom-type nt
***********************************************************************/
