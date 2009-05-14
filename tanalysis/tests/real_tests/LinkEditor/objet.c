/***********************************************************************
objet.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <elf.h>
#include <libelf/libelf.h>

#include "objet.h"
#include "section.h"
#include "symboles.h"
#include "translations.h"
/* DEBUT SUPPRIMER FUSION */
#include "segments.h"
/* FIN SUPPRIMER FUSION */
#include "iterateur.h"

#include "options.h"

#ifndef EST_LIBEDL
#include "objetP.h"
#ifdef AVEC_LIBEDL
#include "_objetI.h"
#endif
#else
#include "upcallI.h"
#endif

#define VRAI 1
#define FAUX 0
/* DEBUT SUPPRIMER FUSION */

#ifndef AVEC_LIBEDL
static unsigned long adresseStack = DEFAUT_STACK;
static char *nomPtEntree = NULL;
/***********************************************************************
adresse de la pile et nom du point d'entree par defaut
***********************************************************************/
#endif /* AVEC_LIBEDL */
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void ouvrirObjet(ObjetEDL *objet, unsigned int lectureEcriture) {
/**********************************************************************/
   Elf32_Ehdr *ehdr;

   /* initialisation libelf */
   initialiserLibelf();

   /* creer identificateur d'objet Elf */
   if (lectureEcriture) {
      objet->mode = ELF_C_RDWR;
   }
   else {
      objet->mode = ELF_C_READ;
   }
   //objet->elf = elf_begin(objet->fd, objet->mode, (Elf *)NULL);
   if (objet->elf == (Elf *)NULL) {
      printf ( "%s%s%s\n",
              "erreur : lecture image ELF (",
              objet->nom,
              ") impossible");
      exit(1);
   }
   
   /* recuperer descripteur (entete) de fichier Elf */
   //ehdr = elf32_getehdr(objet->elf);
   objet->ehdr = ehdr;

   /* verifier qu'il ne s'agit pas d'un objet binaire executable */
   if (ehdr->e_type == ET_EXEC) {
      printf ( "%s%s%s\n",
              "erreur : ouverture d'un objet executable (",
              objet->nom,
              ") interdite");
      exit(1);
   }
   
   /* memorisation des attributs */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   objet->boutisme = ehdr->e_ident[EI_DATA];
   objet->classe = ehdr->e_ident[EI_CLASS];
   objet->systeme = ehdr->e_ident[EI_OSABI];
   objet->processeur = ehdr->e_machine;
   objet->version = ehdr->e_version;
   objet->flags = ehdr->e_flags;
   objet->type = ehdr->e_type;
   objet->estCopie = FAUX;
   /* DEBUT SUPPRIMER FUSION */
   objet->tailleFichier = 0;
   /* FIN SUPPRIMER FUSION */
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* ouvrir sections du fichier d'entree */
   ouvrirSections(objet);
}

/**********************************************************************/
void creerObjet(ObjetEDL *objetCible, ObjetEDL *objetSource,
                TypeObjetEDL type, unsigned int nbSections) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SectionEDL *tableNomsSectionCible;
   SectionEDL *tableNomsSectionSource;

   /* DEBUT COMPLETER FUSION */
   /* creation image ELF de l'objet */
   objetCible->mode = ELF_C_WRITE;

   /* initialisation des attributs */
   objetCible->boutisme = objetSource->boutisme;
   objetCible->classe = objetSource->classe;
   objetCible->systeme = objetSource->systeme;
   objetCible->type = type;
   objetCible->processeur = objetSource->processeur;
   objetCible->version = objetSource->version;
   objetCible->flags = objetSource->flags;
   objetCible->estCopie = FAUX;
   objetCible->nbSections = 0;
   /* DEBUT SUPPRIMER FUSION */
   objetCible->tailleFichier = 0;
   /* FIN SUPPRIMER FUSION */

   /* allouer memoire pour tableau des descripteurs de sections */
   objetCible->sections =
                (SectionEDL **)calloc(nbSections, sizeof(SectionEDL *));
   objetCible->nbMax = nbSections;

   /* creation d'un iterateur d'objet vide */
   creerIterObjet(&(objetCible->iterateur));

   /* ajout de la table des noms de section */
   tableNomsSectionSource = objetSource->tableNomsSection;

   ajouterSection(objetCible, tableNomsSectionSource,
                  tailleSection(tableNomsSectionSource), 0,
                  &tableNomsSectionCible);

   objetCible->tableNomsSection = tableNomsSectionCible;
   /* FIN COMPLETER FUSION */
}

