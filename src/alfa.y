
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

    /* Variables para la declaracion de funciones */
    static int pos_parametro_actual = -1;
    static int num_parametros_actual = 0;
    static int num_variables_locales_actual = 0;
	static int pos_variable_local_actual = 1;
	static int fn_return = 0;
	static int 	en_explist = 0;
	static int num_parametros_llamada_actual;
    static int tamanio_vector_actual = 0;
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
%type <atributos> identificador_nuevo identificador_uso idpf fn_name fn_declaration funcion
%type <atributos> idf_llamada_funcion

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
clase_vector                : TOK_ARRAY tipo '[' constante_entera ']' { 
           REGLA(15,"<clase_vector> ::= array <tipo> [ <constante_entera> ]"); 
           tamanio_vector_actual = $4.valor_entero;
            ASSERT_SEMANTICO(tamanio_vector_actual > 0, "La longitud del vector debe ser positiva", NULL);
            ASSERT_SEMANTICO(tamanio_vector_actual <= MAX_TAMANIO_VECTOR, "Longitud maxima de vector es "MAX_TAMANIO_VECTOR_STR, NULL);



}
                            ;
identificadores             : identificador_nuevo { REGLA(18,"<identificadores> ::= <identificador>"); }
                            | identificador_nuevo ',' identificadores { REGLA(19,"<identificadores> ::= <identificador> , <identificadores>"); }
                            ;
funciones                   : funcion funciones { REGLA(20,"<funciones> ::= <funcion> <funciones>"); }
                            | /* empty regla 21 */ { REGLA(21,"<funciones> ::="); }
                            ;



fn_name : TOK_FUNCTION tipo identificador_uso {

        INFO_SIMBOLO* info = uso_global($3.lexema);
        ASSERT_SEMANTICO(NULL == info, "Declaracion duplicada", NULL);

        /* Abrimos un nuevo ambito */
        /* Esta funcion declarara el nuevo ambito y realizara las inserciones correspondientes */
        /* Aun no sabemos numero de variable y parametros se modificara a posteriori */
        declarar_funcion($3.lexema, tipo_actual, -1, -1);

        /* Inicializamos variables de la funcion */
        num_variables_locales_actual = 0;
		pos_variable_local_actual = 1;
		num_parametros_actual = 0;
		pos_parametro_actual = 0;
		fn_return = 0;
        tamanio_vector_actual = 0;

		/* Propagamos el nombre de la funcion */

		$$ = $3;

		ambito_actual = 1;

        };

fn_declaration : fn_name '(' parametros_funcion ')' '{' declaraciones_funcion {
	INFO_SIMBOLO* info = uso_global($1.lexema);
	info->adicional1 = num_parametros_actual;
	info->adicional2 = num_variables_locales_actual;

	info = uso_local($1.lexema);
	info->adicional1 = num_parametros_actual;
	info->adicional2 = num_variables_locales_actual;

	$$ = $1;

	generar_prologo_funcion(pfasm, $$.lexema, num_variables_locales_actual);

};

funcion : fn_declaration sentencias '}' {
    REGLA(22,"<funcion> ::= function <tipo> <identificador> ( <parametros_funcion> ) { <declaraciones_funcion> <sentencias> }");

	ambito_actual = 0;
	cerrar_scope_local();

	ASSERT_SEMANTICO(fn_return != 0, "Funcion sin sentencia de retorno", $1.lexema);
};



parametros_funcion          : parametro_funcion resto_parametros_funcion { REGLA(23, "<parametros_funcion> ::= <parametro_funcion> <resto_parametros_funcion>"); }
                            | /* empty  regla 24*/ { REGLA(24, "<parametros_funcion> ::="); }
                            ;

resto_parametros_funcion    : ';' parametro_funcion resto_parametros_funcion
                            { REGLA(25,"<resto_parametros_funcion> ::= ; <parametro_funcion> <resto_parametros_funcion>"); }
                            | /* empty regla 26 */ { REGLA(26,"<resto_parametros_funcion> ::="); }
                            ;


