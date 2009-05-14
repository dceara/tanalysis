/***********************************************************************
objetP.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

STATIC void ouvrirSections(ObjetEDL *);
/***********************************************************************
ouvrirSections(o) : ouvre les sections de l'objet o.
- etat initial :
  . o est un objet d'entree en cours d'ouverture
  . ses descripteurs (format ELF et format interne) ont ete crees et
    initialises
- etat final :
  . l'ouverture des sections de o est terminee 
  . les relations entre les sections de o ont ete memorisees
  . les sections de o ont ete ajoutees a son iterateur d'objet et a
    l'iterateur global
***********************************************************************/

STATIC void cloreSections(ObjetEDL *);
/***********************************************************************
cloreSections(o) : effectue la cloture des sections de l'objet o.
- etat initial :
  . o est un objet resultat en cours de cloture
  . son descripteur au format ELF a ete cree
  . les sections et les segments (le cas echeant) de o ont ete crees et
    initialises
  . l'elaboration du contenu au format interne des sections de o est
    terminee
  . les relations entre les sections de o ont ete elaborees 
  . l'iterateur d'objet de o represente la sequence des sections de o
    ordonnee par categorie de section
- etat final :
  . la cloture des sections de o est terminee
***********************************************************************/

STATIC void fermerSections(ObjetEDL *);
/***********************************************************************
fermerSections(o) : libere la memoire utilisee par les sections de o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
- etat final :
  . les sections de o ont ete fermees
  . la memoire qui a ete allouee explicitement par le programme pour les
    sections a ete liberee
  . la memoire qui a ete allouee par la libelf n'a pas ete liberee
***********************************************************************/

STATIC void ajouterSection(ObjetEDL *, struct StructSectionEDL *,
                           unsigned int, unsigned int,
                           struct StructSectionEDL **);
/***********************************************************************
ajouterSection(oc, sr, t, a, sc) : ajoute une nouvelle section a
l'objet oc.
- etat initial :
  . oc designe un objet ouvert en entree ou en sortie
  . sr designe une section de reference appartenant a un objet or et
    dont les proprietes et les eventuelles relations avec d'autres
    sections de or seront utilisees pour decrire la section a ajouter
  . t est la taille initiale prevue pour le contenu de la section
    ajoutee ; t est exprimee en nb. d'octets ou en nb. d'objets, selon
    le type de la section
  . a est une contrainte d'alignement
- pre-condition :
  . oc n'est pas la copie superficielle d'un autre objet
- etat final :
  . une nouvelle section a ete ajoutee a oc
  . la nouvelle section a ete initialisee (creee) a partir des
    caracteristiques de la section sr, de la taille t et de la
    contrainte d'alignement a ; elle a ete ajoutee a l'iterateur d'objet
    de oc
  . la nouvelle section est l'image (relation fonctionnelle) de sr dans
    oc ; cette correspondance entre sr et la section ajoutee a ete
    memorisee dans sr
  . si sc != NULL, *sc designe la nouvelle section
***********************************************************************/

STATIC void allouerSection(ObjetEDL *, struct StructSectionEDL **);
/***********************************************************************
allouerSection(o, &s) : alloue une nouvelle section et l'ajoute a
l'objet o.
- etat initial :
  . o designe un objet auquel une nouvelle section doit etre ajoutee
-pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . une nouvelle section non initialisee a ete allouee et ajoutee a o
  . si s != NULL, *s designe la nouvelle section
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

STATIC void supprimerSection(ObjetEDL *, struct StructSectionEDL *);
/***********************************************************************
supprimerSection(o, s) : supprime la section s de l'objet o.
- etat initial :
  . s designe une section appartenant a l'objet o
-pre-conditions :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . s a ete fermee puis supprimee de o
  . s a ete retiree de l'iterateur d'objet de o
  . la memoire occupee par le descripteur de s a ete liberee
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

STATIC void creerSectionsFusionnees(ObjetEDL *);
/***********************************************************************
creerSectionsFusionnees(o) : cree dans dans l'objet o les sections
destinees a contenir le resultat de la fusion d'un ensemble d'objets
translatables.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
  . l'iterateur global represente la sequence des sections des objets a
    fusionner ordonnee par categorie de section et par nom et par type
    de section
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . une section resultat a ete creee dans o pour chaque couple
    < nom, type > de section d'entree distinct (exception faite de la
    table des noms de sections)
  . l'iterateur d'objet de o represente la sequence des sections creees
    ordonnee par categorie de section
  . la relation fonctionnelle entre sections a fusionner et section
    resultat de meme nom et de meme type a ete memorisee dans chacune
    des sections a fusionner
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER IMPLANTATION */