/**********************************************************************/
void copierObjet(ObjetEDL *objetSource, ObjetEDL *objetCible,
                 TypeObjetEDL type) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   unsigned int i;
   
   /* l'objet cree partage son contenu avec un autre objet */
   objetCible->estCopie = VRAI;

   /* copie image ELF de l'objet */
   objetCible->mode = ELF_C_WRITE;
   objetCible->elf = NULL;
   objetCible->ehdr = NULL;

   /* copie des attributs */
   objetCible->boutisme = objetSource->boutisme;
   objetCible->classe = objetSource->classe;
   objetCible->systeme = objetSource->systeme;
   objetCible->type = type;
   objetCible->processeur = objetSource->processeur;
   objetCible->version = objetSource->version;
   objetCible->flags = objetSource->flags;
   /* DEBUT SUPPRIMER FUSION */
   objetCible->tailleFichier = objetSource->tailleFichier;
   objetCible->pointEntree = objetSource->pointEntree;
   /* FIN SUPPRIMER FUSION */

   /* partage des sections */
   objetCible->sections = objetSource->sections;
   objetCible->nbSections = objetSource->nbSections;
   objetCible->nbMax = objetSource->nbMax;
   sections = objetCible->sections;
   nbSections = objetCible->nbSections;
   for (i = 0 ; i != nbSections ; i++) {
      partagerSection(sections[i], objetCible);
   }
   objetCible->tableNomsSection = objetSource->tableNomsSection;
   /* DEBUT SUPPRIMER FUSION */

   /* partage des segments */
   objetCible->nbSegments = objetSource->nbSegments;
   objetCible->segments = objetSource->segments;
   /* FIN SUPPRIMER FUSION */

   /* partage de l'iterateur */
   objetCible->iterateur = objetSource->iterateur;
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void dupliquerObjet(ObjetEDL *objetSource, ObjetEDL *objetCible,
                    TypeObjetEDL type) {
/**********************************************************************/
   SectionEDL *sectionSource;
   SectionEDL *sectionCible;
   IterateurObjet *iterateurSource;
   IterateurObjet *iterateurCible;

   /* creation de l'objet duplique */
   creerObjet(objetCible, objetSource, type, objetSource->nbMax);
   iterateurCible = objetCible->iterateur;

   /*
   creation des sections (autres que la table des noms de section) dans
   l'ordre ou elles seront copiees (par categorie de section)
   */
   /* DEBUT COMPLETER IMPLANTATION */
   iterateurSource = objetSource->iterateur;
   demarrerIterObjet(iterateurSource, INSTRUCTIONS);

   while (!finIterObjet(iterateurSource)) {
      sectionSource = sectionCouranteIterObjet(iterateurSource);

      /* la table des noms de section a deja ete creee */
      if (sectionSource != objetSource->tableNomsSection) {

         /* ajout de la section */
         ajouterSection(objetCible, sectionSource,
                        tailleSection(sectionSource), 0,
                        &sectionCible);
      }

      avancerIterObjet(iterateurSource);
   }
   /* FIN COMPLETER IMPLANTATION */

   /*
   copie du contenu des sections (autres que la table des noms de
   section) dans l'ordre des categories de section (et dans l'ordre ou
   elles ont ete creees)
   */
   /* DEBUT COMPLETER IMPLANTATION */
   demarrerIterObjet(iterateurSource, INSTRUCTIONS);
   demarrerIterObjet(iterateurCible, INSTRUCTIONS);

   while (!finIterObjet(iterateurSource)) {
      sectionSource = sectionCouranteIterObjet(iterateurSource);
      sectionCible = sectionCouranteIterObjet(iterateurCible);

      /* la table des noms de section a deja ete dupliquee */
      if (sectionSource != objetSource->tableNomsSection) {
         copierContenuSection(sectionSource, sectionCible);
      }

      avancerIterObjet(iterateurSource);
      avancerIterObjet(iterateurCible);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
void fusionnerObjet(ObjetEDL *objetCible) {
/**********************************************************************/
   creerSectionsFusionnees(objetCible);
   fusionnerSections(objetCible);
}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void implanterObjet(ObjetEDL *objet) {
/**********************************************************************/
   if (!objet->estCopie) {

      /*
      resoudre les symboles communs (avant de determiner les adresses
      des sections)
      */
      resoudreSymbolesCommuns(objet);

      /* creer les segments necessaires */
      creerSegments(objet);

      /* implanter les sections allouables */
      implanterSectionsAllouables(objet);

      /* resoudre les symboles utilises par le runtime du langage C */
      resoudreSymbolesCRT(objet);

      /*
      implanter les tables de symboles
      (et supprimer les symboles de section devenus inutiles)
      */
      implanterTablesSymboles(objet);

      /*
      completer les adresses referencees dans les sections de code
      (puis supprimer les tables de translations)
      */
      completerSectionsCode(objet);

      /* ajouter une section de pile (?) */
      ajouterPile(objet);
   }
   else {
      printf ( "%s%s%s%s%s\n",
              "panique (implanterObjet) ! : ",
              "l'objet ",
              objet->nom,
              " est une copie (superficielle) d'un autre objet ",
              "et ne peut etre implante");
      exit(1);
   }
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
void cloreObjet(ObjetEDL *objet) {
/**********************************************************************/
   Elf32_Ehdr *ehdr;
   off_t tailleFichier;

   if (objet->mode != ELF_C_WRITE) {
      printf ( "%s%s%s\n",
              "erreur : l'objet ",
              objet->nom,
              " est ouvert en lecture (cloture impossible)");
      exit(1);
   }

   /* initialisation libelf */
   initialiserLibelf();

   /* creation image ELF de l'objet */
   //objet->elf = elf_begin(objet->fd, objet->mode, NULL);
   if (objet->elf == NULL) {
      printf ( "%s%s%s\n",
              "erreur : creation image ELF (",
              objet->nom,
              ") impossible");
      exit(1);
   }

   /* creer descripteur (entete) de fichier Elf */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   //objet->ehdr = elf32_newehdr(objet->elf);
   ehdr = objet->ehdr;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* cloture des sections */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   cloreSections(objet);
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* enregistrement des attributs d'objet dans entete Elf */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   ehdr->e_ident[EI_DATA] = objet->boutisme;
   ehdr->e_ident[EI_CLASS] = objet->classe;
   ehdr->e_ident[EI_OSABI] = objet->systeme;
   ehdr->e_type = objet->type;
   ehdr->e_machine = objet->processeur;
   ehdr->e_version = objet->version;
   ehdr->e_flags = objet->flags;

   /* index de la table des noms de section */
   ehdr->e_shstrndx = objet->tableNomsSection->indexElf;
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* DEBUT SUPPRIMER FUSION */

   if (objet->type == ET_EXEC) {
      /* cloture des segments */
      cloreSegments(objet);

      /* demande de prise en compte de la disposition du fichier */
      //elf_flagelf(objet->elf, ELF_C_SET, ELF_F_LAYOUT);

      /* attributs d'objet executable */
      ehdr->e_entry = objet->pointEntree;
      ehdr->e_phoff = sizeof(Elf32_Ehdr);
      ehdr->e_shoff = objet->tailleFichier;
   }
   /* FIN SUPPRIMER FUSION */

   /* ecriture du fichier */
   //tailleFichier = elf_update(objet->elf, ELF_C_WRITE);
   if (tailleFichier == -1) {
      printf ( "%s%s%s\n",
              "erreur : ecriture image ELF (",
              objet->nom,
              ") impossible");
      //printf ( "%s\n", elf_errmsg(elf_errno()));
      exit(1);
   }
}

/**********************************************************************/
void fermerObjet(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   if (!objet->estCopie) {
      fermerSections(objet);
      /* DEBUT SUPPRIMER FUSION */

      if (objet->type == ET_EXEC) {
         fermerSegments(objet);
      }
      /* FIN SUPPRIMER FUSION */

      if (objet->iterateur != NULL) {
         fermerIterObjet(&(objet->iterateur));
      }
   }

   if (objet->nom != NULL) {
      free(objet->nom);
   }

   //elf_end(objet->elf);
}

/**********************************************************************/
void afficherObjet(ObjetEDL *objet) {
/**********************************************************************/
   printf("\n");
   afficherAttributs(objet);
   printf("\n");
   afficherSections(objet);
   printf("\n");
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void ajouterSectionBss(ObjetEDL *objet, SectionEDL **sectionResultat) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SectionEDL *sectionReference;
   SectionEDL *sectionBss;

   /* creation section de reference fictive */
   sectionReference = (SectionEDL *)calloc(1, sizeof(SectionEDL));
   sectionReference->nom = creationChaine(".bss", 0, FAUX);
   sectionReference->type = SHT_NOBITS;
   sectionReference->flags = SHF_WRITE;
   sectionReference->alignement = 1;
   sectionReference->tailleElement = 0;

   /* ajout section pile */
   ajouterSection(objet, sectionReference, 0, 0, &sectionBss);

   /* liberation section de reference fictive */
   libererChaine(&(sectionReference->nom));
   free(sectionReference);

   if (sectionResultat != NULL) {
      *sectionResultat = sectionBss;
   }
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
SectionEDL *sectionSelonIndex(ObjetEDL *objet, unsigned int index) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   unsigned int i;

   if (index == SHN_UNDEF || index >= SHN_LORESERVE) {
      return NULL;
   }

   sections = objet->sections;;
   nbSections = objet->nbSections;;

   for (i = 0 ; i < nbSections ; i++) {
      if (sections[i]->indexElf == index) {
         return sections[i];
      }
   }
   printf ( "%s%s%d%s%s\n",
           "panique (sectionSelonIndex) ! : ",
           "aucune section d'index ",
           index,
           " n'existe dans l'objet ",
           objet->nom);
   exit(1);
   /* FIN COMPLETER FUSION IMPLANTATION */
   /* AJOUTER FUSION IMPLANTATION
   return NULL;
   */
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
SectionEDL *sectionSelonNomEtType(ObjetEDL *objet,
                                  char *nom, unsigned int type) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   unsigned int i;

   sections = objet->sections;;
   nbSections = objet->nbSections;;

   for (i = 0 ; i < nbSections ; i++) {
      if ((nom == NULL || strcmp(sections[i]->nom->chaine, nom) == 0)
          && sections[i]->type == type) break;
   }
   if (i < nbSections) {
      return sections[i];
   }
   else {
      return NULL;
   }
   /* FIN COMPLETER IMPLANTATION */
   /* AJOUTER IMPLANTATION
   return NULL;
   */
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
unsigned int sontCompatibles(ObjetEDL *objet1, ObjetEDL *objet2) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   return objet1->type == objet2->type
          && objet1->boutisme == objet2->boutisme
          && objet1->classe == objet2->classe
          && objet1->systeme == objet2->systeme
          && objet1->processeur == objet2->processeur
          && objet1->version == objet2->version
          && objet1->flags == objet2->flags;
}

/**********************************************************************/
unsigned int ontMemeBoutismeHoteObjet(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   static unsigned int boutismeHote = ELFDATANONE;
   unsigned int i = 0x12345678;
   unsigned char *pi = (unsigned char *)&i;

   if (boutismeHote == ELFDATANONE) {
      if (*pi == 0x12) {
         boutismeHote = ELFDATA2MSB;
      }
      else if (*pi == 0x78) {
         boutismeHote = ELFDATA2LSB;
      }
      else {
         printf ( "%s%s\n",
                 "panique (ontMemeBoutismeHoteObjet) ! : ",
                 "boutisme hote inconnu");
         exit(1);
      }
   }
   return objet->boutisme == boutismeHote;
}

/**********************************************************************/
void memoriserNomObjet(ObjetEDL *objet, char *nom) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   objet->nom = (char *)malloc(strlen(nom) + 1);
   strcpy(objet->nom, nom);
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void memoriserAdresseStack(unsigned long adressePile) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   adresseStack = adressePile;
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void memoriserNomPointEntree(char *nomSymbole) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   libererNomPointEntree();
   nomPtEntree = (char *)malloc(strlen(nomSymbole) + 1);
   strcpy(nomPtEntree, nomSymbole);
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
void libererNomPointEntree(void) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   if (nomPtEntree != NULL) {
      free(nomPtEntree);
   }
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
char *nomPointEntree(void) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   return nomPtEntree;
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
STATIC void ouvrirSections(ObjetEDL *objet) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   IterateurObjet *iterateur;
   size_t indexNomsSections, indexCourant;
   SectionEDL *tableNomsSection, *sectionCourante;
   unsigned int i;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* memorisation du nombre de sections */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   nbSections = objet->ehdr->e_shnum - 1;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* allocation des sections */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   sections = (SectionEDL **)calloc(nbSections, sizeof(SectionEDL *));
   for (i = 0 ; i != nbSections ; i++) {
      sections[i] = (SectionEDL *)calloc(1, sizeof(SectionEDL));
   }
   objet->sections = sections;
   objet->nbSections = nbSections;
   objet->nbMax = nbSections;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* creation de l'iterateur d'objet */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   creerIterObjet(&(objet->iterateur));
   iterateur = objet->iterateur;
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* ouverture de la table des noms de section */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   objet->tableNomsSection = sections[0];
   tableNomsSection = objet->tableNomsSection;
   indexNomsSections = objet->ehdr->e_shstrndx;
   initialiserOuvertureSection(tableNomsSection, objet,
                               indexNomsSections);
   ajouterAIterObjet(iterateur, tableNomsSection);

   ouvrirSection(tableNomsSection);
   ajouterAIterGlobal(tableNomsSection);
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* ouverture des autres sections : etape preliminaire */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   i = 1;
   indexCourant = 1;
   while (indexCourant != nbSections + 1) {
      if (indexCourant != indexNomsSections) {
         initialiserOuvertureSection(sections[i], objet, indexCourant);
         ajouterAIterObjet(iterateur, sections[i]);
         i++;
      }
      indexCourant++;
   }
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* ouverture des autres sections : ouverture proprement dite */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   demarrerIterObjet(iterateur, INSTRUCTIONS);
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      if (sectionCourante != tableNomsSection) {
         ouvrirSection(sectionCourante);
         ajouterAIterGlobal(sectionCourante);
      }
      avancerIterObjet(iterateur);
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
STATIC void cloreSections(ObjetEDL *objet) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   IterateurObjet *iterateur;
   SectionEDL *tableNomsSection, *sectionCourante;
   unsigned int i;

   /* initialisation cloture toutes sections */
   sections = objet->sections;
   nbSections = objet->nbSections;
   for (i = 0 ; i != nbSections ; i++) {
      initialiserClotureSection(sections[i]);
   }

   /* cloture table des noms de section */
   tableNomsSection = objet->tableNomsSection;
   cloreSection(tableNomsSection);

   /* cloture autres sections */
   iterateur = objet->iterateur;
   demarrerIterObjet(iterateur, INSTRUCTIONS);
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      if (sectionCourante != tableNomsSection) {
         cloreSection(sectionCourante);
      }
      avancerIterObjet(iterateur);
   }
   /* FIN COMPLETER FUSION IMPLANTATION */
}

/**********************************************************************/
STATIC void fermerSections(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   SectionEDL *tableNomsSection;
   unsigned int i;

   sections = objet->sections;
   nbSections = objet->nbSections;
   tableNomsSection = objet->tableNomsSection;

   if (sections != NULL) {
      /* fermer les sections (sauf table noms de section) */
      for (i = 0 ; i != nbSections ; i++) {
         if (sections[i] != tableNomsSection) {
            fermerSection(sections[i]);
         }
      }

      /* fermer la table des noms de section */
      fermerSection(tableNomsSection);

      /* liberer toutes les descripteurs de section */
      for (i = 0 ; i != nbSections ; i++) {
         if (sections[i] != NULL) {
            free(sections[i]);
            sections[i] = NULL;
         }
      }

      /* liberer le conteneur */
      free(sections);
      objet->sections = NULL;
   }
}

/**********************************************************************/
STATIC void ajouterSection(ObjetEDL *objet,
                           SectionEDL *sectionReference,
                           unsigned int taille,
                           unsigned int alignement,
                           SectionEDL **sectionCible) {
/**********************************************************************/
   SectionEDL *nouvelleSection;

   /* allocation de la section */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   allouerSection(objet, &nouvelleSection);
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* creation de la section */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   creerSection(nouvelleSection, objet, sectionReference, taille,
                alignement);
   /* FIN COMPLETER FUSION IMPLANTATION */

   /* ajout de la section a l'iterateur de l'objet */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   ajouterAIterObjet(objet->iterateur, nouvelleSection);
   /* FIN COMPLETER FUSION IMPLANTATION */

   /*
   memorisation de la correspondance fonctionnelle entre la section
   d'origine et la nouvelle section
   */
   /* DEBUT COMPLETER FUSION IMPLANTATION */
   sectionReference->sectionImage = nouvelleSection;
   /* FIN COMPLETER FUSION IMPLANTATION */

   if (sectionCible != NULL) {
      *sectionCible = nouvelleSection;
   }
}

/**********************************************************************/
STATIC void allouerSection(ObjetEDL *objet, SectionEDL **section) {
/**********************************************************************/
   SectionEDL **sections;
   unsigned int nbSections;
   unsigned int nbMax;

   sections = objet->sections;
   nbSections = objet->nbSections;
   nbMax = objet->nbMax;

   /* DEBUT COMPLETER FUSION IMPLANTATION */
   if (nbSections == nbMax) {
      nbMax = nbMax + 1;
      sections =
                (SectionEDL **)realloc(sections,
                                       nbMax * sizeof(SectionEDL *));
      objet->nbMax = nbMax;
      objet->sections = sections;
   }

   sections[nbSections] = (SectionEDL *)calloc(1, sizeof(SectionEDL));

   objet->nbSections = nbSections + 1;
   /* FIN COMPLETER FUSION IMPLANTATION */

   if (section != NULL) {
      *section = sections[nbSections];
   }
}
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void supprimerSection(ObjetEDL *objet, SectionEDL *section) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   SectionEDL *tableNomsSection;
   IterateurObjet *iterateur;
   unsigned int i;

   sections = objet->sections;
   nbSections = objet->nbSections;
   tableNomsSection = objet->tableNomsSection;
   iterateur = objet->iterateur;

   for (i = 0 ; i != nbSections ; i++) {
      if (sections[i] == section) break;
   }
   if (i != nbSections) {
      retirerDeIterObjet(iterateur, sections[i]);
      supprimerChaine(tableNomsSection, sections[i]->nom);
      fermerSection(sections[i]);
      if (sections[i] != NULL) {
         free(sections[i]);
      }
      sections[i] = sections[nbSections - 1];
      objet->nbSections = nbSections - 1;
   }
   else {
      printf ( "%s%s%s%s%s\n",
              "panique (supprimerSection) ! : ",
              "la section ",
              section->nom->chaine,
              " n'appartient pas a l'objet ",
              objet->nom);
      exit(1);
   }
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC void creerSectionsFusionnees(ObjetEDL *objetCible) {
/**********************************************************************/
   SectionEDL *sectionSource;
   SectionEDL *sectionCible;
   unsigned int tailleMaxCible;
   unsigned int alignementCourant;

   /* acces aux sections d'entree (nom par nom) */
   demarrerIterGlobalNom();

   /*
   parcours nom par nom des sections d'entree et creation des sections
   resultat
   */
   while (!finIterGlobalNom()) {

      /* acces a la 1ere section a fusionner ayant ce nom */
      /* DEBUT COMPLETER FUSION */
      demarrerIterGlobalSection();
      sectionSource = sectionCouranteIterGlobal();
      /* FIN COMPLETER FUSION */

      /* la table des noms de section a deja ete creee */
      if (strcmp(sectionSource->nom->chaine, ".shstrtab") != 0) {

         /* taille minimum de la section resultat */
         /* DEBUT COMPLETER FUSION */
         alignementCourant = sectionSource->alignement;
         tailleMaxCible = tailleSection(sectionSource);
         /* FIN COMPLETER FUSION */

         /* calcul de la taille totale de la section resultat */
         /* DEBUT COMPLETER FUSION */
         avancerIterGlobalSection();
         while (!finIterGlobalSection()) {
            sectionSource = sectionCouranteIterGlobal();
            switch (sectionSource->type) {
               case SHT_STRTAB:
               case SHT_SYMTAB:
               case SHT_REL:
                  tailleMaxCible += tailleSection(sectionSource);
                  break;
               default:
                  alignementCourant = sectionSource->alignement;
                  tailleMaxCible = alignement(tailleMaxCible,
                                              alignementCourant)
                                   + tailleSection(sectionSource);
                  break;
            }
            avancerIterGlobalSection();
         }
         /* FIN COMPLETER FUSION */

         /*
         acces a l'alignement maximum des sections associees au nom
         courant
         */
         /* DEBUT COMPLETER FUSION */
         alignementCourant = alignementCourantIterGlobal();
         /* FIN COMPLETER FUSION */

         /*
         acces a la 1ere section a fusionner dont le nom est le nom
         courant
         */
         /* DEBUT COMPLETER FUSION */
         demarrerIterGlobalSection();
         sectionSource = sectionCouranteIterGlobal();
         /* FIN COMPLETER FUSION */

         /* ajout d'une nouvelle section a l'objet resultat */
         /* DEBUT COMPLETER FUSION */
         ajouterSection(objetCible, sectionSource, tailleMaxCible,
                        alignementCourant, &sectionCible);
         /* FIN COMPLETER FUSION */

         /*
         memorisation de la correspondance fonctionnelle entre les
         autres sections d'entree de meme nom et la section fusionnee
         */
         /* DEBUT COMPLETER FUSION */
         avancerIterGlobalSection();
         while (!finIterGlobalSection()) {
            sectionSource = sectionCouranteIterGlobal();
            sectionSource->sectionImage = sectionCible;
            avancerIterGlobalSection();
         }
         /* FIN COMPLETER FUSION */
      }

      /* nom suivant */
      avancerIterGlobalNom();
   }
}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER IMPLANTATION */

/**********************************************************************/
STATIC void fusionnerSections(ObjetEDL *objetCible) {
/**********************************************************************/
   /* DEBUT COMPLETER FUSION */
   SectionEDL *sectionCible;
   SectionEDL *sectionSource;
   IterateurObjet *iterateurCible;

   iterateurCible = objetCible->iterateur;

   /* acces aux sections resultat */
   demarrerIterObjet(iterateurCible, INSTRUCTIONS);

   /* acces aux sections d'entree (nom par nom) */
   demarrerIterGlobalNom();

   /* parcours nom par nom des sections d'entree */
   while (!finIterGlobalNom()) {

      /* acces a la 1ere section a fusionner ayant ce nom */
      demarrerIterGlobalSection();
      sectionSource = sectionCouranteIterGlobal();

      /* les tables de noms de section ne doivent pas etre fusionnees */
      if (strcmp(sectionSource->nom->chaine, ".shstrtab") != 0) {

         /* acces a la section resultat courante */
         sectionCible = sectionCouranteIterObjet(iterateurCible);

         /* fusion de la 1ere section d'entree ayant ce nom */
         fusionnerSection(sectionSource, sectionCible);

         /* fusion des autres sections d'entree ayant ce nom */
         avancerIterGlobalSection();
         while (!finIterGlobalSection()) {
            sectionSource = sectionCouranteIterGlobal();
            fusionnerSection(sectionSource, sectionCible);
            avancerIterGlobalSection();
         }
      }

      /* section resultat suivante */
      avancerIterObjet(iterateurCible);

      /* nom suivant */
      avancerIterGlobalNom();
   }
   /* FIN COMPLETER FUSION */
}
/* FIN SUPPRIMER IMPLANTATION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void resoudreSymbolesCommuns(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   IterateurObjet *iterateur;
   SectionEDL *sectionCourante;
   CategorieSection categorie;

   iterateur = objet->iterateur;
   demarrerIterObjet(iterateur, TABLE_SYMBOLES);
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != TABLE_SYMBOLES) break;
      promouvoirSymbolesCommuns(sectionCourante);
      avancerIterObjet(iterateur);
   }
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void resoudreSymbolesCRT(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SectionEDL *tableSymboles;
   IterateurObjet *iterateur;
   SectionEDL *sectionCourante;
   CategorieSection categorie;
   unsigned long valeurSymboleStart;
   unsigned long valeurSymboleEnd;
   unsigned int tailleSymboleEnd;

   /* recherche de la 1ere table de symboles de nom ".symtab" */
   tableSymboles = sectionSelonNomEtType(objet, ".symtab", SHT_SYMTAB);

   if (tableSymboles != NULL) {
      /* calcul de la valeur des symboles */
      iterateur = objet->iterateur;
      demarrerIterObjet(iterateur, DONNEES_NON_INITIALISEES);
      if (!finIterObjet(iterateur)) {
         sectionCourante = sectionCouranteIterObjet(iterateur);
         categorie = categorieSection(sectionCourante);
         if (categorie == DONNEES_NON_INITIALISEES) {
            valeurSymboleStart = sectionCourante->adresse;
            valeurSymboleEnd = sectionCourante->adresse;
            tailleSymboleEnd = sectionCourante->tailleContenuElf;
            avancerIterObjet(iterateur);
            while (!finIterObjet(iterateur)) {
               sectionCourante = sectionCouranteIterObjet(iterateur);
               categorie = categorieSection(sectionCourante);
               if (categorie != DONNEES_NON_INITIALISEES) break;
               if (sectionCourante->adresse < valeurSymboleStart) {
                  valeurSymboleStart = sectionCourante->adresse;
               }
               if (sectionCourante->adresse > valeurSymboleEnd) {
                  valeurSymboleEnd = sectionCourante->adresse;
                  tailleSymboleEnd = sectionCourante->tailleContenuElf;
               }
               avancerIterObjet(iterateur);
            }
         }
      }
      valeurSymboleEnd += tailleSymboleEnd;
      valeurSymboleEnd = (valeurSymboleEnd + 3) & ~3;

      /* ajustement des symboles */
      ajusterSymbolesCRT(tableSymboles, valeurSymboleStart,
                                        valeurSymboleEnd);
   }
   else {
      printf ( "%s%s%s%s\n",
              "panique (resoudreSymbolesCRT) ! : ",
              "aucune table symboles de nom .symtab ",
              " n'existe dans l'objet ",
              objet->nom);
      exit(1);
   }
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void implanterTablesSymboles(ObjetEDL *objet) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   IterateurObjet *iterateur;
   SectionEDL *sectionCourante;
   CategorieSection categorie;

   iterateur = objet->iterateur;

   demarrerIterObjet(iterateur, TABLE_SYMBOLES);
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != TABLE_SYMBOLES) break;
      implanterTableSymboles(sectionCourante);
      supprimerSymbolesSectionInutiles(sectionCourante);
      avancerIterObjet(iterateur);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void completerSectionsCode(ObjetEDL *objet) {
/**********************************************************************/
   /* DEBUT COMPLETER IMPLANTATION */
   IterateurObjet *iterateur;
   SectionEDL *sectionCourante;
   CategorieSection categorie;
   SectionEDL *tableSymboles;

   iterateur = objet->iterateur;

   demarrerIterObjet(iterateur, TABLE_TRANSLATION);
   while (!finIterObjet(iterateur)) {
      sectionCourante = sectionCouranteIterObjet(iterateur);
      categorie = categorieSection(sectionCourante);
      if (categorie != TABLE_TRANSLATION) break;

         /* appliquer informations de translation */
         completerSectionCode(sectionCourante);

         /* supprimer table de translation : le symbole de section */
         tableSymboles =
                       sectionCourante->tableTranslation->tableSymboles;
         supprimerSymboleSelonNom(tableSymboles,
                                  sectionCourante->nom);

         /* supprimer table de translation : la section elle-meme */
         supprimerSection(objet, sectionCourante);

         avancerIterObjet(iterateur);
   }
   /* FIN COMPLETER IMPLANTATION */
}
/* FIN SUPPRIMER FUSION */
/* DEBUT SUPPRIMER FUSION */

