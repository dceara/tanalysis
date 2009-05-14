/***********************************************************************
translations.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <elf.h>
#include <libelf/libelf.h>
#include <stdio.h>
#include <stdlib.h>

#include "translations.h"
#include "objet.h"
#include "section.h"
#include "symboles.h"
#include "chaines.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "translationsP.h"
#ifdef AVEC_LIBEDL
#include "_translationsI.h"
#endif
#else
#include "upcallI.h"
#endif

/**********************************************************************/
void ouvrirTableTranslation(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   ObjetEDL *objet;
   TableTranslationEDL *tableTranslation;
   unsigned int nbTranslations;
   Elf32_Rel *translationsSource;
   TranslationEDL *translationsCible;
   unsigned int indexTableSymboles;
   unsigned int indexSectionCode;
   SectionEDL *tableSymboles;
   SectionEDL *sectionCode;
   unsigned int indexSymbole;
   unsigned int type;
   unsigned int i;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* allocation du descripteur specifique */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableTranslation =
             (TableTranslationEDL *)malloc(sizeof(TableTranslationEDL));
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* allocation des elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   nbTranslations = section->tailleContenuElf / sizeof(Elf32_Rel);
   translationsCible = (TranslationEDL *)calloc(nbTranslations,
                                                sizeof(TranslationEDL));
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* copie des elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   translationsSource = (Elf32_Rel *)section->contenuElf;
   objet = section->objetParent;
   indexTableSymboles = section->descripteurElf->sh_link;
   tableSymboles = sectionSelonIndex(objet, indexTableSymboles);
   for (i = 0 ; i < nbTranslations ; i++) {
      translationsCible[i].adresse = translationsSource[i].r_offset;
      indexSymbole = ELF32_R_SYM(translationsSource[i].r_info);
      translationsCible[i].symbole = symboleSelonIndex(tableSymboles,
                                                       indexSymbole);
      type = ELF32_R_TYPE(translationsSource[i].r_info);
      translationsCible[i].type = type;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation des elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableTranslation->translations = translationsCible;
   tableTranslation->nbTranslations = nbTranslations;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /*
   memorisation de la table de symboles associee et de la section de
   code a completer
   */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableTranslation->tableSymboles = tableSymboles;
   indexSectionCode = section->descripteurElf->sh_info;
   sectionCode = sectionSelonIndex(objet, indexSectionCode);
   tableTranslation->sectionCode = sectionCode;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation du descripteur specifique */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->tableTranslation = tableTranslation;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void creerTableTranslation(struct StructSectionEDL *section,
                           struct StructSectionEDL *tableSymboles,
                           struct StructSectionEDL *sectionCode,
                           unsigned int nbMax) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->contenuElf = NULL;
   section->tableTranslation =
          (TableTranslationEDL *)calloc(1, sizeof(TableTranslationEDL));
   section->tableTranslation->translations =
                (TranslationEDL *)calloc(nbMax, sizeof(TranslationEDL));
   section->tableTranslation->nbTranslations = 0;
   section->tableTranslation->tableSymboles = tableSymboles;
   section->tableTranslation->sectionCode = sectionCode;
   /* FIN COMPLETER FUSION IMPLANTATION */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void copierContenuTranslation(SectionEDL *tableSource,
                              SectionEDL *tableCible) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   TranslationEDL *translationsSource;
   unsigned int nbTranslations;
   unsigned int i;

   translationsSource = tableSource->tableTranslation->translations;
   nbTranslations = tableSource->tableTranslation->nbTranslations;

   for (i = 0 ; i != nbTranslations ; i++) {
      ajouterTranslation(tableCible, &(translationsSource[i]), NULL);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void cloreTableTranslation(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   Elf32_Shdr *descripteurElf;
   Elf_Data *descripteurContenu;
   TranslationEDL *translationsSource;
   unsigned int nbTranslations;
   SectionEDL *tableSymboles;
   Elf32_Rel *translationsCible;
   unsigned indexSymboleSource;
   unsigned int i;

   translationsSource = section->tableTranslation->translations;
   nbTranslations = section->tableTranslation->nbTranslations;

   tableSymboles = section->tableTranslation->tableSymboles;

   /* allocation de la memoire requise */
   section->contenuElf = calloc(nbTranslations, sizeof(Elf32_Rel));

   /* traduction et recopie des informations de translation */
   translationsCible = (Elf32_Rel *)section->contenuElf;
   for (i = 0 ; i != nbTranslations ; i++) {
      translationsCible[i].r_offset = translationsSource[i].adresse;
      indexSymboleSource = indexSymbole(tableSymboles,
                                        translationsSource[i].symbole);
      translationsCible[i].r_info =
                   ELF32_R_INFO(indexSymboleSource,
                                translationsSource[i].type);
   }

   section->tailleContenuElf = nbTranslations * sizeof(Elf32_Rel);

   /* elaboration descripteur ELF */
   descripteurElf = section->descripteurElf;
   descripteurElf->sh_link =
                     section->tableTranslation->tableSymboles->indexElf;
   descripteurElf->sh_info =
                       section->tableTranslation->sectionCode->indexElf;

   /* elaboration descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_buf = section->contenuElf;
   descripteurContenu->d_type = ELF_T_REL;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void fermerTableTranslation(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   /* liberation du contenu au format interne */
   libererFormatInterneTranslation(section);
   
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
void fusionnerTableTranslation(SectionEDL *sectionSource,
                               SectionEDL *sectionCible) {
/**********************************************************************/
   TableTranslationEDL *tableTranslationSource;
   TranslationEDL *translationsSource;
   unsigned int nbTranslationsSource;
   SectionEDL *sectionCodeSource;
   TableTranslationEDL *tableTranslationCible;
   SectionEDL *sectionCodeCible;
   TranslationEDL translationSource;
   TranslationEDL *translationCible;
   unsigned int i;

   tableTranslationSource = sectionSource->tableTranslation;
   translationsSource = tableTranslationSource->translations;
   nbTranslationsSource = tableTranslationSource->nbTranslations;
   sectionCodeSource = tableTranslationSource->sectionCode;

   tableTranslationCible = sectionCible->tableTranslation;
   sectionCodeCible = tableTranslationCible->sectionCode;

   for (i = 0 ; i != nbTranslationsSource ; i++) {
      /* DEBUT COMPLETER FUSION */
      translationSource = translationsSource[i];
      ajouterTranslation(sectionCible, &translationSource,
                         &translationCible);
      ajusterOffset(translationCible, sectionCodeSource);
      ajusterEmplacement(translationSource, *translationCible,
                         sectionCodeCible);
      /* FIN COMPLETER FUSION */
   }
}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void completerSectionCode(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   TableTranslationEDL *tableTranslation;
   TranslationEDL *translations;
   unsigned int nbTranslations;
   SectionEDL *sectionCode;
   unsigned int i;

   tableTranslation = section->tableTranslation;

   translations = tableTranslation->translations;
   nbTranslations = tableTranslation->nbTranslations;
   sectionCode = tableTranslation->sectionCode;

   for (i = 0 ; i != nbTranslations ; i++) {
      completerEmplacement(sectionCode, translations[i]);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void ajouterTranslation(SectionEDL *sectionCible,
                        TranslationEDL *translationSource,
                        TranslationEDL **translationResultat) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   TableTranslationEDL *tableTranslation;
   TranslationEDL *translations;
   unsigned int nbTranslations;
   
   tableTranslation = sectionCible->tableTranslation;
   translations = tableTranslation->translations;
   nbTranslations = tableTranslation->nbTranslations;

   translations[nbTranslations].adresse = translationSource->adresse;
   translations[nbTranslations].type = translationSource->type;

   translations[nbTranslations].symbole =
                               translationSource->symbole->symboleImage;

   tableTranslation->translations = translations;
   tableTranslation->nbTranslations = nbTranslations + 1;

   if (translationResultat != NULL) {
      *translationResultat = &(translations[nbTranslations]);
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
unsigned int tailleTableTranslation(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   return section->tableTranslation->nbTranslations;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}

/**********************************************************************/
void afficherTranslations(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   TranslationEDL *translations;
   unsigned int nbTranslations;
   unsigned int i;

   printf("%s:%s\n", section->objetParent->nom, section->nom->chaine);

   afficherTitreTranslations();

   translations = section->tableTranslation->translations;
   nbTranslations = section->tableTranslation->nbTranslations;
   for (i = 0 ; i != nbTranslations ; i++) {
      afficherTranslation(translations[i]);
   }

   printf("\n");
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void completerEmplacement(SectionEDL *sectionCode,
                                 TranslationEDL translation) {
/**********************************************************************/
   /*
   
   unsigned int memeBoutisme;
   SymboleEDL *symboleReference;
   unsigned int typeAjustement;
   unsigned long instruction;
   unsigned int operation;
   void *code;
   long TouV;
   unsigned long S;
   long A;
   long X;

   
   memeBoutisme = ontMemeBoutismeHoteObjet(sectionCode->objetParent);

   code = sectionCode->contenuElf + translation.adresse;
   symboleReference = translation.symbole;
   typeAjustement = translation.type;

   TouV = (long)(symboleReference->valeur);

   switch (typeAjustement) {

      case R_ARM_PC24:
         if (memeBoutisme) {
            instruction = alignementMot(code);
         }
         else {
            instruction = inversionBoutismeMot(alignementMot(code));
         }
         operation = instruction & 0xFF000000;

         S = sectionCode->adresse;
         A = (long)(S + translation.adresse);

         X = (long)(instruction & 0x00FFFFFF);
         X = (X << 8) >> 8;  

         X = X + (TouV - A) / 4;
         verifierValeur(X, -(1 << 23), (1 << 23) - 1);
         X &= 0x00FFFFFF;    

         instruction = operation | X;
         if (memeBoutisme) {
            desalignerMot(instruction, code);
         }
         else {
            desalignerMot(inversionBoutismeMot(instruction), code);
         }
         break;

      case R_ARM_ABS32:
         if (memeBoutisme) {
            X = alignementMot(code);
         }
         else {
            X = inversionBoutismeMot(alignementMot(code));
         }
         
         X = X + TouV;

         if (memeBoutisme) {
            desalignerMot(X, code);
         }
         else {
            desalignerMot(inversionBoutismeMot(X), code);
         }
         break;

      case R_ARM_ABS16:
         if (memeBoutisme) {
            X = alignementDemiMot(code);
         }
         else {
            X = inversionBoutismeDemiMot(alignementDemiMot(code));
         }
         
         X = X + TouV;
         verifierValeur(X, 0, 0xFFFF);

         if (memeBoutisme) {
            desalignerDemiMot((unsigned short)X, code);
         }
         else {
            desalignerDemiMot(inversionBoutismeDemiMot((unsigned short)X),
                              code);
         }
         break;

      case R_ARM_ABS8:
         X = *(unsigned char *)code;
         
         X = X + TouV;
         verifierValeur(X, 0, 0xFF);

         *(unsigned char *)code = X;
         break;
   } */
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC void ajusterEmplacement(TranslationEDL translationSource,
                               TranslationEDL translationCible,
                               SectionEDL *sectionCodeCible) {
/**********************************************************************/
   unsigned int memeBoutisme;
   unsigned int typeAjustement;
   unsigned long instruction;
   unsigned int operation;
   void *code;
   SymboleEDL *symboleSource;
   SectionEDL *sectionDefinitionSource;
   unsigned int h;
   long x;

}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC void ajusterOffset(TranslationEDL *translation,
                          SectionEDL *sectionCodeSource) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   translation->adresse +=
                   sectionCodeSource->sectionProgramme->decalageContenu;
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC unsigned long inversionBoutismeMot(unsigned long donnee) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   unsigned int resultat;
   unsigned char *pDonnee = (unsigned char *)&donnee;
   unsigned char *pResultat = (unsigned char *)&resultat;

   *pResultat = *(pDonnee + 3);
   *(pResultat + 1) = *(pDonnee + 2);
   *(pResultat + 2) = *(pDonnee + 1);
   *(pResultat + 3) = *pDonnee;
   return resultat;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}

/**********************************************************************/
STATIC unsigned short inversionBoutismeDemiMot(unsigned short donnee) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   unsigned short resultat;
   unsigned char *pDonnee = (unsigned char *)&donnee;
   unsigned char *pResultat = (unsigned char *)&resultat;

   *pResultat = *(pDonnee + 1);
   *(pResultat + 1) = *(pDonnee);
   return resultat;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}

/**********************************************************************/
STATIC unsigned long alignementMot(void *donnee) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   unsigned long resultat;
   unsigned char *pDonnee = (unsigned char *)donnee;
   unsigned char *pResultat = (unsigned char *)&resultat;

   *pResultat = *pDonnee;
   *(pResultat + 1) = *(pDonnee + 1);
   *(pResultat + 2) = *(pDonnee + 2);
   *(pResultat + 3) = *(pDonnee + 3);
   return resultat;
}

/**********************************************************************/
STATIC unsigned short alignementDemiMot(void *donnee) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   unsigned short resultat;
   unsigned char *pDonnee = (unsigned char *)donnee;
   unsigned char *pResultat = (unsigned char *)&resultat;

   *pResultat = *pDonnee;
   *(pResultat + 1) = *(pDonnee + 1);
   return resultat;
}

/**********************************************************************/
STATIC void desalignerMot(unsigned long donnee, void *resultat) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   unsigned char *pDonnee = (unsigned char *)&donnee;
   unsigned char *pResultat = (unsigned char *)resultat;

   *pResultat = *pDonnee;
   *(pResultat + 1) = *(pDonnee + 1);
   *(pResultat + 2) = *(pDonnee + 2);
   *(pResultat + 3) = *(pDonnee + 3);
}

/**********************************************************************/
STATIC void desalignerDemiMot(unsigned short donnee, void *resultat) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   unsigned char *pDonnee = (unsigned char *)&donnee;
   unsigned char *pResultat = (unsigned char *)resultat;

   *pResultat = *pDonnee;
   *(pResultat + 1) = *(pDonnee + 1);
}

/**********************************************************************/
STATIC void verifierValeur(long valeur, long min, long max) {
/**********************************************************************/
   if (valeur < min || valeur > max) {
      printf ( "%s%s\n", "erreur : ",
              "depassement capacite translation ");
      exit(1);
   }
}

/**********************************************************************/
STATIC void afficherTitreTranslations(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   printf("%s\n",
          " offset  | type | nom symbole");
   printf("---------|------|------------\n");
}

/**********************************************************************/
STATIC void afficherTranslation(TranslationEDL translation) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   printf("%08lx |  %2d  | %-30s\n", 
          translation.adresse,
          translation.type,
          translation.symbole->nom->chaine);
}

/**********************************************************************/
STATIC void libererFormatInterneTranslation(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   if (section->tableTranslation != NULL) {
      /* liberation de la structure de donnees globale */
      if (section->tableTranslation->translations != NULL) {
         /* liberation du tableau de translations */
         free(section->tableTranslation->translations);
      }
      free(section->tableTranslation);
      section->tableTranslation = NULL;
   }
}
