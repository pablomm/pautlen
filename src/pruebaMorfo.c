
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>

extern FILE* yyin;
extern FILE* yyout;
extern int yylex();

/* Abre un fichero, y cierra el program si no lo puede abrir */
static FILE* open_file(const char* path, const char* mode)
{
	FILE* fp = fopen(path, mode);
	if (NULL == fp) {
		perror(path);
		exit(1);
	}
	return fp;
}

int main(int argc, char const *argv[])
{
	if (argc >= 2) yyin  = open_file(argv[1], "r");
	if (argc >= 3) yyout = open_file(argv[2], "r");
	while (yylex() > 0);
	return 0;
}
