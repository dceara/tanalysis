/***********************************************************************
section.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "section.h"
#include "objet.h"
#include "chaines.h"
#include "symboles.h"
#include "code.h"
#include "translations.h"
#include "vide.h"
#include "autre.h"
#include "iterateur.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "sectionP.h"
#ifdef AVEC_LIBEDL
#include "_sectionI.h"
#endif
#else
#include "upcallI.h"
#endif

#define FAUX 0
#define VRAI 1

/**********************************************************************/
void initialiserOuvertureSection(SectionEDL *section, ObjetEDL *objet,
                                 size_t index) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   Elf *elf;
   Elf_Scn *descripteurLibelf;
   Elf32_Shdr *descripteurElf;
   Elf_Data *descripteurContenu;
   SectionEDL *tableNomsSection;
   unsigned int indexNomSection;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /*
   memorisation des descripteurs et du contenu au formats ELF et libelf
   */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   elf = objet->elf;
   //descripteurLibelf = elf_getscn(elf, index);
   //descripteurContenu = elf_getdata(descripteurLibelf, NULL);
   //descripteurElf = elf32_getshdr(descripteurLibelf);

   section->objetParent = objet;
   section->parentInitial = NULL;
   section->type = descripteurElf->sh_type;
   section->flags = descripteurElf->sh_flags;
   section->alignement = descripteurElf->sh_addralign;
   section->tailleElement = descripteurElf->sh_entsize;
   section->contenuElf = descripteurContenu->d_buf;
   section->tailleContenuElf = descripteurElf->sh_size;

   section->indexElf = index;
   section->descripteurElf = descripteurElf;
   section->descripteurLibelf = descripteurLibelf;
   section->descripteurContenu = descripteurContenu;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /*
   memorisation du nom de la section (pour toute section autre que la
   table des noms de section
   */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableNomsSection = section->objetParent->tableNomsSection;
   if (section != tableNomsSection) {
      indexNomSection = section->descripteurElf->sh_name;
      extraireChaineSelonIndex(tableNomsSection, indexNomSection,
                               &(section->nom));
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void ouvrirSection(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SectionEDL *tableNomsSection;
   unsigned int indexNomSection;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /*
   elaboration des structures de donnees internes representant le
   contenu de la section
   */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   switch (section->type) {
      case SHT_PROGBITS:
      case SHT_NOBITS:
         ouvrirSectionProgramme(section);
         break;
      case SHT_STRTAB:
         ouvrirTableChaines(section);
         break;
      case SHT_SYMTAB:
         ouvrirTableSymboles(section);
         break;
      case SHT_REL:
         ouvrirTableTranslation(section);
         break;
      default:
         /* il n'y a rien a faire pour les sections de type autre */
         break;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation du nom de la table des noms de section */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   tableNomsSection = section->objetParent->tableNomsSection;
   if (section == tableNomsSection) {
      indexNomSection = section->descripteurElf->sh_name;
      extraireChaineSelonIndex(tableNomsSection, indexNomSection,
                               &(section->nom));
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void creerSection(SectionEDL *section, ObjetEDL *objet,
                  SectionEDL *sectionReference,
                  unsigned int tailleMax,
                  unsigned int alignement) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SectionEDL *tableNomsSection;
   SectionEDL *tableChaines;
   SectionEDL *tableSymboles;
   SectionEDL *sectionCode;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* initialisation du descripteur de la section creee */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   section->objetParent = objet;
   section->parentInitial = NULL;
   section->type = sectionReference->type;
   section->flags = sectionReference->flags;
   section->alignement = alignement == 0
                         ? sectionReference->alignement
                         : alignement;
   section->tailleElement = sectionReference->tailleElement;
   section->adresse = 0;
   section->contenuElf = NULL;
   section->tailleContenuElf = 0;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /*
   initialisation des structures de donnees representant le contenu au
   format interne de la section
   */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   switch (section->type) {
      case SHT_PROGBITS:
      case SHT_NOBITS:
         creerSectionProgramme(section, tailleMax);
         break;
      case SHT_STRTAB:
         creerTableChaines(section, tailleMax);
         break;
      case SHT_SYMTAB:
         tableChaines = sectionReference->tableSymboles->tableChaines;
         creerTableSymboles(section,
                            tableChaines->sectionImage,
                            tailleMax);
         break;
      case SHT_REL:
         tableSymboles =
                      sectionReference->tableTranslation->tableSymboles;
         sectionCode = sectionReference->tableTranslation->sectionCode;
         creerTableTranslation(section,
                               tableSymboles->sectionImage,
                               sectionCode->sectionImage,
                               tailleMax);
         break;
      default:
         creerAutreSection(section, tailleMax);
         break;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* ajout du nom de la section a la table des noms de section */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   if (strcmp(sectionReference->nom->chaine, ".shstrtab") == 0) {
      /* il s'agit de la table des noms de section */
      ajouterChaine(section, sectionReference->nom->chaine, 0, FAUX,
                    &(section->nom));
      objet->tableNomsSection = section;
   }
   else {
      /* il s'agit d'une autre section */
      tableNomsSection = section->objetParent->tableNomsSection;
      ajouterChaine(tableNomsSection, sectionReference->nom->chaine, 0,
                    FAUX, &(section->nom));
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void partagerSection(SectionEDL *section, ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   section->parentInitial = section->objetParent;
   section->objetParent = objet;
   section->indexElf = 0;
   section->descripteurLibelf = NULL;
   section->descripteurContenu = NULL;
   section->descripteurElf = NULL;
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void copierContenuSection(SectionEDL *sectionSource,
                          SectionEDL *sectionCible) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   switch (sectionSource->type) {
      case SHT_STRTAB:
         copierContenuChaines(sectionSource, sectionCible);
         break;
      case SHT_SYMTAB:
         copierContenuSymboles(sectionSource, sectionCible);
         break;
      case SHT_REL:
         copierContenuTranslation(sectionSource, sectionCible);
         break;
      case SHT_NOBITS:
         sectionCible->tailleContenuElf =
                                        sectionSource->tailleContenuElf;
         break;
      default: /* section de programme ou autre type de section */
         sectionCible->tailleContenuElf =
                                        sectionSource->tailleContenuElf;
         memcpy(sectionCible->contenuElf, sectionSource->contenuElf,
                sectionCible->tailleContenuElf);
         break;
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerSection(SectionEDL *sectionSource,
                      SectionEDL *sectionCible) {
/**********************************************************************/
   unsigned int type = sectionSource->type;

   /* realisation de la fusion proprement dite */
   switch (type) {
      case SHT_PROGBITS:
      case SHT_NOBITS:
         fusionnerSectionProgramme(sectionSource, sectionCible);
         break;
      case SHT_STRTAB:
         fusionnerTableChaines(sectionSource, sectionCible);
         break;
      case SHT_SYMTAB:
         fusionnerTableSymboles(sectionSource, sectionCible);
         break;
      case SHT_REL:
         fusionnerTableTranslation(sectionSource, sectionCible);
         break;
      default:
         fusionnerAutreSection(sectionSource, sectionCible);
         break;
   }
}
/* FIN SUPPRIMER IMPLANTATION */

/**********************************************************************/
void initialiserClotureSection(SectionEDL *section) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   //section->descripteurLibelf = elf_newscn(section->objetParent->elf);
   //section->descripteurContenu =
   //                             elf_newdata(section->descripteurLibelf);
   //section->indexElf = elf_ndxscn(section->descripteurLibelf);
   //section->descripteurElf = elf32_getshdr(section->descripteurLibelf);
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void cloreSection(SectionEDL *section) {
/**********************************************************************/
   Elf32_Shdr *descripteurElf;
   Elf_Data *descripteurContenu;
   /* DEBUT SUPPRIMER FUSION */
   unsigned long position;
   /* FIN SUPPRIMER FUSION */

   /* traduction du contenu de la section vers le format ELF */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   switch (section->type) {
      case SHT_PROGBITS:
      case SHT_NOBITS:
         cloreSectionProgramme(section);
         break;
      case SHT_STRTAB:
         cloreTableChaines(section);
         break;
      case SHT_SYMTAB:
         cloreTableSymboles(section);
         break;
      case SHT_REL:
         cloreTableTranslation(section);
         break;
      default:
         cloreAutreSection(section);
         break;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* DEBUT SUPPRIMER FUSION */
   /*
   calcul de la position dans le fichier des sections non allouables
   (pour les objets executables uniquement)
   */
   if (section->objetParent->type == ET_EXEC) {
      switch (categorieSection(section)) {
         case INSTRUCTIONS:
         case DONNEES_INITIALISEES:
         case DONNEES_LECTURE_SEULE:
         case DONNEES_NON_INITIALISEES:
            /* position des sections allouables deja calculee */
            break;
         default:
            /* sections non allouables */
            position = alignement(section->objetParent->tailleFichier,
                                  section->alignement);
            section->position = position;
            position += section->tailleContenuElf;
            section->objetParent->tailleFichier = position;
            break;
      }
   }

   /* FIN SUPPRIMER FUSION */
   /* cloture descripteur libelf */
   descripteurContenu = section->descripteurContenu;
   descripteurContenu->d_size = section->tailleContenuElf;
   descripteurContenu->d_align = section->alignement;
   descripteurContenu->d_version = EV_CURRENT;
   if (section->objetParent->type == ET_EXEC) {
      descripteurContenu->d_off = 0;
   }

   /* cloture descripteur ELF */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   descripteurElf = section->descripteurElf;
   descripteurElf->sh_name = section->nom->indexElf;
   descripteurElf->sh_type = section->type;
   descripteurElf->sh_flags = section->flags;
   descripteurElf->sh_entsize = section->tailleElement;
   descripteurElf->sh_addr = section->adresse;
   if (section->objetParent->type == ET_EXEC) {
      descripteurElf->sh_offset = section->position;
      descripteurElf->sh_size = section->tailleContenuElf;
      descripteurElf->sh_addralign = section->alignement;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
void fermerSection(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   switch (section->type) {
      case SHT_PROGBITS:
      case SHT_NOBITS:
         fermerSectionProgramme(section);
         break;
      case SHT_STRTAB:
         fermerTableChaines(section);
         break;
      case SHT_SYMTAB:
         fermerTableSymboles(section);
         break;
      case SHT_REL:
         fermerTableTranslation(section);
         break;
      default:
         fermerAutreSection(section);
         break;
   }
}

/**********************************************************************/
unsigned int tailleSection(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION */
   switch (section->type) {
      case SHT_PROGBITS:
      case SHT_NOBITS:
         return tailleSectionProgramme(section);
         break;
      case SHT_STRTAB:
         return tailleTableChaines(section);
         break;
      case SHT_SYMTAB:
         return tailleTableSymboles(section);
         break;
      case SHT_REL:
         return tailleTableTranslation(section);
         break;
      default:
         return tailleAutreSection(section);
         break;
   }
}

/**********************************************************************/
void afficherSection(SectionEDL *section) {
/**********************************************************************/
   /*
   TableSymbolesEDL *tableSymboles;
   TableTranslationEDL *tableTranslation;
   unsigned int colonne;

   if (section->indexElf != 0) {
      printf(" %3d  | ", section->indexElf);
   }
   else {
       printf("      | ");
   }

   printf("%-20s | ", section->nom->chaine);

   switch (section->type) {
      case SHT_PROGBITS:
         printf("%-10s | ", "PROGBITS");
         break;
      case SHT_NOBITS:
         printf("%-10s | ", "NOBITS");
         break;
      case SHT_STRTAB:
         printf("%-10s | ", "STRTAB");
         break;
      case SHT_SYMTAB:
         printf("%-10s | ", "SYMTAB");
         break;
      case SHT_REL:
         printf("%-10s | ", "REL");
         break;
      case SHT_RELA:
         printf("%-10s | ", "RELA");
         break;
      default:
         printf("%-10s | ", "Autre Type");
         break;
   }

   switch (section->type) {
      case SHT_STRTAB:
         if (section->tailleContenuElf != 0) {
            printf("%6x |  ", section->tailleContenuElf);
         }
         else {
            printf("%6d*|  ", section->tableChaines->nbChaines);
         }
         break;
      case SHT_SYMTAB:
         printf("%6x |  ", (section->tableSymboles->nbLocaux
                            + section->tableSymboles->nbGlobaux)
                           * sizeof(Elf32_Sym));
         break;
      case SHT_REL:
         printf("%6x |  ", section->tableTranslation->nbTranslations
                           * sizeof(Elf32_Rel));
         break;
      default:
         printf("%6x |  ", section->tailleContenuElf);
         break;
   }

   
   colonne = 3;
   if ((section->flags & SHF_WRITE) == SHF_WRITE) {
      printf("%c", 'W');
      colonne--;
   }
   if ((section->flags & SHF_ALLOC) == SHF_ALLOC) {
      printf("%c", 'A');
      colonne--;
   }
   if ((section->flags & SHF_EXECINSTR) == SHF_EXECINSTR) {
      printf("%c", 'X');
      colonne--;
   }
   while (colonne != 0) {
      printf(" ");
      colonne--;
   }
   printf("  |  ");

   printf("%3d  | ", section->alignement);

   if (section->type == SHT_SYMTAB) {
      tableSymboles = section->tableSymboles;
      if (tableSymboles->tableChaines->indexElf != 0) {
         printf("%3d  |  ", tableSymboles->tableChaines->indexElf);
      }
      else {
      printf("     |  ");
      }
      printf("%2d", tableSymboles->nbLocaux + 1);
   }
   else if (section->type == SHT_REL) {
      tableTranslation = section->tableTranslation;
      if (tableTranslation->tableSymboles->indexElf != 0) {
         printf("%3d  |  ", tableTranslation->tableSymboles->indexElf);
      }
      else {
         printf("     |  ");
      }
      if (tableTranslation->sectionCode->indexElf != 0) {
         printf("%2d", tableTranslation->sectionCode->indexElf);
      }
   }
   else {
      printf("     |");
   }

   printf("\n"); */
}

/**********************************************************************/
void afficherTitreSections(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   printf("index |     nom              |  type      | ");
   printf("taille | flags | align | link | info\n");
   printf("------|----------------------|------------|-");
   printf("-------|-------|-------|------|-----\n");
}

/**********************************************************************/
void afficherInterligneSections(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   printf("------|----------------------|------------|-");
   printf("-------|-------|-------|------|-----\n");
}

/**********************************************************************/
unsigned long alignement(unsigned long adresse,
                         unsigned int alignement) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return (adresse + alignement - 1) & ~(alignement - 1);
}
