/***********************************************************************
autreI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "autreP.h"
#endif

void creerAutreSection(struct StructSectionEDL *, unsigned int);
/***********************************************************************
creerAutreSection(s, t) : alloue la memoire necessaire pour l'ajout
ulterieur de t octets au contenu (format ELF) de la section s.
- etat initial :
  . s est une section de type autre en cours de creation
  . t est la taille en nb. d'octets prevue pour le contenu de la section
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise a ete allouee
  . le contenu de la section est vide
***********************************************************************/

void cloreAutreSection(struct StructSectionEDL *);
/***********************************************************************
cloreAutreSection(s) : elabore les informations specifiques decrivant
le contenu au format ELF de la section s.
- etat initial :
  . s designe une section de type autre en cours de cloture
  . les descripteurs ELF et libelf de la section ont ete crees
  . le contenu de la section a ete complete
- etat final :
  . les informations specifiques decrivant le contenu au format ELF de s
    ont ete memorisees dans le descripteur de contenu (libelf).
***********************************************************************/

void fermerAutreSection(struct StructSectionEDL *);
/***********************************************************************
fermerAutreSection(s) : libere la memoire utilisee par la section s.
- etat initial :
  . s designe une section de type autre en cours de fermeture
- etat final :
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee et a pour parent initial un objet ouvert en
    sortie, la memoire utilisee pour le contenu au format ELF de la
    section a ete liberee
***********************************************************************/
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerAutreSection(struct StructSectionEDL *,
                           struct StructSectionEDL *);
/***********************************************************************
fusionnerAutreSection(sSource, sCible) : concatene le contenu de la
section de type autre sSource a celui de la section de type autre
sCible.
- etat initial :
  . sSource a pour taille tSource
  . sCible a pour taille tCible et pour contrainte d'alignement
    alignement
- pre-condition :
  . sCible n'est pas partagee par plusieurs objets
- etat final :
  . le contenu de sSource a ete concatene a celui de sCible
  . soit tAlign la plus petite taille >= tailleCible et multiple de
    alignement : sCible a pour taille tAlign + tSource
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */

unsigned int tailleAutreSection(struct StructSectionEDL *);
/***********************************************************************
tailleAutreSection(s) est la taille courante de la section s, en nb.
d'octets.
***********************************************************************/
