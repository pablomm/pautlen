#ifndef GLOBAL_H
#define GLOBAL_H 1

enum StatusCode { OK = 0, ERR = -1 };

#define UNUSED(arg) ((void)(arg))
#define MACRO_BODY(block) do { BLOCK } while (0)
#define SWAP(a, b, type) MACRO_BODY( type c = a; a = b; b = c; )

#endif /* GLOBAL_H */
