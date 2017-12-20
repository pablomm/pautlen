
#include <stdio.h>

#include "comun.h"

int main(int argc, char* argv[])
{
    compiler.f_asm = fopen("/dev/null", "r");
    compiler.f_dbg = stdout;
    compiler.f_err = stderr;

    if (argc > 1) compiler.f_in  = fopen_or_die(argv[1], "r");
    if (argc > 2) compiler.f_dbg = fopen_or_die(argv[2], "w");

    run_compiler();

    return 0;
}
