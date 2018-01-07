
%{
    #include "comun.h"
    #include "alfa.tab.h"

    #include <string.h>

    /* Imprime un desplazamiento con el formato de la practica 4 */
    /* No se añade a YY_USER_ACTION para que no imprima desplazamientos en blanco */
    #define DESPLAZAMIENTO() fprintf(compiler.f_dbg, ";D:\t%s\n", yytext)

    /* Se ejecuta antes de cada regla para actualizar la posicion */
    #define YY_USER_ACTION { compiler.pos.column += yyleng; }
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

"+="        {  DESPLAZAMIENTO(); return TOK_MASIGUAL; }
"compare"   {  DESPLAZAMIENTO(); return TOK_COMPARE; }
"with"      {  DESPLAZAMIENTO(); return TOK_WITH; }
"less"      {  DESPLAZAMIENTO(); return TOK_LESS; }
"equal"     {  DESPLAZAMIENTO(); return TOK_EQUAL; }
"greater"   {  DESPLAZAMIENTO(); return TOK_GREATER; }
"init"      {  DESPLAZAMIENTO(); return TOK_INIT; }


[0-9]+      {  DESPLAZAMIENTO();
               yylval.atributos.valor_entero=atoi(yytext); 
               return TOK_CONSTANTE_ENTERA;
            }
"true"      {  DESPLAZAMIENTO(); return TOK_TRUE; }
"false"     {  DESPLAZAMIENTO(); return TOK_FALSE; }

[a-zA-Z][a-zA-Z_0-9]*   { DESPLAZAMIENTO(); 
                          if (yyleng > MAX_LONG_ID) {
                              compiler.pos.column -= yyleng;
                              compiler.error = ERR_MORFOLOGICO; 
                              error_handler("identificador demasiado largo", yytext); 
                              return TOK_ERROR; 
                          }
                          else { 
                               strcpy(yylval.atributos.lexema,yytext);
                               return TOK_IDENTIFICADOR;
                          } 
                        }

\/\/[^\n]*  { /* Comentario */ }
\n          { compiler.pos.column = 1; compiler.pos.line++; /* Salto de linea */ }
[[:space:]] { /* Ignorado */ }
.           { compiler.pos.column -= 1; compiler.error = ERR_MORFOLOGICO; error_handler("simbolo no permitido", yytext); return TOK_ERROR; }

%%

