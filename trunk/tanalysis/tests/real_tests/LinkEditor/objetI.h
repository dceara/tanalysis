/***********************************************************************
objetI.h

P.Habraken - 22/02/2009
***********************************************************************/

#include "options.h"

#if defined AVEC_LIBEDL || defined EST_LIBEDL
#include "objetP.h"
#endif

void ouvrirObjet(ObjetEDL *, unsigned int);
/***********************************************************************
ouvrirObjet(o, le) : lit a partir d'un fichier le contenu au format ELF
de l'objet binaire translatable o et le traduit en structures de donnees
au format interne (EDL).
- etat initial :
  . o designe le descripteur d'un objet dont seul le nom no est
    initialise
  . le fichier de nom no contenant l'objet o est ouvert en entree
- etat final :
  . la libelf est initialisee
  . le descripteur au format interne de l'objet est initialise
  . l'iterateur de l'objet a ete cree
  . les sections de l'objet sont ouvertes et ont ete ajoutees a son
    iterateur d'objet ainsi qu'a l'iterateur global
  . si le = VRAI le contenu au format ELF des sections de l'objet est
    accessible en lecture ecriture, sinon il n'est accessible qu'en
    lecture seule et ne peut etre modifie
- remarque :
  . dans son etat actuel, ce programme ne permet pas d'ouvir un objet
    binaire executable
***********************************************************************/

void creerObjet(ObjetEDL *, ObjetEDL *, TypeObjetEDL, unsigned int);
/***********************************************************************
creerObjet(oResultat, oReference, t, n) : cree un nouvel objet oResultat
sur la base des caracteristiques de l'objet oReference ; oResultat est
destine a contenir n sections et a etre ecrit dans un fichier resultat.
- etat initial :
  . oResultat designe le descripteur d'un objet dont seul le nom no est
    initialise
  . oReference designe un objet ouvert en entree dont les attributs
    boutisme, processeur, version et flags seront utilises pour
    initialiser le descripteur de oResultat
  . t est le type de l'objet cree (binaire translatable ou executable)
  . n est le nombre de sections initialement prevu pour l'objet cree
- etat final :
  . le descripteur au format interne de l'objet oResultat est initialise
  . la memoire requise pour les n sections de l'objet a ete allouee
  . l'iterateur de l'objet a ete cree
  . la table des noms de sections a ete creee et ajoutee a l'iterateur
    d'objet
  . le contenu de l'objet cree est vide
***********************************************************************/

void copierObjet(ObjetEDL *, ObjetEDL *, TypeObjetEDL);
/***********************************************************************
copierObjet(oSource, oDest, t) : cree un nouvel objet oDest identique a
l'objet oSource.
Les deux objets partagent les meme structures de donnees et le meme
contenu : sections, segments, iterateur (copie superficielle).
- etat initial :
  . oSource designe un objet ouvert en entree ou en sortie ; son contenu
    est accessible via son descripteur au format interne
  . oDest designe un objet dont seul le nom est initialise
- etat final :
  . oDest est ouvert en sortie et a pour type t
  . oDest a memes caracteristiques que oSource
  . oDest est une copie superficielle de oSource : oSource et oDest
    partagent les memes descripteurs au format interne de section et de
    segment, ainsi que le meme iterateur d'objet
- remarque :
  . oDest disposera apres cloture de ses propres descripteurs au format
    ELF (et libelf) d'objet, de section, de contenu et de segment
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void dupliquerObjet(ObjetEDL *, ObjetEDL *, TypeObjetEDL);
/***********************************************************************
dupliquerObjet(oSource, oDest, t) : cree un nouvel objet oDest identique
a l'objet oSource.
Le contenu du nouvel objet est represente par des structures de donnees
distinctes de celles de l'objet original (copie profonde).
- etat initial :
  . oSource designe un objet ouvert en entree ou en sortie ; son contenu
    est accessible via son descripteur au format interne
  . l'iterateur d'objet de oSource represente la sequence de ses
    sections ordonnee par categorie de section
  . oDest designe un objet dont seul le nom est initialise
- etat final :
  . oDest est ouvert en sortie et a pour type t
  . oDest est un duplicata de oSource
  . oDest possede ses propres descripteurs de section et de segment au
    format interne, ainsi que son propre iterateur d'objet
  . l'iterateur d'objet de oDest represente la sequence des sections de
    oDest ordonnee par categorie de section
  . toutes les sections de oSource ont ete dupliquees dans oDest
- remarque :
  . oDest disposera apres cloture de ses propres descripteurs au format
    ELF (et libelf) d'objet, de section, de contenu et de segment
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

void fusionnerObjet(ObjetEDL *);
/***********************************************************************
fusionnerObjet(o) : fusionne un ensemble d'objets d'entree dans l'objet
resultat designe par o.
- etat initial :
  . tous les objets a fusionner sont ouverts
  . l'objet resultat o a ete cree ; il est vide
  . les sections des objets a fusionner sont accessibles a partir de la
    sequence de sections representee par l'iterateur global (sequence
    ordonnee par categorie, nom et par type de section)
- etat final :
  . toutes les sections (de tous les objets) d'entree ayant meme nom et
    meme type ont ete fusionnees ensemble (exception faite des tables de
    noms de section)
  . le resultat de cette fusion est une section unique creee dans o pour
    chaque couple < nom, type > de section d'entree distinct
  . les relations entre les sections creees dans o ont ete
    re-construites
***********************************************************************/
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

