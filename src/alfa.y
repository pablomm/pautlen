
%{
    #include "alfa.tab.h"

    #include "comun.h"

    #include <stdio.h>
    #include <stdlib.h>

    /* Fichero de salida */
    extern FILE* out;

    /* Flag global con tipo de error */
      extern ErrorType error_flag;


    #define REGLA(numero,msg) fprintf(out,";R%d:\t%s\n",numero,msg)


    int yyerror()
    {

        if(NO_ERR == error_flag) {
            error_flag = ERR_SINTACTICO;
            error_handler(NULL, NULL);
        }
        return 1;
    }

    /* Prototipo para evitar warning */
    int yylex(void);


%}

%left '+' '-' TOK_OR
%left '*' '/' TOK_AND
%right NEG '!'

%token TOK_MAIN
%token TOK_INT
%token TOK_BOOLEAN
%token TOK_ARRAY


%token TOK_FUNCTION
%token TOK_IF
%token TOK_ELSE
%token TOK_WHILE
%token TOK_SCANF
%token TOK_PRINTF
%token TOK_RETURN


%token TOK_IGUAL
%token TOK_DISTINTO
%token TOK_MENORIGUAL
%token TOK_MAYORIGUAL


%token TOK_IDENTIFICADOR

%token TOK_CONSTANTE_ENTERA
%token TOK_TRUE
%token TOK_FALSE

%token TOK_ERROR

%%


programa                	: TOK_MAIN '{' declaraciones funciones sentencias '}' { REGLA(1,"<programa> ::= main { <declaraciones> <funciones> <sentencias> }"); }
                        	;
declaraciones           	: declaracion { REGLA(2,"<declaraciones> ::= <declaracion>"); }
                        	| declaracion declaraciones { REGLA(3,"<declaraciones> ::= <declaracion> <declaraciones>"); }
                        	;
declaracion              	: clase identificadores ';' { REGLA(4,"<declaracion> ::= <clase> <identificadores> ;"); }
                        	;
clase                   	: clase_escalar { REGLA(5,"<clase> ::= <clase_escalar>"); }
                        	| clase_vector { REGLA(7,"<clase> ::= <clase_vector>"); }
                        	;
clase_escalar           	: tipo { REGLA(9,"<clase_escalar> ::= <tipo>"); }
                        	;
tipo                    	: TOK_INT  { REGLA(10,"<tipo> ::= int"); }
                        	| TOK_BOOLEAN { REGLA(11,"<tipo> ::= boolean"); }
                        	;
clase_vector            	: TOK_ARRAY tipo '[' constante_entera ']' { REGLA(15,"<clase_vector> ::= array <tipo> [ <constante_entera> ]"); }
                        	;
identificadores         	: identificador { REGLA(18,"<identificadores> ::= <identificador>"); }
                        	| identificador ',' identificadores { REGLA(19,"<identificadores> ::= <identificador> , <identificadores>"); }
                        	;
funciones               	: funcion funciones { REGLA(20,"<funciones> ::= <funcion>"); }
                        	| /* empty regla 21 */ { REGLA(21,"<funciones> ::="); }
                        	;
funcion                 	: TOK_FUNCTION tipo identificador '(' parametros_funcion ')' '{' declaraciones_funcion sentencias '}'
                        	{ REGLA(22,"<funcion> ::= function <tipo> <identificador> ( <parametros_funcion> ) { <declaraciones_funcion> <sentencias> }"); }
                        	;
parametros_funcion      	: parametro_funcion resto_parametros_funcion { REGLA(23, "<parametros_funcion> ::= <parametro_funcion> <resto_parametros_funcion>"); }
                        	| /* empty  regla 24*/ { REGLA(24, "<parametros_funcion> ::="); }
                        	;
resto_parametros_funcion	: ';' parametro_funcion resto_parametros_funcion 
                        	{ REGLA(25,"<resto_parametros_funcion> ::= ; <parametro_funcion> <resto_parametros_funcion>"); }
                        	| /* empty regla 26 */ { REGLA(26,"<resto_parametros_funcion> ::="); }
                        	;
parametro_funcion       	: tipo identificador { REGLA(27,"<parametro_funcion> : <tipo> <identificador>"); }
                        	;
declaraciones_funcion   	: declaraciones { REGLA(28,"<declaraciones_funcion> ::= <declaraciones>"); }
                        	| /* empty regla 29 */ { REGLA(29,"<declaraciones_funcion> ::="); }
                        	;
sentencias              	: sentencia { REGLA(30,"<sentencias> ::= <sentencia>"); }
                        	| sentencia sentencias { REGLA(31,"<sentencias> ::= <sentencia> <sentencias>"); }
                        	;
sentencia               	: sentencia_simple ';' { REGLA(32,"<sentencia> ::= <sentencia_simple> ;"); }
                        	| bloque { REGLA(33,"<sentencia> ::= <bloque>"); }
                        	;
sentencia_simple        	: asignacion { REGLA(34,"<sentencia_simple> ::= <asignacion>"); }
                        	| lectura { REGLA(35,"<sentencia_simple> ::= <lectura>"); }
                        	| escritura { REGLA(36,"<sentencia_simple> ::= <escritura>"); }
                        	| retorno_funcion { REGLA(38,"<sentencia_simple> ::= <retorno_funcion>"); }
                        	;
