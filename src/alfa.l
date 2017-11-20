
%{
    #include "alfa.tab.h"
    #include "comun.h"

    extern LexerPosition lexpos;
    extern ErrorType error_flag;
    extern FILE * out;

    /* Imprime un desplazamiento con el formato de la practica 4 */
    /* No se añade a YY_USER_ACTION para que no imprima desplazamientos en blanco */
    #define DESPLAZAMIENTO() fprintf(out, ";D:\t%s\n", yytext)

    /* Se ejecuta antes de cada regla para actualizar la posicion */
    #define YY_USER_ACTION { lexpos.column += yyleng; lexpos.offset += yyleng; }
%}

/* Suprimir la accion por defecto de imprimir los caracteres que no encajan */
%option nodefault
/* Como solo leemos un fichero, esto nos evita tener que definir esta funcion */
%option noyywrap
/* No genera estas funciones y evita avisos al compilar */
%option nounput noinput
/* No necesitamos que flex calcule la linea porque ya lo hacemos nosotros */
/* %option yylineno */

%%

"main"      {  DESPLAZAMIENTO(); return TOK_MAIN; }
"int"       {  DESPLAZAMIENTO(); return TOK_INT; }
"boolean"   {  DESPLAZAMIENTO(); return TOK_BOOLEAN; }
"array"     {  DESPLAZAMIENTO(); return TOK_ARRAY; }
"function"  {  DESPLAZAMIENTO(); return TOK_FUNCTION; }
"if"        {  DESPLAZAMIENTO(); return TOK_IF; }
"else"      {  DESPLAZAMIENTO(); return TOK_ELSE; }
"while"     {  DESPLAZAMIENTO(); return TOK_WHILE; }
"scanf"     {  DESPLAZAMIENTO(); return TOK_SCANF; }
"printf"    {  DESPLAZAMIENTO(); return TOK_PRINTF; }
"return"    {  DESPLAZAMIENTO(); return TOK_RETURN; }

";"         {  DESPLAZAMIENTO(); return yytext[0]; }
","         {  DESPLAZAMIENTO(); return yytext[0]; }
"("         {  DESPLAZAMIENTO(); return yytext[0]; }
")"         {  DESPLAZAMIENTO(); return yytext[0]; }
"["         {  DESPLAZAMIENTO(); return yytext[0]; }
"]"         {  DESPLAZAMIENTO(); return yytext[0]; }
"{"         {  DESPLAZAMIENTO(); return yytext[0]; }
"}"         {  DESPLAZAMIENTO(); return yytext[0]; }
"="         {  DESPLAZAMIENTO(); return yytext[0]; }
"+"         {  DESPLAZAMIENTO(); return yytext[0]; }
"-"         {  DESPLAZAMIENTO(); return yytext[0]; }
"/"         {  DESPLAZAMIENTO(); return yytext[0]; }
"*"         {  DESPLAZAMIENTO(); return yytext[0]; }
"!"         {  DESPLAZAMIENTO(); return yytext[0]; }
"<"         {  DESPLAZAMIENTO(); return yytext[0]; }
">"         {  DESPLAZAMIENTO(); return yytext[0]; }

"&&"        {  DESPLAZAMIENTO(); return TOK_AND; }
"||"        {  DESPLAZAMIENTO(); return TOK_OR; }
"=="        {  DESPLAZAMIENTO(); return TOK_IGUAL; }
"!="        {  DESPLAZAMIENTO(); return TOK_DISTINTO; }
"<="        {  DESPLAZAMIENTO(); return TOK_MENORIGUAL; }
">="        {  DESPLAZAMIENTO(); return TOK_MAYORIGUAL; }


[0-9]+      {  DESPLAZAMIENTO(); return TOK_CONSTANTE_ENTERA; }
"true"      {  DESPLAZAMIENTO(); return TOK_TRUE; }
"false"     {  DESPLAZAMIENTO(); return TOK_FALSE; }

[a-zA-Z][a-zA-Z_0-9]*   { if (yyleng > 100) { error_flag = ERR_MORFOLOGICO; error_handler("identificador demasiado largo", yytext); return TOK_ERROR; }
                          else {  DESPLAZAMIENTO(); return TOK_IDENTIFICADOR;} }

\/\/[^\n]*  { /* Comentario */                                          }
\n          { lexpos.column = 1; lexpos.line++; /* Salto de linea */    }
[[:space:]] { /* Ignorado */                                            }
.           { error_flag = ERR_MORFOLOGICO; error_handler("simbolo no permitido", yytext); return TOK_ERROR;  }

%%

