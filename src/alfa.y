
%{
    #include "comun.h"
    #include "alfa.tab.h"
    #include "tablaSimbolos.h"
    #include "generacion.h"
    #include "tablaHash.h"

    #include <stdio.h>
    #include <stdlib.h>

    /* Fichero con codigo asm */
    #define pfasm (compiler.f_asm)

    #define ASSERT_SEMANTICO(cond, err_msg, extra_info) \
        MACRO_BODY( \
            if (!(cond)) { \
                compiler.error = ERR_SEMANTICO; \
                error_handler((err_msg), (extra_info)); \
                YYABORT; \
            } \
        ) /* end ASSERT_SEMANTICO */

    #define REGLA(numero, msg) \
        MACRO_BODY( \
            fprintf(compiler.f_dbg, ";R%d:\t%s\n", (numero), (msg)); \
        ) /* end REGLA */

    int yyerror(const char* str)
    {
        UNUSED(str);
        compiler.error = ERR_SINTACTICO;
        error_handler(NULL, NULL);
        return 1;
    }

    /* Prototipo para evitar warning */
    int yylex(void);

    /* Variables globales para simular herencia */
    static int clase_actual, ambito_actual, tipo_actual;

    /* Variable para la generacion de etiquetas */
    static int etiqueta = 1;

%}


%union {
    tipo_atributos atributos;
}


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


%token <atributos> TOK_CONSTANTE_ENTERA
%token <atributos> TOK_CONSTANTE_REAL
%token <atributos> TOK_IDENTIFICADOR

%token TOK_TRUE
%token TOK_FALSE

%token TOK_ERROR

%type <atributos> condicional
%type <atributos> comparacion
%type <atributos> elemento_vector
%type <atributos> exp
%type <atributos> constante
%type <atributos> constante_entera
%type <atributos> constante_logica
%type <atributos> identificador_nuevo identificador_uso

/* Manejo de las sentencias IF y ELSE */
%type <atributos> if_exp if_exp_sentencias
%type <atributos> while_exp while

%%


programa                    : TOK_MAIN '{' _inicio declaraciones _escritura1 funciones _escritura2 sentencias _final '}' 
                              { // Axioma de nuestra gramatica
                                REGLA(1,"<programa> ::= main { <declaraciones> <funciones> <sentencias> }");
                              }
                            ;
_inicio                     : { // Inicializacion inicial
                                iniciar_scope();
                                escribir_cabecera_compatibilidad(pfasm);
                                escribir_subseccion_data(pfasm);
                                escribir_cabecera_bss(pfasm);
                              }
                            ;
_escritura1                 : { // Declaramos en el segmento bss todas las variables globales declaradas
                                INFO_SIMBOLO* simbolos = simbolos_globales();

                                for (; NULL != simbolos; simbolos = simbolos->siguiente) {

                                  if (VARIABLE == simbolos->categoria)
                                    declarar_variable(pfasm, simbolos->lexema, simbolos->tipo, (VECTOR == simbolos->clase) ? simbolos->adicional1 : 1);
                                  
                                }
                                escribir_segmento_codigo(pfasm);
                              }
                            ;
_escritura2                 : { escribir_inicio_main(pfasm);}
                            ;
_final                      : { escribir_fin(pfasm); liberar_scope(); }
                            ;
declaraciones               : declaracion { REGLA(2,"<declaraciones> ::= <declaracion>"); }
                            | declaracion declaraciones { REGLA(3,"<declaraciones> ::= <declaracion> <declaraciones>"); }
                            ;
declaracion                 : clase identificadores ';' { REGLA(4,"<declaracion> ::= <clase> <identificadores> ;"); }
                            ;
clase                       : clase_escalar { REGLA(5,"<clase> ::= <clase_escalar>"); clase_actual = ESCALAR; }
                            | clase_vector { REGLA(7,"<clase> ::= <clase_vector>"); clase_actual = VECTOR; }
                            ;
clase_escalar               : tipo { REGLA(9,"<clase_escalar> ::= <tipo>"); }
                            ;
tipo                        : TOK_INT  { REGLA(10,"<tipo> ::= int"); tipo_actual = ENTERO; }
                            | TOK_BOOLEAN { REGLA(11,"<tipo> ::= boolean"); tipo_actual = BOOLEANO; }
                            ;