STATIC void fusionnerSections(ObjetEDL *);
/***********************************************************************
fusionnerSections(o) : fusionne dans l'objet o les sections d'un
ensemble d'objets translatables.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
  . l'iterateur global represente la sequence des sections des objets a
    fusionner ordonnee par categorie de section et par nom et par type
    de section
  . les sections de o destinees a contenir le resultat de la fusion des
    objets donnes ont toutes ete creees
  . l'iterateur d'objet de o represente la sequence des sections de o
    ordonnee par categorie de section
  . la relation fonctionnelle entre sections a fusionner et section
    resultat de meme nom et de meme type a ete memorisee dans chacune
    des sections a fusionner
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . o est le resultat de la fusion des objets d'entree
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

STATIC void resoudreSymbolesCommuns(ObjetEDL *);
/***********************************************************************
resoudreSymbolesCommuns(o) : alloue dans la section vide de nom .bss de
l'objet o un emplacement de taille suffisante pour chaque symbole commun
present dans l'une ou l'autre des tables de symboles de o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
- pre-condition :
  . les adresses (absolues) des sections de o n'ont pas ete determinees
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . chaque symbole commun declare dans l'objet o a ete transforme en un
    symbole global defini
    un emplacement de la taille voulue a ete reserve dans la 1ere
    section vide de nom .bss trouvee dans l'objet o ; si aucune section
    vide de ce nom n'a ete trouvee, une section vide de nom .bss a ete
    creee au prealable dans o
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

STATIC void resoudreSymbolesCRT(ObjetEDL *);
/***********************************************************************
resoudreSymbolesCRT(o) : definit la valeur des symboles __bss_start__ et
__bss_end__ delimitant les donnees non initialisees de l'objet o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
- pre-condition :
  . les adresses (absolues) des sections de o sont determinees
  . une table de symboles de nom .symtab existe dans o
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . les symboles globaux de noms __bss_start__ et __bss_end__ (symboles
    utilises par le runtime du langage C) de la 1ere table de symboles
    de nom .symtab trouvee dans o ont ete definis de facon absolue
  . la valeur du symbole __bss_start__ est l'adresse de la 1ere section
    vide de o
  . la valeur du symbole __bss_end__ est l'adresse alignee (modulo 4)
    qui suit la derniere section vide de o
  . le symbole de nom end de la table de symboles .symtab a ete supprime
- remarque :
  . pour une raison inconnue, l'editeur de lien GNU (ld) aligne le
    symbole __bss_end__ sur une adresse multiple de 4 ; notre editeur de
    liens reproduit le meme comportement
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

STATIC void implanterTablesSymboles(ObjetEDL *);
/***********************************************************************
implanterTablesSymboles(o) : determine les adresses absolues des
symboles translatables des tables de symboles de l'objet o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
  . l'iterateur d'objet represente la sequence des sections de o
    ordonnee par categorie de section
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . les adresses absolues des symboles translatables des tables de
    symboles de o ont ete determinees
  . l'adresse du point d'entree de o a ete determinee
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

STATIC void completerSectionsCode(ObjetEDL *);
/***********************************************************************
completerSectionsCode(o) : complete les adresses ou deplacements
incomplets dans les sections de code de l'objet o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
  . l'iterateur d'objet represente la sequence des sections de o
    ordonnee par categorie de section
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . toutes les sections de code de o faisant reference a des symboles
    translatables ont ete completees
  . toutes les tables de translation de o ont ete supprimees et retirees
    de l'iterateur de o
  . les symboles de section correspondants ont ete supprimes des tables
    de symboles associees
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

STATIC void ajouterPile(ObjetEDL *);
/***********************************************************************
ajouterPile(o) : ajoute une section de pile a l'objet o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
  . l'iterateur d'objet represente la sequence des sections de o
    ordonnee par categorie de section
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
  . une table de symboles de nom .symtab existe dans o
- etat final :
  . une section s non allouable de nom .stack, de type SHT_PROGBITS, de
    taille nulle a ete creee et ajoutee aux sections de o
  . l'adresse de s est l'adresse de pile qui a ete memorisee au
    demarrage du programme, ou bien l'adresse de pile par defaut
  . la section s a ete ajoutee a l'iterateur de o
  . un symbole de pile de nom _stack a ete ajoute a la 1ere table de
    symboles de nom .symtab
  . la valeur du symbole de pile cree est l'adresse de la section .stack
    creee
***********************************************************************/
/* FIN SUPPRIMER FUSION */

STATIC void initialiserLibelf(void);
/***********************************************************************
initialiserLibelf() : initialise la libelf avant son utilisation.
- etat initial : indifferent
- etat final : la bibliotheque libelf est initialisee ; ses primitives
  peuvent etre utilisees
***********************************************************************/

STATIC void afficherSections(ObjetEDL *);
/***********************************************************************
afficherSections(o) : affiche les descripteurs des sections de
l'objet o.
- etat initial : o est un objet ouvert en entree ou en sortie
- etat final : les informations decrivant les sections contenues dans o
  ont ete affichees
***********************************************************************/

STATIC void afficherAttributs(ObjetEDL *);
/***********************************************************************
afficherAttributs(o) : affiche le descripteur de l'objet o.
- etat initial : o est un objet ouvert en entree ou en sortie
- etat final : les informations decrivant l'objet o ont ete affichees
***********************************************************************/
