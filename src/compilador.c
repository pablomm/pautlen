


#include <stdio.h>

extern FILE* yyin;
extern FILE* yyout;

extern FILE* out;
extern FILE* pfasm;

int yyparse();


int main(int argc, char* argv[])
{

    FILE* in = stdin;
    pfasm = stdout;

    /* Fichero de entrada con codigo alfa */
    if (argc > 1) in = fopen(argv[1],"r");

    /* Fichero de salida con codigo ensamblador */
    if (argc > 2) pfasm = fopen(argv[2],"w");

    /* Fichero de salida con debug */
    if (argc > 3) out = fopen(argv[2],"w");

    yyin = in;
    yyout = out;

    yyparse();

    fclose(in);
    fclose(pfasm);
    if(NULL != out) fclose(out);

    return 0;
}
