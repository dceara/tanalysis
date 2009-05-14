/***********************************************************************
chaines.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef CHAINES_H
#define CHAINES_H

/**********************************************************************/
struct StructChaineEDL {
   unsigned int indexElf;
   unsigned int estSuffixe;
   unsigned int longueur;
   unsigned int nbReferences;
   char *chaine;
};
typedef struct StructChaineEDL ChaineEDL;
/***********************************************************************
ChaineEDL :
- indexElf     : index de la chaine dans la table de chaines au format ELF
- estSuffixe   : estSuffixe = VRAI <=> le contenu de la chaine est un
                 suffixe d'une autre chaine de la meme table de chaines
                 et ne devrait pas etre enregistre dans la table de
                 chaines au format ELF
- longueur     : nb. de caracteres de la chaine (hors '\0')
- nbReferences : nombre d'utilisations de cette chaine
- chaine       : designe la chaine C representant le contenu de cette
                 chaine
***********************************************************************/

/**********************************************************************/
struct StructTableChainesEDL {
   ChaineEDL **chaines;
   unsigned int nbChaines;
   unsigned int nbMax;
};
typedef struct StructTableChainesEDL TableChainesEDL;
/***********************************************************************
TableChainesEDL :
- chaines               : les chaines au format interne (EDL)
- nbChaines             : nombre de chaines courant
- nbMax                 : nombre maximum de chaines
***********************************************************************/

#include "chainesI.h"

#endif /* CHAINES_H */
