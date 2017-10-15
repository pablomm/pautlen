
#include "tablaHash.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void chomp(const char* str)
{
	char* nl = strrchr(str, '\n'); if (nl) *nl = '\0';
}

static TABLA_HASH* scope_local = NULL;
static TABLA_HASH* scope_global = NULL;

static void parse_file(FILE* in)
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
					printf("%s\t%i\n", ident, info->adicional1);
					continue;
				}
			}
			info = buscar_simbolo(scope_global, ident);
			if (NULL != info) {
				printf("%s\t%i\n", ident, info->adicional1);
			}
			else {
				printf("%s\t-1\n", ident);
			}
			continue;
		}

		int num = atoi(num_str);

		if (num == -999) {
			printf("cierre\n");
			liberar_tabla(scope_local);
			scope_local = NULL;
		}
		else if (num < -1) {
			scope_local = crear_tabla(50);
			insertar_simbolo(scope_global, ident, FUNCION, ENTERO, ESCALAR, num, 0);
			insertar_simbolo(scope_local,  ident, FUNCION, ENTERO, ESCALAR, num, 0);
			printf("%s\n", ident);
		}
		else {
			TABLA_HASH* scope = scope_local ? scope_local : scope_global;
			if (OK == insertar_simbolo(scope, ident, VARIABLE, ENTERO, ESCALAR, num, 0)) {
				printf("%s\n", ident);
			}
			else {
				printf("-1\t%s\n", ident);
			}
		}
	}

	if (scope_local) liberar_tabla(scope_local);
	liberar_tabla(scope_global);
}

int main(int argc, char** argv)
{
	if (argc >= 2) {
		FILE* fp = fopen(argv[1], "r");
		if (NULL == fp) {
			fprintf(stderr, "Unable to open file %s", argv[1]);
			exit(1);
		}
		parse_file(fp);
		fclose(fp);
	}
	else {
		parse_file(stdin);
	}

	return 0;
}
