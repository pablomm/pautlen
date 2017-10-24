
#include "tablaSimbolos.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Se carga */
static void chomp(char* str)
{
	while (*str != '\0' && *str != '\n') str++;
	*str = '\0'; 
}

static FILE* open_file(const char* path, const char* mode)
{
	FILE* fp = fopen(path, mode);
	if (NULL == fp) {
		fprintf(stderr, "%s: No such file or directory\n", path);
		exit(1);
	}
	return fp;
}

static void parse_file(FILE* in, FILE* out)
{
	char buffer[256];
	int ambito_abierto = 0;

	/* Debemos inicializar y destruir la tabla de simbolos */
	iniciar_scope();

	// Leemos linea a linea
	while (1) {
		if (NULL == fgets(buffer, sizeof buffer, in)) break;
		chomp(buffer);
		if ('\0' == buffer[0]) continue;

		char* ident   = strtok(buffer, "\t");
		char* num_str = strtok(NULL,   "\t");

		/* Si no hay numero, buscamos el simbolo */
		if (NULL == num_str) {
			INFO_SIMBOLO* info = uso_local(ident);
			int val = info ? info->adicional1 : -1;
			fprintf(out, "%s\t%i\n", ident, val);
			continue;
		}

		int num = atoi(num_str);

		/* Si hay numero, nos indica que hacer */

		if (-999 == num) {
			if (0 != strcmp(ident, "cierre")) {
				/* Esto no deberia darse nunca */
				fprintf(stderr, "Invalid close command\n");
				exit(1);
			}
			cerrar_scope_local();
			fprintf(out, "cierre\n");
			ambito_abierto = 1;
		}
		else if (num < -1) {
			declarar_funcion(ident, ENTERO, num, 0);
			fprintf(out, "%s\n", ident);
		}
		else {
			STATUS code;
			
			/* Intentamos */
			code = declarar_local(ident, VARIABLE, ENTERO, ESCALAR, num, 0);
			
			/* Intentamos */
			if (ERR == code) {
				code = declarar_global(ident, ENTERO, ESCALAR, num);
			}

			/* Si no hemos conseguido  */
			if (OK == code) {
				fprintf(out, "%s\n", ident);
			}
			else {
				fprintf(out, "-1\t%s\n", ident);
			}
		}
	}

	liberar_scope();
}

/*
 * Argumentos:
 *    - argv[1] - fichero de entrada (stdin por defecto)
 *    - argv[2] - fichero de salida (stdout por defecto)
 */
int main(int argc, char** argv)
{
	FILE *in = stdin, *out = stdout;

	if (argc >= 2) in  = open_file(argv[1], "r");
	if (argc >= 3) out = open_file(argv[2], "w");

	parse_file(in, out);

	/* Puede que cerremos stdin/out pero no pasa nada porque se acaba el programa */
	fclose(in);
	fclose(out);
	return 0;
}

