/***********************************************************************
segmentsP.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "objet.h"

struct StructObjetEDL;

STATIC void determinerSegments(struct StructObjetEDL *);
/***********************************************************************
determinerSegments(o) : determine et cree les segments de l'objet o.
- etat initial :
  . o designe un objet a implanter
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
  . les sections allouables precedent les sections non allouables dans
    la sequence de sections representee par l'iterateur d'objet de o
- etat final :
  . les segments de o ont ete crees
  . les segments de meme adresse ont ete fusionnes
  . les segments vides ou inutilises ont ete elimines :
    nbSegments > 0 et 0 <= i <= nbSegments - 1 =>
    segments[i].nbSections != 0
  . la position de la 1ere section dans le 1er segment a ete determinee
  . les adresses des segments et de la 1ere section de chaque segment
    ont ete determinees
***********************************************************************/

STATIC void repartirSectionsAllouables(struct StructObjetEDL *);
/***********************************************************************
repartirSectionsAllouables(o) : affecte chacune des sections allouables
a l'un des segments de l'objet o.
- etat initial : 
  . o est un objet en cours d'implantation
  . les segments de o ont ete determines
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
  . les sections allouables precedent les sections non allouables dans
    la sequence de sections representee par l'iterateur d'objet de o
- etat final :
  . les sections allouables de o ont ete reparties selon leur type dans
    les segments appropries
***********************************************************************/

STATIC void fusionnerSegments(SegmentEDL *, unsigned int *);
/***********************************************************************
fusionnerSegments(s, n) : fusionne les segments ayant meme adresse
d'implantation.
- etat initial : 
  . s designe le tableau des segments par defaut (TEXT, DATA, RODATA et
    BSS) d'un objet o en cours d'implantation
  . *n est le nombre de segments par defaut
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . s contient les segments resultant de la fusion
  . *n est le nombre de segments apres fusion
***********************************************************************/

STATIC void verifierChevauchementSegments(SegmentEDL *, unsigned int);
/***********************************************************************
verifierChevauchementSegments(s, n) : verifie si les segments ne se
chevauchent pas en memoire.
- etat initial : 
  . s designe un tableau de n segments d'un objet o
  . l'adresse d'implantation et la taille des segments de o ont ete
    determinees
- etat final :
  . les segments de o ne se chevauchent pas en memoire
***********************************************************************/
