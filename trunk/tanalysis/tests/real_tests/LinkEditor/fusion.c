/***********************************************************************
fusion.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "objet.h"
#include "iterateur.h"

#define VRAI 1
#define FAUX 0

#define NB_MAX_FICHIERS 13

static void analyserLigneCommande(int, char **);
/***********************************************************************
analyserLigneCommande(argc, argv) : analyse les arguments de la ligne de
commande.
- etat initial : indifferent
- etat final :
  . la ligne de commande a ete analysee
  . les noms des fichiers d'entree et du fichier de sortie sont
    memorises
***********************************************************************/

static void ouvrirObjetsEntree();
/***********************************************************************
ouvrirObjetsEntree() : ouvre les objets a fusionner
- etat initial : les noms des fichiers d'entree ont ete memorises
- etat final :
  . les objets contenus dans les fichiers d'entree sont ouverts et prets
    a etre fusionnes
***********************************************************************/

static void realiserFusion();
/***********************************************************************
realiserFusion() : fusionne les objets d'entree dans l'objet resultat.
- etat initial :
  . les objets d'entree sont ouverts
  . le nom du fichier de sortie a ete memorise
- etat final :
  . l'objet de sortie a ete cree
  . son contenu represente le resultat de la fusion des objets d'entree
***********************************************************************/

static void cloreObjetSortie();
/***********************************************************************
cloreObjetSortie() :
- etat initial :
  . les objets d'entree ont ete fusionnes
  . l'objet de sortie represente le resultat de cette fusion
- etat final :
  . le fichier de sortie a ete cree
  . l'objet resultat a ete ecrit dans le fichier de sortie
***********************************************************************/

static void fermerObjetsEntree();
/***********************************************************************
fermerObjetsEntree() : libere la memoire utilisee par les objets
d'entree et ferme les fichiers correspondants.
- etat initial :
  . les objets d'entree sont ouverts
- etat final :
  . la memoire utilisee par les objets d'entree a ete liberee
  . les fichiers d'entree sont fermes
***********************************************************************/

static void fermerObjetSortie();
/***********************************************************************
fermerObjetSortie() : libere la memoire utilisee par l'objet resultat et
et ferme le fichier correspondant.
- etat initial :
  . l'objet de sortie a ete cree
- etat final :
  . la memoire utilisee par l'objet de sortie a ete liberee
  . le fichier de sortie est ferme
***********************************************************************/

static void libererDonneesGlobales();
/***********************************************************************
libererDonneesGlobales() : libere la memoire utilisee par les donnees
globales du programme (l'iterateur global en particulier).
- etat initial :
  . les objets d'entree ont ete fusionnes et fermes
  . l'objet resultat a ete ferme
- etat final :
  . la memoire utilisee par les donnees globales a ete liberee
***********************************************************************/

static void afficherUsage();
/***********************************************************************
afficherUsage(s) : affiche sur la sortie designee par s le format de la
ligne de commande permettant d'invoquer le programme
***********************************************************************/

static ObjetEDL objets[NB_MAX_FICHIERS];
/***********************************************************************
objets[0] :                        objet resultat
objets[1 .. NB_MAX_FICHIERS - 1] : objets a fusionner
***********************************************************************/

static unsigned int nbFichiersDonnees;
/***********************************************************************
nombre de fichiers d'entree contenant des objets a fusionner
***********************************************************************/

/**********************************************************************/
int main(int argc, char **argv) {
/**********************************************************************/
   analyserLigneCommande(argc, argv);
   ouvrirObjetsEntree();
   realiserFusion();
   cloreObjetSortie();
   fermerObjetSortie();
   fermerObjetsEntree();
   libererDonneesGlobales();
   return 0;
}

