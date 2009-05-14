/***********************************************************************
implantation.c

P.Habraken - 22/02/2009
***********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

#include "objet.h"
#include "segments.h"
#include "iterateur.h"

#define VRAI 1
#define FAUX 0

static void analyserLigneCommande(int, char **);
/***********************************************************************
analyserLigneCommande(argc, argv) :
- etat initial : indifferent
- etat final :
  . la ligne de commande a ete analysee
  . les noms du fichier d'entree et du fichier de sortie sont memorises
  . les parametres d'implantation de l'objet d'entree sont memorises
***********************************************************************/

static void ouvrirObjetEntree();
/***********************************************************************
ouvrirObjetEntree() :
- etat initial : le nom du fichier d'entree a ete memorise
- etat final :
  . l'objet contenu dans le fichier d'entree est ouvert et pret a etre
    implante
***********************************************************************/

static void realiserImplantation();
/***********************************************************************
realiserImplantation() :
- etat initial :
  . l'objet d'entree est ouvert
  . le nom du fichier de sortie a ete memorise
- etat final :
  . l'objet de sortie a ete cree
  . son contenu represente le resultat de l'implantation de l'objet
    d'entree
  . l'objet de sortie a ete ecrit dans le fichier resultat
***********************************************************************/

static void cloreObjetSortie();
/***********************************************************************
cloreObjetSortie() :
- etat initial :
  . l'objet d'entree a ete implante
  . l'objet de sortie represente le resultat de cette implantation
- etat final :
  . le fichier de sortie a ete cree
  . l'objet resultat a ete ecrit dans le fichier de sortie
***********************************************************************/

static void fermerObjetEntree();
/***********************************************************************
fermerObjetEntree() :
- etat initial :
  . l'objet d'entree est ouvert
- etat final :
  . la memoire utilisee par l'objet d'entree a ete liberee
  . le fichier d'entree est ferme
***********************************************************************/

static void fermerObjetSortie();
/***********************************************************************
fermerObjetSortie() :
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
  . l'objet d'entree a ete implante et ferme
  . l'objet resultat a ete ferme
- etat final :
  . la memoire utilisee par les donnees globales a ete liberee
***********************************************************************/

static void afficherUsage();
/***********************************************************************
afficherUsage(s) : affiche sur la sortie designee par s le format de la
ligne de commande permettant d'invoquer le programme
***********************************************************************/

static ObjetEDL objets[2];
/***********************************************************************
objets[0] : objet de sortie (objet resultat)
objets[1] : objet d'entree (objet a implanter)
***********************************************************************/

static unsigned int copieSuperficielle = VRAI;
/***********************************************************************
copieSuperficielle = VRAI <=> l'implantation sera appliquee a l'objet
                              d'entree => ouverture de l'objet d'entree
                              en lecture/ecriture
copieSuperficielle = FAUX <=> l'implantation sera appliquee a l'objet
                              resultat => ouverture de l'objet d'entree
                              en lecture seule
***********************************************************************/

/**********************************************************************/
int main(int argc, char **argv) {
/**********************************************************************/
   analyserLigneCommande(argc, argv);
   ouvrirObjetEntree();
   realiserImplantation();
   cloreObjetSortie();
   fermerObjetSortie();
   fermerObjetEntree();
   libererDonneesGlobales();
   return 0;
}

