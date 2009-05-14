/***********************************************************************
iterateurI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "iterateurP.h"
#endif

/***********************************************************************
Categories de section :
- definition :
  . INSTRUCTIONS              : section de code allouable et executable
  . DONNEES_INITIALISEES      : section de code allouable, non
                                executable et modifiable
  . DONNEES_LECTURE_SEULE     : section de code allouable, non
                                executable et non modifiable
  . DONNEEES_NON_INITIALISEES : section vide allouable
  . DONNEES_NON_ALLOUABLES    : section de programme non allouable ou 
                                section de type autre
  . TABLE_CHAINES             : table de chaines
  . TABLES_SYMBOLES           : table de symboles
  . TABLE_TRANSLATION         : table de translation
- ordre sur les categories :
  . l'ensemble des categories de section est ordonne comme suit :
    INSTRUCTIONS -> DONNEES INITIALISEES -> DONNEES EN LECTURE SEULE ->
    DONNEES NON INITIALISEES -> DONNEES NON ALLOUABLES ->
    TABLES DE CHAINES -> TABLES DE SYMBOLES -> TABLES DE TRANSLATION 
  . cette relation d'ordre exprime la dependance existant entre les
    differents types de section. Par exemple, le contenu d'une table de
    symboles dépend de celui d'une table de chaines et doit donc etre
    elabore et cloture apres le contenu de cette derniere.
- remarques :
  . deux sections de meme categorie et de meme nom ne sont pas
    necessairement de meme type
***********************************************************************/

/***********************************************************************
Iterateur d'objet :
- description :
  . permet d'acceder dans un ordre determine a l'ensemble des sections
    d'un objet donne
  . l'ensemble des sections est organise en une sequence
    [C1, ..., Cj, ..., Cm] de categories de section, ordonnee selon
    l'ordre sur les categories
  . les sections d'une meme categorie Cj forment une sequence de
    sections [Cj1, ..., Cjk, ..., Cjn] dont l'ordre est indetermine
- remarques :
  . l'iterateur (la sequence de categories) d'un objet est initialement
    vide
  . une categorie (une sequence de sections) peut etre vide
***********************************************************************/

CategorieSection categorieSection(struct StructSectionEDL *);
/***********************************************************************
categorieSection(s) est la categorie de la section s.
***********************************************************************/

void creerIterObjet(IterateurObjet **);
/***********************************************************************
creerIterObjet(i) : cree un nouvel iterateur d'objet.
- etat initial : 
  . *i = NULL ou alors *i designe un iterateur d'objet
- etat final :
  . *i designe un iterateur d'objet initialise
  . la sequence de categories representee par *i est vide
***********************************************************************/

void viderIterObjet(IterateurObjet *);
/***********************************************************************
viderIterObjet(i) : vide un iterateur d'objet existant.
- etat initial :
  . i designe un iterateur d'objet initialise
- etat final :
  . la sequence de categories representee par i est vide
***********************************************************************/

void ajouterAIterObjet(IterateurObjet *, struct StructSectionEDL *);
/***********************************************************************
ajouterAIterObjet(i, s) : ajoute la section s a l'iterateur d'objet i.
- etat initial :
  . i designe l'iterateur d'objet d'un objet o
  . s designe une section de categorie c appartenant a o
- etat final :
  . s a ete ajoutee a la fin de la sequence des sections de categorie c
***********************************************************************/

void retirerDeIterObjet(IterateurObjet *, struct StructSectionEDL *);
/***********************************************************************
retirerDeIterObjet(i, s) : retire la section s de l'iterateur d'objet i.
- etat initial :
  . i designe un iterateur d'objet
  . s designe une section appartenant a l'une des sequences de sections
    de i
- etat final :
  . s n'appartient plus a l'iterateur d'objet i
***********************************************************************/

void demarrerIterObjet(IterateurObjet *, CategorieSection);
/***********************************************************************
demarrerIterObjet(i, c) : demarre le parcours des sequences de sections
de l'iterateur d'objet i, en commencant par la sequence des sections de
categorie c.
- etat initial :
  . i designe un iterateur d'objet
- etat final :
  . finIterObjet() est vrai ou alors sectionCouranteIterObjet() designe
    la 1ere section de la sequence des sections de categorie c de i
***********************************************************************/

