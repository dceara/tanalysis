/***********************************************************************
symboles.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "symboles.h"
#include "objet.h"
#include "section.h"
#include "chaines.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "symbolesP.h"
#ifdef AVEC_LIBEDL
#include "_symbolesI.h"
#endif
#else
#include "upcallI.h"
#endif

#define FAUX 0
#define VRAI 1

/**********************************************************************/
void ouvrirTableSymboles(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   ObjetEDL *objet;
   TableSymbolesEDL *tableSymboles;
   Elf32_Sym *symbolesSource;
   SymboleEDL **symbolesLocaux;
   SymboleEDL **symbolesGlobaux;
   unsigned int nbLocaux;
   unsigned int nbGlobaux;
   unsigned int nbSymboles;
   unsigned int indexTableChaines;
   SectionEDL *tableChaines;
   unsigned int indexNom;
   SectionEDL *sectionDefinition;
   unsigned int i, j;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* allocation du descripteur specifique */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableSymboles =
                (TableSymbolesEDL *)calloc(1, sizeof(TableSymbolesEDL));
   /* FIN COMPLETER FUSION IMPLANTATION */
   
   /* allocation des elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   nbSymboles = section->tailleContenuElf / sizeof(Elf32_Sym);
   nbLocaux = section->descripteurElf->sh_info;
   nbGlobaux = nbSymboles - nbLocaux;
   symbolesLocaux = (SymboleEDL **)calloc(nbLocaux,
                                          sizeof(SymboleEDL *));
   symbolesGlobaux = (SymboleEDL **)calloc(nbGlobaux,
                                           sizeof(SymboleEDL *));
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* acces a la table de chaines associee */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   objet = section->objetParent;
   indexTableChaines = section->descripteurElf->sh_link;
   tableChaines = sectionSelonIndex(objet, indexTableChaines);
   /* FIN COMPLETER FUSION IMPLANTATION */
   
   /* copie des elements : symboles locaux */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   symbolesSource = (Elf32_Sym *)section->contenuElf;
   for (i = 0 ; i != nbLocaux ; i++) {
      symbolesLocaux[i] = (SymboleEDL *)calloc(1, sizeof(SymboleEDL));
      symbolesLocaux[i]->valeur = symbolesSource[i].st_value;
      symbolesLocaux[i]->taille = symbolesSource[i].st_size;
      symbolesLocaux[i]->portee =
                               ELF32_ST_BIND(symbolesSource[i].st_info);
      symbolesLocaux[i]->type =
                               ELF32_ST_TYPE(symbolesSource[i].st_info);
      symbolesLocaux[i]->natureDefinition = symbolesSource[i].st_shndx;
      switch (symbolesSource[i].st_shndx) {
         case SHN_UNDEF:
         case SHN_ABS:
            sectionDefinition = NULL;
            break;
         default:
            sectionDefinition =
                   sectionSelonIndex(objet, symbolesSource[i].st_shndx);
      }
      symbolesLocaux[i]->sectionDefinition = sectionDefinition;

      if (symbolesLocaux[i]->type == STT_SECTION) {
         /* symbole de section => nom du symbole = nom de la section */
         dupliquerChaine(sectionDefinition->nom,
                         &(symbolesLocaux[i]->nom));
      }
      else {
         indexNom = symbolesSource[i].st_name;
         extraireChaineSelonIndex(tableChaines, indexNom,
                                  &(symbolesLocaux[i]->nom));
      }
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* copie des elements : symboles globaux */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   for (i = 0, j = nbLocaux ; i != nbGlobaux ; i++, j++) {
      symbolesGlobaux[i] = (SymboleEDL *)calloc(1, sizeof(SymboleEDL));
      symbolesGlobaux[i]->valeur = symbolesSource[j].st_value;
      symbolesGlobaux[i]->taille = symbolesSource[j].st_size;
      symbolesGlobaux[i]->portee =
                               ELF32_ST_BIND(symbolesSource[j].st_info);
      symbolesGlobaux[i]->type =
                               ELF32_ST_TYPE(symbolesSource[j].st_info);
      symbolesGlobaux[i]->natureDefinition = symbolesSource[j].st_shndx;
      switch (symbolesSource[j].st_shndx) {
         case SHN_UNDEF:
         case SHN_ABS:
         case SHN_COMMON:
            sectionDefinition = NULL;
            break;
         default:
            sectionDefinition =
                   sectionSelonIndex(objet, symbolesSource[j].st_shndx);
      }
      symbolesGlobaux[i]->sectionDefinition = sectionDefinition;
      indexNom = symbolesSource[j].st_name;
      extraireChaineSelonIndex(tableChaines, indexNom,
                               &(symbolesGlobaux[i]->nom));

      /* pour la fusion des symboles communs */
      symbolesGlobaux[i]->tableSymbolesOrigine = section;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation des elements */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableSymboles->symbolesLocaux = symbolesLocaux;
   tableSymboles->symbolesGlobaux = symbolesGlobaux;
   tableSymboles->nbLocaux = nbLocaux;
   tableSymboles->nbGlobaux = nbGlobaux;
   tableSymboles->nbMaxLocaux = nbLocaux;
   tableSymboles->nbMaxGlobaux = nbGlobaux;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation de la table de chaines associee */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableSymboles->tableChaines = tableChaines;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation du descripteur specifique */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->tableSymboles = tableSymboles;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void creerTableSymboles(SectionEDL *section, SectionEDL *tableChaines,
                        unsigned int nbMax) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SymboleEDL symboleNul;

   section->contenuElf = NULL;
   section->tableSymboles =
                (TableSymbolesEDL *)calloc(1, sizeof(TableSymbolesEDL));
   section->tableSymboles->symbolesLocaux =
                     (SymboleEDL **)calloc(nbMax, sizeof(SymboleEDL *));
   section->tableSymboles->symbolesGlobaux =
                     (SymboleEDL **)calloc(nbMax, sizeof(SymboleEDL *));
   section->tableSymboles->nbLocaux = 0;
   section->tableSymboles->nbGlobaux = 0;
   section->tableSymboles->nbMaxLocaux = nbMax;
   section->tableSymboles->nbMaxGlobaux = nbMax;
   section->tableSymboles->tableChaines = tableChaines;

   symboleNul.nom = creationChaine("", 0, FAUX);
   symboleNul.valeur = 0;
   symboleNul.taille = 0;
   symboleNul.portee = STB_LOCAL;
   symboleNul.type = STT_NOTYPE;
   symboleNul.natureDefinition = SHN_UNDEF;
   symboleNul.sectionDefinition = NULL;
   symboleNul.tableSymbolesOrigine = NULL;
   ajouterSymbole(section, &symboleNul, NULL);
   libererChaine(&(symboleNul.nom));
   /* FIN COMPLETER FUSION IMPLANTATION */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void copierContenuSymboles(SectionEDL *tableSource,
                           SectionEDL *tableCible) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   SymboleEDL **locauxSource, **globauxSource;
   unsigned int nbLocaux, nbGlobaux;
   unsigned int i;

   locauxSource = tableSource->tableSymboles->symbolesLocaux;
   globauxSource = tableSource->tableSymboles->symbolesGlobaux;
   nbLocaux = tableSource->tableSymboles->nbLocaux;
   nbGlobaux = tableSource->tableSymboles->nbGlobaux;

   for (i = 1 ; i != nbLocaux ; i++) {
      ajouterSymbole(tableCible, locauxSource[i], NULL);
   }

   for (i = 0 ; i != nbGlobaux ; i++) {
      ajouterSymbole(tableCible, globauxSource[i], NULL);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void cloreTableSymboles(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   Elf32_Shdr *descripteurElf;
   Elf_Data *descripteurContenu;
   SymboleEDL **symbolesLocaux, **symbolesGlobaux;
   SymboleEDL **symbolesTemp;
   unsigned int nbLocaux, nbGlobaux;
   SectionEDL *tableChaines;
   Elf32_Sym *symbolesCible;
   unsigned int i, j;

   symbolesLocaux = section->tableSymboles->symbolesLocaux;
   symbolesGlobaux = section->tableSymboles->symbolesGlobaux;
   nbLocaux = section->tableSymboles->nbLocaux;
   nbGlobaux = section->tableSymboles->nbGlobaux;

   tableChaines = section->tableSymboles->tableChaines;

   /* allocation de la memoire requise */
   section->contenuElf = calloc(nbLocaux + nbGlobaux,
                                sizeof(Elf32_Sym));
   symbolesCible = (Elf32_Sym *)section->contenuElf;

   /* pour la mise en ordre des symboles locaux */
   symbolesTemp = (SymboleEDL **)calloc(nbLocaux, sizeof(SymboleEDL *));

   j = 1;

   /* traduction et recopie des symboles de section */
   for (i = 1 ; i != nbLocaux ; i++) {
      if (symbolesLocaux[i]->type == STT_SECTION) {
         symbolesCible[j].st_name = 0;
         symbolesCible[j].st_value = symbolesLocaux[i]->valeur;
         symbolesCible[j].st_size = symbolesLocaux[i]->taille;
         symbolesCible[j].st_info =
                                ELF32_ST_INFO(symbolesLocaux[i]->portee,
                                              symbolesLocaux[i]->type);
         symbolesCible[j].st_shndx =
                         symbolesLocaux[i]->sectionDefinition->indexElf;
         symbolesTemp[j] = symbolesLocaux[i];
         j++;
      }
   }

   /* traduction et recopie des autres symboles locaux */
   symbolesCible = (Elf32_Sym *)section->contenuElf;
   for (i = 1 ; i != nbLocaux ; i++) {
      if (symbolesLocaux[i]->type != STT_SECTION) {
         symbolesCible[j].st_name = symbolesLocaux[i]->nom->indexElf;
         symbolesCible[j].st_value = symbolesLocaux[i]->valeur;
         symbolesCible[j].st_size = symbolesLocaux[i]->taille;
         symbolesCible[j].st_info =
                                ELF32_ST_INFO(symbolesLocaux[i]->portee,
                                              symbolesLocaux[i]->type);
         if (symbolesLocaux[i]->sectionDefinition != NULL) {
            symbolesCible[j].st_shndx =
                         symbolesLocaux[i]->sectionDefinition->indexElf;
         }
         else {
            symbolesCible[j].st_shndx =
                                    symbolesLocaux[i]->natureDefinition;
         }
         symbolesTemp[j] = symbolesLocaux[i];
         j++;
      }
   }

   /* mise en ordre des symboles locaux */
   for (i = 1 ; i != nbLocaux ; i++) {
      symbolesLocaux[i] = symbolesTemp[i];
   }
   free(symbolesTemp);

   /* traduction et recopie des symboles globaux */
   for (i = 0, j = nbLocaux ; i != nbGlobaux ; i++, j++) {
      symbolesCible[j].st_name = symbolesGlobaux[i]->nom->indexElf;
      symbolesCible[j].st_value = symbolesGlobaux[i]->valeur;
      symbolesCible[j].st_size = symbolesGlobaux[i]->taille;
      symbolesCible[j].st_info =
                               ELF32_ST_INFO(symbolesGlobaux[i]->portee,
                                             symbolesGlobaux[i]->type);
      if (symbolesGlobaux[i]->sectionDefinition != NULL) {
         symbolesCible[j].st_shndx =
                        symbolesGlobaux[i]->sectionDefinition->indexElf;
      }
      else {
         symbolesCible[j].st_shndx =
                                   symbolesGlobaux[i]->natureDefinition;
      }
   }

   /* traduction de la taille de la table de symboles */
   section->tailleContenuElf =
                             (nbLocaux + nbGlobaux) * sizeof(Elf32_Sym);

   /* elaboration descripteur ELF */
   descripteurElf = section->descripteurElf;
   descripteurElf->sh_link =
                         section->tableSymboles->tableChaines->indexElf;
   descripteurElf->sh_info = section->tableSymboles->nbLocaux;

   /* elaboration descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_buf = section->contenuElf;
   descripteurContenu->d_type = ELF_T_SYM;
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void fermerTableSymboles(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   /* liberation du contenu au format interne */
   libererFormatInterneSymboles(section);
   
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
void fusionnerTableSymboles(SectionEDL *sectionSource,
                            SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   SymboleEDL **symbolesLocaux;
   SymboleEDL **symbolesGlobaux;
   unsigned int nbLocaux;
   unsigned int nbGlobaux;
   unsigned int i;
   unsigned int erreur;

   symbolesLocaux = sectionSource->tableSymboles->symbolesLocaux;
   nbLocaux = sectionSource->tableSymboles->nbLocaux;

   for (i = 1 ; i != nbLocaux ; i++) {
      fusionnerSymboleLocal(symbolesLocaux[i],
                            sectionSource, sectionCible);
   }

   symbolesGlobaux = sectionSource->tableSymboles->symbolesGlobaux;
   nbGlobaux = sectionSource->tableSymboles->nbGlobaux;

   erreur = FAUX;
   for (i = 0 ; i != nbGlobaux ; i++) {
      fusionnerSymboleGlobal(symbolesGlobaux[i],
                             sectionSource, sectionCible, &erreur);
   }
   if (erreur) {
      exit(1);
   }
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
void ajouterSymbole(SectionEDL *section, SymboleEDL *symboleSource,
                    SymboleEDL **symboleResultat) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   TableSymbolesEDL *tableSymboles;
   SymboleEDL **symboles;
   SymboleEDL *symboleCible;
   unsigned int nbSymboles;
   unsigned int nbMax;
   SectionEDL *tableChainesCible;
   SectionEDL *sectionDefinitionSource;
   
   tableSymboles = section->tableSymboles;
   tableChainesCible = tableSymboles->tableChaines;

   if (symboleSource->portee == STB_LOCAL) {
      symboles = tableSymboles->symbolesLocaux;
      nbSymboles = tableSymboles->nbLocaux;
      nbMax = tableSymboles->nbMaxLocaux;
   }
   else {
      symboles = tableSymboles->symbolesGlobaux;
      nbSymboles = tableSymboles->nbGlobaux;
      nbMax = tableSymboles->nbMaxGlobaux;
   }

   if (nbSymboles == nbMax) {
      nbMax = nbMax + 1;
      symboles =
                (SymboleEDL **)realloc(symboles,
                                       nbMax * sizeof(SymboleEDL *));
   }
   symboleCible = (SymboleEDL *)calloc(1, sizeof(SymboleEDL));

   if (symboleSource->type == STT_SECTION) {
      /* symbole de section => nom du symbole = nom de la section */
      dupliquerChaine(symboleSource->sectionDefinition->nom,
                      &(symboleCible->nom));
   }
   else {
      ajouterChaine(tableChainesCible, symboleSource->nom->chaine, 0,
                    FAUX, &(symboleCible->nom));
   }
   symboleCible->valeur = symboleSource->valeur;
   symboleCible->taille = symboleSource->taille;
   symboleCible->portee = symboleSource->portee;
   symboleCible->type = symboleSource->type;
   symboleCible->natureDefinition = symboleSource->natureDefinition;
   sectionDefinitionSource = symboleSource->sectionDefinition;
   if (sectionDefinitionSource != NULL) {
      symboleCible->sectionDefinition =
                                  sectionDefinitionSource->sectionImage;
   }
   symboleSource->symboleImage = symboleCible;

   /* pour la fusion des symboles communs */
   symboleCible->tableSymbolesOrigine =
                                    symboleSource->tableSymbolesOrigine;

   symboles[nbSymboles] = symboleCible;

   if (symboleSource->portee == STB_LOCAL) {
      tableSymboles->symbolesLocaux = symboles;
      tableSymboles->nbLocaux = nbSymboles + 1;
      tableSymboles->nbMaxLocaux = nbMax;
   }
   else {
      tableSymboles->symbolesGlobaux = symboles;
      tableSymboles->nbGlobaux = nbSymboles + 1;
      tableSymboles->nbMaxGlobaux = nbMax;
   }

   if (symboleResultat != NULL) {
      *symboleResultat = symboleCible;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void promouvoirSymbolesCommuns(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   static SectionEDL *sectionBss = NULL;
   ObjetEDL *objet;
   TableSymbolesEDL *tableSymboles;
   SymboleEDL **symboles;
   unsigned int nbSymboles;
   unsigned int i;

   objet = section->objetParent;
   tableSymboles = section->tableSymboles;
   symboles = tableSymboles->symbolesGlobaux;
   nbSymboles = tableSymboles->nbGlobaux;

   for (i = 0 ; i != nbSymboles ; i++) {
      if (symboles[i]->natureDefinition == SHN_COMMON) {
         if (sectionBss == NULL) {
            sectionBss = sectionSelonNomEtType(objet,
                                               ".bss", SHT_NOBITS);
            if (sectionBss == NULL) {
               ajouterSectionBss(objet, &sectionBss);
            }
         }
         etendreSectionVide(sectionBss, symboles[i]->taille,
                                        symboles[i]->valeur,
                                        &(symboles[i]->valeur));
         symboles[i]->sectionDefinition = sectionBss;
      }
   }
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void ajusterSymbolesCRT(SectionEDL *section,
                        unsigned long bssStart, unsigned long bssEnd) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SymboleEDL *symbole;
   ChaineEDL nom;

   nom.chaine = (char *)malloc(sizeof(BSS_START));
   strcpy(nom.chaine, BSS_START);
   symbole = symboleGlobalSelonNom(section, &nom);
   if (symbole != NULL && symbole->natureDefinition == SHN_UNDEF) {
      symbole->valeur = bssStart;
      symbole->natureDefinition = SHN_ABS;
   }
   free(nom.chaine);

   nom.chaine = (char *)malloc(sizeof(BSS_END));
   strcpy(nom.chaine, BSS_END);
   symbole = symboleGlobalSelonNom(section, &nom);
   if (symbole != NULL && symbole->natureDefinition == SHN_UNDEF) {
      symbole->valeur = bssEnd;
      symbole->natureDefinition = SHN_ABS;
   }
   free(nom.chaine);

   nom.chaine = (char *)malloc(sizeof(END));
   strcpy(nom.chaine, END);
   symbole = symboleGlobalSelonNom(section, &nom);
   if (symbole != NULL && symbole->natureDefinition == SHN_UNDEF) {
      supprimerSymbole(section, symbole);
   }
   free(nom.chaine);
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void implanterTableSymboles(SectionEDL *section) {
/**********************************************************************/
   ObjetEDL *objet;
   TableSymbolesEDL *tableSymboles;
   SymboleEDL **symboles;
   unsigned int nbSymboles;
   SectionEDL *sectionDefinition;
   unsigned int nbIndefinis;
   unsigned int i;

   tableSymboles = section->tableSymboles;

   /* symboles locaux */
   /* DEBUT COMPLETER IMPLANTATION */
   symboles = tableSymboles->symbolesLocaux;
   nbSymboles = tableSymboles->nbLocaux;

   for (i = 1 ; i != nbSymboles ; i++) {
      sectionDefinition = symboles[i]->sectionDefinition;
      if (sectionDefinition != NULL) {
         /* le symbole est translatable */
         symboles[i]->valeur += sectionDefinition->adresse;
      }
      else {
         /* le symbole n'est pas translatable */
      }
   }
   /* FIN COMPLETER IMPLANTATION */

   /* symboles globaux */
   /* DEBUT COMPLETER IMPLANTATION */
   objet = section->objetParent;
   symboles = tableSymboles->symbolesGlobaux;
   nbSymboles = tableSymboles->nbGlobaux;
   nbIndefinis = 0;

   for (i = 0 ; i != nbSymboles ; i++) {
      sectionDefinition = symboles[i]->sectionDefinition;
      if (sectionDefinition != NULL) {
         /* le symbole est translatable et defini */
         symboles[i]->valeur += sectionDefinition->adresse;
         if (symboles[i]->portee == STB_WEAK) {
            symboles[i]->portee = STB_GLOBAL;
         }
         if (strcmp(symboles[i]->nom->chaine,
                    nomPointEntree()) == 0) {
            /* memorisation de l'adresse du point d'entree */
            objet->pointEntree = symboles[i]->valeur;
         }
      }
      else {
         /* le symbole est non translatable ou non defini */
         switch (symboles[i]->natureDefinition) {
            case SHN_UNDEF:
               /* le symbole est indefini */
               printf ( "%s%s%s%s%s%s%s%s\n", "erreur : ",
                       "le symbole global ",
                       symboles[i]->nom->chaine,
                       " (section ",
                       section->nom->chaine,
                       ", objet ",
                       objet->nom,
                       ") est indefini");
               nbIndefinis += 1;
               break;
            case SHN_ABS:
               /* le symbole est un symbole absolu */
               break;
            case SHN_COMMON:
               /* le symbole est un symbole commun */
               printf ( "%s%s%s%s%s%s%s%s\n",
                       "panique (implanterTableSymboles) ! : ",
                       "le symbole de programme ",
                       symboles[i]->nom->chaine,
                       " est un symbole commun (",
                       section->objetParent->nom,
                       ":",
                       section->nom->chaine,
                       ")");
               exit(1);
               break;
            default:
               /* symbole non translatable de nature inconnue (?) */
               break;
         }
      }
   }
   if (nbIndefinis != 0) {
      exit(1);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
unsigned int indexSymbole(SectionEDL *section, SymboleEDL *symbole) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SymboleEDL **symboles;
   unsigned int nbSymboles;
   unsigned int i;

   if (symbole->portee == STB_LOCAL) {
      symboles = section->tableSymboles->symbolesLocaux;
      nbSymboles = section->tableSymboles->nbLocaux;
   }
   else {
      symboles = section->tableSymboles->symbolesGlobaux;
      nbSymboles = section->tableSymboles->nbGlobaux;
   }

   for (i = 0 ; i != nbSymboles ; i++) {
      if (symboles[i] == symbole) break;
   }
   
   if (i != nbSymboles) {
      if (symbole->portee == STB_LOCAL) {
         return i;
      }
      else {
         return section->tableSymboles->nbLocaux + i;
      }
   }
   else {
      printf ( "%s%s%s%s%s%s%s\n",
              "panique (indexSymbole) ! : ",
              "le symbole ",
              symbole->nom->chaine,
              " n'existe dans pas dans la table de symboles ",
              section->nom->chaine,
              " de l'objet ",
              section->objetParent->nom);
      exit(1);
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}

/**********************************************************************/
SymboleEDL *symboleSelonIndex(SectionEDL *section, unsigned int index) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   TableSymbolesEDL *tableSymboles;
   SymboleEDL **symbolesLocaux;
   SymboleEDL **symbolesGlobaux;
   unsigned int nbLocaux;
   unsigned int nbGlobaux;
   unsigned int nbSymboles;
   
   tableSymboles = section->tableSymboles;
   symbolesLocaux = tableSymboles->symbolesLocaux;
   symbolesGlobaux = tableSymboles->symbolesGlobaux;
   nbLocaux = tableSymboles->nbLocaux;
   nbGlobaux = tableSymboles->nbGlobaux;
   nbSymboles = nbLocaux + nbGlobaux;

   if (index < nbLocaux) {
      return *(symbolesLocaux + index);
   }
   else if (index < nbSymboles) {
      return *(symbolesGlobaux + index - nbLocaux);
   }
   else {
      printf ( "%s%s%d%s%s%s%s\n",
              "panique (symboleSelonIndex) ! : ",
              "aucun symbole d'index ",
              index,
              " n'existe dans la table de symboles ",
              section->nom->chaine,
              " de l'objet ",
              section->objetParent->nom);
      exit(1);
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return NULL;
   */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void supprimerSymboleSelonNom(SectionEDL *section,
                              ChaineEDL *nomSymbole) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   SymboleEDL *symbole;

   symbole = symboleSectionSelonNom(section, nomSymbole);
   if (symbole != NULL) {
      supprimerSymbole(section, symbole);
   }
   else {
      symbole = symboleGlobalSelonNom(section, nomSymbole);
      if (symbole != NULL) {
         supprimerSymbole(section, symbole);
      }
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void supprimerSymbolesSectionInutiles(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   SymboleEDL **symbolesLocaux;
   unsigned int nbLocaux;
   TypeSectionEDL typeSection;
   unsigned int i;

   symbolesLocaux = section->tableSymboles->symbolesLocaux;
   nbLocaux = section->tableSymboles->nbLocaux;

   for (i = 1 ; i != nbLocaux ; i++) {
      if (symbolesLocaux[i]->type == STT_SECTION) {
         typeSection = symbolesLocaux[i]->sectionDefinition->type;
         if (typeSection != SHT_PROGBITS && typeSection != SHT_NOBITS) {
            supprimerSymbole(section, symbolesLocaux[i]);
         }
      }
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
unsigned int tailleTableSymboles(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   TableSymbolesEDL *tableSymboles;
   SymboleEDL **symbolesLocaux;
   SymboleEDL **symbolesGlobaux;
   unsigned int nbLocaux;
   unsigned int nbGlobaux;

   tableSymboles = section->tableSymboles;
   symbolesLocaux = tableSymboles->symbolesLocaux;
   symbolesGlobaux = tableSymboles->symbolesGlobaux;
   nbLocaux = tableSymboles->nbLocaux;
   nbGlobaux = tableSymboles->nbGlobaux;

   return nbLocaux > nbGlobaux ? nbLocaux : nbGlobaux;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return 0;
   */
}

/**********************************************************************/
void afficherSymboles(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   SymboleEDL **symbolesLocaux, **symbolesGlobaux;
   unsigned int nbLocaux, nbGlobaux;
   unsigned int i;

   printf("%s:%s\n", section->objetParent->nom, section->nom->chaine);

   afficherTitreSymboles();

   symbolesLocaux = section->tableSymboles->symbolesLocaux;
   nbLocaux = section->tableSymboles->nbLocaux;
   for (i = 0 ; i != nbLocaux ; i++) {
      afficherSymbole(*(symbolesLocaux[i]), i);
   }

   symbolesGlobaux = section->tableSymboles->symbolesGlobaux;
   nbGlobaux = section->tableSymboles->nbGlobaux;
   for (i = 0 ; i != nbGlobaux ; i++) {
      afficherSymbole(*(symbolesGlobaux[i]), nbLocaux + i);
   }

   printf("\n");
}
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC void fusionnerSymboleLocal(SymboleEDL *symboleSource,
                                  SectionEDL *sectionSource,
                                  SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   SymboleEDL *symboleCible;
   SectionEDL *sectionDefinitionSource;
   SectionProgrammeEDL *sectionProgrammeSource;

   symboleCible = symboleSectionSelonNom(sectionCible,
                                         symboleSource->nom);
   if (symboleSource->type != STT_SECTION
       || symboleCible == NULL) {
      /*
      le symbole a fusionner est un symbole de programme ou bien aucun
      symbole de section de meme nom n'existe deja dans la table des
      symboles fusionnee
      */
      ajouterSymbole(sectionCible, symboleSource, &symboleCible);

      sectionDefinitionSource = symboleSource->sectionDefinition;
      if (sectionDefinitionSource != NULL) {
         /* le symbole a fusionner est un symbole translatable */
         if (symboleSource->type != STT_SECTION) {
            /* le symbole a fusionner est un symbole de programme */
            sectionProgrammeSource =
                              sectionDefinitionSource->sectionProgramme;
            if (sectionProgrammeSource != NULL) {
               /*
               la section de definition du symbole a fusionner est une
               section de programme
               */
               symboleCible->valeur += 
                                sectionProgrammeSource->decalageContenu;
            }
            else {
               /*
               la section de definition du symbole a fusionner n'est pas
               une section de programme
               */
               printf ( "%s%s%s%s%s%s%s%s\n",
                       "panique (fusionnerSymboleLocal) ! : ",
                       "le symbole de programme ",
                       symboleSource->nom->chaine,
                       " est defini dans une section mal formee (",
                       sectionSource->objetParent->nom,
                       ":",
                       sectionSource->nom->chaine,
                       ")");
               exit(1);
            }
         }
      }
   }
   else {
      /*
      symboleSource->type == STT_SECTION && symboleCible != NULL :
      le symbole a fusionner est un symbole de section deja present
      dans la table des symboles fusionnee
      */
      symboleSource->symboleImage = symboleCible;
   }
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC void fusionnerSymboleGlobal(SymboleEDL *symboleSource,
                                   SectionEDL *sectionSource,
                                   SectionEDL *sectionCible,
                                   unsigned int *erreur) {
/**********************************************************************/
   /* DEBUT SUPPRIMER FUSION */
   /********************************************************************
   - specification etendue, avec prise en compte des symboles faibles
     (STB_WEAK) :
     . si un symbole sCible de meme portee et de meme nom existait dans
       tCible, la section de definition, la nature et la portee des deux
       symboles sCible et sSource ont ete considerees, chacun des deux
       symboles pouvant etre : indefini, global (= global, defini et
       translatable), absolu (= global, defini et absolu), faible
       (= faible, defini et translatable), commun.
       Les cas suivants (sCible-sSource) ont ete consideres :
       + indefini-faible   => la taille, le type, la valeur et la
                              section de definition de sCible ont ete
                              mis a jour
       + global-faible     => sCible est inchange
       + absolu-faible     => sCible est inchange
       + faible-indefini   => sCible est inchange
       + faible-global     => la taille, le type, la valeur et la
                              section de definition de sCible ont ete
                              mis a jour
       + faible-absolu     => la taille, le type, la valeur et la nature
                              de sCible ont ete mis a jour
       + faible-faible     => sCible est inchange
       + faible-commun     => la taille, le type, la valeur et la nature
                              de sCible ont ete mis a jour
       + commun-faible     => sCible est inchange
   ********************************************************************/
   /* FIN SUPPRIMER FUSION */
   /* DEBUT COMPLETER FUSION */
   SymboleEDL *symboleCible;
   SectionEDL *sectionDefinitionSource;
   SectionProgrammeEDL *sectionProgrammeSource;
   SymboleEDL *symboleDefini, *symboleCommun;
   SectionEDL *sectionDefini, *sectionCommun;

   /* variables d'entree */
   enum {GLOBAL, FAIBLE, ABSOLU, COMMUN, INDEFINI, INCONNU}
        natureSource, natureCible;

   /* variables de sortie */
   unsigned int creationNouveauSymbole,
                remplacementTailleEtType,
                miseAJourSectionDefinitionEtValeur,
                miseAJourNatureDefinitionEtValeur,
                verificationAlignementEtTaille,
                retentionMaxAlignementEtTaille,
                erreurDoubleDefinition;

   creationNouveauSymbole =
   remplacementTailleEtType =
   miseAJourSectionDefinitionEtValeur =
   miseAJourNatureDefinitionEtValeur =
   verificationAlignementEtTaille =
   retentionMaxAlignementEtTaille =
   erreurDoubleDefinition = FAUX;

   symboleCible = symboleGlobalSelonNom(sectionCible,
                                        symboleSource->nom);
   if (symboleCible == NULL) {
      /*
      aucun symbole global de meme nom que le symbole a fusionner
      n'existe deja dans la table des symboles fusionnee
      */
      creationNouveauSymbole = VRAI;
      if (symboleSource->sectionDefinition != NULL) {
         /* symbole translatable defini */
         miseAJourSectionDefinitionEtValeur = VRAI;
      }
   }
   else {
      /*
      le symbole a fusionner est deja present dans la table des symboles
      fusionnee
      */

      /*
      memorisation de la relation fonctionnelle associant le symbole
      existant au symbole a fusionner
      */
      symboleSource->symboleImage = symboleCible;

      /* caracterisation du symbole existant */
      if (symboleCible->sectionDefinition != NULL) {
         if (symboleCible->portee == STB_GLOBAL) {
            natureCible = GLOBAL;
         }
         else {
            natureCible = FAIBLE;
         }
      }
      else {
         switch (symboleCible->natureDefinition) {
            case SHN_ABS:
               natureCible = ABSOLU;
               break;
            case SHN_COMMON:
               natureCible = COMMUN;
               break;
            case SHN_UNDEF:
               natureCible = INDEFINI;
               break;
            default:
               natureCible = INCONNU;
               break;
         }
      }

      /* caracterisation du symbole a fusionner */
      if (symboleSource->sectionDefinition != NULL) {
         if (symboleSource->portee == STB_GLOBAL) {
            natureSource = GLOBAL;
         }
         else {
            natureSource = FAIBLE;
         }
      }
      else {
         switch (symboleSource->natureDefinition) {
            case SHN_ABS:
               natureSource = ABSOLU;
               break;
            case SHN_COMMON:
               natureSource = COMMUN;
               break;
            case SHN_UNDEF:
               natureSource = INDEFINI;
               break;
            default:
               natureSource = INCONNU;
               break;
         }
      }

      /* determination des actions a effectuer */
      if ((natureCible == GLOBAL && natureSource == COMMUN) ||
          (natureCible == ABSOLU && natureSource == COMMUN)) {
         verificationAlignementEtTaille = VRAI;
      }
      else if ((natureCible == COMMUN && natureSource == COMMUN)) {
         retentionMaxAlignementEtTaille = VRAI;
      }
      else if ((natureCible == FAIBLE && natureSource == ABSOLU) ||
               (natureCible == FAIBLE && natureSource == COMMUN) ||
               (natureCible == INDEFINI && natureSource == ABSOLU) ||
               (natureCible == INDEFINI && natureSource == COMMUN)) {
         remplacementTailleEtType = VRAI;
         miseAJourNatureDefinitionEtValeur = VRAI;
      }
      else if ((natureCible == FAIBLE && natureSource == GLOBAL) ||
               (natureCible == INDEFINI && natureSource == GLOBAL) ||
               (natureCible == INDEFINI && natureSource == FAIBLE)) {
         remplacementTailleEtType = VRAI;
         miseAJourSectionDefinitionEtValeur = VRAI;
      }
      else if ((natureCible == COMMUN && natureSource == ABSOLU)) {
         verificationAlignementEtTaille = VRAI;
         remplacementTailleEtType = VRAI;
         miseAJourNatureDefinitionEtValeur = VRAI;
      }
      else if ((natureCible == COMMUN && natureSource == GLOBAL)) {
         verificationAlignementEtTaille = VRAI;
         remplacementTailleEtType = VRAI;
         miseAJourSectionDefinitionEtValeur = VRAI;
      }
      else if ((natureCible == GLOBAL && natureSource == GLOBAL) ||
               (natureCible == GLOBAL && natureSource == ABSOLU) ||
               (natureCible == ABSOLU && natureSource == GLOBAL) ||
               (natureCible == ABSOLU && natureSource == ABSOLU)) {
         erreurDoubleDefinition = VRAI;
      }
      else {
         /* aucune action a effectuer */
      }
   }

   /* creation d'un nouveau symbole */
   if (creationNouveauSymbole) {
      ajouterSymbole(sectionCible, symboleSource, &symboleCible);
   }

   /*
   verification de la compatibilite entre valeur d'un symbole defini et
   contrainte d'alignement d'un symbole commun, et de l'egalite des
   tailles des deux symboles
   */
   if (verificationAlignementEtTaille) {
      if (natureCible == COMMUN) {
         symboleCommun = symboleCible;
         sectionCommun = symboleCommun->tableSymbolesOrigine;
         symboleDefini = symboleSource;
         sectionDefini = sectionSource;
      }
      else {
         symboleCommun = symboleSource;
         sectionCommun = sectionSource;
         symboleDefini = symboleCible;
         sectionDefini = symboleDefini->tableSymbolesOrigine;
      }

      /* verification contrainte d'alignement */
      if (symboleDefini->valeur % symboleCommun->valeur != 0) {
         /*
         la valeur du symbole a fusionner n'est pas compatible avec la
         contrainte d'alignement du symbole existant
         */
         printf ( "%s%s%ld%s%s%s%s%s%s%s%ld%s%s%s%s%s%s%s\n",
                 "avertissement : ",
                 "la valeur ",
                 symboleDefini->valeur,
                 " du symbole defini ",
                 symboleDefini->nom->chaine,
                 " (",
                 sectionDefini->objetParent->nom,
                 ":",
                 sectionDefini->nom->chaine,
                 ") est incompatible avec la contrainte d'alignement ",
                 symboleCommun->valeur,
                 " du symbole commun ",
                 symboleCommun->nom->chaine,
                 " (",
                 sectionCommun->objetParent->nom,
                 ":",
                 sectionCommun->nom->chaine,
                 ")"
         );
      }

      /* verification tailles */
      if (symboleCible->taille != symboleSource->taille) {
         /*
         les tailles des deux symboles ne sont pas identiques
         */
         printf ( "%s%s%s%s%d%s%s%s%s%s%d%s%s%s%s\n",
                 "avertissement : ",
                 "le symbole ",
                 symboleDefini->nom->chaine,
                 " a pour taille ",
                 symboleDefini->taille,
                 " dans ",
                 sectionDefini->objetParent->nom,
                 ":",
                 sectionDefini->nom->chaine,
                 " et pour taille ",
                 symboleCommun->taille,
                 " dans ",
                 sectionCommun->objetParent->nom,
                 ":",
                 sectionCommun->nom->chaine
         );
      }
   }

   /********************************************************************
   remplacement de la taille et du type du symbole existant par ceux du
   symbole a fusionner
   ********************************************************************/
   if (remplacementTailleEtType) {
      symboleCible->taille = symboleSource->taille;
      symboleCible->type = symboleSource->type;
   }

   /********************************************************************
   mise a jour de la valeur et de la section de definition du symbole
   fusionne
   ********************************************************************/
   if (miseAJourSectionDefinitionEtValeur) {
      /* le symbole a fusionner est un symbole defini translatable */
      sectionDefinitionSource = symboleSource->sectionDefinition;
      symboleCible->sectionDefinition =
                                  sectionDefinitionSource->sectionImage;
      sectionProgrammeSource =
                              sectionDefinitionSource->sectionProgramme;
      if (sectionProgrammeSource != NULL) {
         /*
         la section de definition du symbole a fusionner est une section
         de programme
         */
         symboleCible->valeur =
                              symboleSource->valeur
                              + sectionProgrammeSource->decalageContenu;
      }
      else {
         /*
         la section de definition du symbole a fusionner n'est pas une
         section de programme
         */
         printf ( "%s%s%s%s%s%s%s%s\n",
                 "panique (fusionnerSymboleGlobal) ! : ",
                 "le symbole de programme ",
                 symboleSource->nom->chaine,
                 " est defini dans une section mal formee (",
                 sectionSource->objetParent->nom,
                 ":",
                 sectionSource->nom->chaine,
                 ")"
         );
         exit(1);
      }
   }

   /********************************************************************
   mise a jour de la valeur et de la nature du symbole fusionne
   ********************************************************************/
   if (miseAJourNatureDefinitionEtValeur) {
      /*
      le symbole a fusionner est un symbole defini absolu ou un symbole
      commun
      */

      /* mise a jour de la nature */
      symboleCible->natureDefinition = symboleSource->natureDefinition;

      /*
      mise a jour de la valeur (cas d'un symbole absolu) ou de la
      contrainte d'alignement (cas d'un symbole commun)
      */
      symboleCible->valeur = symboleSource->valeur;
   }

   /********************************************************************
   mise a jour de la contrainte d'alignement et de la taille du symbole
   existant si l'une de ces deux grandeurs est inferieure a la grandeur
   correspondante du symbole a fusionner
   ********************************************************************/
   if (retentionMaxAlignementEtTaille) {
      /* conservation de la contrainte d'alignement la plus forte */
      if (symboleSource->valeur > symboleCible->valeur) {
         symboleCible->valeur = symboleSource->valeur;
      }
      /* conservation de la taille la plus grande */
      if (symboleSource->taille > symboleCible->taille) {
         symboleCible->taille = symboleSource->taille;
      }
   }

   /********************************************************************
   erreur : double definition d'un symbole
   ********************************************************************/
   if (erreurDoubleDefinition) {
      printf ( "%s%s%s%s%s%s%s%s%s%s%s%s%s%s\n",
              "erreur : ",
              "le symbole ",
              symboleCible->nom->chaine,
              " (",
              sectionCible->objetParent->nom,
              ":",
              sectionCible->nom->chaine,
              ") et le symbole ",
              symboleSource->nom->chaine,
              " (",
              sectionSource->objetParent->nom,
              ":",
              sectionSource->nom->chaine,
              ") sont definis dans deux objets distincts"
      );
      *erreur = VRAI;
   }
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC SymboleEDL *symboleGlobalSelonNom(SectionEDL *tableSymboles,
                                         ChaineEDL *nomSymbole) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SymboleEDL **symbolesGlobaux;
   unsigned int nbGlobaux;
   unsigned int nbLocaux;
   unsigned int i;

   symbolesGlobaux = tableSymboles->tableSymboles->symbolesGlobaux;
   nbGlobaux = tableSymboles->tableSymboles->nbGlobaux;
   for (i = 0 ; i != nbGlobaux ; i++) {
      if (strcmp(symbolesGlobaux[i]->nom->chaine,
                 nomSymbole->chaine) == 0) break;
   }
   if (i != nbGlobaux) {
      nbLocaux = tableSymboles->tableSymboles->nbLocaux;
      return symbolesGlobaux[i];
   }
   else {
      return NULL;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return NULL;
   */
}

/**********************************************************************/
STATIC SymboleEDL *symboleSectionSelonNom(SectionEDL *tableSymboles,
                                          ChaineEDL *nomSymbole) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SymboleEDL **symbolesLocaux;
   unsigned int nbLocaux;
   unsigned int i;

   symbolesLocaux = tableSymboles->tableSymboles->symbolesLocaux;
   nbLocaux = tableSymboles->tableSymboles->nbLocaux;
   for (i = 0 ; i != nbLocaux ; i++) {
      if (symbolesLocaux[i]->type == STT_SECTION
          && strcmp(symbolesLocaux[i]->sectionDefinition->nom->chaine,
                    nomSymbole->chaine) == 0) break;
   }
   if (i != nbLocaux) {
      return symbolesLocaux[i];
   }
   else {
      return NULL;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return NULL;
   */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void supprimerSymbole(SectionEDL *section, SymboleEDL *symbole) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   SymboleEDL **symboles;
   unsigned int nbSymboles;
   PorteeSymboleEDL portee;
   SectionEDL *tableChaines;
   unsigned int i;
   
   portee = symbole->portee;
   if (portee == STB_LOCAL) {
      symboles = section->tableSymboles->symbolesLocaux;
      nbSymboles = section->tableSymboles->nbLocaux;
   }
   else { /* portee == STB_GLOBAL || symbole->portee == STB_WEAK) */
      symboles = section->tableSymboles->symbolesGlobaux;
      nbSymboles = section->tableSymboles->nbGlobaux;
   }
   for (i = 0 ; i != nbSymboles ; i++) {
         if (symboles[i] == symbole) break;
   }
   if (i != nbSymboles) {
      if (symbole->type == STT_SECTION) {
         libererChaine(&(symbole->nom));
      }
      else {
         tableChaines = section->tableSymboles->tableChaines;
         supprimerChaine(tableChaines, symbole->nom);
      }
      free(symboles[i]);
      symboles[i] = symboles[nbSymboles - 1];
      if (portee == STB_LOCAL) {
         section->tableSymboles->nbLocaux = nbSymboles - 1;
      }
      else {
         section->tableSymboles->nbGlobaux = nbSymboles - 1;
      }
   }
   else {
      printf ( "%s%s%s%s%s%s\n",
              "panique (supprimerSymbole) ! : ",
              "le symbole a supprimer n'existe pas ",
              "dans la table de symboles ",
              section->nom->chaine,
              " de l'objet ",
              section->objetParent->nom);
      exit(1);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
STATIC void afficherTitreSymboles(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   printf("%s%s%s%s%s%s\n",
          "index | ",
          "nom                               | ",
          "valeur | ",
          "portee | ",
          "type  | ",
          "sect. def.");
   printf("%s%s%s%s%s%s\n",
          "------|-",
          "----------------------------------|-",
          "-------|-",
          "-------|-",
          "------|-",
          "----------");
}

/**********************************************************************/
STATIC void afficherSymbole(SymboleEDL symbole, unsigned int indexElf) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   char nomSectionDef[11];

   printf(" %3d  | %-30s    |%8lx| ",
          indexElf,
          symbole.nom->chaine,
          symbole.valeur);

   if (symbole.portee == STB_LOCAL) {
      printf("%-6s |", "LOCAL");
   }
   else {
      printf("%-6s |", "GLOBAL");
   }

   if (symbole.type == STT_SECTION) {
      printf("%-7s| ", "SECTION");
   }
   else {
      printf("%-7s| ", "");
   }

   if (symbole.sectionDefinition == NULL) {
      if (symbole.natureDefinition == SHN_UNDEF) {
         printf("%-3s\n", "UND");
      }
      else if (symbole.natureDefinition == SHN_ABS) {
         printf("%-3s\n", "ABS");
      }
      else if (symbole.natureDefinition == SHN_COMMON) {
         printf("%-3s\n", "COM");
      }
      else {
         printf("%-3s\n", "   ");
      }
   }
   else {
      memcpy(nomSectionDef, symbole.sectionDefinition->nom->chaine, 10);
      nomSectionDef[10] = '\0';
      printf("%s\n", nomSectionDef);
   }
}

/**********************************************************************/
STATIC void libererFormatInterneSymboles(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   TableSymbolesEDL *tableSymboles;
   SymboleEDL *symbole;
   unsigned int i;

   if (section->tableSymboles != NULL) {
      tableSymboles = section->tableSymboles;
      /* liberation de la structure de donnees globale */
      if (tableSymboles->symbolesLocaux != NULL) {
         /* liberation du tableau de symboles locaux */
         for (i = 0 ; i != tableSymboles->nbLocaux ; i++) {
            if (tableSymboles->symbolesLocaux[i] != NULL) {
               /* liberation du i+1eme symbole */
               symbole = tableSymboles->symbolesLocaux[i];
               if (symbole->type == STT_SECTION) {
                  libererChaine(&(symbole->nom));
               }
               free(tableSymboles->symbolesLocaux[i]);
            }
         }
         free(tableSymboles->symbolesLocaux);
      }
      if (tableSymboles->symbolesGlobaux != NULL) {
         /* liberation du tableau de symboles globaux */
         for (i = 0 ; i != tableSymboles->nbGlobaux ; i++) {
            if (tableSymboles->symbolesGlobaux[i] != NULL) {
               /* liberation du i+1eme symbole */
               free(tableSymboles->symbolesGlobaux[i]);
            }
         }
         free(tableSymboles->symbolesGlobaux);
      }
      free(section->tableSymboles);
      section->tableSymboles = NULL;
   }
}
