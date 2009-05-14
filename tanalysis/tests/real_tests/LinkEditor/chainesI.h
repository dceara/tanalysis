/***********************************************************************
chainesI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "chainesP.h"
#endif

struct StructSectionEDL;

void ouvrirTableChaines(struct StructSectionEDL *);
/***********************************************************************
ouvrirTableChaines(s) : cree le descripteur specifique de la table de
chaines s et traduit le contenu au format ELF de s vers le format
interne.
- etat initial :
  . s est une table de chaines en cours d'ouverture
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . le contenu de la section est disponible sous forme d'un tableau
    d'elements de type ChaineEDL
  . s contient une chaine vide et une seule
  . toutes les chaines de s ont des contenus distincts deux a deux
  . aucune des chaines de s n'est referencee (n'est utilisee)
***********************************************************************/

void creerTableChaines(struct StructSectionEDL *, unsigned int);
/***********************************************************************
creerTableChaines(s, n) : cree le descripteur specifique de la table de
chaines s et alloue la memoire necessaire pour l'ajout ulterieur de n
chaines dans s.
- etat initial :
  . s est une table de chaines en cours de creation
  . n est le nb. de chaines prevu pour le contenu de la section
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise pour le contenu de la section a ete allouee
  . la section ne contient qu'une seule chaine, la chaine vide ; cette
    chaine est inutilisee (n'est pas referencee)
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void copierContenuChaines(struct StructSectionEDL *,
                          struct StructSectionEDL *);
/***********************************************************************
copierContenuChaines(s, c) : copie le contenu au format interne de la
table de chaines s dans la table de chaines c
- etat initial :
  . s et c designent deux tables de chaines, d'un meme objet ou de deux
    objets distincts
  . c a ete creee au prealable et ne contient qu'une seule chaine, la
    chaine vide
- etat final :
  . le contenu au format interne de c est l'image conforme du contenu au
    format interne de s
***********************************************************************/
/* FIN SUPPRIMER FUSION */

void cloreTableChaines(struct StructSectionEDL *);
/***********************************************************************
cloreTableChaines(s) : traduit le contenu au format interne de la table
de chaines s vers le format ELF.
- etat initial :
  . s designe une table de chaines en cours de cloture
  . le contenu de la section (au format interne) a ete elabore
  . les descripteurs ELF et libelf de la section ont ete crees
  . s contient une chaine vide et une seule
  . toutes les chaines de s ont des contenus distincts deux a deux
  . s peut comporter des chaines dont le contenu est un suffixe strict
    du contenu d'autres chaines de s
- etat final :
  . l'index de chaque chaine dans la table de chaines au format ELF a
    ete re-calcule
  . le contenu de la section a ete traduit et recopie dans les
    structures de donnees au format ELF (et libelf)
  . les chaines inutilisees (non referencees) et les suffixes redondants
    ont ete elimines
  . la chaine vide initiale a ete conservee
***********************************************************************/

void fermerTableChaines(struct StructSectionEDL *);
/***********************************************************************
fermerTableChaines(s) : libere la memoire utilisee par la table de
chaines s.
- etat initial :
  . s designe une table de chaines en cours de fermeture
- etat final :
  . si s n'est pas partagee, la memoire utilisee pour le contenu au
    format interne de la section a ete liberee
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee, la memoire utilisee pour le contenu au format ELF
    de la section a ete liberee
***********************************************************************/
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerTableChaines(struct StructSectionEDL *,
                           struct StructSectionEDL *);
/***********************************************************************
fusionnerTableChaines(sSource, sCible) :
- etat initial : 
  . sSource est une table de chaines ouverte en entree
  . sCible est une table de chaines dont le contenu est en cours
    d'elaboration
- pre-condition :
  . sCible n'est pas partagee par plusieurs objets
- etat final :
  . le contenu de sSource a ete fusionne avec celui de sCible
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */

void ajouterChaine(struct StructSectionEDL *, char *, unsigned int,
                   unsigned int, ChaineEDL **);
