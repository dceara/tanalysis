/***********************************************************************
sectionI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "objet.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "sectionP.h"
#endif

void initialiserOuvertureSection(SectionEDL *, struct StructObjetEDL *,
                                 size_t);
/***********************************************************************
initialiserOuvertureSection(s, o, i) : lit depuis un fichier d'entree le
contenu au format ELF de la section s d'index i appartenant a l'objet o.
- etat initial :
  . s designe le descripteur de la section a ouvrir,
    o designe le descripteur de l'objet auquel appartient s,
    i est l'index de s dans la table ELF des entetes de section
  . le descripteur de l'objet d'entree o est initialise
  . la memoire requise pour la creation du descripteur de la section s
    est allouee
- pre-condition :
  . l'ouverture complete de la table des noms de section a ete effectuee
    avant l'initialisation de l'ouverture de toute autre section
- etat final :
  . le contenu de s est accessible au moyen d'un ensemble de structures
    de donnees au format ELF (et libelf) ; ce contenu est disponible
    sous la forme d'un tableau d'octets
  . la partie commune du descripteur de la section s est initialisee
  . les informations decrivant les relations eventuelles de cette
    section avec d'autres sections du meme objet seront elaborees lors
    de l'ouverture effective de la section
  . le nom de la section a ete ajoute a la table des noms de section,
    sauf si s est elle-meme la table des noms de section
***********************************************************************/

void ouvrirSection(SectionEDL *);
/***********************************************************************
ouvrirSection(s) : traduit le contenu de la section s depuis le format
ELF vers le format interne EDL.
- etat initial :
  . s designe une section en cours d'ouverture
  . le contenu de s est accessible au moyen de structures de donnees au
    format ELF (et libelf) et disponible sous la forme d'un tableau
    d'octets
  . la partie commune du descripteur de s est initialisee
  . le nom de la section a ete ajoute a la table des noms de section,
    sauf si s est elle-meme la table des noms de section
- pre-conditions :
    si s est la table des noms de sections, l'initialisation de
    l'ouverture de toutes les sections de l'objet auquel appartient s a
    ete effectuee au prealable
  . l'ouverture de toutes les sections dont dependent le descripteur et
    le contenu de s a ete effectuee au prealable
- etat final :
  . la partie specifique du descripteur de la section s a ete
    initialisee
  . si necessaire, le contenu de la section a ete traduit et recopie
    dans des structures de donnees internes dont le format (le type) est
    specifique a chaque type de section
  . les informations decrivant les relations eventuelles de cette
    section avec d'autres sections du meme objet ont ete elaborees
  . si s est la table des noms de section, s contient son propre nom
***********************************************************************/

void creerSection(SectionEDL *, struct StructObjetEDL *, SectionEDL *,
                  unsigned int, unsigned int);
/***********************************************************************
creerSection(sc, oc, sr, t, a) : cree une nouvelle section sc dans
l'objet oc.  Le contenu de la section creee est vide.
- etat initial :
  . sc designe une section non initialisee appartenant a un objet oc
  . sr designe une section de reference appartenant a un objet or et
    dont les proprietes et les eventuelles relations avec d'autres
    sections seront utilisees pour decrire la section sc a creer
  . t est la taille initiale prevue pour le contenu de la section sc ; t
    est exprimee en nb. d'octets ou en nb. d'objets, selon le type de la
    section consideree
  . a est la contrainte d'alignement minimum requise pour la section sc
  . la memoire requise pour la creation du descripteur de la section sc
    est allouee
- pre-conditions :
  . oc n'est pas la copie superficielle d'un autre objet
  . la table des noms de section l'objet oc a ete creee
  . toutes les sections de l'objet os dont depend sc ont ete creees
  . soit psc_i l'une des sections de oc dont depend sc ; soient psr_ij
    les sections de or auxquelles correspond psc_i dans oc : psc_i
    est l'image dans oc des psr_ij. Cette relation fonctionnelle a ete
    memorisee dans chaque section psr_ij avant la creation de sc
- etat final :
  . le descripteur de la section sc a ete initialise, ses relations
    eventuelles avec d'autres sections de l'objet oc ont ete elaborees
  . sc a meme caracteristiques que la section de reference sr et a pour
    taille t
  . sc a pour contrainte d'alignement la contrainte d'alignement de sr
    si a = 0, a sinon
  . la memoire requise pour le contenu de sc a ete allouee
  . le contenu de sc est vide (sauf si sc est la table des noms de
    section de l'objet oc)
  . le nom de la section sc ete ajoute a la table des noms de section de
    oc
***********************************************************************/

