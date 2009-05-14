/***********************************************************************
segments.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "segments.h"
#include "objet.h"
#include "section.h"
#include "iterateur.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "segmentsP.h"
#ifdef AVEC_LIBEDL
#include "_segmentsI.h"
#endif
#else
#include "upcallI.h"
#endif
/* DEBUT CACHER IMPLANTATION */

/* types de segment */
#define INUTILISE 0
#define TEXT      1
#define DATA      2
#define RODATA    4
#define BSS       8

#ifndef AVEC_LIBEDL
static unsigned long adresseText = DEFAUT_TEXT;
static unsigned long adresseData = DEFAUT_DATA;
static unsigned long adresseRodata = DEFAUT_RODATA;
static unsigned long adresseBss = DEFAUT_BSS;
/***********************************************************************
adresses des segments
***********************************************************************/
#endif /* AVEC_LIBEDL */
/* FIN CACHER IMPLANTATION */

/**********************************************************************/
void creerSegments(ObjetEDL *objetCible) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   determinerSegments(objetCible);
   repartirSectionsAllouables(objetCible);
}

/**********************************************************************/
void cloreSegments(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SegmentEDL *segments;
   unsigned int nbSegments;
   Elf32_Phdr *phdr;
   unsigned int i;

   segments = objet->segments;
   nbSegments = objet->nbSegments;

   if (nbSegments != 0) {
      /* creation et cloture des descripteurs de programmes */
      //phdr = elf32_newphdr(objet->elf, nbSegments);
      for (i = 0 ; i != nbSegments ; i++) {
         phdr[i].p_type = PT_LOAD;
         phdr[i].p_offset = segments[i].position;
         phdr[i].p_vaddr = segments[i].adresse;
         phdr[i].p_paddr = phdr[i].p_vaddr;
         phdr[i].p_filesz = segments[i].tailleFichier;
         phdr[i].p_memsz = segments[i].tailleMemoire;
         phdr[i].p_align = TAILLE_PAGE;
         phdr[i].p_flags = 0;
         if (segments[i].type & TEXT) {
            phdr[i].p_flags |= PF_R | PF_X;
         }
         if (segments[i].type & DATA || segments[i].type & BSS) {
            phdr[i].p_flags |= PF_R | PF_W;
         }
         if (segments[i].type & RODATA) {
            phdr[i].p_flags |= PF_R;
         }
      }
   }
}

/**********************************************************************/
void fermerSegments(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   unsigned int i;

   if (objet->segments != NULL) {
      /* liberation des segments */
      for (i = 0 ; i != objet->nbSegments ; i++) {
         if (objet->segments[i].sections != NULL) {
            /* liberation de la liste des sections du segment */
            free(objet->segments[i].sections);
         }
      }
      free(objet->segments);
      objet->segments = NULL;
   }
}

/**********************************************************************/
void implanterSectionsAllouables(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SegmentEDL *segments;
   unsigned int nbSegments;
   SectionEDL *section;
   unsigned int i;
   unsigned int noSegment;
   unsigned long adresse;  /* adresse courante dans segment courant */
   unsigned long position; /* position courante dans fichier */

   nbSegments = objet->nbSegments;

   /* calcul positions et adresses des sections du segment 0 */
   if (nbSegments != 0) {
      /* le segment 0 existe et contient une section au moins */
      segments = objet->segments;

      /* calcul adresse et position 1ere section */
      section = segments[0].sections[0];
      section->adresse = segments[0].adresse1ereSection;
      section->position = segments[0].position1ereSection;
      adresse = section->adresse + section->tailleContenuElf;
      position = section->position + section->tailleContenuElf;

      /* calcul adresse et position autres sections */
      for (i = 1 ; i != segments[0].nbSections ; i++) {
         section = segments[0].sections[i];
         section->adresse = alignement(adresse, section->alignement);
         section->position = alignement(position, section->alignement);
         adresse = section->adresse + section->tailleContenuElf;
         if (section->type != SHT_NOBITS) {
            position = section->position + section->tailleContenuElf;
         }
      }

      /* calcul tailles du segment 0 */
      segments[0].tailleMemoire = adresse - segments[0].adresse;
      segments[0].tailleFichier = position - segments[0].position;
   }

   /* calcul positions et adresses des sections des autres segments */
   for (noSegment = 1 ; noSegment != nbSegments ; noSegment++) {
      /* le segment courant existe et contient une section au moins */
      segments[noSegment].position = alignement(position, TAILLE_PAGE);

      /* calcul adresse et position 1ere section */
      section = segments[noSegment].sections[0];
      section->adresse = segments[noSegment].adresse;
      section->position = segments[noSegment].position;
      adresse = section->adresse + section->tailleContenuElf;
      position = section->position;
      if (section->type != SHT_NOBITS) {
         position += section->tailleContenuElf;
      }

      /* calcul adresse et position autres sections */
      for (i = 1 ; i != segments[noSegment].nbSections ; i++) {
         section = segments[noSegment].sections[i];
         section->adresse = alignement(adresse, section->alignement);
         section->position = alignement(position,
                                        section->alignement);
         adresse = section->adresse + section->tailleContenuElf;
         position = section->position;
         if (section->type != SHT_NOBITS) {
            position += section->tailleContenuElf;
         }
      }

      /* calcul tailles du segment courant */
      segments[noSegment].tailleMemoire =
                                  adresse - segments[noSegment].adresse;
      segments[noSegment].tailleFichier =
                                position - segments[noSegment].position;
   }

   /* verifier que les segments ne se chevauchent pas en memoire */
   verifierChevauchementSegments(segments, nbSegments);

   /* memoriser la taille occupee par les segments dans le fichier */
   objet->tailleFichier = position;
}

