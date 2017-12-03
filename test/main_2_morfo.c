
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

#include "alfa.tab.h"

extern FILE* yyin;
extern FILE* yyout;
extern char* yytext;
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

        switch (tok) {
            case  TOK_ERROR:
                return 0; /* Error en el lexer */
            case  0:
                return 1; /* Fin del fichero */
            default:
                fprintf(stderr, "Token no definido: %i\n", tok);
                return 0;

            case TOK_MAIN:
                fprintf(out, "%s\t%d\t%s\n", "TOK_MAIN", 100, yytext);
                break;

            case TOK_INT:
                fprintf(out, "%s\t%d\t%s\n", "TOK_INT", 101, yytext);
                break;

            case TOK_BOOLEAN:
                fprintf(out, "%s\t%d\t%s\n", "TOK_BOOLEAN", 102, yytext);
                break;

            case TOK_ARRAY:
                fprintf(out, "%s\t%d\t%s\n", "TOK_ARRAY", 103, yytext);
                break;

            case TOK_FUNCTION:
                fprintf(out, "%s\t%d\t%s\n", "TOK_FUNCTION", 104, yytext);
                break;

            case TOK_IF:
                fprintf(out, "%s\t%d\t%s\n", "TOK_IF", 105, yytext);
                break;

            case TOK_ELSE:
                fprintf(out, "%s\t%d\t%s\n", "TOK_ELSE", 106, yytext);
                break;

            case TOK_WHILE:
                fprintf(out, "%s\t%d\t%s\n", "TOK_WHILE", 107, yytext);
                break;

            case TOK_SCANF:
                fprintf(out, "%s\t%d\t%s\n", "TOK_SCANF", 108, yytext);
                break;

            case TOK_PRINTF:
                fprintf(out, "%s\t%d\t%s\n", "TOK_PRINTF", 109, yytext);
                break;

            case TOK_RETURN:
                fprintf(out, "%s\t%d\t%s\n", "TOK_RETURN", 110, yytext);
                break;

            case ';':
                fprintf(out, "%s\t%d\t%s\n", "TOK_PUNTOYCOMA", 200, yytext);
                break;

            case ',':
                fprintf(out, "%s\t%d\t%s\n", "TOK_COMA", 201, yytext);
                break;

            case '(':
                fprintf(out, "%s\t%d\t%s\n", "TOK_PARENTESISIZQUIERDO", 202, yytext);
                break;

            case ')':
                fprintf(out, "%s\t%d\t%s\n", "TOK_PARENTESISDERECHO", 203, yytext);
                break;

            case '[':
                fprintf(out, "%s\t%d\t%s\n", "TOK_CORCHETEIZQUIERDO", 204, yytext);
                break;

            case ']':
                fprintf(out, "%s\t%d\t%s\n", "TOK_CORCHETEDERECHO", 205, yytext);
                break;

            case '{':
                fprintf(out, "%s\t%d\t%s\n", "TOK_LLAVEIZQUIERDA", 206, yytext);
                break;

            case '}':
                fprintf(out, "%s\t%d\t%s\n", "TOK_LLAVEDERECHA", 207, yytext);
                break;

            case '=':
                fprintf(out, "%s\t%d\t%s\n", "TOK_ASIGNACION", 208, yytext);
                break;

            case '+':
                fprintf(out, "%s\t%d\t%s\n", "TOK_MAS", 209, yytext);
                break;

            case '-':
                fprintf(out, "%s\t%d\t%s\n", "TOK_MENOS", 210, yytext);
                break;

            case '/':
                fprintf(out, "%s\t%d\t%s\n", "TOK_DIVISION", 211, yytext);
                break;

            case '*':
                fprintf(out, "%s\t%d\t%s\n", "TOK_ASTERISCO", 212, yytext);
                break;

            case TOK_AND:
                fprintf(out, "%s\t%d\t%s\n", "TOK_AND", 213, yytext);
                break;

            case TOK_OR:
                fprintf(out, "%s\t%d\t%s\n", "TOK_OR", 214, yytext);
                break;

            case '!':
                fprintf(out, "%s\t%d\t%s\n", "TOK_NOT", 215, yytext);
                break;

            case TOK_IGUAL:
                fprintf(out, "%s\t%d\t%s\n", "TOK_IGUAL", 216, yytext);
                break;

            case TOK_DISTINTO:
                fprintf(out, "%s\t%d\t%s\n", "TOK_DISTINTO", 217, yytext);
                break;

            case TOK_MENORIGUAL:
                fprintf(out, "%s\t%d\t%s\n", "TOK_MENORIGUAL", 218, yytext);
                break;

            case TOK_MAYORIGUAL:
                fprintf(out, "%s\t%d\t%s\n", "TOK_MAYORIGUAL", 219, yytext);
                break;

            case '<':
                fprintf(out, "%s\t%d\t%s\n", "TOK_MENOR", 220, yytext);
                break;

            case '>':
                fprintf(out, "%s\t%d\t%s\n", "TOK_MAYOR", 221, yytext);
                break;

            case TOK_IDENTIFICADOR:
                fprintf(out, "%s\t%d\t%s\n", "TOK_IDENTIFICADOR", 300, yytext);
                break;

            case TOK_CONSTANTE_ENTERA:
                fprintf(out, "%s\t%d\t%s\n", "TOK_CONSTANTE_ENTERA", 400, yytext);
                break;

            case TOK_TRUE:
                fprintf(out, "%s\t%d\t%s\n", "TOK_TRUE", 401, yytext);
                break;

            case TOK_FALSE:
                fprintf(out, "%s\t%d\t%s\n", "TOK_FALSE", 402, yytext);
                break;
        }
    }
}

int main(int argc, char const* argv[])
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
