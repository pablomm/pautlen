
%{
    #include "tokens.h"

    /* Estructura privada para guardar la posicion del lexer */
    static struct _LexerPosition {
        unsigned line, column, offset;
    } lexpos = { 1, 1, 0 };

    void error_handler(const char* msg) {
        lexpos.column -= yyleng;
        lexpos.offset -= yyleng;
        fprintf(stderr, "Error en [lin %u, col %u]: %s (%s)\n",
            lexpos.line, lexpos.column,
            msg,
            yytext
        );
        exit(1);
    }

    //#define MACRO_BODY(statements) do { statements } while (0)
    #define RETURN_TOKEN(tok) { fprintf(yyout, "%s\t%i\t%s\n", #tok, tok, yytext); return tok; }

    /* Los contenidos de esta macro se ejecutan justo antes de que se corra el
     * codigo correspondiente a una regla. Lo usamos para contar las columnas.
     * Nota: Si esta macro es lenta, puede ralentizar bastante todo el programa. */
    #define YY_USER_ACTION { lexpos.column += yyleng; lexpos.offset += yyleng; }
%}

/* Suprimir la accion por defecto de imprimir los caracteres que no encajan */
%option nodefault
/* Como solo leemos un fichero, esto nos evita tener que definir esta funcion */
%option noyywrap
/* No genera estas funciones y evita avisos al compilar */
%option nounput noinput
/* No necesitamos que flex calcule la linea porque ya lo haecemos nosotros */
/* %option yylineno */

%%

"main"      RETURN_TOKEN(TOK_MAIN);
"int"       RETURN_TOKEN(TOK_INT);
"boolean"   RETURN_TOKEN(TOK_BOOLEAN);
"array"     RETURN_TOKEN(TOK_ARRAY);
"function"  RETURN_TOKEN(TOK_FUNCTION);
"if"        RETURN_TOKEN(TOK_IF);
"else"      RETURN_TOKEN(TOK_ELSE);
"while"     RETURN_TOKEN(TOK_WHILE);
"scanf"     RETURN_TOKEN(TOK_SCANF);
"printf"    RETURN_TOKEN(TOK_PRINTF);
"return"    RETURN_TOKEN(TOK_RETURN);

";"         RETURN_TOKEN(TOK_PUNTOYCOMA);
","         RETURN_TOKEN(TOK_COMA);
"("         RETURN_TOKEN(TOK_PARENTESISIZQUIERDO);
")"         RETURN_TOKEN(TOK_PARENTESISDERECHO);
"["         RETURN_TOKEN(TOK_CORCHETEIZQUIERDO);
"]"         RETURN_TOKEN(TOK_CORCHETEDERECHO);
"{"         RETURN_TOKEN(TOK_LLAVEIZQUIERDA);
"}"         RETURN_TOKEN(TOK_LLAVEDERECHA);
"="         RETURN_TOKEN(TOK_ASIGNACION);
"+"         RETURN_TOKEN(TOK_MAS);
"-"         RETURN_TOKEN(TOK_MENOS);
"/"         RETURN_TOKEN(TOK_DIVISION);
"*"         RETURN_TOKEN(TOK_ASTERISCO);
"&&"        RETURN_TOKEN(TOK_AND);
"||"        RETURN_TOKEN(TOK_OR);
"!"         RETURN_TOKEN(TOK_NOT);
"=="        RETURN_TOKEN(TOK_IGUAL);
"!="        RETURN_TOKEN(TOK_DISTINTO);
"<="        RETURN_TOKEN(TOK_MENORIGUAL);
">="        RETURN_TOKEN(TOK_MAYORIGUAL);
"<"         RETURN_TOKEN(TOK_MENOR);
">"         RETURN_TOKEN(TOK_MAYOR);

[0-9]+      RETURN_TOKEN(TOK_CONSTANTE_ENTERA);
"true"      RETURN_TOKEN(TOK_TRUE);
"false"     RETURN_TOKEN(TOK_FALSE);

[a-zA-Z][a-zA-Z_0-9]*   { if (yyleng > 100) error_handler("identificador demasiado largo");
                          else RETURN_TOKEN(TOK_IDENTIFICADOR); }

\/\/[^\n]*  { /* Comentario */                                          }
\n          { lexpos.column = 1; lexpos.line++; /* Salto de linea */    }
[[:space:]] { /* Ignorado */                                            }
.           { error_handler("simbolo no permitido");                    }

%%