idpf : identificador_uso   {    
                             $$ = $1;   
                             clase_actual = ESCALAR;
                            }
                            ;

parametro_funcion           : tipo idpf {

                                REGLA(27,"<parametro_funcion> ::= <tipo> <identificador>");

                                /* Declaracion de un parametro al declarar la funcion */
                                INFO_SIMBOLO* info = uso_solo_local($2.lexema);
                                ASSERT_SEMANTICO(NULL == info, "Declaracion duplicada", NULL);


                                /* Declaramos el parametro en la tabla local */
                                /* Adicional 2 indica la posicion del parametro en llamada a funcion */
                                declarar_local($2.lexema, PARAMETRO, tipo_actual, clase_actual, 0, pos_parametro_actual);

                                /* Se debe incrementar tras insertar en la tabla para que el primer parametro comience en 0 */
                                pos_parametro_actual++;
                                num_parametros_actual++;

                            }
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

                                /* Caso variables globales */
                                if(NULL == uso_solo_local($1.lexema)) {
                                    asignar(pfasm, $1.lexema, $3.es_direccion);

                                /* Caso parametros en funciones en funciones */
                                } else if (PARAMETRO == info->categoria){
                                    asignar_parametro(pfasm, $3.es_direccion, info->adicional2, num_parametros_actual);
                                /* El ultimo caso son variables locales */
                                } else {
                                    asignar_variable_local(pfasm, $3.es_direccion, info->adicional2);
                                }

                                
                              }
                            | elemento_vector '=' exp { REGLA(44,"<asignacion> ::= <elemento_vector> = <exp>"); 


                            asignar_elemento_vector(pfasm, $3.es_direccion);



                        }
                            ;
elemento_vector             : identificador_uso '[' exp ']' { REGLA(48,"<elemento_vector> ::= <identificador> [ <exp> ]"); 

                                INFO_SIMBOLO * info = uso_global($1.lexema);

                                ASSERT_SEMANTICO(info != NULL,"Identificador de vector declarado", $1.lexema);
                                ASSERT_SEMANTICO(VECTOR == info->clase, "Debe ser un vector", $1.lexema);
                                ASSERT_SEMANTICO(ENTERO == $3.tipo, "El indice debe ser un entero", NULL);
                                comprobar_acceso_vector(pfasm, info->adicional1, $1.lexema, $3.es_direccion);
                                $$.tipo = info->tipo;
                                $$.es_direccion = 0;
}
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
                                if(NULL == uso_solo_local($2.lexema)){
                                    leer(pfasm, info->lexema, info->tipo);
                                } else if(PARAMETRO == info->categoria) {
                                    apilar_parametro(pfasm, 1, info->adicional2, num_parametros_actual);
                                    leer_ya_apilado(pfasm, info->tipo);
                                } else {
                                    apilar_variable_local(pfasm, 1, info->adicional2);
                                    leer_ya_apilado(pfasm, info->tipo);
                                }


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
                                ASSERT_SEMANTICO(ambito_actual, "Sentencia de retorno fuera del cuerpo de una funcion", NULL);

                                generar_retorno_funcion(pfasm, $2.es_direccion);
                                fn_return++;

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
                                REGLA(80,"<exp> ::= <identificador>");
                                INFO_SIMBOLO* info = uso_local($1.lexema);
                                ASSERT_SEMANTICO(NULL != info, "Acceso a variable no declarada", $1.lexema);
                                ASSERT_SEMANTICO(FUNCION != info->categoria, "El identificador es una funcion", NULL);
                                ASSERT_SEMANTICO(VECTOR != info->clase, "El identificador es un vector", $1.lexema);

                                /* Miramos si la variable esta en solo local */

                                /* Caso variable global */
                                if(NULL == uso_solo_local($1.lexema)){
                                    escribir_valor_operando(pfasm, $1.lexema, 1);
                                    $$.tipo = info->tipo;
                                    $$.es_direccion = 0;
                                } else if(PARAMETRO == info->categoria) {

                                /* Queremos apilar el valor del parametro */
                                    apilar_parametro(pfasm, 0, info->adicional2, num_parametros_actual);
                                    $$.tipo = info->tipo;
                                    $$.es_direccion = 0;
                                } else {

                                    apilar_variable_local(pfasm, 0, info->adicional2);
                                    $$.tipo = info->tipo;
                                    $$.es_direccion = 0;
                                }
                                
                              }

                            | constante
                              {
                                REGLA(81,"<exp> ::= <constante>");
                                $$.tipo = $1.tipo;
                                $$.es_direccion = $1.es_direccion;
                                apilar_constante(pfasm, $1.valor_entero);
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
                                apilar_valor_vector(pfasm);
                                $$.tipo = $1.tipo;
                                $$.es_direccion = 0;
                              }
                            | idf_llamada_funcion '(' lista_expresiones ')'
                              {
                                REGLA(88,"<exp> ::= <identificador> ( <lista_expresiones> )");
                                INFO_SIMBOLO* info = uso_local($1.lexema);
                                ASSERT_SEMANTICO(NULL != info, "Funcion no declarada", $1.lexema);
                                ASSERT_SEMANTICO(FUNCION == info->categoria, "No es una funcion", $1.lexema);
                                ASSERT_SEMANTICO(num_parametros_llamada_actual == info->adicional1, "Numero incorrecto de parametros en llamada a funcion", $1.lexema);
                                generar_llamada_funcion(pfasm, $1.lexema, num_parametros_llamada_actual);
                                en_explist = 0;
                                $$.tipo = info->tipo;
                                $$.es_direccion = 0;
                              }
                            ;

