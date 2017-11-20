
#include "comun.h"

#include <stdio.h>

/* Estructura global actualizada por analizador morfologico */
LexerPosition lexpos = {1, 0, 1};

/* Flag para hacer seguimiento del tipo de error */
ErrorType error_flag = NO_ERR;

/* Fichero de salida para las pruebas en general */
FILE *out = NULL;


static const char* type2string(ErrorType type)
{
	if (ERR_LEXICO     == type) return "lexico";
	if (ERR_SINTACTICO == type) return "sintactico";
	if (ERR_MORFOLOGICO == type) return "morfologico";
	if (ERR_SEMANTICO  == type) return "semantico";
	return "desconocido";
}

void error_handler(const char* msg1, const char* msg2) {


	fprintf(stderr, "****Error %s en [lin %u, col %u]",
		type2string(error_flag),
		lexpos.line, lexpos.column - 1
	);
	if(NULL != msg1)
		fprintf(stderr, ": %s", msg1);

	if(NULL != msg2)
		fprintf(stderr, " (%s)", msg2);

	fprintf(stderr,"\n");
}
