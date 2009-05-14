/***********************************************************************
symbolesI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"
#include "chaines.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "symbolesP.h"
#endif

void ouvrirTableSymboles(struct StructSectionEDL *);
/***********************************************************************
ouvrirTableSymboles(s) : cree le descripteur specifique de la table de
symboles s et traduit le contenu au format ELF de s vers le format
interne.
- etat initial :
  . s est une table de symboles en cours d'ouverture
  . la partie commune du descripteur de la section est initialisee
- pre-conditions
  . l'ouverture des sections de definition des symboles de s a ete
    initialisee
  . la table de chaines associee a cette table de symboles est ouverte
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . le contenu de la section est disponible sous forme d'un tableau
    d'elements de type SymboleEDL
- remarques :
  . portee d'un symbole : outre les portees "local" et "global", la
    specification ELF definit la portee "weak". Dans sa version
    minimale, notre editeur de liens ne prend pas en compte cette portee
  . section de definition d'un symbole : la specification ELF prevoit
    des symboles "commun" (valeur speciale pour la section de
    definition). Un symbole "commun" est un symbole global indefini
    designant une donnee qui sera allouee par l'editeur de liens
    lui-meme au cours de la phase d'implantation. Un tel symbole est
    produit notamment (en langage C) par la declaration d'une variable
    globale non initialisee. Dans sa version minimale notre editeur de
    liens ne prend pas en compte ce type de symbole
***********************************************************************/

void creerTableSymboles(struct StructSectionEDL *,
                        struct StructSectionEDL *,
                        unsigned int);
/***********************************************************************
creerTableSymboles(s, tc, n) : cree le descripteur specifique de la
table de symboles s et alloue la memoire necessaire pour l'ajout
ulterieur de n symboles locaux et n symboles globaux dans s.
- etat initial :
  . s est une table de symboles en cours de creation
  . tc est la table de chaines associee a s ; tc a ete creee
    au prealable
  . n represente le nb. de symboles locaux ainsi que le nb. de symboles
    globaux prevus pour le contenu de la section
  . la partie commune du descripteur de la section est initialisee
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise pour le contenu de la section a ete allouee
  . la section ne contient qu'un seul symbole, le symbole nul
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void copierContenuSymboles(struct StructSectionEDL *,
                           struct StructSectionEDL *);
/***********************************************************************
copierContenuSymboles(s, c) : copie le contenu au format interne de la
table de symboles s dans la table de symboles c
- etat initial :
  . s et c designent deux tables de symboles, d'un meme objet ou de deux
    objets distincts
  . la section c a ete creee au prealable et a pour objet parent o
  . c ne contient que le symbole nul
- pre-condition :
  . le contenu des sections de o dont depend le contenu de c a ete cree
    (copie) au prealable
- etat final :
  . le contenu au format interne de c est l'image conforme du contenu au
    format interne de s
  . les informations dans le contenu de c qui dependent du contenu des
    autres sections de o ou de o lui-meme ont ete mises a jour
***********************************************************************/
/* FIN SUPPRIMER FUSION */

void cloreTableSymboles(struct StructSectionEDL *);
/***********************************************************************
cloreTableSymboles(s) : traduit le contenu au format interne de la table
de symboles s dans le format ELF.
- etat initial :
  . s designe une table de symboles en cours de cloture
  . le contenu de la section (au format interne) a ete elabore
  . les descripteurs ELF et libelf de la section ont ete crees
- pre-condition :
  . la cloture de la section de chaines associee a s a ete effectuee
- etat final :
  . le contenu de la section a ete traduit et recopie dans les
    structures de donnees au format ELF (libelf)
***********************************************************************/

void fermerTableSymboles(struct StructSectionEDL *);
/***********************************************************************
fermerTableSymboles(s) : libere la memoire utilisee par la table de
symboles s.
- etat initial :
  . s designe une table de symboles en cours de fermeture
- etat final :
  . si s n'est pas partagee, la memoire utilisee pour le contenu au
    format interne de la section a ete liberee
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee, la memoire utilisee pour le contenu au format ELF
    de la section a ete liberee
***********************************************************************/
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerTableSymboles(struct StructSectionEDL *sectionSource,
                            struct StructSectionEDL *sectionCible);
/***********************************************************************
fusionnerTableSymboles(sSource, sCible) :
- etat initial :
  . sSource est une table de symboles ouverte en entree
  . sCible est une table de symboles dont le contenu est en cours
    d'elaboration
- pre-conditions :
  . sCible n'est pas partagee par plusieurs objets
  . la fusion de la table de chaines associee a sCible est terminee
  . la fusion des sections de programme concernees par sCible est
    terminee
- etat final :
  . le contenu de sSource a ete fusionne avec celui de sCible
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */

void ajouterSymbole(struct StructSectionEDL *, SymboleEDL *,
                    SymboleEDL **);
