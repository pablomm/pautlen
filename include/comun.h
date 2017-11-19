#ifndef COMUN_H
#define COMUN_H 1

enum StatusCode { OK = 0, ERR = -1 };

#define UNUSED(arg) ((void)(arg))
#define MACRO_BODY(block) do { BLOCK } while (0)
#define SWAP(a, b, type) MACRO_BODY( type c = a; a = b; b = c; )

typedef enum ErrorType {
	ERR_LEXICO,
	ERR_SINTACTICO,
	ERR_SEMANTICO,
} ErrorType;

void error_handler_syntax(ErrorType type, const char* msg);

#endif /* COMUN_H */
