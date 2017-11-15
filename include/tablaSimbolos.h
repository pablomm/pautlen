#ifndef TABLASIMBOLOS_H
#define TABLASIMBOLOS_H

/* Categoría de un símbolo: variable, parámetro de función o función */
typedef enum CATEGORIA { VARIABLE, PARAMETRO, FUNCION } CATEGORIA;

/* Tipo de un símbolo: sólo se trabajará con enteros y booleanos */
typedef enum TIPO { ENTERO, BOOLEANO } TIPO;

/* Clase de un símbolo: pueden ser variables atómicas (escalares) o listas/arrays (vectores) */
typedef enum CLASE { ESCALAR, VECTOR } CLASE;

/* Información de un símbolo */
typedef struct INFO_SIMBOLO {
    char *lexema;           /* identificador */
    CATEGORIA categoria;    /* categoría */
    TIPO tipo;              /* tipo */
    CLASE clase;            /* clase */
    int adicional1;    /* valor si escalar, longitud si vector, número de parámetros si función */
    int adicional2;    /* posición en llamada a función si parámetro, posición de declaración si variable local de función, número de variables locales si función */
} INFO_SIMBOLO;

/** Inicializa la tabla de simbolos */
int iniciar_scope(void);

/** Destruye la tabla de simbolos */
void liberar_scope(void);

/** Cierra el ambito local activo */
int cerrar_scope_local(void);

/** Declara una variable */
int declarar_global(const char *id, TIPO tipo, CLASE clase, int valor);

/** Declara una variable local nueva */
int declarar_local(const char *id, CATEGORIA categ, TIPO tipo, CLASE clase, int adic1, int adic2);

/** Declara una funcion y abre un ambito local nuevo */
int declarar_funcion(const char *id, TIPO tipo, int n_params, int n_vars);

/** Obtiene la informacion de un simbolo global */
INFO_SIMBOLO* uso_global(const char *id);

/** Obtiene la informacion de un simbolo local */
INFO_SIMBOLO* uso_local(const char *id);

#endif  /* TABLASIMBOLOS_H */