clase_vector                : TOK_ARRAY tipo '[' constante_entera ']' { REGLA(15,"<clase_vector> ::= array <tipo> [ <constante_entera> ]"); }
                            ;
identificadores             : identificador_nuevo { REGLA(18,"<identificadores> ::= <identificador>"); }
                            | identificador_nuevo ',' identificadores { REGLA(19,"<identificadores> ::= <identificador> , <identificadores>"); }
                            ;
funciones                   : funcion funciones { REGLA(20,"<funciones> ::= <funcion> <funciones>"); }
                            | /* empty regla 21 */ { REGLA(21,"<funciones> ::="); }
                            ;
funcion                     : TOK_FUNCTION tipo identificador_nuevo '(' parametros_funcion ')' '{' declaraciones_funcion sentencias '}'
                            { REGLA(22,"<funcion> ::= function <tipo> <identificador> ( <parametros_funcion> ) { <declaraciones_funcion> <sentencias> }"); }
                            ;
parametros_funcion          : parametro_funcion resto_parametros_funcion { REGLA(23, "<parametros_funcion> ::= <parametro_funcion> <resto_parametros_funcion>"); }
                            | /* empty  regla 24*/ { REGLA(24, "<parametros_funcion> ::="); }
                            ;
resto_parametros_funcion    : ';' parametro_funcion resto_parametros_funcion 
                            { REGLA(25,"<resto_parametros_funcion> ::= ; <parametro_funcion> <resto_parametros_funcion>"); }
                            | /* empty regla 26 */ { REGLA(26,"<resto_parametros_funcion> ::="); }
                            ;
parametro_funcion           : tipo identificador_nuevo { REGLA(27,"<parametro_funcion> ::= <tipo> <identificador>"); }
                            ;
declaraciones_funcion       : declaraciones { REGLA(28,"<declaraciones_funcion> ::= <declaraciones>"); }
                            | /* empty regla 29 */ { REGLA(29,"<declaraciones_funcion> ::="); }
                            ;
sentencias                  : sentencia { REGLA(30,"<sentencias> ::= <sentencia>"); }
                            | sentencia sentencias { REGLA(31,"<sentencias> ::= <sentencia> <sentencias>"); }
                            ;
sentencia                   : sentencia_simple ';' { REGLA(32,"<sentencia> ::= <sentencia_simple> ;"); }
                            | bloque { REGLA(33,"<sentencia> ::= <bloque>"); }
                            ;
sentencia_simple            : asignacion { REGLA(34,"<sentencia_simple> ::= <asignacion>"); }
                            | lectura { REGLA(35,"<sentencia_simple> ::= <lectura>"); }
                            | escritura { REGLA(36,"<sentencia_simple> ::= <escritura>"); }
                            | retorno_funcion { REGLA(38,"<sentencia_simple> ::= <retorno_funcion>"); }
                            ;
bloque                      : condicional { REGLA(40,"<bloque> ::= <condicional>"); }
                            | bucle { REGLA(41,"<bloque> ::= <bucle>"); }
                            ;
asignacion                  : identificador_uso '=' exp
                              {
                                REGLA(43,"<asignacion> ::= <identificador> = <exp>");
                                INFO_SIMBOLO* info = uso_local($1.lexema);
                                ASSERT_SEMANTICO(NULL != info, "Asignacion incompatible", NULL);
                                ASSERT_SEMANTICO(FUNCION != info->categoria, "Asignacion incompatible", NULL);
                                ASSERT_SEMANTICO(VECTOR != info->clase, "Asignacion incompatible", NULL);
                                ASSERT_SEMANTICO($3.tipo == info->tipo, "Asignacion incompatible", NULL);
                                asignar(pfasm, $1.lexema, $3.es_direccion);
                              }
                            | elemento_vector '=' exp { REGLA(44,"<asignacion> ::= <elemento_vector> = <exp>"); }
                            ;
elemento_vector             : identificador_uso '[' exp ']' { REGLA(48,"<elemento_vector> ::= <identificador> [ <exp> ]"); }
                            ;