void implanterObjet(ObjetEDL *);
/***********************************************************************
implanterObjet(o) : realise l'implantation de l'objet o.
- etat initial :
  . o designe un objet translatable, ouvert en entree ou en sortie ; son
    contenu est accessible via son descripteur au format interne
  . les tables de symboles de o ne contiennent aucun symbole indefini
  . les adresses d'implantation des segments et de la pile et le nom du
    symbole du point d'entree ont ete determines
  . l'iterateur d'objet de o represente la sequence de ses sections
    ordonnee par categorie de section
- pre-condition :
  . o n'est pas la copie superficielle d'un autre objet
- etat final (version minimale) :
  . les sections allouables et les tables de symboles de o ont ete
    implantees
  . les sections de code ont ete completees
  . les sections de translation ont ete supprimees et retirees de
    l'iterateur d'objet
- etat final (version etendue, avec ajout d'une section de pile et
  resolution des symboles communs (SHN_COMMON)) :
  . les symboles communs declares dans les tables de symboles de o ont
    fait l'objet d'une reservation de place dans l'une des sections
    vides de o et ont ete resolus
  . une section de pile a ete creee et ajoutee a l'iterateur d'objet
***********************************************************************/
/* FIN SUPPRIMER FUSION */

void cloreObjet(ObjetEDL *);
/***********************************************************************
cloreObjet(o) : traduit le contenu au format interne de l'objet o vers
le format ELF et ecrit le resultat obtenu dans un fichier.
- etat initial : o designe un objet resultat (ouvert en sortie)
  . le fichier f destine a contenir l'objet o est ouvert en sortie
  . les sections de o ont ete creees et completees
   // DEBUT SUPPRIMER FUSION //
  . si o est un objet executable, les segments requis ont ete crees et
    completes
   // FIN SUPPRIMER FUSION //
  . l'iterateur d'objet de o represente la sequence de ses sections
    ordonnee par categorie de section
- etat final :
  . la libelf a ete initialisee
  . le descripteur ELF de l'objet a ete cree
   // DEBUT SUPPRIMER FUSION //
  . si o est un objet executable, les segments requis ont ete clotures
   // FIN SUPPRIMER FUSION //
  . les sections de o ont ete cloturees
  . le contenu de l'objet o a ete ecrit dans le fichier f
***********************************************************************/

void fermerObjet(ObjetEDL *);
/***********************************************************************
fermerObjet(o) : libere la memoire utilisee par l'objet o.
- etat initial :
  . o designe un objet ouvert en entree ou en sortie
  . si o est ouvert en sortie, son contenu a ete cloture et ecrit dans
    le fichier ELF correspondant
- etat final :
  . si o n'est pas la copie superficielle d'un autre objet :
    + les sections de o sont fermees
   // DEBUT SUPPRIMER FUSION //
    + si o est un objet executable ouvert en sortie ses segments sont
      fermes
   // FIN SUPPRIMER FUSION //
    + la memoire allouee pour l'objet, ses sections et ses segments (le
      cas echeant) a ete liberee
  . le descripteur ELF de l'objet est ferme
***********************************************************************/