void partagerSection(SectionEDL *, struct StructObjetEDL *);
/***********************************************************************
partagerSection(s, o) : partage la section s d'un objet p avec
l'objet o.
- etat initial :
  . s designe une section d'un objet p ouvert en entree ou en sortie
  . o designe une copie superficielle de l'objet p
  . o est ouvert en sortie
  . s n'est pas partagee
- pre-conditions :
  . si p est ouvert en sortie, la cloture de p n'a pas ete effectuee
  . la cloture de o n'a pas ete effectuee
- etat final :
  . le descripteur et le contenu au format interne de la section s sont
    partages entre les objets p (parent initial) et o (parent courant)
  . les contenus au format interne et au format ELF de s, s'ils
    existent, ne doivent plus etre modifies
  . les descripteurs au format ELF et libelf decrivant s dans o doivent
    etre re-crees
- remarque : cycle de vie du contenu au format ELF
  . si l'objet p est ouvert en entree, le contenu au format ELF initial
    de s a ete cree lors de l'ouverture de p
    + si s est une table de chaines, une table de symboles ou une table
      de translation, le contenu au format ELF final de s sera cree lors
      de la cloture de l'objet o
    + si s est une section de programme ou une section de type autre, le
      contenu au format ELF de s restera inchange jusqu'a la fermeture
      de l'objet p
  . si l'objet p est ouvert en sortie :
    + si s est une table de chaines, une table de symboles ou une table
      de translation, s ne possede pas de contenu au format ELF
      initial ; le contenu au format ELF final de s sera cree lors de la
      cloture de l'objet o
    + si s est une section de programme ou une section de type autre, le
      contenu au format ELF de s a ete cree avant que la section ne soit
      partagee et restera inchange jusqu'a la fermeture de l'objet p
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void copierContenuSection(SectionEDL *, SectionEDL *);
/***********************************************************************
copierContenuSection(s, c) : duplique le contenu au format interne (EDL)
de la section s dans la section c.
- etat initial :
  . s et c designent deux sections, d'un meme objet ou de deux objets
    distincts
  . la section c a ete creee au prealable et a pour objet parent o
  . le contenu au format interne de c est vide
- pre-condition
  . le contenu des sections de o dont depend le contenu de c a ete
    duplique au prealable
- etat final :
  . le contenu au format interne de c est l'image conforme du contenu au
    format interne de s
  . les informations dans le contenu de c qui dependent du contenu des
    autres sections de o ou de o lui-meme ont ete mises a jour
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerSection(SectionEDL *, SectionEDL *);
/***********************************************************************
fusionnerSection(sSource, sCible) : fusionne la section d'entree sSource
avec la section de sortie sCible.
- etat initial : 
  . sSource designe une section ouverte en entree
  . sCible designe une section d'un objet oCible ayant ete creee au
    prealable et dont le contenu est en cours d'elaboration
- pre-condition :
  . sCible n'est pas partagee par plusieurs objets
  . toutes les sections de l'objet oCible ont ete creees avant le debut
    de la fusion
  . sSource et sCible ne sont pas des tables de noms de section
- etat final :
  . le contenu de sSource a ete fusionne avec le contenu existant
    (eventuellement vide) de sCible
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */

void initialiserClotureSection(SectionEDL *);
/***********************************************************************
initialiserClotureSection(s) : cree les descripteurs ELF et libelf de la
section s en vue de son ecriture dans le fichier de sortie.
- etat initial :
  . s designe une section nouvellement creee
  . l'elaboration des structures de donnees internes representant le
    contenu de s est terminee
- etat final :
  . les descripteurs ELF et libelf de la section s ont ete crees
***********************************************************************/

