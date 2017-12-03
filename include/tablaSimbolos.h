#ifndef TABLASIMBOLOS_H
#define TABLASIMBOLOS_H

#include "tablaHash.h"

#define TAM_SCOPE_GLOBAL 50
#define TAM_SCOPE_LOCAL 50

/** Inicializa la tabla de simbolos */
STATUS iniciar_scope(void);

/** Destruye la tabla de simbolos */
void liberar_scope(void);

/** Cierra el ambito local activo */
STATUS cerrar_scope_local(void);

/** Declara una variable */
STATUS declarar_global(const char* id, TIPO tipo, CLASE clase, int valor);

/** Declara una variable local nueva */
STATUS declarar_local(const char* id, CATEGORIA categ, TIPO tipo, CLASE clase, int adic1, int adic2);

/** Declara una funcion y abre un ambito local nuevo */
STATUS declarar_funcion(const char* id, TIPO tipo, int n_params, int n_vars);

/** Obtiene la informacion de un simbolo global */
INFO_SIMBOLO* uso_global(const char* id);

/** Obtiene la informacion de un simbolo local */
INFO_SIMBOLO* uso_local(const char* id);

#endif  /* TABLASIMBOLOS_H */
