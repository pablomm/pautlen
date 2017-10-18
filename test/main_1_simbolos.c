
#include "tablaHash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void chomp(const char* str)
{
	char* nl = strrchr(str, '\n'); if (nl) *nl = '\0';
}

static TABLA_HASH* scope_local = NULL;
static TABLA_HASH* scope_global = NULL;

static void parse_file(FILE* in, FILE* out)
{
	char buffer[256];

	scope_global = crear_tabla(50);

	// Leemos linea a linea
	while (1) {
		if (NULL == fgets(buffer, sizeof buffer, in)) break;
		chomp(buffer);
		if ('\0' == buffer[0]) continue;

		char* ident   = strtok(buffer, "\t");
		char* num_str = strtok(NULL,   "\t");

		if (NULL == num_str) {
			INFO_SIMBOLO* info;
			if (scope_local) {
				info = buscar_simbolo(scope_local, ident);
				if (NULL != info) {
					fprintf(out, "%s\t%i\n", ident, info->adicional1);
					continue;
				}
			}
			info = buscar_simbolo(scope_global, ident);
			if (NULL != info) {
				fprintf(out,"%s\t%i\n", ident, info->adicional1);
			}
			else {
				fprintf(out, "%s\t-1\n", ident);
			}
			continue;
		}

		int num = atoi(num_str);

		if (num == -999) {
			fprintf(out, "cierre\n");
			liberar_tabla(scope_local);
			scope_local = NULL;
		}
		else if (num < -1) {
			scope_local = crear_tabla(50);
			insertar_simbolo(scope_global, ident, FUNCION, ENTERO, ESCALAR, num, 0);
			insertar_simbolo(scope_local,  ident, FUNCION, ENTERO, ESCALAR, num, 0);
			fprintf(out, "%s\n", ident);
		}
		else {
			TABLA_HASH* scope = scope_local ? scope_local : scope_global;
			if (OK == insertar_simbolo(scope, ident, VARIABLE, ENTERO, ESCALAR, num, 0)) {
				fprintf(out, "%s\n", ident);
			}
			else {
				fprintf(out, "-1\t%s\n", ident);
			}
		}
	}

	if (scope_local) liberar_tabla(scope_local);
	liberar_tabla(scope_global);
}

/*
 * Argumentos:
 *    - argv[1] - fichero de entrada (stdin por defecto)
 *    - argv[2] - fichero de salida (stdout por defecto)
 */
int main(int argc, char** argv)
{

	FILE *in, *out;

	/* Cargamos fichero de entrada o stdin por su defecto */
	if (argc >= 2) {
		in = fopen(argv[1], "r");
		if (NULL == in) {
			fprintf(stderr, "Unable to open input file %s", argv[1]);
			exit(1);
		}

	} else {
		in = stdin;
	}
	/* Cargamos fichero de salida o stdout en su defecto */
	if (argc >= 3) {
		out = fopen(argv[2], "w");
		if (NULL == out) {
			fprintf(stderr, "Unable to open output file %s", argv[2]);
			exit(1);
		}
	} else {
		out = stdout;
	}

	parse_file(in, out);

	fclose(in);
	fclose(out);

	return 0;
}