/***********************************************************************
ajouterSymbole(tc, ss, asc) : ajoute a la table de symboles tc un
symbole sc dont le contenu est decrit par le symbole ss.
- etat initial :
  . tc designe une table de symboles appartenant a un objet oc
  . ss designe un symbole d'une table de symboles ts appartenant a un
    objet os
- pre-conditions :
  . tc n'est pas partagee par plusieurs objets
  . le symbole ajoute n'est pas le symbole nul
  . la section de definition (dans l'objet oc) du symbole sc a ete creee
    au prealable
- etat final :
  . un symbole (au format interne) sc de meme contenu que le symbole ss
    a ete cree et ajoute a tc
  . le symbole sc a pour nom une copie du nom du symbole ss
  . ss n'est pas un symbole de section <=> la table de chaines associee
    a tc contient le nom du symbole sc
  . ss est un symbole translatable <=> sc a pour section de definition
    (dans oc) la section image de la section de definition (dans os) du
    symbole ss
  . le symbole sc est l'image dans tc du symbole ss ; cette relation
    fonctionnelle a ete memorisee dans ss
  . si asc != NULL, *asc designe le symbole cree sc
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void promouvoirSymbolesCommuns(struct StructSectionEDL *);
/***********************************************************************
promouvoirSymbolesCommuns(t) : transforme en un symbole global defini
chaque symbole commun present dans la table de symboles t appartenant a
un objet o, apres avoir alloue un emplacement de taille suffisante dans
la section vide de nom .bss de o.
- etat initial : indifferent
- pre-conditions :
  . t n'est pas partagee par plusieurs objets
  . les adresses des sections allouables de l'objet o n'ont pas ete
    determinees
- etat final :
  . chaque symbole commun present dans t a ete transforme en un symbole
    global defini.
    un emplacement de la taille voulue a ete reserve dans la 1ere
    section vide de nom .bss trouvee dans l'objet o ; si aucune section
    vide de ce nom n'a ete trouvee, une section vide de nom .bss a ete
    creee au prealable dans o
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

void ajusterSymbolesCRT(struct StructSectionEDL *,
                        unsigned long, unsigned long);
/***********************************************************************
ajusterSymbolesCRT(t, startBss, endBss) : definit la valeur des symboles
__bss_start__ et __bss_end__ utilises par le runtime du langage C pour
initialiser les sections vides au demarrage du programme.
- etat initial :
  . t designe la table de symboles de nom .symtab
- pre-conditions :
  . t n'est pas partagee par plusieurs objets
- etat final :
  . s'il existait dans t, le symbole global indefini de nom
    __bss_start__ a ete defini de facon absolue avec pour valeur
    startBss
  . s'il existait dans t, le symbole global indefini de nom __bss_end__
    a ete defini de facon absolue avec pour valeur endBss
  . s'il existait dans t, le symbole global indefini de nom end a ete
    supprime
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

void implanterTableSymboles(struct StructSectionEDL *);
/***********************************************************************
implanterTableSymboles(t) : determine la valeur absolue et definitive
des symboles translatables de la table de symboles t.
- etat initial :
  . t designe une table de symboles non implantee appartenant a un
    objet o
- pre-conditions :
  . t n'est pas partagee par plusieurs objets
  . la section de definition des symboles a implanter a elle-meme ete
    implantee au prealable
  . t ne contient aucun symbole commun
- etat final (specification minimale) :
  . la valeur definitive des symboles translatables de t a ete
    determinee
  . l'adresse du point d'entree a ete determinee a partir du nom de
    symbole memorise au demarrage du programme (ou du nom de point
    d'entree par defaut)
  . si un symbole global indefini subsistait dans t, une erreur a ete
    signalee a l'utilisateur et l'execution du programme a ete
    interrompue
- etat final (specification etendue, avec prise en compte des symboles
  faibles (STB_WEAK)) :
  . les symboles faibles ont ete transformes en symboles globaux, leur
    valeur definitive a ete determinee
***********************************************************************/
/* FIN SUPPRIMER FUSION */

unsigned int indexSymbole(struct StructSectionEDL *, SymboleEDL *);
/***********************************************************************
indexSymbole(t, s) est l'index (au format ELF) du symbole designe par
s dans la table de symboles t.
- pre-condition : le symbole s appartient a t
***********************************************************************/

SymboleEDL *symboleSelonIndex(struct StructSectionEDL *, unsigned int);
/***********************************************************************
sectionSelonIndex(t, i) designe le symbole dont l'index (au format ELF)
dans la table de symboles t est i.
- pre-condition : un symbole d'index i existe dans t
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void supprimerSymboleSelonNom(struct StructSectionEDL *,
                              struct StructChaineEDL *);
/***********************************************************************
supprimerSymboleSelonNom(t, n) : supprime de la table de symboles t le
symbole global ou de section de nom n.
- etat initial :
  . t designe une table de symboles
  . t n'est pas partagee par plusieurs objets
  . n designe le nom d'un symbole de section ou d'un symbole global
  . le symbole a supprimer n'est plus utilise
- etat final :
  . si un symbole global ou de section ayant pour nom n etait present
    dans t, il a ete supprime de t et la memoire correspondante a ete
    liberee ; sinon, t est inchangee
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

void supprimerSymbolesSectionInutiles(struct StructSectionEDL *);
/***********************************************************************
supprimerSymbolesSectionInutiles(t) : supprime de la table de symboles t
les symboles de section inutiles.
- etat initial :
  . t designe une table de symboles
  . t n'est pas partagee par plusieurs objets
  . les symboles de section a supprimer ne sont plus utilises
- etat final :
  . tous les symboles de section des sections de type autre que
    SHT_PROGBITS ou SHT_NOBITS ont ete supprimes de t
***********************************************************************/
/* FIN SUPPRIMER FUSION */

unsigned int tailleTableSymboles(struct StructSectionEDL *);
/***********************************************************************
tailleTableSymboles(s) est le maximum du nb. de symboles locaux et du
nb. de symbole globaux.
***********************************************************************/

void afficherSymboles(struct StructSectionEDL *);
/***********************************************************************
afficherSymboles(s) : affiche le contenu de la table de symboles s.
- etat initial : s est une table de symboles
- etat final : les symboles de s ont ete affiches
***********************************************************************/
