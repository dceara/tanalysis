/***********************************************************************
_wrapper.c

P.Habraken - 22/02/2009

fichier genere automatiquement
***********************************************************************/
#include <stdio.h>

#include "options.h"

#include "objet.h"
#include "section.h"
#include "chaines.h"
#include "symboles.h"
#include "translations.h"
#include "programme.h"
#include "code.h"
#include "vide.h"
#include "autre.h"
#include "iterateur.h"
/* DEBUT SUPPRIMER FUSION */
#include "segments.h"
/* FIN SUPPRIMER FUSION */

#define VRAI 1
#define FAUX 0

void journaliser(char *sp) {
   static FILE *journal;
   static unsigned int premiereFois = VRAI;

   if (premiereFois) {
      premiereFois = FAUX;
      journal = fopen("libedl.log", "w");
      fprintf(journal, "sous-programmes non realises ");
      fprintf(journal, "(invocation de la version libedl) :\n");
   }
   fprintf(journal, "- %s()\n", sp);
}
