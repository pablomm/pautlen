#include <stdio.h>

static const char* type2string(ErrorType type)
{
	if (ERR_LEXICO     == type) return "lexico";
	if (ERR_SINTACTICO == type) return "sintactico";
	if (ERR_SEMANTICO  == type) return "semantico";
}

void error_handler_syntax(ErrorType type, const char* msg) {
	fprintf(stderr, "****Error %s en [lin %u, col %u]: %s\n",
		type2string(type),
		lexpos.line, lexpos.column,
		msg
	);
}