/*
    Copyright (c) 2017 Manuel Blanc

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include "trie.h"

#include <string.h>
#include <stdlib.h>
#include <limits.h>

// Return codes
#define OK 	(0)
#define ERR	(-1)

// Sentinel value to mark an empty trie node
static int dummy = 0xE5C0BA;
#define NO_VALUE (&dummy)

#define UNUSED(arg) ((void)(arg))
#define SWAP(a, b, type) { type c = a; a = b; b = c; }

// Macros para facilitar la lectura y acceso a propiedades
#define t_next(T)	( (T)->next )
#define t_down(T)	( (T)->down )
#define t_cpy(T) 	( (T)->cpy  )
#define t_len(T) 	( (T)->len  )
#define t_key(T) 	( (T)->key  )
#define t_val(T) 	( (T)->val  )

// Makes a new TNode
static TNode tnode_new(const char* key, size_t len, int copy)
{
	// Reserva de memoria,
	TNode node = malloc(sizeof(struct TNode) + (size_t)copy*len);
	if (NULL == node) return NULL;
	// Inicializamos punteros
	node->next   = node->down = NULL;
	node->len    = len;
	node->val    = NO_VALUE;
	node->cpy[0] = '\0';
	// Copiamos los datos si es necesario
	if (copy)	node->key = strncat(t_cpy(node), key, len);
	else     	node->key = key;
	return node;
}

// Destruye a trie recursively
static void tnode_delete(TNode node)
{
	if (NULL == node) return;
	tnode_delete(t_next(node));
	tnode_delete(t_down(node));
	free(node);
}

static TNode tnode_split(TNode pre, size_t i)
{
	TNode suf = tnode_new(t_key(pre)+i, t_len(pre)-i, 0);
	if (NULL == suf) return NULL;
	suf->down  = t_down(pre);
	suf->val   = t_val(pre);
	pre->val   = NO_VALUE;
	pre->down  = suf;
	pre->len   = i;
	return suf;
}

#include <stdio.h>
void tnode_debug_print(TNode node, int i_len, int i_mask)
{
	int has_next = (NULL != t_next(node));
	int has_down = (NULL != t_down(node));
	int i;

	for (i = 0; i < i_len; i++) {
		printf("%s ", ((i_mask>>i) & 1) ? "│" : " ");
	}

	printf("%s─%s \033[%im%s%.*s%s\033[0m <%p>\n",
		has_next ? "├" : "└",
		has_down ? "┬" : "─",
		(NO_VALUE == t_val(node)) ? 0 : 33, // nuevo
		(0 < i_len) ? "…" : "",
		(int)t_len(node), t_key(node),
		(NO_VALUE == t_val(node)) ? "…" : "",
		t_val(node)
	);
	if (has_down) {
		tnode_debug_print(
			t_down(node),
			i_len+1,
			i_mask | has_next << i_len
		);
	}
	if (has_next) tnode_debug_print(t_next(node), i_len, i_mask);
}

/* Operacion principal de un Trie
 * ==============================
 * Busca la clave 'key' (NTS) en el arbol dado 'trie'.
 * El resultado de la busqueda queda almacenado en *trie.
 * Esta funcion devuelve:
 *  = 0    Si se encuentra el nodo o se llega a un NULL.
 *  > 0    Si se ha abortado en mitad de una comparacion de cadenas
 */
/* static */ size_t tnode_find(Trie* trie, const char** key, size_t* len)
{
	size_t i, min;
	TNode node = **trie;
	// Caso base, se ha llegado a un NULL
	if (NULL == node) return 0;

	// Si esta rama no vale, comprobamos la siguiente
	//printf("[next] %c == %c? %i\n", **key, *t_key(node), **key == *t_key(node));
	if (*t_key(node) != **key) {
		*trie = &t_next(node);
		return tnode_find(trie, key, len);
	}


	min = (*len < t_len(node)) ? *len : t_len(node);

	// Se busca el prefijo maximo
	for (i = 1; i < min; i++) {
		//printf("[down] %c == %c? %i\n", (*key)[i], t_key(node)[i], (*key)[i] == t_key(node)[i]);
		if (t_key(node)[i] != (*key)[i]) {
			break;
		}
	}
	// Se consume el prefijo
	(*key) += i;
	(*len) -= i;

	// Si hemos consumido la clave, abortamos con el indice
	if (i < t_len(node)) return i;

	if (0 == *len) return 0;

	// Si no, se continua hacia abajo.
	*trie = &t_down(node);
	return tnode_find(trie, key, len);
}

#define tnode_valid(node, len, i) (NULL != (node) && NO_VALUE != t_val(node) && 0 == (len) && 0 == (i))