/**********************************************************************/
static void analyserLigneCommande(int argc, char **argv) {
/**********************************************************************/
   unsigned int nbFichiersDonnees;
   unsigned int i;
   unsigned long adresseText;
   unsigned long adresseData;
   unsigned long adresseRodata;
   unsigned long adresseBss;
   unsigned long adresseStack;

   objets[0].nom = NULL;
   nbFichiersDonnees = 0;
   memoriserNomPointEntree(DEFAUT_ENTREE);

   for (i = 1 ; i != argc ; i++) {
      if (strcmp(argv[i], "-h") == 0) {
         afficherUsage();
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
      else if (strcmp(argv[i], "-t") == 0) {
         i++;
         if (i == argc || sscanf(argv[i], "%li", &adresseText) != 1) {
            printf ( "%s%s\n", "erreur : ",
                    "-t doit etre suivi d'un entier naturel");
            exit(1);
         }
         else {
            memoriserAdresseText(adresseText);
         }
      }
      else if (strcmp(argv[i], "-d") == 0) {
         i++;
         if (i == argc || sscanf(argv[i], "%li", &adresseData) != 1) {
            printf ( "%s%s\n", "erreur : ",
                    "-d doit etre suivi d'un entier naturel");
            exit(1);
         }
         else {
            memoriserAdresseData(adresseData);
         }
      }
      else if (strcmp(argv[i], "-r") == 0) {
         i++;
         if (i == argc || sscanf(argv[i], "%li", &adresseRodata) != 1) {
            printf ( "%s%s\n", "erreur : ",
                    "-r doit etre suivi d'un entier naturel");
            exit(1);
         }
         else {
            memoriserAdresseRodata(adresseRodata);
         }
      }
      else if (strcmp(argv[i], "-b") == 0) {
         i++;
         if (i == argc || sscanf(argv[i], "%li", &adresseBss) != 1) {
            printf ( "%s%s\n", "erreur : ",
                    "-b doit etre suivi d'un entier naturel");
            exit(1);
         }
         else {
            memoriserAdresseBss(adresseBss);
         }
      }
      else if (strcmp(argv[i], "-s") == 0) {
         i++;
         if (i == argc || sscanf(argv[i], "%li", &adresseStack) != 1) {
            printf ( "%s%s\n", "erreur : ",
                    "-s doit etre suivi d'un entier naturel");
            exit(1);
         }
         else {
            memoriserAdresseStack(adresseStack);
         }
      }
      else if (strcmp(argv[i], "-e") == 0) {
         i++;
         if (i == argc) {
            printf ( "%s%s\n", "erreur : ",
                    "-e doit etre suivi d'un nom de symbole");
            exit(1);
         }
         else {
            memoriserNomPointEntree(argv[i]);
         }
      }
      else if (strcmp(argv[i], "-x") == 0) {
         copieSuperficielle = FAUX;
      }
      else {
         /*
         strcmp(argv[i], "-h") != 0 && strcmp(argv[i], "-o") != 0
                                    && strcmp(argv[i], "-t") != 0
                                    && strcmp(argv[i], "-d") != 0
                                    && strcmp(argv[i], "-r") != 0
                                    && strcmp(argv[i], "-b") != 0
                                    && strcmp(argv[i], "-s") != 0
                                    && strcmp(argv[i], "-e") != 0
                                    && strcmp(argv[i], "-x") != 0
         */
         nbFichiersDonnees += 1;
         memoriserNomObjet(&(objets[nbFichiersDonnees]), argv[i]);
      }
   }

   if (objets[0].nom == NULL) {
      printf ( "%s%s\n", "erreur : ",
              "fichier resultat obligatoire");
      afficherUsage();
      exit(1);
   }
   if (nbFichiersDonnees != 1) {
      printf ( "%s%s\n", "erreur : ",
              "un fichier donnee et un seul autorise");
      afficherUsage();
      exit(1);
   }
   if (strcmp(objets[0].nom, objets[1].nom) == 0) {
      printf ( "%s%s%s\n", "erreur : ",
              "le fichier resultat doit etre ",
              "distinct du fichier donnee");
      exit(1);
   }
}

/**********************************************************************/
static void ouvrirObjetEntree(void) {
/**********************************************************************/
   /* ouvrir fichier */
   objets[1].fd = open(objets[1].nom, O_RDONLY);
   if (objets[1].fd == -1) {
      perror(objets[1].nom);
      exit(1);
   }

   /* ouvrir objet en lecture seule ou en lecture ecriture */
   ouvrirObjet(&(objets[1]), copieSuperficielle);
}

/**********************************************************************/
static void realiserImplantation(void) {
/**********************************************************************/
   if (copieSuperficielle) {
      /* implanter l'objet d'entree */
      implanterObjet(&(objets[1]));

      /* creer une copie superficielle de l'objet implante */
      copierObjet(&(objets[1]), &(objets[0]), ET_EXEC);
   }
   else {
      /* dupliquer l'objet a implanter */
      dupliquerObjet(&(objets[1]), &(objets[0]), ET_EXEC);

      /* implanter l'objet de sortie */
      implanterObjet(&(objets[0]));
   }
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
static void fermerObjetEntree(void) {
/**********************************************************************/
   fermerObjet(&objets[1]);
   // close(objets[1].fd);
}

/**********************************************************************/
static void fermerObjetSortie(void) {
/**********************************************************************/
   fermerObjet(&objets[0]);
   // close(objets[0].fd);
}

/**********************************************************************/
static void libererDonneesGlobales(void) {
/**********************************************************************/
   viderIterGlobal();
   libererNomPointEntree();
}

/**********************************************************************/
static void afficherUsage() {
/**********************************************************************/
   printf("%s\n%s\n%s%s\n%s%s\n%s%s\n%s%s\n%s\n%s\n%s\n",
           "usage : implantation -o <resultat> <donnee>",
           "                   [ -e <symbole point d'entree> ]",
           "                   [ -t ",
           "<adresse segment instructions> ]",
           "                   [ -d ",
           "<adresse segment donnees initialisees> ]",
           "                   [ -r ",
           "<adresse segment donnees en lecture seule> ]",
           "                   [ -b ",
           "<adresse segment donnees non initialisees> ]",
           "                   [ -s <adresse pile> ]",
           "                   [ -x ]",
           "                   [ -h ]");
}