void cloreSection(SectionEDL *);
/***********************************************************************
cloreSection(s) : traduit le contenu au format interne de la section s
vers le format ELF.
- etat initial :
  . s designe une section nouvellement creee dans un objet o
  . les descripteurs ELF et libelf de la section ont ete crees
- pre-conditions :
  . l'initialisation de la cloture de toutes les sections de l'objet o
    auquel appartient s est terminee
  . la cloture de la table des noms de section de o a ete effectuee
  . la cloture de toutes les sections de o dont depend le contenu de s a
    ete effectuee
- etat final :
  . si necessaire, le contenu de la section a ete traduit et recopie
    dans les structures de donnees au format ELF (libelf)
  . l'elaboration des descripteurs ELF et libelf de la section est
    terminee
- remarque : le contenu au format interne d'une section dont la cloture
  a ete effectuee ne doit plus etre modifie (si ce n'est apres que cette
  section ait ete fermee puis re-ouverte)
***********************************************************************/

void fermerSection(SectionEDL *);
/***********************************************************************
fermerSection(s) : libere la memoire allouee par le programme pour le
contenu de la section s.
- etat initial :
  . s designe une section ouverte en entree ou en sortie appartenant a
    un objet en cours de fermeture
- etat final :
  . si s n'est pas partagee, la memoire utilisee pour le contenu au
    format interne de la section a ete liberee
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee et est une table de chaines, une table de symboles
    ou une table de translation, la memoire utilisee pour le contenu au
    format ELF de la section a ete liberee
  . si s est partagee et est une section de programme ou de type autre
    et a pour parent initial un objet ouvert en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
- remarques :
  . une section partagee est fermee deux fois : une fois en tant que
    section appartenant a son objet parent initial, une fois en tant que
    section appartenant a l'objet copie du parent initial
  . lors de la seconde fermeture d'une section partagee de type section
    de programme ou de type autre toute la memoire utilisee pour le
    contenu de la section a ete precedemment liberee, aucune nouvelle
    liberation n'est a effectuer
  . si une section s n'est pas partagee et est ouverte en entree, la
    memoire utilisee pour le contenu au format ELF de la section a ete
    allouee par la libelf et devra etre liberee par elle (elf_end) apres
    la fermeture de toutes les sections
  . si une section partagee s a pour parent initial un objet ouvert en
    entree, la memoire utilisee pour le contenu au format ELF initial de
    la section a ete allouee par la libelf et devra etre liberee par
    elle apres la fermeture des sections
  . si une section partagee s a pour parent initial un objet ouvert en
    sortie et est de type table de chaines, table de symboles ou table
    de translation, aucun contenu au format ELF initial n'a ete alloue
    pour la section
***********************************************************************/

unsigned int tailleSection(SectionEDL *);
/***********************************************************************
tailleSection(s) est la taille courante maximum de la section s, en nb.
d'octets ou en nb. d'objets, selon le type de la section
***********************************************************************/

void afficherSection(SectionEDL *);
/***********************************************************************
afficherSection(s) : affiche le descripteur de la section s.
- etat initial : s designe le descripteur d'une section ouverte en
  entree
- etat final : les informations decrivant s ont ete affichees
***********************************************************************/

void afficherTitreSections(void);
/***********************************************************************
afficherTitreSections() : affiche un titre enumerant les informations
decrivant une section.
- etat initial : indifferent
- etat final : le titre decrivant les informations d'une section a ete
  affiche
***********************************************************************/

void afficherInterligneSections(void);
/***********************************************************************
afficherInterligneSections() : affiche un interligne entre deux lignes
decrivant une section.
- etat initial : indifferent
- etat final : un interligne a ete affiche
***********************************************************************/

unsigned long alignement(unsigned long, unsigned int);
/***********************************************************************
alignement(p, a) est la plus petite valeur v >= p telle que
v modulo a = 0
pre-condition : a = 2 ^ k
***********************************************************************/
