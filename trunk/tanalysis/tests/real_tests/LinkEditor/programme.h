/***********************************************************************
programme.h

P.Habraken - 22/02/2009
***********************************************************************/

#ifndef PROGRAMME_H
#define PROGRAMME_H

#include "code.h"
#include "vide.h"

/**********************************************************************/
struct StructSectionProgrammeEDL {
   unsigned int decalageContenu;
};
typedef struct StructSectionProgrammeEDL SectionProgrammeEDL;
/**********************************************************************/
/***********************************************************************
SectionProgrammeEDL :
- decalageContenu : decalage (en nombre d'octets) applique a cette
                    section de code dans la section de code
                    correspondante (de meme nom) de l'objet resultat
***********************************************************************/

#include "programmeI.h"

#endif /* PROGRAMME_H */