/**********************************************************************/
static void analyserLigneCommande(int argc, char **argv) {
/**********************************************************************/
   unsigned int i;

   objets[0].nom = NULL;
   nbFichiersDonnees = 0;

   for (i = 1 ; i != argc ; i++) {
      if (strcmp(argv[i], "-h") == 0) {
         afficherUsage(stdout);
         exit(0);
      }
      else if (strcmp(argv[i], "-o") == 0) {
         if (objets[0].nom == NULL) {
            i++;
            if (i != argc && argv[i][0] != '-') {
               memoriserNomObjet(&(objets[0]), argv[i]);
            }
            else { /* i == argc || argv[i][0] == '-' */
               printf ( "%s%s\n", "erreur : ",
                       "-o doit etre suivi d'un nom de fichier");
               exit(1);
            }
         }
         else { /* objets[0].nom != NULL */
            printf ( "%s%s\n", "erreur : ",
                    "un seul fichier resultat autorise");
            exit(1);
         }
      }
      else {
         /*
         strcmp(argv[i], "-h") != 0 && strcmp(argv[i], "-o") != 0
         */
         if (argv[i][0] != '-') {
            if (nbFichiersDonnees != NB_MAX_FICHIERS - 1) {
               nbFichiersDonnees += 1;
               memoriserNomObjet(&(objets[nbFichiersDonnees]), argv[i]);
            }
            else { /* nbFichiersDonnees == NB_MAX_FICHIERS - 1 */
               printf ( "%s%s%d\n", "erreur : ",
                       "le nb. de fichiers d'entree doit etre < ",
                       NB_MAX_FICHIERS);
               exit(1);
            }
         }
         else { /* argv[i][0] == '-' */
            printf ( "%s\n",
                  "erreur : argument invalide");
            afficherUsage();
            exit(1);
         }
      }
   }

   if (objets[0].nom == NULL) {
      printf ( "%s%s\n", "erreur : ",
              "fichier resultat obligatoire");
      afficherUsage();
      exit(1);
   }
   if (nbFichiersDonnees < 1) {
      printf ( "%s%s\n", "erreur : ",
              "un fichier donnee au moins est requis");
      afficherUsage();
      exit(1);
   }
   for (i = 0 ; i != nbFichiersDonnees + 1 ; i++) {
      unsigned int j;
      for (j = i + 1 ; j != nbFichiersDonnees + 1 ; j++) {
         if (strcmp(objets[j].nom, objets[i].nom) == 0) {
            printf ( "%s%s%s\n", "erreur : ",
                    "les fichiers resultat et donnees ",
                    "doivent etre distincts deux a deux");
            exit(1);
         }
      }
   }
}

/**********************************************************************/
void ouvrirObjetsEntree(void) {
/**********************************************************************/
   unsigned int i;

   for (i = 1 ; i != nbFichiersDonnees + 1 ; i++) {

      /* ouvrir le fichier */
      objets[i].fd = open(objets[i].nom, O_RDONLY);
      if (objets[i].fd == -1) {
         perror(objets[i].nom);
         exit(1);
      }

      /* ouvrir l'objet */
      ouvrirObjet(&(objets[i]), FAUX);

      /* verifier sa compatibilite */
      if (i != 1 && !sontCompatibles(&(objets[i]), &(objets[1]))) {
         printf ( "%s%s%s%s%s\n", "erreur : ",
                 "caracteristiques du fichier (",
                 objets[i].nom,
                 ") incompatibles avec celle du fichier ",
                 objets[1].nom);
         exit(1);
      }
   }
}

/**********************************************************************/
void realiserFusion(void) {
/**********************************************************************/
   unsigned int nbSections;
   unsigned int i;

   /* calculer le nombre de sections maximum de l'objet resultat */
   nbSections = 0;
   for (i = 1 ; i <= nbFichiersDonnees ; i++) {
      nbSections += objets[i].nbSections;
   }

   /*
   creer l'objet de sortie et l'initialiser selon le 1er objet d'entree
   */
   creerObjet(&(objets[0]), &(objets[1]), ET_REL, nbSections);

   /* fusionner les objets d'entree */
   fusionnerObjet(objets);
}

/**********************************************************************/
static void cloreObjetSortie(void) {
/**********************************************************************/
   /* creer le fichier de sortie */
   objets[0].fd = open(objets[0].nom, O_WRONLY | O_CREAT | O_TRUNC,
                       S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
   if (objets[0].fd == -1) {
      perror(objets[0].nom);
      exit(1);
   }

   /* clore l'objet de sortie */
   cloreObjet(&(objets[0]));
}

/**********************************************************************/
void fermerObjetsEntree(void) {
/**********************************************************************/
   unsigned int i;

   for (i = 1 ; i != nbFichiersDonnees + 1 ; i++) {
      fermerObjet(&objets[i]);
      // close(objets[i].fd);
   }
}

/**********************************************************************/
void fermerObjetSortie(void) {
/**********************************************************************/
   fermerObjet(&objets[0]);
   // close(objets[0].fd);
}

/**********************************************************************/
static void libererDonneesGlobales(void) {
/**********************************************************************/
   viderIterGlobal();
}

/**********************************************************************/
static void afficherUsage() {
/**********************************************************************/
   printf("%s%s\n",
              "usage : fusion -o resultat donnee1 [donnee2 [...]]",
              "             [ -h ]");
}