/**********************************************************************/
void memoriserAdresseText(unsigned long adresse) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   adresseText = adresse;
}

/**********************************************************************/
void memoriserAdresseData(unsigned long adresse) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   adresseData = adresse;
}

/**********************************************************************/
void memoriserAdresseRodata(unsigned long adresse) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   adresseRodata = adresse;
}

/**********************************************************************/
void memoriserAdresseBss(unsigned long adresse) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   adresseBss = adresse;
}

/**********************************************************************/
void afficherSegments(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SegmentEDL *segments;
   unsigned int nbSegments;
   unsigned int i;
   unsigned int type;
   unsigned int rang;

   segments = objet->segments;
   nbSegments = objet->nbSegments;

   printf("nature | position |  adresse | taille fichier | ");
   printf("taille memoire |     type     \n");
   printf("-------|----------|----------|----------------|-");
   printf("---------------|--------------\n");

   for (i = 0 ; i != nbSegments ; i++) {
      printf(" LOAD  | ");
      printf("%8lx | ", segments[i].position);
      printf("%8lx |    ", segments[i].adresse);
      printf("%8x    |    ", segments[i].tailleFichier);
      printf("%8x    | ", segments[i].tailleMemoire);
      for (rang = 0 ; rang != sizeof(segments[i].type) ; rang++) {
         type = segments[i].type & (1 << rang);
         switch (type) {
            case TEXT:
               printf("TXT ");
               break;
            case DATA:
               printf("DAT ");
               break;
            case RODATA:
               printf("ROD ");
               break;
            case BSS:
               printf("BSS ");
               break;
            default:
               break;
         }
      }
      printf("\n");
   }
   printf("\n");
}

