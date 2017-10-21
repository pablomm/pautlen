

#ifndef TABLASIMBOLOS_H
#define TABLASIMBOLOS_H

#include "tablaHash.h"

#define TAM_SCOPE_GLOBAL 50
#define TAM_SCOPE_LOCAL 50


STATUS iniciar_scope(void);

void liberar_scope(void);

STATUS cerrar_scope_local(void);

STATUS declarar_global(const char *id, TIPO tipo, CLASE clase, int valor);

STATUS declarar_local(const char *id, CATEGORIA categ, TIPO tipo, CLASE clase, int adic1, int adic2);

STATUS declarar_funcion(const char *id, TIPO tipo, int n_params, int n_vars);

INFO_SIMBOLO* uso_global(const char *id);

INFO_SIMBOLO* uso_local(const char *id);


#endif  /* TABLASIMBOLOS_H */
