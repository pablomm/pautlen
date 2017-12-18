#include "tablaSimbolos.h"

static TABLA_HASH* scope_local = NULL;
static TABLA_HASH* scope_global = NULL;


STATUS iniciar_scope(void)
{
    scope_global = crear_tabla(TAM_SCOPE_GLOBAL);
    return (scope_global) ? OK : ERR;
}

void liberar_scope(void)
{
    liberar_tabla(scope_local);
    liberar_tabla(scope_global);
}

STATUS cerrar_scope_local(void)
{
    if (NULL == scope_local)
        return ERR;

    liberar_tabla(scope_local);
    scope_local = NULL;

    return OK;
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
STATUS declarar_global(const char* id, TIPO tipo, CLASE clase, int valor)
{
    /* Se asume que el ambito global esta inicializado */

    /* Comprobamos que no existe ya el simbolo */
    return insertar_simbolo(scope_global, id, VARIABLE, tipo, clase, valor, 0);
}


STATUS declarar_local(const char* id, CATEGORIA categ, TIPO tipo, CLASE clase, int adic1, int adic2)
{
    if (NULL == scope_local) return ERR;
    /* No hace falta comprobar si ya esta en el ambito local porque tablaHash se encarga de eso */
    return insertar_simbolo(scope_local, id, categ, tipo, clase, adic1, adic2);
}

STATUS declarar_funcion(const char* id, TIPO tipo, int n_params, int n_vars)
{
    /* Se asume que el ambito global esta inicializado */

    /* Error si existe ambito local no pueden declararse funciones dentro de otras */
    if (NULL != scope_local) return ERR;

    /* Asumimos todas las funciones como escalares? */
    /* Error si existe un simbolo global con el mismo nombre */
    if (ERR == insertar_simbolo(scope_global, id, FUNCION, tipo, ESCALAR, n_params, n_vars))
        return ERR;

    /* Creamos ambito local y insertamos la funcion como simbolo */
    scope_local = crear_tabla(TAM_SCOPE_LOCAL);
    if (ERR == insertar_simbolo(scope_local,  id, FUNCION, tipo, ESCALAR, n_params, n_vars))
        return ERR;

    return OK;
}

INFO_SIMBOLO* uso_global(const char* id)
{
    return (scope_global) ? buscar_simbolo(scope_global, id) : NULL;
}

INFO_SIMBOLO* uso_local(const char* id)
{
    INFO_SIMBOLO* info;

    if (NULL != scope_local) {

        info = buscar_simbolo(scope_local, id);
        if (info) return info;
    }

    return uso_global(id);
}


INFO_SIMBOLO* simbolos_globales(void)
{
    return (scope_global) ? lista_simbolos(scope_global) : NULL;
}


INFO_SIMBOLO* simbolos_locales(void)
{
    return (scope_local) ? lista_simbolos(scope_local) : NULL;
}
