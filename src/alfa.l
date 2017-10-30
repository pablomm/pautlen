
%{
    #include "tokens.h"

    /* Estructura privada para guardar la posicion del lexer */
    static struct _LexerPosition {
        unsigned line, column, offset;
    } lexpos = { 1, 1, 0 };

    void error_handler(const char* msg) {
        lexpos.column -= yyleng;
        lexpos.offset -= yyleng;
        fprintf(stderr, "****Error en [lin %u, col %u]: %s (%s)\n",
            lexpos.line, lexpos.column,
            msg,
            yytext
        );
        exit(1);
    }


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

"main"      { return TOK_MAIN; }
"int"       { return TOK_INT; }
"boolean"   { return TOK_BOOLEAN; }
"array"     { return TOK_ARRAY; }
"function"  { return TOK_FUNCTION; }
"if"        { return TOK_IF; }
"else"      { return TOK_ELSE; }
"while"     { return TOK_WHILE; }
"scanf"     { return TOK_SCANF; }
"printf"    { return TOK_PRINTF; }
"return"    { return TOK_RETURN; }

";"         { return TOK_PUNTOYCOMA; }
","         { return TOK_COMA; }
"("         { return TOK_PARENTESISIZQUIERDO; }
")"         { return TOK_PARENTESISDERECHO; }
"["         { return TOK_CORCHETEIZQUIERDO; }
"]"         { return TOK_CORCHETEDERECHO; }
"{"         { return TOK_LLAVEIZQUIERDA; }
"}"         { return TOK_LLAVEDERECHA; }
"="         { return TOK_ASIGNACION; }
"+"         { return TOK_MAS; }
"-"         { return TOK_MENOS; }
"/"         { return TOK_DIVISION; }
"*"         { return TOK_ASTERISCO; }
"&&"        { return TOK_AND; }
"||"        { return TOK_OR; }
"!"         { return TOK_NOT; }
"=="        { return TOK_IGUAL; }
"!="        { return TOK_DISTINTO; }
"<="        { return TOK_MENORIGUAL; }
">="        { return TOK_MAYORIGUAL; }
"<"         { return TOK_MENOR; }
">"         { return TOK_MAYOR; }

[0-9]+      { return TOK_CONSTANTE_ENTERA; }
"true"      { return TOK_TRUE; }
"false"     { return TOK_FALSE; }

[a-zA-Z][a-zA-Z_0-9]*   { if (yyleng > 100) error_handler("identificador demasiado largo");
                          else return TOK_IDENTIFICADOR; }

\/\/[^\n]*  { /* Comentario */                                          }
\n          { lexpos.column = 1; lexpos.line++; /* Salto de linea */    }
[[:space:]] { /* Ignorado */                                            }
.           { error_handler("simbolo no permitido");                    }

%%

