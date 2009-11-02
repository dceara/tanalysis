#include <stdio.h>

/* Type �num�r� pour coder toutes les esp�ces de lex�mes
   du langage �tudi�. On ajoute ENDLEX pour traiter facilement
   la fin de fichier, et ERRLEX en cas de caract�re non
   reconnaissable. 
*/
typedef enum   { NL, ETIQUETTE, NOM_REG, DIRECTIVE, INSTRUCTION, INTEGER,
                CHAINE, VIRGULE, POINT_EXCL, DIESE, MOINS, DEUX_POINTS,
                PLUS, EGAL,
                PAR_OUV, PAR_FERM, ACCOL_OUV, ACCOL_FERM,
                CROCH_OUV, CROCH_FERM, ENDLEX,ERRLEX }  NomLexeme ;

/* Un lex�me est constitu� d'un nom de lex�me, de la 
   sous-cha�ne de caract�res extraite du fichier source,
   et des infos de retour au source (nos de ligne et colonne).
 */


typedef struct {
        char base[6] ;                  /* base du mnemo */
        char cond[3] ;                  /* condition */
        int cod_cond ;                  /* bit S = maj des codes cond. */
        char BHW ;                      /* taille pour LDR et STR */
        int S ;                         /* signe pour LDR et STR */
        int T ;                         /* translation pour LDR et STR */
        char pile[3] ;                  /* param de pile pour STM et LDM */
} champs_inst ;

typedef struct {
        NomLexeme nat ;
       char     *chaine ; 
       int       nol, noc ;
       int       value ; /* utilise pour les INTEGER */
       int       noreg ; /* utilise pour les NOMREG */
		                   /* on garde le num�ro du registre */
       champs_inst s_i ;  /* utilise pour les INSTRUCTION */
} Lexeme ;


void    DemLex (FILE *fpin)
{
}

void    AvLex ()
{
}

Lexeme  LexCour ()
{
    Lexeme l;
    return l;
}
int     FinLex  ()
{
    return 0;
}