
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "tokens.h"


#define PRINT_TOK(tokname) fprintf(out, "%s\t%d\t%s\n", tokname, tok, yytext)

extern FILE* yyin;
extern FILE* yyout;
extern char *yytext;
extern int yylex();

/* Abre un fichero, y cierra el program si no lo puede abrir */
static FILE* open_file(const char* path, const char* mode)
{
	FILE* fp = fopen(path, mode);
	if (NULL == fp) {
		perror(path);
		exit(1);
	}
	return fp;
}

int main(int argc, char const *argv[])
{

    FILE * out = stdout;

	if (argc >= 2) yyin  = open_file(argv[1], "r");
	if (argc >= 3) out = open_file(argv[2], "w");

    yyout = out;

    int tok;

	while ((tok = yylex()) > 0) {

        switch(tok) {

            case TOK_MAIN:
                PRINT_TOK("TOK_MAIN");
                break;

            case TOK_INT:
                PRINT_TOK("TOK_INT");
                break;

            case TOK_BOOLEAN:
                PRINT_TOK("TOK_BOOLEAN");
                break;

            case TOK_ARRAY:
                PRINT_TOK("TOK_ARRAY");
                break;

            case TOK_FUNCTION:
                PRINT_TOK("TOK_FUNCTION");
                break;

            case TOK_IF:
                PRINT_TOK("TOK_IF");
                break;

            case TOK_ELSE:
                PRINT_TOK("TOK_ELSE");
                break;

            case TOK_WHILE:
                PRINT_TOK("TOK_WHILE");
                break;

            case TOK_SCANF:
                PRINT_TOK("TOK_SCANF");
                break;

            case TOK_PRINTF:
                PRINT_TOK("TOK_PRINTF");
                break;

            case TOK_RETURN:
                PRINT_TOK("TOK_RETURN");
                break;

            case TOK_PUNTOYCOMA:
                PRINT_TOK("TOK_PUNTOYCOMA");
                break;

            case TOK_COMA:
                PRINT_TOK("TOK_COMA");
                break;

            case TOK_PARENTESISIZQUIERDO:
                PRINT_TOK("TOK_PARENTESISIZQUIERDO");
                break;

            case TOK_PARENTESISDERECHO:
                PRINT_TOK("TOK_PARENTESISDERECHO");
                break;

            case TOK_CORCHETEIZQUIERDO:
                PRINT_TOK("TOK_CORCHETEIZQUIERDO");
                break;

            case TOK_CORCHETEDERECHO:
                PRINT_TOK("TOK_CORCHETEDERECHO");
                break;

            case TOK_LLAVEIZQUIERDA:
                PRINT_TOK("TOK_LLAVEIZQUIERDA");
                break;

            case TOK_LLAVEDERECHA:
                PRINT_TOK("TOK_LLAVEDERECHA");
                break;

            case TOK_ASIGNACION:
                PRINT_TOK("TOK_ASIGNACION");
                break;

            case TOK_MAS:
                PRINT_TOK("TOK_MAS");
                break;

            case TOK_MENOS:
                PRINT_TOK("TOK_MENOS");
                break;

            case TOK_DIVISION:
                PRINT_TOK("TOK_DIVISION");
                break;

            case TOK_ASTERISCO:
                PRINT_TOK("TOK_ASTERISCO");
                break;

            case TOK_AND:
                PRINT_TOK("TOK_AND");
                break;

            case TOK_OR:
                PRINT_TOK("TOK_OR");
                break;

            case TOK_NOT:
                PRINT_TOK("TOK_NOT");
                break;

            case TOK_IGUAL:
                PRINT_TOK("TOK_IGUAL");
                break;

            case TOK_DISTINTO:
                PRINT_TOK("TOK_DISTINTO");
                break;

            case TOK_MENORIGUAL:
                PRINT_TOK("TOK_MENORIGUAL");
                break;

            case TOK_MAYORIGUAL:
                PRINT_TOK("TOK_MAYORIGUAL");
                break;

            case TOK_MENOR:
                PRINT_TOK("TOK_MENOR");
                break;

            case TOK_MAYOR:
                PRINT_TOK("TOK_MAYOR");
                break;

            case TOK_IDENTIFICADOR:
                PRINT_TOK("TOK_IDENTIFICADOR");
                break;

            case TOK_CONSTANTE_ENTERA:
                PRINT_TOK("TOK_CONSTANTE_ENTERA");
                break;

            case TOK_TRUE:
                PRINT_TOK("TOK_TRUE");
                break;

            case TOK_FALSE:
                PRINT_TOK("TOK_FALSE");
                break;

            case TOK_ERROR: /* No deberia entrar en este caso */
                PRINT_TOK("TOK_ERROR");
                break;

            default:
                PRINT_TOK("TOK_UNKNOWN");
                break;
        }        
    }

    /* Salimos con estado 1 en caso de error */
    if(TOK_ERROR == tok) {
        exit(1);
    }

    fclose(yyin);
    fclose(out);

	return 0;
}