condicional                 : if_exp_sentencias
                              {
                                REGLA(50,"<condicional> ::= if ( <exp> ) { <sentencias> }");
                                generar_endif(pfasm, $1.etiqueta);
                              }
                            | if_exp_sentencias TOK_ELSE '{' sentencias '}'
                              {
                                REGLA(51,"<condicional> ::= if ( <exp> ) { <sentencias> } else { <sentencias> }");
                                generar_endif(pfasm, $1.etiqueta);
                              }
                            ;
if_exp_sentencias           : if_exp sentencias '}'
                              {
                                $$.etiqueta = $1.etiqueta;
                                generar_else(pfasm, $$.etiqueta);
                              }
                            ;
if_exp                      : TOK_IF '(' exp ')' '{'
                              {
                                ASSERT_SEMANTICO(BOOLEANO == $3.tipo, "Condicional con condicion de tipo int", NULL);
                                $$.etiqueta = etiqueta++;
                                generar_if_then(pfasm, $3.es_direccion, $$.etiqueta);
                              }
                            ;
bucle                       : while_exp sentencias '}'
                              {
                                REGLA(52,"<bucle> ::= while ( <exp> ) { <sentencias> }");
                                generar_endwhile(pfasm, $1.etiqueta);
                              }
                            ;
while_exp                   : while '(' exp ')' '{' 
                              {
                                ASSERT_SEMANTICO(BOOLEANO == $3.tipo, "Bucle con condicion de tipo int", NULL);
                                generar_do(pfasm, $3.es_direccion, $$.etiqueta);
                              }
                            ;
while                       : TOK_WHILE
                              {
                                $$.etiqueta = etiqueta++;
                                generar_while(pfasm, $$.etiqueta);
                              }
                            ;
lectura                     : TOK_SCANF identificador_uso
                              {
                                REGLA(54,"<lectura> ::= scanf <identificador>");
                                /*  Buscamos el identificador en las tablas */
                                INFO_SIMBOLO* info = uso_local($2.lexema);

                                ASSERT_SEMANTICO(NULL != info, "Acceso a variable no declarada", $2.lexema);
                                ASSERT_SEMANTICO(FUNCION != info->categoria, "Solo podemos leer enteros o booleanos", NULL);
                                ASSERT_SEMANTICO(ESCALAR == info->clase, "Solo podemos leer escalares", NULL);

                                /* Llamamos a scanf, restauramos la pila en funcion del tipo */
                                leer(pfasm, info->lexema, info->tipo);
                              }
                            ;
escritura                   : TOK_PRINTF exp
                              {
                                REGLA(56,"<escritura> ::= printf <exp>");
                                escribir(pfasm, $2.es_direccion, $2.tipo);
                              }
                            ;
retorno_funcion             : TOK_RETURN exp
                              {
                                REGLA(61, "<retorno_funcion> ::= return <exp>");
                                //ASSERT_SEMANTICO(ambito_actual, "Sentencia de retorno fuera del cuerpo de una funcion", NULL);
                              }
                            ;