idf_llamada_funcion : identificador_uso {

		ASSERT_SEMANTICO(en_explist == 0, "No esta permitido el uso de llamadas a funciones como parametros de otras funciones", NULL);

		en_explist = 1;
		num_parametros_llamada_actual = 0;




};


lista_expresiones           : exp resto_lista_expresiones { REGLA(89,"<lista_expresiones> ::= <exp> <resto_lista_expresiones>");
								num_parametros_llamada_actual++;
								apilar_valor(pfasm, $1.es_direccion);

							}
                            | /* empty regla 90 */ { REGLA(90,"<lista_expresiones> ::="); }
                            ;
resto_lista_expresiones     : ',' exp resto_lista_expresiones {
								REGLA(91,"<resto_lista_expresiones> ::= , <exp> <resto_lista_expresiones>");
								num_parametros_llamada_actual++;
								apilar_valor(pfasm, $2.es_direccion);
							}

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
                                                 $$.valor_entero = $1.valor_entero;
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
                            }
                            | TOK_FALSE { REGLA(102, "<constante_logica> ::= false");
                                                     $$.tipo = BOOLEANO;
                                                     $$.es_direccion = 0;
                                                     $$.valor_entero = 0;
                             }
                            ;
constante_entera            : TOK_CONSTANTE_ENTERA { REGLA(104, "<constante_entera> ::= TOK_CONSTANTE_ENTERA");
                                                     $$.tipo = ENTERO;
                                                     $$.es_direccion = 0;
                                                     $$.valor_entero = $1.valor_entero;
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
                                  declarar_global($1.lexema, tipo_actual, clase_actual, tamanio_vector_actual);
                                  tamanio_vector_actual = 0;
                                } else {
                                    ASSERT_SEMANTICO(clase_actual != VECTOR,"No esta permitida la declaracion de vectores en funciones", $1.lexema);
                                  declarar_local($1.lexema, 0, tipo_actual, clase_actual, 0, pos_variable_local_actual);
                                  num_variables_locales_actual++;
                                 pos_variable_local_actual++;
                                }
                              }
                            ;

%%