/***********************************************************************
ajouterChaine(s, cc, i, es, ce) : ajoute a la table de chaine s une
chaine dont le contenu est cc.
- etat initial :
  . s designe une table de chaines
  . cc designe une chaine C a ajouter a s
  . i est l'index (au format ELF) dans s de la chaine ajoutee s'il est
    connu, 0 sinon
  . es = VRAI <=> la chaine a ajouter est un suffixe d'une autre chaine
  . si une chaine de contenu cc est presente dans s elle est utilisee
    (referencee) n0 >= 0 fois dans le programme
- pre-condition :
  . s n'est pas partagee par plusieurs objets
- etat final :
  . si aucune chaine de contenu cc n'etait presente dans s une chaine au
    format interne de contenu cc a ete creee et ajoutee a s
  . si ce != NULL, *ce designe la chaine de s dont le contenu est cc et
    celle-ci est utilisee (referenceee) n1 = n0 + 1 fois dans le
    programme
  . si ce = NULL et si la chaine ajoutee etait deja presente dans s,
    elle est utilisee (referencee) n1 = n0 fois dans le programme
  . si ce = NULL et si la chaine ajoutee n'etait pas deja presente
    dans s, elle est utilisee (referencee) n1 = 0 fois dans le programme
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void supprimerChaine(struct StructSectionEDL *, ChaineEDL *);
/***********************************************************************
supprimerChaine(s, c) : met a jour le nb. d'utilisations (de references)
de la chaine c et la supprime de la table de chaines s si elle n'est
plus utilisee.
- etat initial :
  . s designe une table de chaines
  . c designe une chaine de s
  . c est utilisee (referencee) n0 > 0 fois dans le programme
- pre-conditions :
  . s n'est pas partagee par plusieurs objets
  . s contient un exemplaire et un seul de c
- etat final :
  . c est utilisee (referencee) n1 = n0 - 1 fois dans le programme :
    si n1 = 0, c a ete supprimee de s et la memoire correspondante a ete
    liberee
***********************************************************************/
/* FIN SUPPRIMER FUSION */

void extraireChaineSelonIndex(struct StructSectionEDL *, unsigned int,
                              ChaineEDL **);
/***********************************************************************
extraireChaineSelonIndex(s, i, c) : recherche dans s une chaine (au
format interne) dont l'index (au format ELF) est i.
- etat initial :
  . s designe une table de chaines ouverte en entree
  . i est un index de chaine dans s
  . c est utilisee (referencee) n0 >= 0 fois dans le programme
- pre-condition : i < taille (en nombre d'octets) de s
- etat final :
  . si une chaine d'index i existe dans s, c designe cette chaine ;
    sinon, i est l'index d'un suffixe d'une chaine de s : une nouvelle
    chaine correspondant a ce suffixe a ete creee et ajoutee a s, et *c
    designe cette nouvelle chaine
  . c est utilisee (referencee) n1 = n0 + 1 fois dans le programme
***********************************************************************/

unsigned int tailleTableChaines(struct StructSectionEDL *);
/***********************************************************************
tailleTableChaines(s) est la taille courante en nb. de chaines de la
section s.
***********************************************************************/

void dupliquerChaine(ChaineEDL *, ChaineEDL **);
/***********************************************************************
dupliquerChaine(cs, cc) : cree une nouvelle chaine dont le contenu est
identique a celui de la chaine cs.
- etat initial : cs designe une chaine existante
- etat final : *cc designe une nouvelle chaine dont le contenu est
  identique a celui de cs
***********************************************************************/

ChaineEDL *creationChaine(char *, unsigned int, unsigned int);
/***********************************************************************
creationChaine(c, index, estSuffixe) est une nouvelle chaine (au format
interne) creee a partir de c, index et estSuffixe.
- pre-condition : c != NULL
***********************************************************************/

void libererChaine(ChaineEDL **);
/***********************************************************************
libererChaine(c) : libere la memoire utilisee par la chaine c.
- etat initial :
  . *c = NULL ou alors *c designe une chaine existante
- etat final :
  . si *c designait une chaine existante la memoire allouee pour cette
    chaine a ete liberee
  . *c = NULL
***********************************************************************/

void afficherChaines(struct StructSectionEDL *);
/***********************************************************************
afficherChaines(s) : affiche le contenu de la table de chaines s.
- etat initial : s est une table de chaines
- etat final : les chaines de s ont ete affichees
***********************************************************************/
