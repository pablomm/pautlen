#ifndef GENERACION_H
#define GENERACION_H

#include <stdio.h>


/* Declaraciones de tipos de datos del compilador */
#define ENTERO  0
#define BOOLEANO    1


/* OBSERVACIÓN GENERAL A TODAS LAS FUNCIONES:
   Todas ellas escriben el código NASM a un FILE* proporcionado como primer argumento.
*/

void escribir_cabecera_compatibilidad(FILE* fpasm);
/*
   Función para imprimir el código NASM necesario para que sea multiplataforma.
*/

void escribir_subseccion_data(FILE* fpasm);
/*
   Declaración (con directiva db) de las variables que contienen el texto de los mensajes para la identificación de errores en tiempo de ejecución.
   En este punto, al menos, debes ser capaz de detectar la división por 0.
*/

void escribir_cabecera_bss(FILE* fpasm);
/*
   Código para el principio de la sección .bss.
   Con seguridad sabes que deberás reservar una variable entera para guardar el puntero de pila extendido (esp). Se te sugiere el nombre __esp para esta variable.
*/

void declarar_variable(FILE* fpasm, char* nombre,  int tipo,  int tamano);
/*
   Para ser invocada en la sección .bss cada vez que se quiera declarar una variable:
El argumento nombre es el de la variable.
tipo puede ser ENTERO o BOOLEANO (observa la declaración de las constantes del principio del fichero).
Esta misma función se invocará cuando en el compilador se declaren vectores, por eso se adjunta un argumento final (tamano) que para esta primera práctica siempre recibirá el valor 1.
*/

void escribir_segmento_codigo(FILE* fpasm);
/*
   Para escribir el comienzo del segmento .text, básicamente se indica que se exporta la etiqueta main y que se usarán las funciones declaradas en la librería alfalib.o
*/

void escribir_inicio_main(FILE* fpasm);
/*
   En este punto se debe escribir, al menos, la etiqueta main y la sentencia que guarda el puntero de pila en su variable (se recomienda usar __esp).
*/

void escribir_fin(FILE* fpasm);
/*
   Al final del programa se escribe:
El código NASM para salir de manera controlada cuando se detecta un error en tiempo de ejecución (cada error saltará a una etiqueta situada en esta zona en la que se imprimirá el correspondiente mensaje y se saltará a la zona de finalización del programa).
En el final del programa se debe:
         ·Restaurar el valor del puntero de pila (a partir de su variable __esp)
         ·Salir del programa (ret).
*/

void escribir_operando(FILE* fpasm, char* nombre, int es_var);
void escribir_valor_operando(FILE* fpasm, char* nombre, int es_var);
/*
   Función que debe ser invocada cuando se sabe un operando de una operación aritmético-lógica y se necesita introducirlo en la pila.
nombre es la cadena de caracteres del operando tal y como debería aparecer en el fuente NASM
es_var indica si este operando es una variable (como por ejemplo b1) con un 1 u otra cosa (como por ejemplo 34) con un 0. Recuerda que en el primer caso internamente se representará como _b1 y, sin embargo, en el segundo se representará tal y como esté en el argumento (34).
*/

void asignar(FILE* fpasm, char* nombre, int es_referencia);
/*
Genera el código para asignar valor a la variable de nombre nombre.
Se toma el valor de la cima de la pila.
El último argumento es el que indica si lo que hay en la cima de la pila es una referencia (1) o ya un valor explícito (0).
*/


/* FUNCIONES ARITMÉTICO-LÓGICAS BINARIAS */
/*
   En todas ellas se realiza la operación como se ha resumido anteriormente:
Se extrae de la pila los operandos
Se realiza la operación
Se guarda el resultado en la pila
   Los dos últimos argumentos indican respectivamente si lo que hay en la pila es una referencia a un valor o un valor explícito.
   Deben tenerse en cuenta las peculiaridades de cada operación. En este sentido sí hay que mencionar explícitamente que, en el caso de la división, se debe controlar si el divisor es “0” y en ese caso se debe saltar a la rutina de error controlado (restaurando el puntero de pila en ese caso y comprobando en el retorno que no se produce “Segmentation Fault”)
*/
void sumar(FILE* fpasm, int es_referencia_1, int es_referencia_2);
void restar(FILE* fpasm, int es_referencia_1, int es_referencia_2);
void multiplicar(FILE* fpasm, int es_referencia_1, int es_referencia_2);
void dividir(FILE* fpasm, int es_referencia_1, int es_referencia_2);
void o(FILE* fpasm, int es_referencia_1, int es_referencia_2);
void y(FILE* fpasm, int es_referencia_1, int es_referencia_2);

void cambiar_signo(FILE* fpasm, int es_referencia);
/*
   Función aritmética de cambio de signo.
   Es análoga a las binarias, excepto que sólo requiere de un acceso a la pila ya que sólo usa un operando.
*/

void no(FILE* fpasm, int es_referencia, int cuantos_no);
/*
   Función monádica lógica de negación. No hay un código de operación de la ALU que realice esta operación por lo que se debe codificar un algoritmo que, si encuentra en la cima de la pila un 0 deja en la cima un 1 y al contrario.
   El último argumento es el valor de etiqueta que corresponde (sin lugar a dudas, la implementación del algoritmo requerirá etiquetas). Véase en los ejemplos de programa principal como puede gestionarse el número de etiquetas cuantos_no.
*/

/* FUNCIONES DE ESCRITURA Y LECTURA */
/*
   Se necesita saber el tipo de datos que se va a procesar (ENTERO o BOOLEANO) ya que hay diferentes funciones de librería para la lectura (idem. escritura) de cada tipo.
   Se deben insertar en la pila los argumentos necesarios, realizar la llamada (call) a la función de librería correspondiente y limpiar la pila.
*/
void leer(FILE* fpasm, char* nombre, int tipo);
void escribir(FILE* fpasm, int es_referencia, int tipo);

void apilar_constante(FILE* fpasm, int valor);

void igual(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta);

void distinto(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta);
void menorigual(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta);
void mayorigual(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta);
void menor(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta);
void mayor(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta);

/* Estas 3 funciones generan las partes necesarias para hacer un if.
 * Algunos comentarios acerca de su implementacion:
 * + _Siempre_ deben generarse el else y el endif.
 * + El valor de etiqueta debe ser el mismo para los tres.
 */
void generar_if_then(FILE* fpasm, int es_referencia, int etiqueta);
void generar_else(FILE* fpasm, int etiqueta);
void generar_endif(FILE* fpasm, int etiqueta);

/* Estas 2 funciones funcionan de manera similar para los bucles while */
void generar_while(FILE* fpasm, int etiqueta);
void generar_do(FILE* fpasm, int es_referencia, int etiqueta);
void generar_endwhile(FILE* fpasm, int etiqueta);

/* TODO: Funciones relacionadas con la generacion de funciones */
void generar_prologo_funcion(FILE* fpasm, const char* nombre, int num_locales);
void generar_retorno_funcion(FILE* fpasm, int es_referencia);
void generar_llamada_funcion(FILE* fpasm, const char* nombre, int aridad);
void apilar_valor(FILE* fpasm, int es_referencia);

void asignar_parametro(FILE *fpasm, int es_referencia, int posicion_parametro, int numero_parametro);


void apilar_variable_local(FILE *fpasm, int direccion, int posicion_variable);
void apilar_parametro(FILE *fpasm, int direccion, int posicion_parametro, int numero_parametro);

void asignar_variable_local(FILE *fpasm, int es_referencia, int posicion_variable);


void leer_ya_apilado(FILE* fpasm, int tipo);

#endif
