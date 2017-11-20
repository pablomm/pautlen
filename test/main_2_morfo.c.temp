
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "tokens.h"

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

/* Hace una pasada del analizador morfologico a un fichero */
int lex_file(FILE* in, FILE* out)
{
    yyin = in;
    /* out lo usamos en la macro */
#define TOKEN_CASE(token) case token: fprintf(out, "%s\t%d\t%s\n", #token, token, yytext); break;
    while (1) {
        int tok = yylex();
        switch(tok) {
        case TOK_ERROR:
            return 0; /* Error en el lexer */
        case 0:
            return 1; /* Fin del fichero */
        default:
            fprintf(stderr, "Token no definido: %i\n", tok);
            return 0;
            TOKEN_CASE(TOK_MAIN);
            TOKEN_CASE(TOK_INT);
            TOKEN_CASE(TOK_BOOLEAN);
            TOKEN_CASE(TOK_ARRAY);
            TOKEN_CASE(TOK_FUNCTION);
            TOKEN_CASE(TOK_IF);
            TOKEN_CASE(TOK_ELSE);
            TOKEN_CASE(TOK_WHILE);
            TOKEN_CASE(TOK_SCANF);
            TOKEN_CASE(TOK_PRINTF);
            TOKEN_CASE(TOK_RETURN);
            TOKEN_CASE(TOK_PUNTOYCOMA);
            TOKEN_CASE(TOK_COMA);
            TOKEN_CASE(TOK_PARENTESISIZQUIERDO);
            TOKEN_CASE(TOK_PARENTESISDERECHO);
            TOKEN_CASE(TOK_CORCHETEIZQUIERDO);
            TOKEN_CASE(TOK_CORCHETEDERECHO);
            TOKEN_CASE(TOK_LLAVEIZQUIERDA);
            TOKEN_CASE(TOK_LLAVEDERECHA);
            TOKEN_CASE(TOK_ASIGNACION);
            TOKEN_CASE(TOK_MAS);
            TOKEN_CASE(TOK_MENOS);
            TOKEN_CASE(TOK_DIVISION);
            TOKEN_CASE(TOK_ASTERISCO);
            TOKEN_CASE(TOK_AND);
            TOKEN_CASE(TOK_OR);
            TOKEN_CASE(TOK_NOT);
            TOKEN_CASE(TOK_IGUAL);
            TOKEN_CASE(TOK_DISTINTO);
            TOKEN_CASE(TOK_MENORIGUAL);
            TOKEN_CASE(TOK_MAYORIGUAL);
            TOKEN_CASE(TOK_MENOR);
            TOKEN_CASE(TOK_MAYOR);
            TOKEN_CASE(TOK_IDENTIFICADOR);
            TOKEN_CASE(TOK_CONSTANTE_ENTERA);
            TOKEN_CASE(TOK_TRUE);
            TOKEN_CASE(TOK_FALSE);
        }
    }
}

int main(int argc, char const *argv[])
{
    // Preparamos los ficheros de entrada/salida
    FILE* in  = stdin;
    FILE* out = stdout;
    if (argc >= 2) in  = open_file(argv[1], "r");
    if (argc >= 3) out = open_file(argv[2], "w");

    // El cuerpo del programa: lexear un fichero
    int status = lex_file(in, out);

    // Los cerramos y finalizamos el programa
    if (argc >= 2) fclose(in);
    if (argc >= 3) fclose(out);
    return status ? EXIT_SUCCESS : EXIT_FAILURE;
}