void avancerIterObjet(IterateurObjet *);
/***********************************************************************
avancerIterObjet(i) : passe a la section suivante lors d'un parcours des
sequences de sections de l'iterateur d'objet i.
- etat initial :
  . l'iterateur d'objet designe par i est demarre
  . finIterObjet() est faux et puis sectionCouranteIterObjet() designe
    la k_eme section de la j_eme sequence de sections (categorie) de i
- etat final :
  . finIterObjet() est vrai ou alors :
    + la section k etait la derniere section de la categorie j =>
      sectionCouranteIterObjet() designe la 1_ere section de la
      categorie j + 1
    + la section k n'etait pas la derniere section de la categorie j =>
      sectionCouranteIterObjet() designe la k+1_eme section de la
      categorie j
***********************************************************************/

struct StructSectionEDL *sectionCouranteIterObjet(IterateurObjet *);
/***********************************************************************
- sectionCouranteIterObjet(i) designe la k_eme section de la j_eme
  sequence de sections de l'iterateur d'objet i.
- pre-condition :
  . i est demarre
  . finIterObjet() est faux
***********************************************************************/

unsigned int finIterObjet(IterateurObjet *);
/***********************************************************************
- finIterObjet(i) est vrai <=> il n'y a plus de section dans la derniere
  sequence de sections de l'iterateur d'objet i.
- pre-condition :
  . i est demarre
***********************************************************************/

void afficherIterObjet(IterateurObjet *);
/***********************************************************************
afficherIterObjet(i) : affiche le contenu des sequences de sections de
l'iterateur d'objet i.
- etat initial :
  . i designe un iterateur d'objet
- etat final :
  . le contenu des sequences de sections de i a ete affiche
***********************************************************************/

void fermerIterObjet(IterateurObjet **);
/***********************************************************************
fermerIterObjet(i) : vide un iterateur d'objet.
d'objet.
- etat initial : *i designe un iterateur d'objet
- etat final :
  . l'iterateur est vide
  . la memoire utilisee par l'iterateur a ete liberee
***********************************************************************/

/***********************************************************************
Iterateur global :
- description :
  . permet d'acceder dans un ordre determine a l'ensemble des sections
    d'un ensemble d'objets
  . les sections sont regroupees par categorie, et dans chaque categorie
    par nom et type de section : l'iterateur global represente donc une
    sequence de categories, chaque categorie representant une sequence
    de couples nom-type, chaque couple nom-type etant associe a une
    sequence de sections (de meme categorie, de meme nom et de meme
    type)
  . a chaque couple nom-type <n, t> d'une categorie c est aussi associee
    une contrainte d'alignement correspondant a la plus forte contrainte
    d'alignement des sections de nom n et de type t (dans la
    categorie c)
  . au sein d'une categorie les couples nom-type sont distincts deux a
    deux mais n'ont pas d'ordre determine
  . les categories sont organisees en une sequence de categories
    ordonnee selon l'ordre sur les categories
- remarques :
  . l'iterateur global est initialement vide
  . une categorie (une sequence de couples nom-type) peut etre vide
  . la sequence de sections associee a un couple nom-type donne est
    necessairement non vide
***********************************************************************/

void viderIterGlobal(void);
/***********************************************************************
viderIterObjet(i) : vide l'iterateur global.
utilisee.
- etat initial : indifferent
- etat final :
  . la sequence de sections representee par l'iterateur global est vide
  . la memoire utilisee par l'iterateur global a ete liberee
***********************************************************************/

void ajouterAIterGlobal(struct StructSectionEDL *);
/***********************************************************************
ajouterAIterGlobal(s) : ajoute la section s a l'iterateur global.
- etat initial :
  . s designe une section de categorie c, de nom n, de type t
- etat final :
  . s a ete ajoutee a l'iterateur global, dans la categorie c, dans la
    sequence des sections associee au couple <n, t>
***********************************************************************/