/**********************************************************************/
STATIC void ajouterPile(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL IMPLANTATION */
   SectionEDL *sectionReference;
   SectionEDL *sectionPile;
   SectionEDL *tableSymboles;
   SymboleEDL symbole;

   /* recherche de la 1ere table de symboles de nom ".symtab" */
   tableSymboles = sectionSelonNomEtType(objet, ".symtab", SHT_SYMTAB);

   if (tableSymboles != NULL) {
      /* creation section de reference fictive */
      sectionReference = (SectionEDL *)calloc(1, sizeof(SectionEDL));
      sectionReference->nom = creationChaine(".stack", 0, FAUX);
      sectionReference->type = SHT_PROGBITS;
      sectionReference->flags = SHF_WRITE;
      sectionReference->alignement = 1;
      sectionReference->tailleElement = 0;

      /* ajout section pile */
      ajouterSection(objet, sectionReference, 0, 0, &sectionPile);
      sectionPile->adresse = (adresseStack + (TAILLE_PAGE - 1))
                             & ~(TAILLE_PAGE - 1);

      /* ajout symbole de pile */
      symbole.nom = creationChaine("_stack", 0, FAUX);
      symbole.valeur = sectionPile->adresse;
      symbole.taille = 0;
      symbole.portee = STB_GLOBAL;
      symbole.type = STT_NOTYPE;
      symbole.natureDefinition = SHN_UNDEF;
      symbole.sectionDefinition = sectionReference;
      ajouterSymbole(tableSymboles, &symbole, NULL);
      libererChaine(&(symbole.nom));

      /* ajout symbole de section pile */
      symbole.nom = creationChaine(".stack", 0, FAUX);
      symbole.portee = STB_LOCAL;
      symbole.type = STT_SECTION;
      ajouterSymbole(tableSymboles, &symbole, NULL);
      libererChaine(&(symbole.nom));

      /* liberation section de reference fictive */
      libererChaine(&(sectionReference->nom));
      free(sectionReference);
   }
   else {
      printf ( "%s%s%s%s\n",
              "panique (ajouterPile) ! : ",
              "aucune table symboles de nom .symtab ",
              " n'existe dans l'objet ",
              objet->nom);
      exit(1);
   }
}
/* FIN SUPPRIMER FUSION */