bloque                  	: condicional { REGLA(40,"<bloque> ::= <condicional>"); }
                        	| bucle { REGLA(41,"<bloque> ::= <bucle>"); }
                        	;
asignacion              	: identificador '=' exp { REGLA(43,"<asignacion> ::= <identificador> = <exp>"); }
                        	| elemento_vector '=' exp { REGLA(44,"<asignacion> ::= <elemento_vector> = <exp>"); }
                        	;
elemento_vector         	: identificador '[' exp ']' { REGLA(48,"<elemento_vector> ::= <identificador> [ <exp> ]"); }
                        	;
condicional             	: TOK_IF '(' exp ')' '{' sentencias '}' { REGLA(50,"<condicional> ::= if ( <exp> ) { <sentencias> }"); }
                        	| TOK_IF '(' exp ')' '{' sentencias '}' TOK_ELSE '{' sentencias '}' { REGLA(51,"<condicional> ::= if ( <exp> ) { <sentencias> } else { <sentencias> }"); }
                        	;
bucle                   	: TOK_WHILE'(' exp ')' '{' sentencias '}' { REGLA(52,"<bucle> ::= while ( <exp> ) { <sentencias> }"); }
                        	;
lectura                 	: TOK_SCANF identificador { REGLA(54,"<lectura> ::= scanf <identificador>"); }
                        	;
escritura               	: TOK_PRINTF exp { REGLA(56,"<escritura> ::= printf <exp>"); }
                        	;
retorno_funcion         	: TOK_RETURN exp { REGLA(61, "<retorno_funcion> ::= return <exp>"); }
                        	;
exp                     	: exp '+' exp                               { REGLA(72,"<exp> ::= <exp> + <exp>"); }
                        	| exp '-' exp                               { REGLA(73,"<exp> ::= <exp> - <exp>"); }
                        	| exp '/' exp                               { REGLA(74,"<exp> ::= <exp> / <exp>"); }
                        	| exp '*' exp                               { REGLA(75,"<exp> ::= <exp> * <exp>"); }
                        	| '-'  %prec NEG exp                        { REGLA(76,"<exp> ::= - <exp>"); }
                        	| exp TOK_AND exp                           { REGLA(77,"<exp> ::= <exp> && <exp>"); }
                        	| exp TOK_OR exp                            { REGLA(78,"<exp> ::= <exp> || <exp>"); }
                        	| '!' exp                                   { REGLA(79,"<exp> ::= ! <exp>"); }
                        	| identificador                             { REGLA(80,"<exp> ::= <identificador>"); }
                        	| constante                                 { REGLA(81,"<exp> ::= <constante>"); }
                        	| '(' exp ')'                               { REGLA(82,"<exp> ::= ( <exp> )"); }
                        	| '(' comparacion ')'                       { REGLA(83,"<exp> ::= ( <comparacion> )"); }
                        	| elemento_vector                           { REGLA(85,"<exp> ::= <elemento_vector>"); }
                        	| identificador '(' lista_expresiones ')'   { REGLA(88,"<exp> ::= <identificador> ( <lista_expresiones> )"); }
                        	;
lista_expresiones       	: exp resto_lista_expresiones { REGLA(89,"<lista_expresiones> ::= <exp> <resto_lista_expresiones>"); }
                        	| /* empty regla 90 */ { REGLA(90,"<lista_expresiones> ::="); }
                        	;
resto_lista_expresiones 	: ',' exp resto_lista_expresiones { REGLA(91,"<resto_lista_expresiones> ::= , <exp> <resto_lista_expresiones>"); }
                        	| /* empty regla 92 */ { REGLA(92,"<resto_lista_expresiones> ::="); }
                        	;
comparacion             	: exp TOK_IGUAL exp        { REGLA(93, "<comparacion> ::= <exp> == <exp>"); }
                        	| exp TOK_DISTINTO exp     { REGLA(94, "<comparacion> ::= <exp> != <exp>"); }
                        	| exp TOK_MENORIGUAL exp   { REGLA(95, "<comparacion> ::= <exp> <= <exp>"); }
                        	| exp TOK_MAYORIGUAL exp   { REGLA(96, "<comparacion> ::= <exp> >= <exp>"); }
                        	| exp '<' exp              { REGLA(97, "<comparacion> ::= <exp> < <exp>"); }
                        	| exp '>' exp              { REGLA(98, "<comparacion> ::= <exp> > <exp>"); }
                        	;
constante               	: constante_logica { REGLA(99, "<constante> ::= <constante_logica>"); }
                        	| constante_entera { REGLA(100, "<constante> ::= <constante_entera>"); }
                        	;
constante_logica        	: TOK_TRUE { REGLA(101, "<constante_logica> ::= true"); }
                        	| TOK_FALSE { REGLA(102, "<constante_logica> ::= false"); }
                        	;
constante_entera        	: TOK_CONSTANTE_ENTERA { REGLA(104, "<constante_entera> ::= TOK_CONSTANTE_ENTERA"); }
                        	;
identificador           	: TOK_IDENTIFICADOR { REGLA(108, "<identificador> ::= TOK_IDENTIFICADOR"); }
                        	;


%%

