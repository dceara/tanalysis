/***********************************************************************
iterateur.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>

#include "iterateur.h"
#include "section.h"
#include "chaines.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "iterateurP.h"
#ifdef AVEC_LIBEDL
#include "_iterateurI.h"
#endif
#else
#include "upcallI.h"
#endif

#define VRAI 1
#define FAUX 0

#define LG_MAX_NOM 80

/***********************************************************************
Iterateur d'objet
***********************************************************************/

/**********************************************************************/
void creerIterObjet(IterateurObjet **iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   if (*iterateur == NULL) {
      *iterateur = (IterateurObjet *)calloc(1, sizeof(IterateurObjet));
   }
   else {
      viderIterObjet(*iterateur);
   }
}

/**********************************************************************/
void fermerIterObjet(IterateurObjet **iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   if (*iterateur != NULL) {
      viderIterObjet(*iterateur);
      free(*iterateur);
      *iterateur = NULL;
   }
}

/**********************************************************************/
void viderIterObjet(IterateurObjet *iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudSection **listesSections;
   NoeudSection **dernieresSections;
   CategorieSection categorie;
   NoeudSection *sectionSupprimee;

   listesSections = iterateur->listesSections;
   dernieresSections = iterateur->dernieresSections;

   for (categorie = INSTRUCTIONS ;
        categorie <= TABLE_TRANSLATION ;
        categorie++) {
      while (listesSections[categorie] != NULL) {
         sectionSupprimee = listesSections[categorie];
         listesSections[categorie] = listesSections[categorie]->suivant;
         free(sectionSupprimee);
      }
      dernieresSections[categorie] = NULL;
   }
}

/**********************************************************************/
void ajouterAIterObjet(IterateurObjet *iterateur, SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   CategorieSection categorie;
   NoeudSection **listesSections;
   NoeudSection **dernieresSections;
   NoeudSection *nouveauNoeud;

   categorie = categorieSection(section);
   listesSections = iterateur->listesSections;
   dernieresSections = iterateur->dernieresSections;

   nouveauNoeud = (NoeudSection *)calloc(1, sizeof(NoeudSection));
   nouveauNoeud->section = section;
   nouveauNoeud->suivant = NULL;

   if (listesSections[categorie] == NULL) {
      /* cette section est la 1ere de cette categorie*/
      listesSections[categorie] = nouveauNoeud;
   }
   else {
      /* il existe deja une section au moins dans cette categorie */
      dernieresSections[categorie]->suivant = nouveauNoeud;
   }
   dernieresSections[categorie] = nouveauNoeud;
}

/**********************************************************************/
void retirerDeIterObjet(IterateurObjet *iterateur,
                        SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   CategorieSection categorie;
   NoeudSection **listesSections;
   NoeudSection *courant;

   categorie = categorieSection(section);
   listesSections = iterateur->listesSections;

   courant = listesSections[categorie];
   while (courant != NULL && courant->section != section) {
      courant = courant->suivant;
   }
   if (courant != NULL) {
      /* neutraliser l'acces a la section */
      courant->section = NULL;
   }
   else {
      printf ( "%s%s%s%s%s\n",
              "panique (retirerDeIterObjet) ! : ",
              "la section ",
              section->nom->chaine,
              " n'appartient pas a l'iterateur de l'objet ",
              section->objetParent->nom);
      exit(1);
   }
}

/**********************************************************************/
void demarrerIterObjet(IterateurObjet *iterateur,
                       CategorieSection categorie) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudSection **listesSections;
   CategorieSection categorieCourante;
   NoeudSection *sectionCourante;

   listesSections = iterateur->listesSections;

   /*
   recherche d'une categorie >= la categorie consideree et contenant au
   moins une section (eventuellement neutralisee)
   */
   categorieCourante = categorie;
   while (categorieCourante != TABLE_TRANSLATION + 1) {
      sectionCourante = listesSections[categorieCourante];
      while (sectionCourante != NULL
             && sectionCourante->section == NULL) {
         sectionCourante = sectionCourante->suivant;
      }
      if (sectionCourante != NULL) break;
      categorieCourante += 1;
   }
   if (categorieCourante != TABLE_TRANSLATION + 1) {
      /*
      la section courante est de categorie >= la categorie consideree et
      n'est pas neutralisee
      */
      iterateur->finSections = FAUX;
      iterateur->sectionCourante = sectionCourante;
      iterateur->categorieCourante = categorieCourante;
   }
   else {
      /*
      il n'existe aucune section non neutralisee de categorie >= la
      categorie consideree
      */
      iterateur->finSections = VRAI;
   }
}

