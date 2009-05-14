/***********************************************************************
segments.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef SEGMENTS_H
#define SEGMENTS_H

#include "section.h"

#define DEFAUT_TEXT   0x8000
#define DEFAUT_DATA   0x8000
#define DEFAUT_RODATA 0x8000
#define DEFAUT_BSS    0x8000

#define TAILLE_PAGE 0x8000

/**********************************************************************/
struct StructSegmentEDL {
   unsigned int type;
   unsigned long adresse;
   unsigned long position;
   unsigned int tailleMemoire;
   unsigned int tailleFichier;
   unsigned long adresse1ereSection;
   unsigned long position1ereSection;
   struct StructSectionEDL **sections;
   unsigned int nbSections;
};
typedef struct StructSegmentEDL SegmentEDL;
/***********************************************************************
SegmentEDL :
- type                  : TEXT | DATA | RODATA | BSS (voir segments.c)
- adresse               : adresse d'implantation du segment en memoire
- position              : position du segment dans le fichier ELF (c'est
                          a dire dans l'objet executable au format ELF)
- tailleMemoire         : nb. d'octets occupes par le segment en memoire
- tailleFichier         : nb. d'octets occupes par le segment dans le
                          fichier ELF
- adresse1ereSection    : adresse d'implantation en memoire de la 1ere
                          section du segment
- position1ereSection   : position de la 1ere section du segment dans le
                          fichier ELF
***********************************************************************/

#include "segmentsI.h"

#endif /* SEGMENTS_H */