exp                         : exp '+' exp
                              {
                                REGLA(72,"<exp> ::= <exp> + <exp>"); 

                                /* Solo podemos sumar enteros  */
                                ASSERT_SEMANTICO($1.tipo == $3.tipo && $1.tipo == ENTERO, "Operacion aritmetica con operandos boolean", NULL);
                                sumar(pfasm, $1.es_direccion, $3.es_direccion);

                                /* Propaga correctamente los atributos */
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              }
                            | exp '-' exp
                              {
                                REGLA(73,"<exp> ::= <exp> - <exp>"); 

                                /* Solo podemos restar enteros  */
                                ASSERT_SEMANTICO($1.tipo == $3.tipo && $1.tipo == ENTERO, "Operacion aritmetica con operandos boolean", NULL);
                                restar(pfasm, $1.es_direccion, $3.es_direccion);
                                /* Propaga correctamente los atributos */
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              }
                            | exp '/' exp
                              {
                                REGLA(74,"<exp> ::= <exp> / <exp>"); 
                                /* Solo podemos dividir enteros  */
                                ASSERT_SEMANTICO($1.tipo == $3.tipo && $1.tipo == ENTERO, "Operacion aritmetica con operandos boolean", NULL);
                                dividir(pfasm, $1.es_direccion, $3.es_direccion);
                                /* Propaga correctamente los atributos */
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              }
                            | exp '*' exp
                              {
                                REGLA(75,"<exp> ::= <exp> * <exp>"); 
                                /* Solo podemos multiplicar enteros  */
                                ASSERT_SEMANTICO($1.tipo == $3.tipo && $1.tipo == ENTERO, "Operacion aritmetica con operandos boolean", NULL);
                                multiplicar(pfasm, $1.es_direccion, $3.es_direccion);
                                /* Propaga correctamente los atributos */
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              }
                            | '-'  %prec NEG exp
                              {
                                REGLA(76,"<exp> ::= - <exp>"); 
                                ASSERT_SEMANTICO($2.tipo == ENTERO, "Operacion aritmetica con operandos boolean", NULL);
                                cambiar_signo(pfasm, $2.es_direccion);
                                /* Propaga correctamente los atributos */
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              }
                            | exp TOK_AND exp
                              {
                                REGLA(77,"<exp> ::= <exp> && <exp>"); 
                                /* Solo podemos ahcer ands de booleanos  */
                                ASSERT_SEMANTICO($1.tipo == $3.tipo && $1.tipo == BOOLEANO, "Operacion logica con operandos int", NULL);
                                y(pfasm, $1.es_direccion, $3.es_direccion);
                                /* Propaga correctamente los atributos */
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | exp TOK_OR exp
                              {
                                REGLA(78,"<exp> ::= <exp> || <exp>"); 
                                /* Solo podemos ahcer ands de booleanos  */
                                ASSERT_SEMANTICO($1.tipo == $3.tipo && $1.tipo == BOOLEANO, "Operacion logica con operandos int", NULL);
                                o(pfasm, $1.es_direccion, $3.es_direccion);
                                /* Propaga correctamente los atributos */
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | '!' exp
                              {
                                REGLA(79,"<exp> ::= ! <exp>"); 
                                ASSERT_SEMANTICO($2.tipo == BOOLEANO, "Operacion logica con operandos int", NULL);
                                no(pfasm, $2.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | identificador_uso
                              {
                                //REGLA(108, "<identificador> ::= TOK_IDENTIFICADOR"); 
                                REGLA(80,"<exp> ::= <identificador>");
                                INFO_SIMBOLO* info = uso_local($1.lexema);
                                ASSERT_SEMANTICO(NULL != info, "Acceso a variable no declarada", $1.lexema);
                                ASSERT_SEMANTICO(FUNCION != info->categoria, "El identificador es una funcion", NULL);
                                ASSERT_SEMANTICO(VECTOR != info->clase, "El identificador es un vector", NULL);
                                $$.tipo = info->tipo;
                                $$.es_direccion = 1;
                                escribir_operando(pfasm, $1.lexema, 1);
                              }

                            | constante
                              {
                                REGLA(81,"<exp> ::= <constante>"); 
                                $$.tipo = $1.tipo; 
                                $$.es_direccion = $1.es_direccion; 
                              }
                            
                            | '(' exp ')'
                              {
                                REGLA(82,"<exp> ::= ( <exp> )"); 
                                $$.tipo = $2.tipo;
                                $$.es_direccion = $2.es_direccion;
                              }
                            | '(' comparacion ')'
                              {
                                REGLA(83,"<exp> ::= ( <comparacion> )"); 
                                $$.tipo = $2.tipo;
                                $$.es_direccion = $2.es_direccion;
                              }
                            | elemento_vector
                              {
                                REGLA(85,"<exp> ::= <elemento_vector>");
                                $$ = $1; // Copiamos todo
                              }
                            | identificador_uso '(' lista_expresiones ')'
                              {
                                REGLA(88,"<exp> ::= <identificador> ( <lista_expresiones> )");
                                // Aqui un call
                              }
                            ;
lista_expresiones           : exp resto_lista_expresiones { REGLA(89,"<lista_expresiones> ::= <exp> <resto_lista_expresiones>"); }
                            | /* empty regla 90 */ { REGLA(90,"<lista_expresiones> ::="); }
                            ;
resto_lista_expresiones     : ',' exp resto_lista_expresiones { REGLA(91,"<resto_lista_expresiones> ::= , <exp> <resto_lista_expresiones>"); }
                            | /* empty regla 92 */ { REGLA(92,"<resto_lista_expresiones> ::="); }
                            ;
comparacion                 : exp TOK_IGUAL exp /* == */
                              {
                                REGLA(93, "<comparacion> ::= <exp> == <exp>"); 
                                ASSERT_SEMANTICO ($1.tipo == ENTERO && $3.tipo == ENTERO, "Comparacion con operandos boolean", NULL);
                                igual(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | exp TOK_DISTINTO exp /* != */
                              {
                                REGLA(94, "<comparacion> ::= <exp> != <exp>"); 
                                ASSERT_SEMANTICO($1.tipo == ENTERO && $3.tipo == ENTERO, "Comparacion con operandos boolean", NULL);
                                distinto(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                            }
                            | exp TOK_MENORIGUAL exp /* <= */
                              {
                                REGLA(95, "<comparacion> ::= <exp> <= <exp>");
                                ASSERT_SEMANTICO($1.tipo == ENTERO && $3.tipo == ENTERO, "Comparacion con operandos boolean", NULL);
                                menorigual(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | exp TOK_MAYORIGUAL exp /* >= */
                              {
                                REGLA(96, "<comparacion> ::= <exp> >= <exp>");
                                ASSERT_SEMANTICO($1.tipo == ENTERO && $3.tipo == ENTERO, "Comparacion con operandos boolean", NULL);
                                mayorigual(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | exp '<' exp
                              {
                                REGLA(97, "<comparacion> ::= <exp> < <exp>");
                                ASSERT_SEMANTICO($1.tipo == ENTERO && $3.tipo == ENTERO, "Comparacion con operandos boolean", NULL);
                                menor(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            | exp '>' exp
                              {
                                REGLA(98, "<comparacion> ::= <exp> > <exp>");
                                ASSERT_SEMANTICO($1.tipo == ENTERO && $3.tipo == ENTERO, "Comparacion con operandos boolean", NULL);
                                mayor(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              }
                            ;
constante                   : constante_logica { REGLA(99, "<constante> ::= <constante_logica>");
                                                 $$.tipo = $1.tipo;
                                                 $$.es_direccion = $1.es_direccion;
                            }
                            | constante_entera { REGLA(100, "<constante> ::= <constante_entera>");
                                                 $$.tipo = $1.tipo;
                                                 $$.es_direccion = $1.es_direccion;
                                               }
                            ;
constante_logica            : TOK_TRUE { REGLA(101, "<constante_logica> ::= true"); 
                                                     $$.tipo = BOOLEANO;
                                                     $$.es_direccion = 0;
                                                     $$.valor_entero = 1;
                                                     apilar_constante(pfasm, 1);

                            }
                            | TOK_FALSE { REGLA(102, "<constante_logica> ::= false");
                                                     $$.tipo = BOOLEANO;
                                                     $$.es_direccion = 0;
                                                     $$.valor_entero = 0;
                                                     apilar_constante(pfasm, 0);
                             }
                            ;
constante_entera            : TOK_CONSTANTE_ENTERA { REGLA(104, "<constante_entera> ::= TOK_CONSTANTE_ENTERA");
                                                     $$.tipo = ENTERO;
                                                     $$.es_direccion = 0;
                                                     $$.valor_entero = $1.valor_entero;
                                                     apilar_constante(pfasm, $1.valor_entero);
                                                   }
                            ;
identificador_uso           : TOK_IDENTIFICADOR
                              {
                                REGLA(108, "<identificador> ::= TOK_IDENTIFICADOR"); 
                                $$ = $1;
                              }
                            ;
identificador_nuevo         : TOK_IDENTIFICADOR
                              {
                                REGLA(108, "<identificador> ::= TOK_IDENTIFICADOR"); 
                                /* Añadimos al ambito actual */
                                INFO_SIMBOLO* info = uso_local($1.lexema);
                                ASSERT_SEMANTICO(NULL == info, "Declaracion duplicada", NULL);
                                if (0 == ambito_actual) {
                                  declarar_global($1.lexema, tipo_actual, clase_actual, 0);
                                } else {
                                  declarar_local($1.lexema, 0, tipo_actual, clase_actual, 0, 0);
                                }
                              }
                            ;

%%
