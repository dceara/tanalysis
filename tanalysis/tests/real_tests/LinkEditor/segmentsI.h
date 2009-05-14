/***********************************************************************
segmentsI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "objet.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "segmentsP.h"
#endif

struct StructObjetEDL;

void creerSegments(struct StructObjetEDL *);
/***********************************************************************
creerSegments(o) : cree les segments au format interne de l'objet o
- etat initial :
  . o designe un objet a implanter
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
  . les sections allouables precedent les sections non allouables dans
    la sequence de sections representee par l'iterateur d'objet de o
- etat final :
  . les segments de o ont ete crees
  . les segments de meme adresse ont ete fusionnes
  . la position de la 1ere section dans le 1er segment a ete determinee
  . les adresses des segments et celle de la 1ere section de chaque
    segment ont ete determinees
  . les sections allouables de l'objet ont ete affectees selon leur type
    aux segments appropries
- principe retenu :
  . ne sont crees que des segments contenant des donnees allouables
    (type ELF PT_LOAD)
  . 4 types de sections (et donc de segments) allouables sont
    consideres : instructions (TEXT), donnees initialisees (DATA),
    donnees en lecture seule (RODATA), et donnees non initialisees (BSS)
  . les sections allouables sont placees de facon NON COMPACTE dans le
    fichier resultat : chaque segment cree est aligne sur une frontiere
    de page, ce qui a pour consequence qu'il existe en general un "trou"
    entre la derniere section d'un segment et la 1ere section du segment
    suivant, et donc que la taille du fichier est plus importante que la
    taille de l'image correspondante creee en memoire
  . de ce principe, il decoule aussi que l'adresse de la 1ere section du
    1er segment differe de l'adresse du dit segment : le debut du 1er
    segment contient toujours le descripteur ELF ainsi que les
    descripteurs de programme (de segment) du fichier ; cela ne
    constitue toutefois pas un inconvenient, la 1ere section du 1er
    segment etant necessairement une section d'instructions dont
    l'adresse de placement en memoire n'est en general pas critique
  . l'avantage du principe retenu est qu'il est possible de
    pre-determiner les adresses d'implantation des segments (adresses
    multiples de la taille de page), et tout particulierement les
    adresses des segments de donnees. Il est notamment possible de
    placer un segment de donnees a l'adresse 0.
    Un inconvenient toutefois : il n'est pas possible de pre-determiner
    l'adresse d'implantation des sections independamment les unes
    des autres
  . une alternative possible a ce principe consisterait a placer les
    sections allouables de facon COMPACTE dans le fichier resultat :
    l'avantage de cette solution en serait une taille moindre du fichier
    resultat ; l'inconvenient en serait une plus grande difficulte a
    pre-determiner les adresses d'implantation des differents segments
    (l'adresse d'un segment et sa position dans le fichier devant etre
    imperativement congrues modulo la taille de page)
  . une autre facon de faire consisterait a pre-determiner les adresses
    d'implantation section par section (ce que fait gnu ld).
    Avantage : chaque section peut etre placee precisement dans l'espace
    d'adressage de la machine cible (cas des systemes embarques, par
    ex.).
    Inconvenient : plus grande complexite de realisation...
***********************************************************************/

void cloreSegments(struct StructObjetEDL *);
/***********************************************************************
cloreSegments(o) : cree les descripteurs de programme au format ELF
de l'objet o.
- etat initial :
  . o designe un objet resultat en cours de cloture
  . les segments (au format interne) de o ont ete crees
  . les sections allouables de o ont ete implantees et cloturees
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . les descripteurs de programme (format ELF) de o ont ete crees
***********************************************************************/

void fermerSegments(struct StructObjetEDL *);
/***********************************************************************
fermerSegments(o) : libere la memoire utilisee par les segments (au
format interne) de l'objet o.
- etat initial :
  . o designe un objet executable en cours de fermeture
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . les segments de o ont ete fermes
  . la memoire allouee pour les segments a ete liberee
***********************************************************************/

void implanterSectionsAllouables(struct StructObjetEDL *);
/***********************************************************************
implanterSectionsAllouables(o) : determine les adresses absolues et
definitives des sections allouables de l'objet o.
- etat initial : 
  . objet designe un objet en cours d'implantation
  . le nombre et la composition des segments de o ont ete determines
  . les sections allouables voulues ont ete affectees aux segments
    appropries
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . l'adresse en memoire ainsi que la position dans le fichier resultat
    des segments et des sections de o ont ete determinees
***********************************************************************/

void memoriserAdresseText(unsigned long);
/***********************************************************************
memoriserAdresseText(adresse) :  memorise l'adresse d'implantation
requise pour le segment d'instructions.
***********************************************************************/

void memoriserAdresseData(unsigned long);
/***********************************************************************
memoriserAdresseData(adresse) : memorise l'adresse d'implantation
requise pour le segment de donnees initialisees
***********************************************************************/

void memoriserAdresseRodata(unsigned long);
/***********************************************************************
memoriserAdresseRodata(adresse) : memorise l'adresse d'implantation
requise pour le segment de donnees en lecture seule
***********************************************************************/

void memoriserAdresseBss(unsigned long);
/***********************************************************************
memoriserAdresseBss(adresse) : memorise l'adresse d'implantation requise
pour le segment de donnees non initialisees
***********************************************************************/

void afficherSegments(struct StructObjetEDL *);
/***********************************************************************
afficherSegments(o) : affiche les segments au format interne de
l'objet o.
- etat initial : indifferent
- etat final : les informations decrivant les segments contenues dans o
  ont ete affichees
***********************************************************************/
