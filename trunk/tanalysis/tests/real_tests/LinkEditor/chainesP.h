/***********************************************************************
chainesP.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

STATIC unsigned int estSuffixe(ChaineEDL *, ChaineEDL *);
/***********************************************************************
estSuffixe(pc, gc) <=> la chaine designee par pc est un suffixe de la
chaine designee par gc
***********************************************************************/

struct StructSectionEDL;

STATIC void libererFormatInterneChaines(struct StructSectionEDL *);
/***********************************************************************
libererFormatInterneChaines(s) : libere la memoire utilisee pour le
contenu de la table de chaines s.
- etat initial :
  . s designe une table de chaines
- etat final : 
  . la memoire allouee pour le contenu au format interne de s a ete
    liberee
***********************************************************************/