void afficherObjet(ObjetEDL *);
/***********************************************************************
afficherObjet(o) : affiche un resume du contenu de l'objet o.
- etat initial : o designe un objet ouvert en entree
- etat final : les informations decrivant l'objet o et les sections
  qu'il contient ainsi que la table des symboles (la premiere si o en
  contient plusieurs) ont ete affichees
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void ajouterSectionBss(ObjetEDL *, struct StructSectionEDL **);
/***********************************************************************
ajouterSectionBss(o, s) : ajoute une nouvelle section vide de nom .bss a
l'objet o.
- etat initial :
  . o designe un objet executable, ouvert en entree ou en sortie
- pre-conditions :
  . o ne contient aucune section vide de nom .bss
  . o n'est pas la copie superficielle d'un autre objet
- etat final :
  . une nouvelle section de nom .bss a ete ajoutee a o
  . la nouvelle section a ete initialisee (creee) et ajoutee a
    l'iterateur d'objet de o
  . si s != NULL, *s designe la nouvelle section
***********************************************************************/
/* FIN SUPPRIMER FUSION */

struct StructSectionEDL *sectionSelonIndex(ObjetEDL *, unsigned int);
/***********************************************************************
sectionSelonIndex(o, i) designe le descripteur de la section dont
  l'index dans la table des entetes de sections de l'objet o est i ;
  sectionSelonIndex(o, i) = NULL <=> i est l'une des valeurs speciales
  prevues dans le format ELF (SHN_UNDEF, etc.).
- pre-condition : une section d'index i existe dans l'objet o, ou alors
  i est l'une des valeurs speciales prevues dans le format ELF
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

struct StructSectionEDL *sectionSelonNomEtType(ObjetEDL *,
                                               char *, unsigned int);
/***********************************************************************
sectionSelonNomEtType(o, n, t) designe le descripteur de la 1ere section
  trouvee dont le nom dans la table des noms de section de l'objet o
  est n et dont le type est t ;
  sectionSelonNomEtType(o, n, t) = NULL <=> (n = NULL et o ne contient
  aucune section de type t) ou alors o ne contient aucune section de nom
  n et de type t
- remarque : la specification ELF prevoit explicitement qu'un meme objet
  peut contenir plusieurs sections de meme nom. Elle ne dit par contre
  pas si plusieurs sections de meme nom doivent etre de meme type...
***********************************************************************/
/* FIN SUPPRIMER FUSION */

unsigned int sontCompatibles(ObjetEDL *, ObjetEDL *);
/***********************************************************************
sontCompatibles(o1, o2) vaut vrai <=> les deux objets designes par o1 et
o2 :
  . sont de meme type (binaire translatable / executable / etc.)
  . ont meme boutisme ("big endian" / "little endian")
  . ont meme taille de mot (32 bits / 64 bits)
  . ont ete compiles pour le meme syteme (SYSV / ARM / etc.) et pour la
    meme architecture (ARM / SPARC / INTEL / etc.)
  . ont meme version de fichier et de descripteur ELF
- pre-condition : o1 et o2 sont deux objets ouverts en entree
***********************************************************************/

unsigned int ontMemeBoutismeHoteObjet(ObjetEDL *);
/***********************************************************************
ontMemeBoutismeHoteObjet(o) vaut vrai <=> l'objet o a meme boutisme que
la machine pour laquelle est compile l'editeur de liens
***********************************************************************/

void memoriserNomObjet(ObjetEDL *, char *);
/***********************************************************************
memoriserNomObjet(o) : memorise dans l'objet o le nom du fichier
correspondant
***********************************************************************/
/* DEBUT SUPPRIMER FUSION */

void memoriserAdresseStack(unsigned long);
/***********************************************************************
memoriserAdresseStack(adresse) : memorise l'adresse de base requise pour
la pile.
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

void memoriserNomPointEntree(char *);
/***********************************************************************
memoriserNomPointEntree(n) : memorise le nom du point d'entree n.
- etat initial : indifferent
- etat final :
  . si un nom avait ete precedemment memorise, la memoire correspondante
    a ete liberee
  . le nom du point d'entree n a ete memorise
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

void libererNomPointEntree(void);
/***********************************************************************
libererNomPointEntree() : libere la memoire utilisee par le nom du point
d'entree memorise.
- etat initial : indifferent
- etat final :
  . si un nom avait ete precedemment memorise, la memoire correspondante
    a ete liberee
***********************************************************************/
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

char *nomPointEntree(void);
/***********************************************************************
nomPointEntree() est le nom du point d'entree qui a ete memorise au
demarrage du programme (ou le nom du point d'entree par defaut si aucun
nom n'a ete memorise)
***********************************************************************/
/* FIN SUPPRIMER FUSION */
