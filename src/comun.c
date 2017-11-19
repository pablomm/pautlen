
#include "comun.h"

#include <stdio.h>

/* Estructura global actualizada por analizador morfologico */
LexerPosition lexpos = {1, 0, 1};

/* Flag para hacer seguimiento del tipo de error */
ErrorType error_flag = NO_ERR;


static const char* type2string(ErrorType type)
{
	if (ERR_LEXICO     == type) return "lexico";
	if (ERR_SINTACTICO == type) return "sintactico";
	if (ERR_MORFOLOGICO == type) return "morfologico";
	if (ERR_SEMANTICO  == type) return "semantico";
	return "desconocido";
}

void error_handler(const char* msg) {
	fprintf(stderr, "****Error %s en [lin %u, col %u]: %s\n",
		type2string(error_flag),
		lexpos.line, lexpos.column,
		msg
	);
}