/**********************************************************************/
STATIC void determinerSegments(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SegmentEDL *segments;
   unsigned int nbSegments;
   IterateurObjet *iterateur;
   CategorieSection categorie;
   SectionEDL *sectionCourante;
   unsigned int nbSectionsInstructions;
   unsigned int nbSectionsInitialisees;
   unsigned int nbSectionsLectureSeule;
   unsigned int nbSectionsNonInitialisees;
   unsigned int i;

   /* alignement des segments */
   if ((adresseText & (TAILLE_PAGE - 1)) != 0) {
      printf("adresse segment text requise : %lx, ", adresseText);
      adresseText &= ~(TAILLE_PAGE - 1);
      printf("adresse segment text retenue : %lx\n", adresseText);
   }
   if ((adresseData & (TAILLE_PAGE - 1)) != 0) {
      printf("adresse segment data requise : %lx, ", adresseData);
      adresseData &= ~(TAILLE_PAGE - 1);
      printf("adresse segment data retenue : %lx\n", adresseData);
   }
   if ((adresseRodata & (TAILLE_PAGE - 1)) != 0) {
      printf("adresse segment rodata requise : %lx, ", adresseRodata);
      adresseRodata &= ~(TAILLE_PAGE - 1);
      printf("adresse segment data retenue : %lx\n", adresseRodata);
   }
   if ((adresseBss & (TAILLE_PAGE - 1)) != 0) {
      printf("adresse segment bss requise : %lx, ", adresseBss);
      adresseBss &= ~(TAILLE_PAGE - 1);
      printf("adresse segment bss retenue : %lx\n", adresseBss);
   }

   /* nb. de segments par defaut */
   nbSegments = 4;

   /* allocation et initialisation des segments par defaut */
   segments = (SegmentEDL *)calloc(nbSegments, sizeof(SegmentEDL));
   segments[0].type = TEXT;   segments[0].adresse = adresseText;
   segments[1].type = DATA;   segments[1].adresse = adresseData;
   segments[2].type = RODATA; segments[2].adresse = adresseRodata;
   segments[3].type = BSS;    segments[3].adresse = adresseBss;

   /* fusion segments de meme adresse */
   //fusionnerSegments(segments, &nbSegments);

   /* calcul nombre de sections par type de section allouable */
   iterateur = objet->iterateur;
   nbSectionsInstructions = 0;
   nbSectionsInitialisees = 0;
   nbSectionsLectureSeule = 0;
   nbSectionsNonInitialisees = 0;
   demarrerIterObjet(iterateur, INSTRUCTIONS);
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != INSTRUCTIONS) break;
      nbSectionsInstructions++;
      avancerIterObjet(iterateur);
   }
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != DONNEES_INITIALISEES) break;
      nbSectionsInitialisees++;
      avancerIterObjet(iterateur);
   }
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != DONNEES_LECTURE_SEULE) break;
      nbSectionsLectureSeule++;
      avancerIterObjet(iterateur);
   }
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != DONNEES_NON_INITIALISEES) break;
      nbSectionsNonInitialisees++;
      avancerIterObjet(iterateur);
   }
   
   if (nbSectionsInstructions == 0) {
      printf ( "%s%s%s\n",
              "attention ! : l'objet ",
              objet->nom,
              " ne contient aucune section d'instructions allouable");
   }

   /* calcul nombre de sections par segment */
   segments[0].nbSections = nbSectionsInstructions;
   for (i = 1 ; i != nbSegments ; i++) {
      segments[i].nbSections = 0;
   }
   for (i = 0 ; i != nbSegments ; i++) {
      if (segments[i].type & DATA) {
         /* segment courant de type ... | DATA | ... | ... */
         segments[i].nbSections += nbSectionsInitialisees;
      }
      if (segments[i].type & RODATA) {
         /* segment courant de type ... | ... | RODATA | ... */
         segments[i].nbSections += nbSectionsLectureSeule;
      }
      if (segments[i].type & BSS) {
         /* segment courant de type ... | ... | ... | BSS */
         segments[i].nbSections += nbSectionsNonInitialisees;
      }
   }

   /*
   neutralisation segments vides ou inutilises (y compris segment 0)
   pre-condition : nbSegments <= 3
   */
   for (i = nbSegments - 1 ; i != 0 ; i--) {
      if (segments[i].nbSections == 0) {
         nbSegments--;
      }
      else {
         if (segments[i - 1].nbSections == 0) {
            segments[i - 1].type = segments[i].type;
            segments[i - 1].nbSections = segments[i].nbSections;
            segments[i - 1].adresse = segments[i].adresse;
            nbSegments--;
         }
      }
   }
   /* 
   post-condition : nbSegments > 0 et 0 <= i <= nbSegments - 1 =>
   segments[i].nbSections != 0
   */

   /* determination adresse et position 1ere section de ch. segment */
   if (nbSegments != 0) {
      /* 1er segment */
      segments[0].position = 0;
      segments[0].position1ereSection =
                   sizeof(Elf32_Ehdr) + sizeof(Elf32_Phdr) * nbSegments;
      segments[0].adresse1ereSection =
                  segments[0].adresse + segments[0].position1ereSection;

      /* autres segments */
      for (i = 1 ; i != nbSegments ; i++) {
         segments[i].adresse1ereSection = segments[i].adresse;
      }
   }


   /* elimination segments inutiles et memorisation segments restants */
   objet->segments =
                 (SegmentEDL *)realloc(segments,
                                       nbSegments * sizeof(SegmentEDL));
   objet->nbSegments = nbSegments;
}

