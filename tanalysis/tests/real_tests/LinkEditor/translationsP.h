/***********************************************************************
translationsP.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"
/* DEBUT SUPPRIMER FUSION */

STATIC void completerEmplacement(struct StructSectionEDL *,
                                 TranslationEDL);
/***********************************************************************
completerEmplacement(s, t) : complete le contenu de l'emplacement de la
section de code s decrit par l'information de translation t.
- etat initial :
  . s designe la section de code a completer
  . t est l'information de translation a appliquer
  . t appartient a une table de translation tt
- pre-conditions :
  . s n'est pas partagee par plusieurs objets
  . la table de symboles associee a la table de translation tt a ete
    implantee
- etat final :
  . le contenu de l'emplacement de s decrit par t a ete complete, sa
    valeur est definitive
***********************************************************************/
/* FIN SUPPRIMER FUSION */

STATIC void ajusterEmplacement(TranslationEDL, TranslationEDL,
                               struct StructSectionEDL *);
/***********************************************************************
ajusterEmplacement(ts, tc, sc) : ajuste le contenu d'un emplacement a
completer de la section de code sc decrit par les informations de
translation ts et tc.
- etat initial :
  . ts est une information de translation appartenant a une table de
    translation d'entree tts en cours de fusion dans la table de
    translation resultat ttc
  . tc est l'information de translation de ttc obtenue a partir de ts
  . sc est la section de code a completer associee a ttc
  . tc decrit un emplacement a completer e appartenant a sc
- pre-condition :
  . sc n'est pas partagee par plusieurs objets
  . la section de code sc et la table des symboles associee a ttc ont
    ete elaborees (fusionnees) au prealable
- etat final :
  . le contenu de l'emplacement a completer e a ete ajuste pour tenir
    compte de la valeur dans la section de code sc du symbole reference
    par e
***********************************************************************/

STATIC void ajusterOffset(TranslationEDL *, struct StructSectionEDL *);
/***********************************************************************
ajusterOffset(t, ss) : ajuste dans l'information de translation t
l'adresse de l'emplacement a completer decrit par t.
- etat initial :
  . ss est une section de code appartenant a un objet d'entree
  . ss a ete fusionnee dans la section de code resultat sc
  . t designe une information de translation issue de la table de
    translation ts decrivant les emplacements a completer de ss
  . t decrit un emplacement a completer es dans la section de code ss
  . a l'emplacement es de ss correspond l'emplacement ec de sc
  . l'adresse contenue dans t est l'adresse dans ss de l'emplacement es
- pre-condition :
  . l'elaboration de la section de code sc est terminee
- etat final :
  . l'adresse contenue dans t est l'adresse dans sc de l'emplacement ec
***********************************************************************/

STATIC unsigned long inversionBoutismeMot(unsigned long);
/***********************************************************************
inversionBoutismeMot(m) est le mot de 32 bits obtenu en inversant (par
symetrie) les 4 octets de m.
***********************************************************************/

STATIC unsigned short inversionBoutismeDemiMot(unsigned short);
/***********************************************************************
inversionBoutismeDemiMot(d) est le mot de 16 bits obtenu en inversant
(par symetrie) les 2 octets de d.
***********************************************************************/

STATIC unsigned long alignementMot(void *);
/***********************************************************************
alignementMot(d) est le mot de 32 bits aligne obtenu a partir des 4
octets successifs (dans l'ordre des adresses croissantes) situes a
l'adresse d.
***********************************************************************/

STATIC unsigned short alignementDemiMot(void *);
/***********************************************************************
alignementDemiMot(d) est le mot de 16 bits aligne obtenu a partir des 2
octets successifs (dans l'ordre des adresses croissantes) situes a
l'adresse d.
***********************************************************************/

STATIC void desalignerMot(unsigned long, void *);
/***********************************************************************
desalignerMot(d, r) ecrit a l'adresse r les 4 octets de d (dans l'ordre
PF -> pf).
***********************************************************************/

STATIC void desalignerDemiMot(unsigned short, void *);
/***********************************************************************
desalignerDemiMot(d, r) ecrit a l'adresse r les 2 octets de poids faible
de d (dans l'ordre PF -> pf).
- pre-condition : les deux octets de poids fort de d sont non
  significatifs
***********************************************************************/

STATIC void verifierValeur(long, long, long);
/***********************************************************************
verifierValeur(v, min, max) : verifie si la valeur v est comprise dans
l'intervalle [min .. max].
- etat initial : indifferent
- etat final : si v >= min et v <= max l'execution du programme continue
  normalement, sinon un message d'erreur est affiche et l'execution est
  interrompue
***********************************************************************/

STATIC void afficherTitreTranslations(void);
/***********************************************************************
afficherTitreTranslations() : affiche une ligne de titre decrivant les
informations affichees pour chaque entree d'une table de translation.
***********************************************************************/

STATIC void afficherTranslation(TranslationEDL);
/***********************************************************************
afficherTranslation(t) : affiche l'information de translation t.
- etat initial : t est une information de translation
- etat final : t a ete affichee
***********************************************************************/

STATIC void libererFormatInterneTranslation(struct StructSectionEDL *);
/***********************************************************************
libererFormatInterneTranslation(s) : libere la memoire utilisee par la
table de translation s.
- etat initial :
  . s designe une table de translation
- etat final : 
  . la memoire allouee pour le contenu au format interne de s a ete
    liberee
***********************************************************************/
