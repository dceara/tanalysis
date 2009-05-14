/***********************************************************************
symbolesP.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"
#include "chaines.h"
/* DEBUT SUPPRIMER IMPLANTATION */

STATIC void fusionnerSymboleLocal(SymboleEDL *,
                                  struct StructSectionEDL *,
                                  struct StructSectionEDL *);
/***********************************************************************
fusionnerSymboleLocal(sSource, tSource, tCible) : fusionne dans la table
de symboles tCible le symbole local sSource appartenant a la table de
symboles tSource.
- etat initial : 
  . sSource designe un symbole local de la table de symboles tSource
    ouverte en entree
  . tCible est la table de symboles resultat en cours d'elaboration
- etat final :
  . si sSource est un symbole de programme, un symbole sCible copie de
    sSource ete ajoute aux symboles locaux de tCible
  . si sSource est un symbole de section et si aucun symbole de meme
    type et de meme nom n'existait auparavant dans tCible, un symbole
    sCible copie de sSource a ete ajoute aux symboles locaux de tCible
  . si sSource est un symbole de section et si un symbole sCible de meme
    type et de meme nom existait deja dans tCible aucun nouveau symbole
    n'est ajoute a tCible
  . si la fusion a pu etre effectuee avec succes :
    + la section de definition et le cas echeant la valeur de sCible ont
      ete ajustees
    + sCible est l'image dans tCible du symbole sSource. Cette relation
      fonctionnelle a ete memorisee dans sSource
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER IMPLANTATION */

STATIC void fusionnerSymboleGlobal(SymboleEDL *,
                                   struct StructSectionEDL *,
                                   struct StructSectionEDL *,
                                   unsigned int *);
/***********************************************************************
fusionnerSymboleGlobal(sSource, tSource, tCible, e) : fusionne dans la
table de symboles tCible le symbole global sSource appartenant a la
table de symboles tSource.

- etat initial :
  . sSource designe un symbole global de la table de symboles tSource
    ouverte en entree
  . tCible est la table de symboles resultat en cours d'elaboration

- etat final (specification minimale) :
  . si aucun symbole de meme portee et de meme nom n'existait dans
    tCible, un symbole sCible copie de sSource a ete ajoute aux symboles
    globaux de tCible
  . si un symbole sCible de meme portee et de meme nom existait dans
    tCible, les cas suivants (sCible-sSource) ont ete traites :
    + indefini-indefini => sCible est inchange
    + indefini-defini   => la taille, le type, la valeur et la section
                           de definition de sCible ont ete mis a jour
    + defini-indefini   => sCible est inchange
    + defini-defini     => une erreur (double definition de symbole) a
                           ete affichee et *e = VRAI
  . si la fusion a pu etre effectuee avec succes :
    + la relation fonctionnelle qui associe le symbole sCible au symbole
      sSource a ete memorisee dans sSource

- etat final (specification etendue, avec prise en compte des symboles
  absolus (SHN_ABS) et communs (SHN_COMMON)) :
  . si un symbole sCible de meme portee et de meme nom existait dans
    tCible, la section de definition, la nature et la portee des deux
    symboles sCible et sSource ont ete considerees, chacun des deux
    symboles pouvant etre : indefini, global (= global, defini et
    translatable), absolu (= global, defini et absolu), commun.
    Les cas suivants (sCible-sSource) ont ete consideres :
    + indefini-indefini => sCible est inchange
    + indefini-global   => la taille, le type, la valeur et la section
                           de definition de sCible ont ete mis a jour
    + indefini-absolu   => la taille, le type, la valeur et la nature de
                           sCible ont ete mis a jour
    + indefini-commun   => la taille, le type, la valeur (contrainte
                           d'alignement) et la nature de sCible ont ete
                           mis a jour
    + global-indefini   => sCible est inchange
    + global-global     => une erreur (double definition de symbole) a
                           ete affichee et *e = VRAI
    + global-absolu     => une erreur (double definition de symbole) a
                           ete affichee et *e = VRAI
    + global-commun     => un avertissement a ete affiche si les tailles
                           des deux symboles ne sont pas identiques ou
                           si la valeur de sCible n'est pas compatible
                           avec la contrainte d'alignement (valeur) de
                           sSource
    + absolu-indefini   => sCible est inchange
    + absolu-global     => une erreur (double definition de symbole) a
                           ete affichee et *e = VRAI
    + absolu-absolu     => une erreur (double definition de symbole) a
                           ete affichee et *e = VRAI
    + absolu-commun     => un avertissement a ete affiche si les tailles
                           des deux symboles ne sont pas identiques ou
                           si la valeur de sCible n'est pas compatible
                           avec la contrainte d'alignement (valeur) de
                           sSource
    + commun-indefini   => sCible est inchange
    + commun-global     => un avertissement a ete affiche si les tailles
                           des deux symboles n'etaient pas identiques ou
                           si la valeur de sSource n'etait pas
                           compatible avec la contrainte d'alignement
                           (valeur) de sCible
                           la taille, le type, la valeur et la section
                           de definition de sCible ont ete mis a jour
    + commun-absolu     => un avertissement a ete affiche si les tailles
                           des deux symboles n'etaient pas identiques ou
                           si la valeur de sSource n'etait pas
                           compatible avec la contrainte d'alignement
                           (valeur) de sCible
                           la taille, le type, la valeur et la nature de
                           sCible ont ete mis a jour
    + commun-commun     => soient tailleCible0 et alignementCible0 la
                           taille et la contrainte d'alignement
                           initiales (avant fusion) de sCible,
                           tailleCible et alignementCible la taille et
                           la contrainte d'alignement finales de sCible,
                           tailleSource et alignementSource la taille et
                           la contrainte d'alignement de sSource :
                           tailleCible est le maximum de tailleCible0 et
                           tailleSource ; alignementCible est le maximum
                           de alignementCible0 et alignementSource
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */

STATIC void afficherTitreSymboles(void);
/***********************************************************************
afficherTitreSymboles() : affiche une ligne de titre decrivant les
informations affichees pour chaque symbole d'une table de symboles.
***********************************************************************/

STATIC void afficherSymbole(SymboleEDL, unsigned int);
/***********************************************************************
afficherSymbole(s, i) : affiche le symbole s d'index i.
- etat initial :
  . s est un symbole correctement forme (son nom est accessible dans la
    table de chaines associee a la table de symboles a la quelle
    appartient s)
  . i est l'index (ELF) de s dans la table de symboles a laquelle
    appartient s
- etat final : les informations decrivant le symbole ont ete affichees
***********************************************************************/

STATIC void libererFormatInterneSymboles(struct StructSectionEDL *);
/***********************************************************************
libererFormatInterneSymboles(s) : libere la memoire utilisee par la
table de symboles s.
- etat initial :
  . s designe une table de symboles
- etat final : 
  . la memoire allouee pour le contenu au format interne de s a ete
    liberee
***********************************************************************/

STATIC SymboleEDL *symboleGlobalSelonNom(struct StructSectionEDL *,
                                         struct StructChaineEDL *);
/***********************************************************************
- symboleGlobalSelonNom(s, n) est l'adresse du symbole global de nom n
  appartenant a la table de symboles s, s'il existe, la valeur NULL
  sinon.
***********************************************************************/

STATIC SymboleEDL *symboleSectionSelonNom(struct StructSectionEDL *,
                                          struct StructChaineEDL *);
/***********************************************************************
- symboleSectionSelonNom(s, n) : est l'adresse du symbole de section de
  nom n appartenant a la table de symboles s, s'il existe, la valeur
  NULL sinon.
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

STATIC void supprimerSymbole(struct StructSectionEDL *, SymboleEDL *);
/***********************************************************************
supprimerSymbole(t, s) : supprime le symbole s de la table de symboles
t.
- etat initial :
  . t designe une table de symboles
  . s designe un symbole de t
- pre-conditions :
  . t n'est pas partagee par plusieurs objets
  . s n'est plus utilise (n'est plus reference dans le reste du
    programme)
- etat final :
  . s a ete supprime de t et la memoire correspondante a ete liberee
***********************************************************************/
/* FIN SUPPRIMER FUSION */