/**********************************************************************/
STATIC void repartirSectionsAllouables(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SegmentEDL *segments;
   unsigned int nbSegments;
   SectionEDL ***sectionsSegment;
   IterateurObjet *iterateur;
   SectionEDL *sectionCourante;
   CategorieSection categorie;
   unsigned int i;

   segments = objet->segments;
   nbSegments = objet->nbSegments;
   iterateur = objet->iterateur;

   /* allocation d'une sequence de sections pour chaque segment */
   if (nbSegments != 0) {
      sectionsSegment =
              (SectionEDL ***)calloc(nbSegments, sizeof(SectionEDL **));
   }
   for (i = 0 ; i != nbSegments ; i++) {
      sectionsSegment[i] = (SectionEDL **)calloc(segments[i].nbSections,
                                                 sizeof(SectionEDL *));
      segments[i].sections = sectionsSegment[i];
   }

   /* construction de la sequence des sections de chaque segment */
   for (i = 0 ; i != nbSegments ; i++) {
      if (segments[i].type & TEXT) {
         demarrerIterObjet(iterateur, INSTRUCTIONS);
         while (!finIterObjet(iterateur)) {
            sectionCourante = sectionCouranteIterObjet(iterateur);
            categorie = categorieSection(sectionCourante);
            if (categorie != INSTRUCTIONS) break;
            *sectionsSegment[i] = sectionCourante;
            sectionsSegment[i]++;
            avancerIterObjet(iterateur);
         }
      }
      if (segments[i].type & DATA) {
         demarrerIterObjet(iterateur, DONNEES_INITIALISEES);
         while (!finIterObjet(iterateur)) {
            sectionCourante = sectionCouranteIterObjet(iterateur);
            categorie = categorieSection(sectionCourante);
            if (categorie != DONNEES_INITIALISEES) break;
            *sectionsSegment[i] = sectionCourante;
            sectionsSegment[i]++;
            avancerIterObjet(iterateur);
         }
      }
      if (segments[i].type & RODATA) {
         demarrerIterObjet(iterateur, DONNEES_LECTURE_SEULE);
         while (!finIterObjet(iterateur)) {
            sectionCourante = sectionCouranteIterObjet(iterateur);
            categorie = categorieSection(sectionCourante);
            if (categorie != DONNEES_LECTURE_SEULE) break;
            *sectionsSegment[i] = sectionCourante;
            sectionsSegment[i]++;
            avancerIterObjet(iterateur);
         }
      }
      if (segments[i].type & BSS) {
         demarrerIterObjet(iterateur, DONNEES_NON_INITIALISEES);
         while (!finIterObjet(iterateur)) {
            sectionCourante = sectionCouranteIterObjet(iterateur);
            categorie = categorieSection(sectionCourante);
            if (categorie != DONNEES_NON_INITIALISEES) break;
            *sectionsSegment[i] = sectionCourante;
            sectionsSegment[i]++;
            avancerIterObjet(iterateur);
         }
      }
   }

   /* liberation des pointeurs de tete de liste temporaires */
   free(sectionsSegment);
}

/**********************************************************************/
STATIC void fusionnerSegments(SegmentEDL *segments,
                              unsigned int *nbSegments) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   /*
   unsigned int ordre[*nbSegments];
   unsigned int type[*nbSegments];
   unsigned int numero[*nbSegments];
   unsigned long adresse[*nbSegments];
   unsigned int i, j, jmin;
   unsigned int k;
   unsigned int temp;


   for (i = 0 ; i != *nbSegments ; i++) {
      ordre[i] = i;
   }
   for (i = 0 ; i != *nbSegments - 1 ; i++) {
      jmin = i;
      for (j = i + 1 ; j != *nbSegments ; j++) {
         if (segments[ordre[j]].adresse <
                                        segments[ordre[jmin]].adresse) {
            jmin = j;
         }
      }
      if (jmin != i) {
         temp = ordre[i];
         ordre[i] = ordre[jmin];
         ordre[jmin] = temp;
      }
   }

   i = 0;
   k = 0;
   while (i != *nbSegments) {
      numero[k] = ordre[i];
      adresse[k] = segments[ordre[i]].adresse;
      type[k] = segments[ordre[i]].type;
      i++;
      while (i != *nbSegments
             && segments[ordre[i]].adresse == adresse[k]) {
         type[k] |= segments[ordre[i]].type;
         i++;
      }
      k += 1;
   }

   for (i = 0 ; i != k ; i++) {
      ordre[i] = i;
   }
   for (i = 0 ; i != k - 1 ; i++) {
      jmin = i;
      for (j = i + 1 ; j != k ; j++) {
         if (numero[ordre[j]] < numero[ordre[jmin]]) {
            jmin = j;
         }
      }
      if (jmin != i) {
         temp = ordre[i];
         ordre[i] = ordre[jmin];
         ordre[jmin] = temp;
      }
   }

   for (i = 0 ; i != k ; i++) {
      segments[i].type = type[ordre[i]];
      segments[i].adresse = adresse[ordre[i]];
   }
   *nbSegments = k; */
}

/**********************************************************************/
STATIC void verifierChevauchementSegments(SegmentEDL *segments,
                                          unsigned int nbSegments) {
/**********************************************************************/
}