/**********************************************************************/
STATIC void initialiserLibelf(void) {
/**********************************************************************/
   static unsigned int libelfInitialisee = FAUX;

   /* if (!libelfInitialisee) {
      if (elf_version(EV_CURRENT) == EV_NONE) {
         printf ( "%s\n",
                 "erreur : version libelf obsolete");
         exit(1);
      }
      libelfInitialisee = VRAI;
   } */
}

/**********************************************************************/
STATIC void afficherSections(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   SectionEDL **sections;
   unsigned int nbSections;
   size_t indexCourant;
   unsigned int i;

   afficherTitreSections();
   sections = objet->sections;
   nbSections = objet->nbSections;
   indexCourant = 1;
   while (indexCourant != nbSections + 1) {
      i = 0;
      while (i != nbSections && sections[i]->indexElf != indexCourant) {
         i++;
      }
      if (i != nbSections) {
         afficherSection(sections[i]);
      }
      else {
         afficherSection(sections[indexCourant - 1]);
      }
      indexCourant++;
   }
}

/**********************************************************************/
STATIC void afficherAttributs(ObjetEDL *objet) {
/**********************************************************************/
   /* LIBEDL FUSION IMPLANTATION */
   Elf32_Ehdr *ehdr;

   printf("%-40s%s\n", "nom de l'objet : ", objet->nom);

   printf("%-40s", "type d'objet : ");
   switch(objet->type) {
      case ET_REL:
         printf("binaire translatable\n");
         break;
      case ET_EXEC:
         printf("executable\n");
         break;
      default:
         printf("autre type\n");
         break;
   }

   printf("%-40s", "processeur cible : ");
   switch(objet->processeur) {
      case EM_SPARC:
         printf("Sparc\n");
         break;
      case EM_386:
         printf("Intel 386\n");
         break;
      case EM_ARM:
         printf("Arm\n");
         break;
      default:
         printf("inconnu\n");
         break;
   }

   printf("%-40s", "ordre des octets : ");
   switch(objet->boutisme) {
      case ELFDATA2LSB:
         printf("petit bout\n");
         break;
      case ELFDATA2MSB:
         printf("gros bout\n");
         break;
      default:
         printf("inconnu\n");
         break;
   }

   if (objet->ehdr != NULL) {
      ehdr = objet->ehdr;
      if (objet->type == ET_EXEC) {
         printf("%-40s0x%x\n", "adresse du point d'entree : ",
                               ehdr->e_entry);
      }
      printf("%-40s%d\n", "taille de l'entete ELF : ", ehdr->e_ehsize);
      printf("%-40s%d\n", "taille d'une entete de section : ",
                          ehdr->e_shentsize);
      printf("%-40s%d\n", "nombre d'entetes de section : ",
                          ehdr->e_shnum);
      printf("%-40s%d\n", "index de la table des noms de section : ",
                          ehdr->e_shstrndx);
   }
}
