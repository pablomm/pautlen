
%{
    #include "comun.h"
    #include "alfa.tab.h"
    #include "tablaSimbolos.h"
    #include "generacion.h"
    #include "tablaHash.h"

    #include <stdio.h>
    #include <stdlib.h>

    /* Fichero de salida de debug */
    extern FILE* out;

    /* Fichero con codigo asm */
    extern FILE* pfasm;

    /* Flag global con tipo de error */
    extern ErrorType error_flag;


    #define REGLA(numero,msg) if(NULL != out) fprintf(out,";R%d:\t%s\n",numero,msg)


    int yyerror(const char* str)
    {
        UNUSED(str);
        if(NO_ERR == error_flag) {
            error_flag = ERR_SINTACTICO;
            error_handler(NULL, NULL);
        }
        return 1;
    }

    /* Prototipo para evitar warning */
    int yylex(void);

    /* Variables globales para simular herencia */
    int tipo_actual = 0;
    int clase_actual = 0;
    int ambito_actual = 0;

    /* variable para la generacion de etiquetas */
    int etiqueta=1;

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
%type <atributos> identificador


%%


programa                    : TOK_MAIN '{' inicio declaraciones escritura1 funciones escritura2 sentencias final '}' 
                                        { REGLA(1,"<programa> ::= main { <declaraciones> <funciones> <sentencias> }"); }
                            ;
inicio                      : { iniciar_scope();
                                escribir_cabecera_compatibilidad(pfasm);
                                escribir_subseccion_data(pfasm);
                                escribir_cabecera_bss(pfasm);
                              }
                            ;
escritura1                  : { /* Declaramos en bss todas las variables globales declaradas */

                                 INFO_SIMBOLO * simbolos = simbolos_globales();

                                 while (NULL != simbolos) {

                                      if(simbolos->categoria == VARIABLE)
                                        declarar_variable(pfasm, simbolos->lexema, simbolos->tipo, (VECTOR == simbolos->clase) ? simbolos->adicional1 : 1);

                                      simbolos = simbolos->siguiente;

                                 };


                                escribir_segmento_codigo(pfasm); }
                            ;
escritura2                  : { escribir_inicio_main(pfasm);}
                            ;
final                       : { escribir_fin(pfasm);
                                liberar_scope(); }
                            ;
declaraciones               : declaracion { REGLA(2,"<declaraciones> ::= <declaracion>"); }
                            | declaracion declaraciones { REGLA(3,"<declaraciones> ::= <declaracion> <declaraciones>"); }
                            ;
declaracion                 : clase identificadores ';' { REGLA(4,"<declaracion> ::= <clase> <identificadores> ;"); }
                            ;
clase                       : clase_escalar { REGLA(5,"<clase> ::= <clase_escalar>");
                                              clase_actual = ESCALAR;
                                            }
                            | clase_vector { REGLA(7,"<clase> ::= <clase_vector>"); 
                                             clase_actual = VECTOR;
                                            }
                            ;
clase_escalar               : tipo { REGLA(9,"<clase_escalar> ::= <tipo>"); }
                            ;
tipo                        : TOK_INT  { REGLA(10,"<tipo> ::= int"); 
                                         tipo_actual = ENTERO;
                                        }
                            | TOK_BOOLEAN { REGLA(11,"<tipo> ::= boolean"); 
                                            tipo_actual = BOOLEANO;
                                           }
                            ;
clase_vector                : TOK_ARRAY tipo '[' constante_entera ']' { REGLA(15,"<clase_vector> ::= array <tipo> [ <constante_entera> ]"); }
                            ;
identificadores             : identificador { REGLA(18,"<identificadores> ::= <identificador>"); }
                            | identificador ',' identificadores { REGLA(19,"<identificadores> ::= <identificador> , <identificadores>"); }
                            ;
funciones                   : funcion funciones { REGLA(20,"<funciones> ::= <funcion> <funciones>"); }
                            | /* empty regla 21 */ { REGLA(21,"<funciones> ::="); }
                            ;
funcion                     : TOK_FUNCTION tipo identificador '(' parametros_funcion ')' '{' declaraciones_funcion sentencias '}'
                            { REGLA(22,"<funcion> ::= function <tipo> <identificador> ( <parametros_funcion> ) { <declaraciones_funcion> <sentencias> }"); }
                            ;
parametros_funcion          : parametro_funcion resto_parametros_funcion { REGLA(23, "<parametros_funcion> ::= <parametro_funcion> <resto_parametros_funcion>"); }
                            | /* empty  regla 24*/ { REGLA(24, "<parametros_funcion> ::="); }
                            ;
resto_parametros_funcion    : ';' parametro_funcion resto_parametros_funcion 
                            { REGLA(25,"<resto_parametros_funcion> ::= ; <parametro_funcion> <resto_parametros_funcion>"); }
                            | /* empty regla 26 */ { REGLA(26,"<resto_parametros_funcion> ::="); }
                            ;
parametro_funcion           : tipo identificador { REGLA(27,"<parametro_funcion> ::= <tipo> <identificador>"); }
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
asignacion                  : TOK_IDENTIFICADOR '=' exp { REGLA(43,"<asignacion> ::= <identificador> = <exp>");
                                                          INFO_SIMBOLO* info = uso_local($1.lexema);
                                                          if (NULL == info) { 
															printf("Error identificador no asignado\n");
															YYABORT;
                                                          }
                                                          if (FUNCION == info->categoria){ 
															printf("Error no puedes asignar valor a una funcion\n");
															YYABORT;
                                                          }
                                                          if (VECTOR == info->clase){ 
															printf("No puedes asignar valor a un vector directamente\n");
															YYABORT;
                                                          }
                                                          if ($3.tipo != info->tipo){ 
															printf("TIpos incompatibles\n");
															YYABORT;
                                                          }

                                                          asignar(pfasm, $1.lexema, $3.es_direccion);

                                                        }
                            | elemento_vector '=' exp { REGLA(44,"<asignacion> ::= <elemento_vector> = <exp>"); }
                            ;
elemento_vector             : identificador '[' exp ']' { REGLA(48,"<elemento_vector> ::= <identificador> [ <exp> ]"); }
                            ;
condicional                 : TOK_IF '(' exp ')' '{' sentencias '}' { REGLA(50,"<condicional> ::= if ( <exp> ) { <sentencias> }"); }
                            | TOK_IF '(' exp ')' '{' sentencias '}' TOK_ELSE '{' sentencias '}' { REGLA(51,"<condicional> ::= if ( <exp> ) { <sentencias> } else { <sentencias> }"); }
                            ;
bucle                       : TOK_WHILE'(' exp ')' '{' sentencias '}' { REGLA(52,"<bucle> ::= while ( <exp> ) { <sentencias> }"); }
                            ;
lectura                     : TOK_SCANF TOK_IDENTIFICADOR { REGLA(54,"<lectura> ::= scanf <identificador>"); 
                                  /*  Buscamos el identificador en las tablas */
                                  INFO_SIMBOLO* info = uso_local($2.lexema);

                                  /* Gestionamos el error de que el simbolo no existe */
                                  if(NULL == info) {
                                      YYABORT;
                                  }

                                  /* Solo podemos leer en caso de escalares */
                                  if(ESCALAR != info->categoria) {
                                      YYABORT;
                                  }

                                  /* Apilamos la direccion de la variable */


                                  /* Llamamos a scanf, restauramos la pila en funcion del tipo */
                                  if(info->tipo == ENTERO || info->tipo == BOOLEANO) {
                                      leer(pfasm, info->lexema, info->tipo);

                                  } else { /* Error en el tipo */
                                      YYABORT;
                                  }
                            }
                            ;
escritura                   : TOK_PRINTF exp { REGLA(56,"<escritura> ::= printf <exp>");

                                               escribir(pfasm, $2.es_direccion, $2.tipo);
                                             }
                            ;
retorno_funcion             : TOK_RETURN exp { REGLA(61, "<retorno_funcion> ::= return <exp>"); }
                            ;
exp                         : exp '+' exp                               { 
                              REGLA(72,"<exp> ::= <exp> + <exp>"); 

                            /* Solo podemos sumar enteros  */
                              if($1.tipo == $3.tipo && $1.tipo == ENTERO) {
                                sumar(pfasm, $1.es_direccion, $3.es_direccion);

                                /* Propaga correctamente los atributos*/
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              } else {
                                printf("No se pueden sumar expresiones no enteras\n");
                                YYABORT;
                              }
                            }

                            | exp '-' exp                               { 

                               REGLA(73,"<exp> ::= <exp> - <exp>"); 

                              /* Solo podemos restar enteros  */
                              if($1.tipo == $3.tipo && $1.tipo == ENTERO) {
                                restar(pfasm, $1.es_direccion, $3.es_direccion);

                                /* Propaga correctamente los atributos*/
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              } else {
                                printf("No se pueden restar expresiones no enteras\n");
                                YYABORT;
                              }


                            }
                            | exp '/' exp                               { 
                                REGLA(74,"<exp> ::= <exp> / <exp>"); 

                              /* Solo podemos dividir enteros  */
                              if($1.tipo == $3.tipo && $1.tipo == ENTERO) {
                                dividir(pfasm, $1.es_direccion, $3.es_direccion);

                                /* Propaga correctamente los atributos*/
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              } else {
                                printf("No se pueden dividir expresiones no enteras\n");
                                YYABORT;
                              }

                            }
                            | exp '*' exp                               { 
                               REGLA(75,"<exp> ::= <exp> * <exp>"); 
                              /* Solo podemos multiplicar enteros  */
                              if($1.tipo == $3.tipo && $1.tipo == ENTERO) {
                                multiplicar(pfasm, $1.es_direccion, $3.es_direccion);

                                /* Propaga correctamente los atributos*/
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              } else {
                                printf("No se pueden multiplicar expresiones no enteras\n");
                                YYABORT;
                              }

                            }
                            | '-'  %prec NEG exp                        { 
                               REGLA(76,"<exp> ::= - <exp>"); 

                              if($2.tipo == ENTERO) {
                                cambiar_signo(pfasm, $2.es_direccion);

                                /* Propaga correctamente los atributos*/
                                $$.tipo = ENTERO;
                                $$.es_direccion = 0;
                              } else {
                                printf("No se pueden cambiar signo a expresiones no enteras\n");
                                YYABORT;
                              }

                            }
                            | exp TOK_AND exp                           { 

                                   REGLA(77,"<exp> ::= <exp> && <exp>"); 

                                   /* Solo podemos ahcer ands de booleanos  */
	                              	if($1.tipo == $3.tipo && $1.tipo == BOOLEANO) {

	                                	y(pfasm, $1.es_direccion, $3.es_direccion);

	                                /* Propaga correctamente los atributos*/
	                                $$.tipo = BOOLEANO;
	                                $$.es_direccion = 0;
	                              } else {
	                                printf("No se puede hacer and de expresiones no booleanas\n");
	                                YYABORT;
	                              }

                            }
                            | exp TOK_OR exp                            { 

                                 REGLA(78,"<exp> ::= <exp> || <exp>"); 

                                  /* Solo podemos ahcer ands de booleanos  */
	                              	if($1.tipo == $3.tipo && $1.tipo == BOOLEANO) {

	                                	o(pfasm, $1.es_direccion, $3.es_direccion);

	                                /* Propaga correctamente los atributos*/
	                                $$.tipo = BOOLEANO;
	                                $$.es_direccion = 0;
	                              } else {
	                                printf("No se puede hacer or de expresiones no booleanas\n");
	                                YYABORT;
	                              }

                             }


                            | '!' exp                                   { 

                                REGLA(79,"<exp> ::= ! <exp>"); 

								if($2.tipo == BOOLEANO) {

                                no(pfasm, $2.es_direccion,etiqueta++);
                                $$.tipo = BOOLEANO;
                                $$.es_direccion = 0;
                              } else {
                                printf("No se pueden negar expresiones no booleanas\n");
                                YYABORT;
                              }

                            }


                            | TOK_IDENTIFICADOR                         { 


                                 REGLA(80,"<exp> ::= <identificador>");
                                 INFO_SIMBOLO* info = uso_local($1.lexema);
                                 if (NULL == info) YYABORT;
                                 if (FUNCION == info->categoria) YYABORT;
                                 if (VECTOR == info->clase) YYABORT;
                                 $$.tipo = info->tipo;
                                 $$.es_direccion = 1;
                                 escribir_operando(pfasm, $1.lexema, 1);
                                                                        }

                            | constante                                 { 
                                  REGLA(81,"<exp> ::= <constante>"); 
                                  $$.tipo = $1.tipo; 
                                  $$.es_direccion = $1.es_direccion; 
                            }
                            
                            | '(' exp ')'                               { 
                                REGLA(82,"<exp> ::= ( <exp> )"); 
                                $$.tipo = $2.tipo;
                                $$.es_direccion = $2.es_direccion;
                            }
                            | '(' comparacion ')'                       { 

                               REGLA(83,"<exp> ::= ( <comparacion> )"); 
							    $$.tipo = $2.tipo;
							    $$.es_direccion = $2.es_direccion;
                            }
                            | elemento_vector                           { REGLA(85,"<exp> ::= <elemento_vector>"); }
                            | identificador '(' lista_expresiones ')'   { REGLA(88,"<exp> ::= <identificador> ( <lista_expresiones> )"); }
                            ;
lista_expresiones           : exp resto_lista_expresiones { REGLA(89,"<lista_expresiones> ::= <exp> <resto_lista_expresiones>"); }
                            | /* empty regla 90 */ { REGLA(90,"<lista_expresiones> ::="); }
                            ;
resto_lista_expresiones     : ',' exp resto_lista_expresiones { REGLA(91,"<resto_lista_expresiones> ::= , <exp> <resto_lista_expresiones>"); }
                            | /* empty regla 92 */ { REGLA(92,"<resto_lista_expresiones> ::="); }
                            ;
comparacion                 : exp TOK_IGUAL exp        { REGLA(93, "<comparacion> ::= <exp> == <exp>"); 

								if($1.tipo == ENTERO && $3.tipo == ENTERO) {

									igual(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
									$$.tipo = BOOLEANO;
   									$$.es_direccion = 0;
								} else {
								    printf("Solo pueden hacerse comparaciones de igualdad entre enteros\n");
								    YYABORT;
								}

                            }
                            | exp TOK_DISTINTO exp     { REGLA(94, "<comparacion> ::= <exp> != <exp>"); 
								if($1.tipo == ENTERO && $3.tipo == ENTERO) {
									distinto(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
									$$.tipo = BOOLEANO;
   									$$.es_direccion = 0;
								} else {
								    printf("Solo pueden hacerse comparaciones de desigualdad entre enteros\n");
								    YYABORT;
								}

                            }
                            | exp TOK_MENORIGUAL exp   { REGLA(95, "<comparacion> ::= <exp> <= <exp>"); 
                                if($1.tipo == ENTERO && $3.tipo == ENTERO) {
									menorigual(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
									$$.tipo = BOOLEANO;
   									$$.es_direccion = 0;
								} else {
								    printf("Solo pueden hacerse comparaciones de menor o igual entre enteros\n");
								    YYABORT;
								}

                            }
                            | exp TOK_MAYORIGUAL exp   { REGLA(96, "<comparacion> ::= <exp> >= <exp>"); 
	                            if($1.tipo == ENTERO && $3.tipo == ENTERO) {
										mayorigual(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
										$$.tipo = BOOLEANO;
	   									$$.es_direccion = 0;
									} else {
									    printf("Solo pueden hacerse comparaciones de mayor o igual entre enteros\n");
									    YYABORT;
									}
							}
                            | exp '<' exp              { REGLA(97, "<comparacion> ::= <exp> < <exp>"); 
								if($1.tipo == ENTERO && $3.tipo == ENTERO) {
									menor(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
									$$.tipo = BOOLEANO;
   									$$.es_direccion = 0;
								} else {
								    printf("Solo pueden hacerse comparaciones de menor entre enteros\n");
								    YYABORT;
								}
                            }
                            | exp '>' exp              { REGLA(98, "<comparacion> ::= <exp> > <exp>");
                               if($1.tipo == ENTERO && $3.tipo == ENTERO) {
									mayor(pfasm, $1.es_direccion, $3.es_direccion, etiqueta++);
									$$.tipo = BOOLEANO;
   								    $$.es_direccion = 0;
								} else {
								    printf("Solo pueden hacerse comparaciones de menor entre enteros\n");
								    YYABORT;
								}
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
identificador               : TOK_IDENTIFICADOR { REGLA(108, "<identificador> ::= TOK_IDENTIFICADOR"); 
                                                  /* Añadimos al ambito actual */
                                                  /*
                                                    Esta regla no esta acabada, habria que gestionar si falla la insercion
                                                    y ver bien los argumentos de tipo y categoria
                                                  */
                                                  if(0 == ambito_actual) {
                                                      declarar_global($1.lexema, tipo_actual, clase_actual, 0);

                                                  } else {
                                                     declarar_local($1.lexema, 0, tipo_actual, clase_actual, 0,0);
                                                  }


                                                }
                            ;


%%

