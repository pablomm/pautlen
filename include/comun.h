#ifndef COMUN_H
#define COMUN_H 1

#include <stdio.h>

#define UNUSED(arg) ((void)(arg))
#define MACRO_BODY(block) do { block } while (0)
#define SWAP(a, b, type) MACRO_BODY( type c = a; a = b; b = c; )

#define MAX_LONG_ID 100

/**************** DECLARACIONES DE TIPOS ****************/

/* Retorno de función error/ok */
typedef enum { ERR = 0, OK = 1 } STATUS;

/* Categoría de un símbolo: variable, parámetro de función o función */
typedef enum { VARIABLE, PARAMETRO, FUNCION } CATEGORIA;

/* Tipo de un símbolo: sólo se trabajará con enteros y booleanos */
typedef enum { ENTERO, BOOLEANO } TIPO;

/* Clase de un símbolo: pueden ser variables atómicas (escalares) o listas/arrays (vectores) */
typedef enum { ESCALAR, VECTOR } CLASE;

/*
TODO PASO 11
typedef struct elemento
{
   lexema
   categoría  (variable, parámetro o función)
   clase  (escalar o vector)
   tipo  (entero o booleano)
   tamaño  (número de elementos de un vector)
   número de variables locales
   posición de variable local
   número de parámetros
   posición de parámetro


} elementoTablaSimbolos;

*/

typedef enum ErrorType {
    NO_ERR,
    ERR_LEXICO,
    ERR_MORFOLOGICO,
    ERR_SINTACTICO,
    ERR_SEMANTICO,
} ErrorType;

typedef struct _LexerPosition {
    unsigned line, column;
}  LexerPosition;

extern struct CompilerData {
    ErrorType error;
    LexerPosition pos;
    FILE* f_in, *f_asm, *f_err, *f_dbg;
} compiler;



void error_handler(const char* msg1, const char* msg2);

/* Estructura comun valores de los atributos */
typedef struct {
    char lexema[MAX_LONG_ID+1];
    int valor_entero;
    TIPO tipo;
    int es_direccion;
    int etiqueta;
} tipo_atributos;

FILE* fopen_or_die(const char* path, const char* mode);

int run_compiler(void);

#endif /* COMUN_H */
