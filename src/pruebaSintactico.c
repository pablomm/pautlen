

#include <stdio.h>

extern FILE *yyin;
extern FILE *yyout;

FILE *out = NULL;

int yyerror(char* s);


int main(int argc, char* argv[])
{

	FILE *in = stdin;
	out = stdout;

	if(argc > 1) in = fopen(argv[1],"r");
	if(argc > 2) out = fopen(argv[2],"w");

	yyin = in;
	yyout = out;

	yyparse();

	fclose(in);
	fclose(out);

	return 0;
}