void demarrerIterGlobalNom(void);
/***********************************************************************
demarrerIterGlobalNom() : demarre le parcours couple nom-type par couple
nom-type de la sequence
de sections representee par l'iterateur global.
- etat initial : indifferent
- etat final :
  . finIterGlobalNom() est vrai ou alors la sequence de sections
    courante est la 1ere sequence de sections (ayant meme nom et meme
    type) de la 1ere categorie de sections
***********************************************************************/

void avancerIterGlobalNom(void);
/***********************************************************************
avancerIterGlobalNom() : passe a la sequence de sections suivante dans
l'iterateur global.
- etat initial :
  . finIterGlobalNom() est faux et puis la sequence de sections courante
    est la i_eme sequence de sections de meme nom et meme type de la
    j_eme categorie de sections
- etat final :
  . finIterGlobalNom(i) est vrai ou alors :
    + la sequence de sections i etait la derniere sequence de sections
      de la categorie j =>
      la sequence de sections courante est la 1ere sequence de sections
      de la categorie j+1
    + la sequence de sections i n'etait pas la derniere sequence de
      sections de la categorie j =>
      la sequence de sections courante est la i+1_eme sequence de
      sections de meme nom et meme type de la categorie j
***********************************************************************/

unsigned int alignementCourantIterGlobal(void);
/***********************************************************************
- alignementCourantIterGlobal() est la contrainte d'alignement la plus
  elevee des sections de la sequence de sections courante dans
  l'iterateur global.
***********************************************************************/

unsigned int finIterGlobalNom(void);
/***********************************************************************
- finIterGlobalNom() est vrai <=> il n'y a plus de sequence de sections
  dans l'iterateur global.
***********************************************************************/

void demarrerIterGlobalSection(void);
/***********************************************************************
demarrerIterGlobalSection() : demarre le parcours section par section
de la sequence de sections courante dans l'iterateur global.
- etat initial :
  . finIterGlobalNom() est faux et puis la sequence de sections courante
    est la i_eme sequence de sections de meme nom et meme type de la
    j_eme categorie de sections
- etat final :
  . finIterGlobalSection() est faux et puis sectionCouranteIterGlobal()
    designe la 1ere section de la sequence de sections courante
***********************************************************************/

void avancerIterGlobalSection(void);
/***********************************************************************
avancerIterGlobalSection() : passe a la section suivante dans la
sequence de sections courante de l'iterateur global.
- etat initial :
  . finIterGlobalSection() est faux et puis sectionCouranteIterGlobal()
    designe la k_eme section de la i_eme sequence de sections de la
    j_eme categorie de section
- etat final :
  . finIterGlobalSection() est vrai ou alors :
    + la section k etait la derniere section de la sequence de
      sections i et la sequence de sections i etait la derniere sequence
      de sections de la categorie j =>
      sectionCouranteIterGlobal() designe la 1ere section de la
      sequence de sections i + 1 de la categorie j + 1
    + la section k etait la derniere section de la sequence de
      sections i et la sequence de sections i n'etait pas la derniere
      sequence de sections de la categorie j =>
      sectionCouranteIterGlobal() designe la 1ere section de la
      sequence de sections i + 1 de la categorie j
    + la section k n'etait pas la derniere section de la sequence de
      sections i =>
      sectionCouranteIterObjet() designe la k+1_eme section de la
      sequence de sections i de la categorie j
***********************************************************************/

struct StructSectionEDL *sectionCouranteIterGlobal(void);
/***********************************************************************
- sectionCouranteIterGlobal() designe la k_eme section de la i_eme
  sequence de sections (de meme nom et meme type) de la j_eme categorie
  de sections dans l'iterateur global.
- pre-condition :
  . finIterGlobalSection() est faux
***********************************************************************/

unsigned int finIterGlobalSection(void);
/***********************************************************************
- finIterGlobalSection() est vrai <=> il n'y a plus de section dans la
  i_eme sequence de sections (de meme nom et meme type) de la j_eme
  categorie de sections dans l'iterateur global
***********************************************************************/

void afficherIterGlobal(void);
/***********************************************************************
afficherIterGlobal() : affiche le contenu des sequences de sections de
l'iterateur global.
- etat initial :
  . indifferent
- etat final :
  . le contenu des sequences de sections de l'iterateur global a ete
    affiche
***********************************************************************/
