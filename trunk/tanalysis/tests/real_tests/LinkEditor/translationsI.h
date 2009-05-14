/***********************************************************************
translationsI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "section.h"

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "translationsP.h"
#endif

void ouvrirTableTranslation(struct StructSectionEDL *);
/***********************************************************************
ouvrirTableTranslation(s) : cree le descripteur specifique de la table
de translation s et traduit le contenu au format ELF de s vers le format
interne.
- etat initial :
  . s est une table de translation en cours d'ouverture
  . la partie commune du descripteur de la section est initialisee
- pre-condition :
  . la table de symboles et la section de code a completer associees a
    cette table de symboles sont ouvertes
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . le contenu de la section est disponible sous forme d'un tableau
    d'elements de type TranslationEDL
***********************************************************************/

void creerTableTranslation(struct StructSectionEDL *,
                           struct StructSectionEDL *,
                           struct StructSectionEDL *,
                           unsigned int);
/***********************************************************************
creerTableTranslation(s, ts, sc, n) : cree le descripteur specifique de
la table de translation s et alloue la memoire necessaire pour l'ajout
ulterieur de n informations de translation dans s.
- etat initial :
  . s est une table de translation en cours de creation
  . ts est la table de symboles associee a s
  . sc est la section de code que s doit permettre de completer
  . n represente le nb. d'informations de translation prevu pour le
    contenu de la section
  . la partie commune du descripteur de la section est initialisee
- pre-condition :
  . ts et sc ont ete creees au prealable
- etat final :
  . les informations specifiques au type de la section s ont ete
    initialisees
  . la memoire requise pour le contenu de la section a ete allouee
  . le contenu de la section est vide
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void copierContenuTranslation(struct StructSectionEDL *,
                              struct StructSectionEDL *);
/***********************************************************************
copierContenuTranslation(s, c) : copie le contenu au format interne de
la table de translation s dans la table de translation c
- etat initial :
  . s et c designent deux tables de translation, d'un meme objet ou de
    deux objets distincts
  . la section c a ete creee au prealable et a pour objet parent o
  . le contenu au format interne de c est vide
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

void cloreTableTranslation(struct StructSectionEDL *);
/***********************************************************************
cloreTableTranslation(s) : traduit le contenu au format interne de la
table de translation s vers le format ELF.
- etat initial :
  . s designe une table de translation en cours de cloture
  . le contenu de la section (au format interne) a ete elabore
  . les descripteurs ELF et libelf de la section ont ete crees
  . la table de symboles associee a s a ete cloturee
- pre-condition :
  . la cloture de la table de symboles associee a s a ete effectuee
- etat final :
  . le contenu de la section a ete traduit et recopie dans les
    structures de donnees au format ELF (libelf)
***********************************************************************/

void fermerTableTranslation(struct StructSectionEDL *);
/***********************************************************************
fermerTableTranslation(s) : libere la memoire utilisee par la table de
translation s.
- etat initial :
  . s designe une table de translation en cours de fermeture
- etat final :
  . si s n'est pas partagee, la memoire utilisee pour le contenu au
    format interne de la section a ete liberee
  . si s n'est pas partagee et est ouverte en sortie, la memoire
    utilisee pour le contenu au format ELF de la section a ete liberee
  . si s est partagee, la memoire utilisee pour le contenu au format ELF
    de la section a ete liberee
***********************************************************************/

void fusionnerTableTranslation(struct StructSectionEDL *,
                               struct StructSectionEDL *);
/***********************************************************************
fusionnerTableTranslation(tSource, tCible) :
- etat initial :
  . tSource est une table de translation ouverte en entree
  . tCible est une table de translation dont le contenu est en cours
    d'elaboration
- pre-conditions :
  . tCible n'est pas partagee par plusieurs objets
  . la fusion de la table des symboles associee a tCible est terminee
  . la fusion de la section a completer concernee par tCible est elle
    aussi terminee
- etat final :
  . le contenu de tSource a ete fusionne avec celui de tCible
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void completerSectionCode(struct StructSectionEDL *);
/***********************************************************************
completerSectionCode(tt) : complete tous les emplacements de la section
de code decrits par la table de translation tt.
- etat initial :
  . tt designe une table de translation concernant la section de code
    sc et associee a la table de symboles ts
  . sc contient des emplacements referencant chacun un symbole
    translatable decrit dans ts
- pre-condition :
  . s n'est pas partagee par plusieurs objets
- etat final :
  . les emplacements a completer de sc contiennent tous une adresse
    absolue definitive
  . la table de translation tt n'est plus d'aucune utilite et peut etre
    supprimee
***********************************************************************/
/* FIN SUPPRIMER FUSION */

void ajouterTranslation(struct StructSectionEDL *, TranslationEDL *,
                        TranslationEDL **);
/***********************************************************************
ajouterTranslation(ttc, ts, tc) : ajoute a la table de translation ttc
une information de translation dont le contenu est decrit par ts.
- etat initial :
  . ttc designe une table de translation
  . ts designe une information de translation a ajouter a ttc
  . ts est issue d'une table de translation tts
  . le symbole reference par ts appartient a la table de symboles
    associee a tts
- pre-conditions :
  . ttc n'est pas partagee par plusieurs objets
  . la memoire allouee pour ttc est suffisante pour accueillir la
    nouvelle information de translation
- etat final :
  . une information de translation au format interne representant
    l'information de translation ts a ete creee et ajoutee a ttc
  . le symbole reference par l'information de translation ajoutee
    appartient a la table de symboles associee a ttc
  . si tc != NULL, *tc designe la translation creee
***********************************************************************/

unsigned int tailleTableTranslation(struct StructSectionEDL *);
/***********************************************************************
tailleTableTranslation(s) est la taille courante de la section s, en nb.
d'informations de translation.
***********************************************************************/

void afficherTranslations(struct StructSectionEDL *section);
/***********************************************************************
afficherTranslations(s) : affiche le contenu de la table de
translation s.
- etat initial : s est une table de translation
- etat final : les informations de translation de s ont ete affichees
***********************************************************************/