/**********************************************************************/
void avancerIterObjet(IterateurObjet *iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudSection **listesSections;
   NoeudSection *sectionCourante;
   CategorieSection categorieCourante;

   sectionCourante = iterateur->sectionCourante->suivant;
   while (sectionCourante != NULL && sectionCourante->section == NULL) {
      sectionCourante = sectionCourante->suivant;
   }
   if (sectionCourante == NULL) {
      /*
      il n'y a plus de section non neutralisee dans la categorie
      courante : recherche d'une categorie contenant une section non
      neutralisee au moins
      */
      listesSections = iterateur->listesSections;
      categorieCourante = iterateur->categorieCourante + 1;
      while (categorieCourante != TABLE_TRANSLATION + 1) {
         sectionCourante = listesSections[categorieCourante];
         while (sectionCourante != NULL
                && sectionCourante->section == NULL) {
            sectionCourante = sectionCourante->suivant;
         }
         if (sectionCourante != NULL) break;
         categorieCourante += 1;
      }
      if (categorieCourante != TABLE_TRANSLATION + 1) {
         /* une section non neutralisee a ete trouvee */
         iterateur->sectionCourante = sectionCourante;
         iterateur->categorieCourante = categorieCourante;
      }
      else {
         /* il n'existe plus aucune section non neutralisee */
         iterateur->finSections = VRAI;
      }
   }
   else {
      /*
      il existe encore une section non neutralisee au moins dans la
      categorie courante
      */
      iterateur->sectionCourante = sectionCourante;
   }
}

/**********************************************************************/
SectionEDL *sectionCouranteIterObjet(IterateurObjet *iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return iterateur->sectionCourante->section;
}

/**********************************************************************/
unsigned int finIterObjet(IterateurObjet *iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return iterateur->finSections;
}

/**********************************************************************/
void afficherIterObjet(IterateurObjet *iterateur) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   CategorieSection i;
   NoeudSection *sectionCourante;
   
   for (i = INSTRUCTIONS ; i != TABLE_TRANSLATION + 1 ; i++) {
      if (iterateur->listesSections[i] != NULL) break;
   }
   if (i != TABLE_TRANSLATION + 1) {
      afficherTitreSections();
      sectionCourante = iterateur->listesSections[i];
      while (sectionCourante != NULL) {
         afficherSection(sectionCourante->section);
         sectionCourante = sectionCourante->suivant;
      }
      i++;
      while (i != TABLE_TRANSLATION + 1) {
         if (iterateur->listesSections[i] != NULL) {
            afficherInterligneSections();
            sectionCourante = iterateur->listesSections[i];
            while (sectionCourante != NULL) {
               afficherSection(sectionCourante->section);
               sectionCourante = sectionCourante->suivant;
            }
         }
         i++;
      }
   }
}

/***********************************************************************
iterateur global
***********************************************************************/
/* DEBUT CACHER FUSION IMPLANTATION */

/**********************************************************************/
typedef struct StructNoeudNom {
   unsigned int alignementMax;
   NoeudSection *sections;
   NoeudSection *derniereSection;
   struct StructNoeudNom *suivant;
} NoeudNom;
/**********************************************************************/

/**********************************************************************/
typedef struct StructIterateurGlobal {
   NoeudNom *listesNoms[TABLE_TRANSLATION + 1];
   NoeudNom *nomCourant;
   unsigned int finNoms;
   NoeudSection *sectionCourante;
   unsigned int finSections;
   CategorieSection categorieCourante;
} IterateurGlobal;
/**********************************************************************/

static IterateurGlobal iterateurGlobal = {
   { NULL, NULL, NULL, NULL, NULL, NULL, NULL },
   NULL,
   VRAI,
   NULL,
   VRAI,
   INSTRUCTIONS
};
/* FIN CACHER FUSION IMPLANTATION */

/**********************************************************************/
void viderIterGlobal(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudNom **listesNoms;
   CategorieSection categorie;
   NoeudNom *nomSupprime;
   NoeudSection *sectionSupprimee;

   listesNoms = iterateurGlobal.listesNoms;
   for (categorie = INSTRUCTIONS ;
        categorie <= TABLE_TRANSLATION ;
        categorie++) {
      while (listesNoms[categorie] != NULL) {
         nomSupprime = listesNoms[categorie];
         do {
            sectionSupprimee = nomSupprime->sections;
            nomSupprime->sections = nomSupprime->sections->suivant;
            free(sectionSupprimee);
         } while (nomSupprime->sections != NULL);
         listesNoms[categorie] = listesNoms[categorie]->suivant;
         free(nomSupprime);
      }
   }
}

