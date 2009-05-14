/***********************************************************************
videI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "videP.h"
#endif

void cloreSectionVide(struct StructSectionEDL *);
/***********************************************************************
cloreSectionVide(s) : elabore les informations specifiques decrivant le
contenu au format ELF de la section vide s.
- etat initial :
  . s designe une section vide en cours de cloture
  . les descripteurs ELF et libelf de la section ont ete crees
  . la taille de la section a ete determinee
- etat final :
  . les informations specifiques decrivant le contenu au format ELF de s
    ont ete memorisees dans le descripteur de contenu (libelf).
***********************************************************************/
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerSectionVide(struct StructSectionEDL *,
                          struct StructSectionEDL *);
/***********************************************************************
fusionnerSectionVide(sSource, sCible) : concatene la section vide
sSource a la section vide sCible.
- etat initial :
  . sSource a pour taille tSource et pour contrainte d'alignement
    alignement
  . sCible a pour taille tCible
- pre-condition :
  . sCible n'est pas partagee par plusieurs objets
- etat final :
  . soit tAlign la plus petite taille >= tailleCible et multiple de
    alignement : sCible a pour taille tAlign + tSource
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

void etendreSectionVide(struct StructSectionEDL *, unsigned int,
                        unsigned int, unsigned long *);
/***********************************************************************
etendreSectionVide(s, n, a, ts) : ajoute n octets au contenu de la
section vide s.
- etat initial :
  . s a pour taille t
- pre-condition :
  . s n'est pas partagee par plusieurs objets
- etat final :
  . soit ta la plus petite taille >= t et multiple de alignement :
    s a pour taille ta + n
  . si ts != NULL, *ts = ta
***********************************************************************/
/* FIN SUPPRIMER FUSION */
