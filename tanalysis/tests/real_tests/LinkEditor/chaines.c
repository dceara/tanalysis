/***********************************************************************
chaines.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "chaines.h"
#include "section.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "chainesP.h"
#ifdef AVEC_LIBEDL
#include "_chainesI.h"
#endif
#else
#include "upcallI.h"
#endif

#define FAUX 0
#define VRAI 1

/**********************************************************************/
void ouvrirTableChaines(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   char *chainesSource;
   ChaineEDL **chainesCible;
   unsigned int nbMax;
   unsigned int index;
   char *chaine;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* allocation du descripteur specifique */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->tableChaines =
                  (TableChainesEDL *)calloc(1, sizeof(TableChainesEDL));
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* allocation du tableau d'elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   chainesSource = (char *)section->contenuElf;
   nbMax = 0;
   for (index = 0 ; index != section->tailleContenuElf ; index++) {
      if (chainesSource[index] == '\0') {
         nbMax++;
      }
   }
   chainesCible = (ChaineEDL **)calloc(nbMax, sizeof(ChaineEDL *));
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation du tableau d'elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->tableChaines->chaines = chainesCible;
   section->tableChaines->nbChaines = 0;
   section->tableChaines->nbMax = nbMax;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* creation des chaines et copie depuis le format ELF */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   index = 0;
   while (index != section->tailleContenuElf) {
      chaine = chainesSource + index;
      ajouterChaine(section, chaine, index, FAUX, NULL);
      index += strlen(chaine) + 1;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void creerTableChaines(SectionEDL *section, unsigned int nbMax) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->contenuElf = NULL;
   section->tableChaines =
                  (TableChainesEDL *)calloc(1, sizeof(TableChainesEDL));
   section->tableChaines->chaines =
                       (ChaineEDL **)calloc(nbMax, sizeof(ChaineEDL *));
   section->tableChaines->nbChaines = 0;
   section->tableChaines->nbMax = nbMax;
   ajouterChaine(section, "", 0, FAUX, NULL);
   /* FIN COMPLETER FUSION IMPLANTATION */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void copierContenuChaines(SectionEDL *tableSource,
                          SectionEDL *tableCible) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   ChaineEDL **chainesSource;
   unsigned int nbChaines;
   unsigned int i;

   chainesSource = tableSource->tableChaines->chaines;
   nbChaines = tableSource->tableChaines->nbChaines;

   for (i = 1 ; i != nbChaines ; i++) {
      ajouterChaine(tableCible, chainesSource[i]->chaine,
                    chainesSource[i]->indexElf,
                    chainesSource[i]->estSuffixe, NULL);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void cloreTableChaines(SectionEDL *section) {
/**********************************************************************/
   
}

/**********************************************************************/
void fermerTableChaines(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   /* liberation du contenu au format interne */
   libererFormatInterneChaines(section);
   
   /* liberation du contenu au format ELF */
   if (section->objetParent->mode == ELF_C_WRITE) {
      if (section->contenuElf != NULL) {
         free(section->contenuElf);
         section->contenuElf = NULL;
      }
   }
}
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerTableChaines(SectionEDL *sectionSource,
                           SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   ChaineEDL **chainesSource;
   unsigned int nbChaines;
   unsigned int i;

   chainesSource = sectionSource->tableChaines->chaines;
   nbChaines = sectionCible->tableChaines->nbChaines;

   for (i = 1 ; i != nbChaines ; i++) {
      ajouterChaine(sectionCible, chainesSource[i]->chaine, 0, 0, NULL);
   }
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
void ajouterChaine(SectionEDL *section, char *chaineC,
                   unsigned int index, unsigned int estSuffixe,
                   ChaineEDL **chaineAjoutee) {
/**********************************************************************/
   /* LIBEDL FUSION */
   /* DEBUT COMPLETER IMPLANTATION */
   ChaineEDL **chaines;
   unsigned int nbChaines;
   unsigned int nbMax;
   ChaineEDL *chaineEdl;
   unsigned int i;

   chaines = section->tableChaines->chaines;
   nbChaines = section->tableChaines->nbChaines;

   for (i = 0 ; i != nbChaines ; i++) {
      if (strcmp(chaines[i]->chaine, chaineC) == 0) break;
   }
   if (i != nbChaines) {
      chaineEdl = chaines[i];
   }
   else {
      nbMax = section->tableChaines->nbMax;

      if (nbChaines == nbMax) {
         nbMax = nbMax + 1;
         chaines = (ChaineEDL **)realloc(chaines,
                                         nbMax * sizeof(ChaineEDL *));
         section->tableChaines->nbMax = nbMax;
         section->tableChaines->chaines = chaines;
      }
      chaineEdl = creationChaine(chaineC, index, estSuffixe);
      chaines[nbChaines] = chaineEdl;
         
      section->tableChaines->nbChaines = nbChaines + 1;
      if (chaineAjoutee == NULL) {
         chaineEdl->nbReferences = 0;
      }
   }

   if (chaineAjoutee != NULL) {
      chaineEdl->nbReferences++;
      *chaineAjoutee = chaineEdl;
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void supprimerChaine(SectionEDL *section, ChaineEDL *chaine) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   TableChainesEDL *tableChaines;
   ChaineEDL **chaines;
   unsigned int nbChaines;
   unsigned int i;

   tableChaines = section->tableChaines;
   chaines = tableChaines->chaines;
   nbChaines = tableChaines->nbChaines;

   for (i = 0 ; i != nbChaines ; i++) {
      if (chaines[i] == chaine) break;
   }
   if (i != nbChaines) {
      if (chaines[i]->nbReferences != 0) {
         chaines[i]->nbReferences--;
      }
      if (chaines[i]->nbReferences == 0) {
         libererChaine(&(chaines[i]));
         chaines[i] = chaines[nbChaines - 1];
         tableChaines->nbChaines = nbChaines - 1;
      }
   }
   else {
      printf ( "%s%s%s%s%s%s%s\n",
              "panique (supprimerChaine) ! : ",
              "la chaine ",
              chaine->chaine,
              " n'appartient pas a la table de chaines ",
              section->nom->chaine,
              " de l'objet ",
              section->objetParent->nom);
      exit(1);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void extraireChaineSelonIndex(SectionEDL *section, unsigned int index,
                              ChaineEDL **chaine) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   char *chainesSource;
   unsigned int tailleSource;
   ChaineEDL **chainesCible;
   unsigned int nbChaines;
   char *chaineSource;
   unsigned int i;

   tailleSource = section->tailleContenuElf;

   if (index < tailleSource) {
      chainesSource = (char *)section->contenuElf;
      chainesCible = section->tableChaines->chaines;

      if (chainesSource[index] == '\0') {
         /* chaine vide */
         *chaine = chainesCible[0];
      }
      else {
         /* recherche d'une chaine non vide d'index i */
         nbChaines = section->tableChaines->nbChaines;

         for (i = nbChaines - 1 ; i != 0 ; i--) {
            if (chainesCible[i]->indexElf == index) break;
         }
         if (i != 0) {
            /* une chaine d'index i a ete trouvee */
            chainesCible[i]->nbReferences++;
            *chaine = chainesCible[i];
         }
         else {
            /*
            aucune chaine d'index i n'a ete trouvee : il s'agit donc
            d'un suffixe d'une autre chaine
            */
            chaineSource = chainesSource + index;
            ajouterChaine(section, chaineSource, index, VRAI, chaine);
         }
      }
   }
   else {
      /* index invalide */
      printf ( "%s%s%d%s%s%s%s\n",
              "panique (extraireChaineSelonIndex) ! : ",
              "aucune chaine d'index ",
              index,
              " n'existe dans la table de chaines ",
              section->nom->chaine,
              " de l'objet ",
              section->objetParent->nom);
      exit(1);
   }
}

/**********************************************************************/
unsigned int tailleTableChaines(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   return section->tableChaines->nbChaines;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}

/**********************************************************************/
void dupliquerChaine(ChaineEDL *chaineSource, ChaineEDL **chaineCible) {
/**********************************************************************/
   /* LIBEDL FUSION */
   /* DEBUT COMPLETER IMPLANTATION */
   char *chaineC;

   if (chaineSource != NULL) {
      chaineC = chaineSource->chaine;
      *chaineCible = creationChaine(chaineC, chaineSource->indexElf,
                                    chaineSource->estSuffixe);
   }
   else {
      *chaineCible = NULL;
   }
   /* FIN COMPLETER IMPLANTATION */
}

/**********************************************************************/
ChaineEDL *creationChaine(char *chaineC, unsigned int index,
                          unsigned int estSuffixe) {
/**********************************************************************/
   /* LIBEDL FUSION */
   /* DEBUT COMPLETER IMPLANTATION */
   ChaineEDL *chaine;
   unsigned int longueur;

   chaine = (ChaineEDL *)calloc(1, sizeof(ChaineEDL));
   chaine->indexElf = index;
   chaine->estSuffixe = estSuffixe;
   longueur = strlen(chaineC);
   chaine->chaine = (char *)malloc(longueur + 1);
   memcpy(chaine->chaine, chaineC, longueur + 1);
   chaine->longueur = longueur;

   return chaine;
   /* FIN COMPLETER IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return NULL;
   */
}

/**********************************************************************/
void libererChaine(ChaineEDL **chaine) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   if (*chaine != NULL) {
      if ((*chaine)->chaine != NULL) {
         free((*chaine)->chaine);
      }
      free(*chaine);
      *chaine = NULL;
   }
}

/**********************************************************************/
void afficherChaines(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   char *chaine;
   unsigned int index;
   unsigned int estSuffixe;
   unsigned int longueur;
   unsigned int nbReferences;
   unsigned int i;

   printf("%s", section->objetParent->nom);
   if (section->nom != NULL) {
      printf(":%s", section->nom->chaine);
   }
   printf("\n");

   printf("%s%s%s%s%s%s\n", " numero | ", "index | ", "estSuffixe | ",
          "longueur | ", "nbRef | ", "chaine");
   printf("%s%s%s%s%s%s\n", "--------|-", "------|-", "-----------|-",
          "---------|-", "------|-", "------");

   chaine = section->tableChaines->chaines[0]->chaine;
   index = section->tableChaines->chaines[0]->indexElf;
   estSuffixe = section->tableChaines->chaines[0]->estSuffixe;
   longueur = section->tableChaines->chaines[0]->longueur;
   nbReferences = section->tableChaines->chaines[0]->nbReferences;
   printf("   %3d  | %5d |      %d     |   %3d    |  %2d   | %s\n",
          0, index, estSuffixe, longueur, nbReferences, chaine);

   for (i = 1 ; i != section->tableChaines->nbChaines ; i++) {
      chaine = section->tableChaines->chaines[i]->chaine;
      index = section->tableChaines->chaines[i]->indexElf;
      estSuffixe = section->tableChaines->chaines[i]->estSuffixe;
      longueur = section->tableChaines->chaines[i]->longueur;
      nbReferences = section->tableChaines->chaines[i]->nbReferences;
      printf("   %3d  | %5d |      %d     |   %3d    |  %2d   | %s\n",
             i, index, estSuffixe, longueur, nbReferences, chaine);
   }

   printf("\n");
}

/**********************************************************************/
STATIC unsigned int estSuffixe(ChaineEDL *petiteChaine,
                               ChaineEDL *grandeChaine) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   unsigned int decalage;

   decalage = grandeChaine->longueur - petiteChaine->longueur;
   return strcmp(petiteChaine->chaine,
                 grandeChaine->chaine + decalage) == 0;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return FAUX;
   */
}

/**********************************************************************/
STATIC void libererFormatInterneChaines(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   TableChainesEDL *tableChaines;
   unsigned int i;

   if (section->tableChaines != NULL) {
      /* liberation de la structure de donnees globale */
      tableChaines = section->tableChaines;
      if (tableChaines->chaines != NULL) {
         /* liberation du tableau de chaines */
         for (i = 0 ; i != tableChaines->nbChaines ; i++) {
            libererChaine(&(tableChaines->chaines[i]));
         }
         free(tableChaines->chaines);
      }
      free(section->tableChaines);
      section->tableChaines = NULL;
   }
}