/**********************************************************************/
void ajouterAIterGlobal(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   CategorieSection categorie;

   categorie = categorieSection(section);

   ajouterSectionGlobal(section, categorie);
}

/**********************************************************************/
void demarrerIterGlobalNom(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudNom **listesNoms;
   CategorieSection categorieCourante;

   listesNoms = iterateurGlobal.listesNoms;

   categorieCourante = INSTRUCTIONS;
   while (categorieCourante != TABLE_TRANSLATION + 1
          && listesNoms[categorieCourante] == NULL) {
      categorieCourante += 1;
   }
   if (categorieCourante != TABLE_TRANSLATION + 1) {
      iterateurGlobal.finNoms = FAUX;
      iterateurGlobal.nomCourant = listesNoms[categorieCourante];
   }

   iterateurGlobal.categorieCourante = categorieCourante;
}

/**********************************************************************/
void avancerIterGlobalNom(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudNom **listesNoms;
   CategorieSection categorieCourante;

   iterateurGlobal.nomCourant = iterateurGlobal.nomCourant->suivant;
   if (iterateurGlobal.nomCourant == NULL) {
      /* plus de nom dans sous-sequence courante */
      listesNoms = iterateurGlobal.listesNoms;
      categorieCourante = iterateurGlobal.categorieCourante + 1;
      /* recherche d'une autre sous-sequence non vide */
      while (categorieCourante != TABLE_TRANSLATION + 1
             && listesNoms[categorieCourante] == NULL) {
         categorieCourante += 1;
      }
      if (categorieCourante == TABLE_TRANSLATION + 1) {
         /* aucune autre sous-sequence non vide */
         iterateurGlobal.finNoms = VRAI;
      }
      else {
         /* sous-sequence suivante */
         iterateurGlobal.nomCourant = listesNoms[categorieCourante];
      }
      iterateurGlobal.categorieCourante = categorieCourante;
   }
}

/**********************************************************************/
unsigned int alignementCourantIterGlobal(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return iterateurGlobal.nomCourant->alignementMax;
}

/**********************************************************************/
unsigned int finIterGlobalNom(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return iterateurGlobal.finNoms;
}

/**********************************************************************/
void demarrerIterGlobalSection(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   iterateurGlobal.sectionCourante =
                                   iterateurGlobal.nomCourant->sections;
   if (iterateurGlobal.sectionCourante == NULL) {
      /*
      plus de section pour le nom courant : cas en principe
      impossible...
      */
      iterateurGlobal.finSections = VRAI;
   }
   else {
      iterateurGlobal.finSections = FAUX;
   }
}

/**********************************************************************/
void avancerIterGlobalSection(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   iterateurGlobal.sectionCourante =
                               iterateurGlobal.sectionCourante->suivant;
   if (iterateurGlobal.sectionCourante == NULL) {
      /* plus de section dans nom courant */
      iterateurGlobal.finSections = VRAI;
   }
}

/**********************************************************************/
SectionEDL *sectionCouranteIterGlobal(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return iterateurGlobal.sectionCourante->section;
}

/**********************************************************************/
unsigned int finIterGlobalSection(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return iterateurGlobal.finSections;
}

/**********************************************************************/
void afficherIterGlobal(void) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   CategorieSection i;
   NoeudNom *noeudNomCourant;
   NoeudSection *sectionCourante;

   for (i = INSTRUCTIONS ; i != TABLE_TRANSLATION + 1 ; i++) {
      if (iterateurGlobal.listesNoms[i] != NULL) break;
   }
   if (i != TABLE_TRANSLATION + 1) {
      afficherTitreSections();
      noeudNomCourant = iterateurGlobal.listesNoms[i];
      do {
         sectionCourante = noeudNomCourant->sections;
         do {
            afficherSection(sectionCourante->section);
            sectionCourante = sectionCourante->suivant;
         } while (sectionCourante != NULL);
         noeudNomCourant = noeudNomCourant->suivant;
      } while (noeudNomCourant != NULL);

      i++;
      while (i != TABLE_TRANSLATION + 1) {
         if (iterateurGlobal.listesNoms[i] != NULL) {
            afficherInterligneSections();
            noeudNomCourant = iterateurGlobal.listesNoms[i];
            do {
               sectionCourante = noeudNomCourant->sections;
               do {
                  afficherSection(sectionCourante->section);
                  sectionCourante = sectionCourante->suivant;
               } while (sectionCourante != NULL);
               noeudNomCourant = noeudNomCourant->suivant;
            } while (noeudNomCourant != NULL);
         }
         i++;
      }
   }
}

