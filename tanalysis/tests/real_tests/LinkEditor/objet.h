/***********************************************************************
objet.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef OBJET_H
#define OBJET_H

#include <elf.h>

#include "section.h"
/* DEBUT SUPPRIMER FUSION */
#include "segments.h"
/* FIN SUPPRIMER FUSION */
#include "iterateur.h"
/* DEBUT SUPPRIMER FUSION */

#define DEFAUT_STACK  0x800000
#define DEFAUT_ENTREE "main"
/* FIN SUPPRIMER FUSION */

typedef unsigned int ModeOuvertureEDL;
/* ELF_C_READ, ELF_C_WRITE, ... */

typedef unsigned int BoutismeEDL;
/* ELFDATA2LSB, ELFDATA2MSB, ... */

typedef unsigned int ClasseEDL;
/* ELFCLASS32, ELFCLASS64, ... */

typedef unsigned int SystemeEDL;
/* ELFOSABI_ARM, ELFOSABI_SYSV, ... */

typedef unsigned int TypeObjetEDL;
/* ET_EXEC, ET_REL, ... */

typedef unsigned int ProcesseurEDL;
/* EM_SPARC, EM_ARM, ... */

typedef unsigned int VersionEDL;
/* EV_NONE, EV_CURRENT, ... */

typedef unsigned int FlagsObjetEDL;
/* pour l'archit. ARM : EF_ARM_HASENTRY, EF_ARM_SYMSARESORTED, ... */

/**********************************************************************/
struct StructObjetEDL {
   char *nom;
   int fd;
   ModeOuvertureEDL mode;
   BoutismeEDL boutisme;
   ClasseEDL classe;
   SystemeEDL systeme;
   TypeObjetEDL type;
   ProcesseurEDL processeur;
   VersionEDL version;
   FlagsObjetEDL flags;
   unsigned int estCopie;
   struct StructSectionEDL **sections;
   unsigned int nbSections;
   unsigned int nbMax;
   struct StructSectionEDL *tableNomsSection;
   /* DEBUT SUPPRIMER FUSION */
   struct StructSegmentEDL *segments;
   unsigned int nbSegments;
   unsigned long pointEntree;
   unsigned long tailleFichier;
   /* FIN SUPPRIMER FUSION */
   struct StructIterateurObjet *iterateur;
   Elf *elf;
   Elf32_Ehdr *ehdr;
};
typedef struct StructObjetEDL ObjetEDL;
/***********************************************************************
ObjetEDL :
- nom                : nom de l'objet
- fd                 : descripteur du fichier contenant l'objet
- mode               : mode d'ouverture de l'objet : lecture, ecriture,
                       etc.
- boutisme           : boutisme de l'objet : petit bout, gros bout
- systeme            : systeme d'exploitation pour laquel a ete cree
                       l'objet
- type               : type de l'objet : binaire translatable, binaire
                       executable, etc.
- processeur         : architecture materielle pour laquelle l'objet a
                       ete cree
- version            : version ELF de l'objet
- flags              : indicateurs booleens precisant la nature de
                       l'objet (dependent de l'architecture materielle)
- estCopie           : estCopie = VRAI <=> l'objet a ete cree par copie
                       superficielle d'un autre objet (les deux objets
                       partagent en particulier les memes sections)
- sections           : les sections de l'objet (hors section nulle)
- nbSections         : nombre de sections courant
- nbMax              : nombre de sections maximum
- tableNomsSection   : table des noms de section
// DEBUT SUPPRIMER FUSION //
- segments           : les segments de l'objet (si objet executable)
- nbSegments         : nombre de segments (si objet executable)
- pointEntree        : adresse du point d'entree (si objet executable)
- tailleFichier      : taille occupee par l'objet dans le fichier
// FIN SUPPRIMER FUSION //
- iterateur          : iterateur de l'objet
- elf                : descripteur libelf de l'objet
- ehdr               : descripteur ELF de l'objet
***********************************************************************/

#include "objetI.h"

#endif /* OBJET_H */
