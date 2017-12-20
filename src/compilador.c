/**
 * Punto de entrada al compilador
 */

#include <stdio.h>
#include "comun.h"

int main(int argc, char* argv[])
{

	compiler.f_in = stdin;
	compiler.f_asm = stdout;

    if (argc > 1) compiler.f_in = fopen_or_die(argv[1], "r");
    if (argc > 2) compiler.f_asm = fopen_or_die(argv[2], "w");
    if (argc > 3) compiler.f_err = fopen_or_die(argv[3], "w");

    run_compiler();

    return 0;
}