int trie_add(Trie trie, const char* key, void* val)
{
	size_t i, len = strlen(key);
	if (NULL == trie) return 0;
	i = tnode_find(&trie, &key, &len);

	// If we reach an empty leaf, we just insert a new node
	if (NULL == *trie) {
		*trie = tnode_new(key, len, 1);
		if (NULL == *trie) return ERR; // malloc failed
		t_val(*trie) = val;
		return OK;
	}

	// Es un prefijo: dividimos el nodo
	if (0 != i) {
		TNode suf = tnode_split(*trie, i);
		if (NULL == suf) return ERR; // malloc failed
		if (0 == len) {
			t_val(*trie) = val;
		}
		else {
			suf->next = tnode_new(key, len, 1);
			if (NULL == t_next(suf)) return ERR; // malloc failed
			suf->next->val = val;
		}
		return OK;
	}

	return ERR;
}

int trie_del(Trie trie, const char* key)
{
	size_t i, len = strlen(key);
	if (NULL == trie) return 0;
	i = tnode_find(&trie, &key, &len);
	
	if (!tnode_valid(*trie, len, i)) return ERR;

	t_val(*trie) = NO_VALUE;
	return OK;
}

int trie_get(Trie trie, const char* key, void** val)
{
	size_t i, len = strlen(key);
	if (NULL == trie) return ERR;
	i = tnode_find(&trie, &key, &len);

	if (!tnode_valid(*trie, len, i)) return ERR;

	*val = t_val(*trie);
	return OK;
}

int trie_set(Trie trie, const char* key, void* val)
{
	size_t i, len = strlen(key);
	if (NULL == trie) return 0;
	i = tnode_find(&trie, &key, &len);

	if (!tnode_valid(*trie, len, i)) return ERR;

	t_val(*trie) = val;
	return OK;
}

int trie_has(Trie trie, const char* key)
{
	size_t i, len = strlen(key);
	if (NULL == trie) return 0;
	i = tnode_find(&trie, &key, &len);
	return tnode_valid(*trie, len, i);
}

typedef void(*TrieCallback)(void** val);
void trie_forEach(Trie trie, TrieCallback callback) {
	TNode node;
	if (NULL == trie || NULL == *trie) return;
	node = *trie;
	if (NO_VALUE != t_val(node)) callback(&t_val(node));
	trie_forEach(&t_down(node), callback);
	trie_forEach(&t_next(node), callback);
}

Trie trie_new(void)
{
	return calloc(1, sizeof(TNode));
}

void trie_delete(Trie trie)
{
	if (NULL == trie) return;
	if (*trie) tnode_delete(*trie);
	free(trie);
}

// Find the minimum in a list
static int min_vec(const int* a, size_t len)
{
	int min = INT_MAX;
	while (len --> 0) if (a[len] < min) min = a[len];
	return min;
}
static int min3(int a, int b, int c)
{
	if (b < a) a = b;
	if (c < a) a = c;
	return a;
}

/* static */ void tnode_fuzzy_find(TNode node, const char* key, size_t len, void** val, int* d, int* vlast)
{
	if (NULL == node) return;
	tnode_fuzzy_find(t_next(node), key, len, val, d, vlast);

	int v0_buffer[len+1], *v0 = v0_buffer;
	int v1_buffer[len+1], *v1 = v1_buffer;
	memcpy(v0_buffer, vlast, sizeof v0_buffer);

	for (size_t i = 0; i < t_len(node); i++) {
		v1[0] = v0[0] + 1;
		for (size_t j = 1; j <= len; j++) {
			int cost = (t_key(node)[i] == key[j-1]) ? 0 : 1;
			v1[j] = min3(
				v1[j-1] + 1,    // insertion
				v0[j]   + 1,    // deletion
				v0[j-1] + cost  // substitution
			);
		}

		SWAP(v0, v1, int*);
	}

	// If this node is closer, we update the closest word
	if (NO_VALUE != t_val(node) && v0[len] < *d) {
		*d = v0[len];
		*val = t_val(node);
		tnode_fuzzy_find(t_down(node), key, len, val, d, v0);
	}
	// We continue the seach downwards only if the key is
	else if (min_vec(v0, len+1) < *d) {
		tnode_fuzzy_find(t_down(node), key, len, val, d, v0);
	}
}

int trie_closest_match(Trie trie, const char* key, void** val)
{
	size_t len = strlen(key);

	int v0[len+1];
	for (size_t i=0; i <= len; i++) v0[i] = i;

	int dist = INT_MAX;
	tnode_fuzzy_find(*trie, key, len, val, &dist, v0);
	if (NULL == val) return ERR;
	return dist;
}
