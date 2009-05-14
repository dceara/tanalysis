/***********************************************************************
codeI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "codeP.h"
#endif

struct StructSectionEDL;

void creerSectionCode(struct StructSectionEDL *, unsigned int);
/***********************************************************************
creerSectionCode(s, t) : cree les informations specifiques a la section
de code s et alloue la memoire necessaire pour l'ajout ulterieur de t
octets au contenu (format ELF) de s.
- etat initial :
  . s est une section de code en cours de creation
  . t est la taille en nb. d'octets prevue pour le contenu de la section
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise a ete allouee
  . le contenu de la section est vide
***********************************************************************/

void cloreSectionCode(struct StructSectionEDL *);
/***********************************************************************
cloreSectionCode(s) : elabore les informations specifiques decrivant le
contenu au format ELF de la section de code s.
- etat initial :
  . s designe une section de code en cours de cloture
  . les descripteurs ELF et libelf de la section ont ete crees
  . le contenu de la section est complet
- etat final :
  . les informations specifiques decrivant le contenu au format ELF de s
    ont ete memorisees dans le descripteur de contenu (libelf).
***********************************************************************/

void fermerSectionCode(struct StructSectionEDL *);
/***********************************************************************
fermerSectionCode(s) : libere la memoire utilisee par la section de
code s.
- etat initial :
  . s designe une section de code en cours de fermeture
  . si s n'est pas partagee, la memoire utilisee pour le contenu au
    format interne de la section a ete liberee
- etat final :
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee et a pour parent initial un objet ouvert en
    sortie, la memoire utilisee pour le contenu au format ELF de la
    section a ete liberee
***********************************************************************/
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerSectionCode(struct StructSectionEDL *,
                          struct StructSectionEDL *);
/***********************************************************************
fusionnerSectionCode(sSource, sCible) : concatene le contenu de la
section de code sSource a celui de la section de code sCible.
- etat initial :
  . sSource a pour taille tSource et pour contrainte d'alignement
    alignement
  . sCible a pour taille tCible : tCible = 0 modulo alignement
- pre-condition :
  . sCible n'est pas partagee par plusieurs objets
- etat final :
  . le contenu de sSource a ete concatene a celui de sCible
  . soit tAlign la plus petite taille >= tailleCible et multiple de
    alignement : sCible a pour taille tAlign + tSource
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */
