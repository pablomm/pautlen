#define _POSIX_C_SOURCE (200809L)

#include "tablaSimbolos.h"
#include "trie.h"

#include <stdlib.h>

#define OK 	(0)
#define ERR	(-1)

static Trie scope_local  = NULL;
static Trie scope_global = NULL;

int iniciar_scope(void)
{
    scope_global = trie_new();
    return (scope_global) ? OK : ERR;
}

void liberar_scope(void)
{
    trie_delete(scope_local);
    trie_delete(scope_global);
}

int cerrar_scope_local(void)
{
	if (NULL == scope_local)
		return ERR;

    trie_delete(scope_local);
    scope_local = NULL;

    return OK;
}

/*
 * Crea una estructura INFO_SIMBOLO a partir de los datos pasados.
 * La memoria para el lexema se duplica, con lo cual el lexema pasado se puede reasignar fuera de la función sin cambiar la estructura.
 *
 * Entrada:
 *      lexema: identificador del símbolo.
 *      categ: categoría del símbolo.
 *      tipo: tipo del símbolo.
 *      clase: clase del símbolo.
 *      adic1: valor adicional del símbolo (dependiente de lo anterior, consultar tablaHash.h)
 *      adic2: valor adicional del símbolo (dependiente de lo anterior, consultar tablaHash.h)
 *
 * Salida:
 *      INFO_SIMBOLO *: puntero a la estructura reservada, NULL si se produce algún error.
 */
INFO_SIMBOLO *crear_info_simbolo(const char *lexema, CATEGORIA categ, TIPO tipo, CLASE clase, int adic1, int adic2)
{
    INFO_SIMBOLO *is;

    if ((is = (INFO_SIMBOLO *) malloc(sizeof(INFO_SIMBOLO)))) {
        /* Duplicar lexema */
        if (!(is->lexema = strdup(lexema))) {
            free(is);
            return NULL;
        }
        is->categoria = categ;
        is->tipo = tipo;
        is->clase = clase;
        is->adicional1 = adic1;
        is->adicional2 = adic2;
    }
    return is;
}
/*
 * Libera una estructura INFO_SIMBOLO.
 * La memoria del lexema se libera también.
 *
 * Entrada:
 *      is: estructura INFO_SIMBOLO a liberar.
 *
 * Salida: ninguna.
 */
void liberar_info_simbolo(INFO_SIMBOLO *is)
{
    if (is) {
        if (is->lexema) {
            free(is->lexema);
        }
        free(is);
    }
}

/*
 * No puede haber parametros en el scope global
 * 
 *  TIPO -> ENTERO/BOOLEANO
 *  CLASE -> ESCALAR/VECTOR
 *  valor ->
 *      longitud si VECTOR
 *      valor si ESCALAR
 */
int declarar_global(const char *id, TIPO tipo, CLASE clase, int valor)
{
    if (NULL == scope_global) return ERR;

    INFO_SIMBOLO* info = crear_info_simbolo(id, VARIABLE, tipo, clase, valor, 0);
    if (NULL == info) return ERR;

    /* No hace falta comprobar si ya esta en el ambito global porque trie_add se encarga de eso */
    return trie_add(scope_global, id, info);
}


int declarar_local(const char *id, CATEGORIA categ, TIPO tipo, CLASE clase, int adic1, int adic2)
{
    if (NULL == scope_local) return ERR;

    INFO_SIMBOLO* info = crear_info_simbolo(id, categ, tipo, clase, adic1, adic2);
    if (NULL == info) return ERR;

    /* No hace falta comprobar si ya esta en el ambito local porque trie_add se encarga de eso */
    return trie_add(scope_local, id, info);
}

int declarar_funcion(const char *id, TIPO tipo, int n_params, int n_vars)
{
    /* Se asume que el ambito global esta inicializado */
    if (NULL == scope_global) return ERR;

	/* Error si existe ambito local no pueden declararse funciones dentro de otras */
	if (NULL != scope_local) return ERR;

    /* Asumimos todas las funciones como escalares? */
    /* Error si existe un simbolo global con el mismo nombre */
    INFO_SIMBOLO* info = crear_info_simbolo(id, FUNCION, tipo, ESCALAR, n_params, n_vars);
    if (NULL == info) return ERR;

    if (ERR == trie_add(scope_global, id, info))
        return ERR;

    /* Creamos ambito local y insertamos la funcion como simbolo */
    scope_local = trie_new();
    if (ERR == trie_add(scope_local, id, info))
        return ERR;

	return OK;
}

INFO_SIMBOLO* uso_global(const char *id)
{
    void* info;
    return OK == trie_get(scope_global, id, &info) ? info : NULL;
}

INFO_SIMBOLO* uso_local(const char *id)
{
    void* info = NULL;

    if (NULL != scope_local) {
        trie_get(scope_local, id, &info);
        if (info) return info;
    }

	return uso_global(id);
}
