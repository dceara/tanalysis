/***********************************************************************
programmeI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "programmeP.h"
#endif

struct StructSectionEDL;

void ouvrirSectionProgramme(struct StructSectionEDL *);
/***********************************************************************
ouvrirSectionProgramme(s) : cree le descripteur specifique de la section
de programme s.
- etat initial :
  . s est une section de programme en cours d'ouverture
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise a ete allouee
***********************************************************************/

void creerSectionProgramme(struct StructSectionEDL *, unsigned int);
/***********************************************************************
creerSectionProgramme(s, t) : cree le descripteur specifique de la
section de programme s et alloue la memoire necessaire pour l'ajout
ulterieur de t octets au contenu (format ELF) de s.
- etat initial :
  . s est une section de programme en cours de creation
  . t est la taille en nb. d'octets prevue pour le contenu de la section
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise a ete allouee
  . le contenu de la section est vide
***********************************************************************/

void cloreSectionProgramme(struct StructSectionEDL *);
/***********************************************************************
cloreSectionProgramme(s) : elabore les informations specifiques
decrivant le contenu au format ELF de la section de code s.
- etat initial :
  . s designe une section de programme en cours de cloture
  . les descripteurs ELF et libelf de la section ont ete crees
  . le contenu de la section est complet
- etat final :
  . les informations specifiques decrivant le contenu au format ELF de s
    ont ete memorisees dans le descripteur de contenu (libelf).
***********************************************************************/

void fermerSectionProgramme(struct StructSectionEDL *);
/***********************************************************************
fermerSectionProgramme(s) : libere la memoire utilisee par la section de
programme s.
- etat initial :
  . s designe une section de programme en cours de fermeture
- etat final :
  . si s n'est pas partagee, la memoire utilisee pour le contenu au
    format interne de la section a ete liberee
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee et a pour parent initial un objet ouvert en
    sortie, la memoire utilisee pour le contenu au format ELF de la
    section a ete liberee
***********************************************************************/
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerSectionProgramme(struct StructSectionEDL *,
                               struct StructSectionEDL *);
/***********************************************************************
fusionnerSectionProgramme(sSource, sCible) :
- etat initial :
  . sSource est une section de programme ouverte en entree
  . sCible est une section de programme en cours d'elaboration
  . sSource a pour contrainte d'alignement a
- pre-condition :
  . sCible n'est pas partagee par plusieurs objets
- etat final :
  . le contenu de sSource a ete fusionne avec celui de sCible
  . la position relative du contenu de sSource dans sCible est dc, avec
    dc = 0 modulo a
  . dc a ete memorisee dans sSource
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */

unsigned int tailleSectionProgramme(struct StructSectionEDL *);
/***********************************************************************
tailleSectionProgramme(s) est la taille courante de la section s, en nb.
d'octets.
***********************************************************************/