/**********************************************************************/
STATIC void ajouterSectionGlobal(SectionEDL *section,
                                 CategorieSection categorie) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   NoeudNom **listesNoms;
   NoeudNom *noeudNomCourant;
   NoeudNom *noeudNomPrecedent;
   NoeudSection *noeudSection;

   listesNoms = iterateurGlobal.listesNoms;
   noeudNomCourant = listesNoms[categorie];

   /* allocation du noeud de section */
   noeudSection = (NoeudSection *)calloc(1, sizeof(NoeudSection));
   noeudSection->section = section;
   noeudSection->suivant = NULL;

   /* recherche du couple nom/type de la section a ajouter */
   while (noeudNomCourant != NULL) {
      if (strcmp(noeudNomCourant->sections->section->nom->chaine,
                 section->nom->chaine) == 0
          && noeudNomCourant->sections->section->type == section->type)
         break;
      noeudNomPrecedent = noeudNomCourant;
      noeudNomCourant = noeudNomCourant->suivant;
   }

   if (noeudNomCourant == NULL) {
      /* nouveau nom pour cette categorie de section */
      noeudNomCourant = (NoeudNom *)calloc(1, sizeof(NoeudNom));
      noeudNomCourant->alignementMax = section->alignement;
      noeudNomCourant->sections = noeudSection;
      noeudNomCourant->derniereSection = noeudSection;
      noeudNomCourant->suivant = NULL;
      if (listesNoms[categorie] == NULL) {
         /* ce nom de section est le 1er pour cette categorie */
         listesNoms[categorie] = noeudNomCourant;
      }
      else {
         /* il existe d'autre noms dans cette categorie de section */
         noeudNomPrecedent->suivant = noeudNomCourant;
      }
   }
   else {
      /* il existe deja une section au moins du meme nom */

      /* retention de la plus forte contrainte d'alignement */
      if (section->alignement > noeudNomCourant->alignementMax) {
         noeudNomCourant->alignementMax = section->alignement;
      }

      /* ajout en queue */
      noeudNomCourant->derniereSection->suivant = noeudSection;
      noeudNomCourant->derniereSection = noeudSection;

#if 0
      noeudSectionCourant = noeudNomCourant->sections;
      alignementCourant = noeudSectionCourant->section->alignement;
      if (section->alignement > alignementCourant) {
         /*
         la contrainte d'alignement est la plus forte : insertion en tete
         */
         noeudSection->suivant = noeudNomCourant->sections;
         noeudNomCourant->sections = noeudSection;
      }
      else {
         /*
         la contrainte d'alignement n'est pas la plus forte : insertion
         apres la derniere section dont la contrainte est superieure ou
         egale a celle de la section a inserer
         */
         noeudSectionPrecedent = noeudSectionCourant;
         noeudSectionCourant = noeudSectionCourant->suivant;
         while (noeudSectionCourant != NULL) {
            alignementCourant =
                               noeudSectionCourant->section->alignement;
            if (section->alignement > alignementCourant) break;
            noeudSectionPrecedent = noeudSectionCourant;
            noeudSectionCourant = noeudSectionCourant->suivant;
         }
         noeudSectionPrecedent->suivant = noeudSection;
         noeudSection->suivant = noeudSectionCourant;
      }
#endif
   }
}

/**********************************************************************/
CategorieSection categorieSection(SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   unsigned int type;
   unsigned int flags;

   type = section->type;
   flags = section->flags;
   switch (type) {
      case SHT_PROGBITS:
         if (flags & SHF_ALLOC) {
            if (flags & SHF_EXECINSTR) {
               return INSTRUCTIONS;
            }
            else if (flags & SHF_WRITE) {
               return DONNEES_INITIALISEES;
            }
            else {
               return DONNEES_LECTURE_SEULE;
            }
         }
         else {
            return DONNEES_NON_ALLOUABLES;
         }
         break;
      case SHT_NOBITS:
         if (flags & SHF_ALLOC) {
            return DONNEES_NON_INITIALISEES;
         }
         else {
            return DONNEES_NON_ALLOUABLES;
         }
         break;
      case SHT_STRTAB:
         return TABLE_CHAINES;
         break;
      case SHT_SYMTAB:
         return TABLE_SYMBOLES;
         break;
      case SHT_REL:
         return TABLE_TRANSLATION;
         break;
      default:
         return DONNEES_NON_ALLOUABLES;
         break;
   }
}
