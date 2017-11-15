/*!
 * @header 	Trie
 * @author 	@textblock Manuel Blanc <manuel.blanc@estudiante.uam.es> @/textblock
 * @updated	2017-11-13
 * @charset	utf-8
 *
 * Prefix tree (Patricia trie) implementation.
 */

#ifndef TRIE_H
#define TRIE_H

#include <string.h>

/* Trie datatype */
typedef struct TNode** Trie;
/* Actual node */
typedef struct TNode*  TNode;
struct TNode {
	TNode        next;     /* Next branch */
	TNode        down;     /* Down branch */
	const char*  key;      /* Key pointer */
	size_t       len;      /* Key length  */
	void*        val;      /* Node contents   */
	char         cpy[1];   /* Copia de la clave */
};

/*! Crea un nuevo trie (nodo raiz).
 * @return Trie
 */
Trie trie_new(void);

/*! Destroy a trie and all its nodes.
 * Note that it does NOT free the valuesi
 * @param trie Trie
 */
void trie_delete(Trie trie);

/*! Comprueba si esta contenida una clave.
 * @param trie Trie
 * @param key Clave
 * @return Codigo de error
 */
int trie_has(Trie trie, const char* key);

/*! Inserta una palabra.
 * @param trie Trie
 * @param key Clave
 * @param val Nuevo valor
 * @return Codigo de error
 */
int trie_add(Trie trie, const char* key, void* val);

/*! Borra una clave.
 * @param trie Trie
 * @param key Clave
 * @return TRUE o FALSE, ERR en caso de error
 */
int trie_del(Trie trie, const char* key);

/*! Busca un valor.
 * @param trie Trie
 * @param key Clave
 * @param val Puntero donde se guarda el valor
 * @return Codigo de error
 */
int trie_get(Trie trie, const char* key, void** val);

/*! Modifica una palabra.
 * @param trie Trie
 * @param key Clave
 * @param val Nuevo valor
 * @return Codigo de error
 */
int trie_set(Trie trie, const char* key, void* val);

/*! Busca el valor con la clave mas cercana.
 * @param trie Trie
 * @param key Clave
 * @param val Puntero donde se guarda el valor
 * @return Distancia de Levenhstein
 */
int trie_closest_match(Trie trie, const char* key, void** val);

#endif /* TRIE_H */
