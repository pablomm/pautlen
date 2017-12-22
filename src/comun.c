
#include "comun.h"

#include <stdio.h>
#include <stdlib.h>

// NOTA: Esta sintaxis es C99 puro.
struct CompilerData compiler = {
    /* Flag para hacer seguimiento del tipo de error */
    .error = NO_ERR,

    /* Posicion actual, actualizada por el analizador morfologico */
    .pos = { .line = 1, .column = 1, },

    /* Fichero de entrada con codigo ALFA */
    .f_in = NULL,

    /* Fichero de salida de ensamblador NASM */
    .f_asm = NULL,

    /* Fichero de errores de compilacion */
    .f_err = NULL,

    /* Informacion de depuracion del estado interno */
    .f_dbg = NULL,
};

FILE* fopen_or_die(const char* path, const char* mode)
{
    FILE* f = fopen(path, mode);
    if (NULL == f) {
        perror(path);
        exit(EXIT_FAILURE);
    }
    return f;
}

static const char* type2string(ErrorType type)
{
    switch (type) {
        case ERR_LEXICO      :
            return "lexico";
        case ERR_SINTACTICO  :
            return "sintactico";
        case ERR_MORFOLOGICO :
            return "morfologico"; // kludge
        case ERR_SEMANTICO   :
            return "semantico";
        default              :
            return "desconocido";
    }
}

#define fperr   (compiler.f_err)

void error_handler(const char* err_msg, const char* extra_info)
{
    fputs("****Error ", fperr);

    if (ERR_MORFOLOGICO != compiler.error) {
        fprintf(fperr, "%s ", type2string(compiler.error));
    }

    fputs("en ", fperr);

    if (ERR_SEMANTICO != compiler.error) {
        fputc('[', fperr);
    }

    fprintf(fperr, "lin %u", compiler.pos.line);

    if (ERR_SEMANTICO != compiler.error) {
        fprintf(fperr, ", col %u]", compiler.pos.column);
    }

    if (NULL != err_msg) {
        fprintf(fperr, ": %s", err_msg);
    }

    if (NULL != extra_info) {
        fprintf(fperr, " (%s)", extra_info);
    }

    if (ERR_MORFOLOGICO != compiler.error) {
        fputs(".", fperr);
    }

    fputs("\n", fperr);
}

extern FILE* yyin;
extern FILE* yyout;

extern int yyparse(void);

int run_compiler()
{
    // Valores por defecto
    if (!compiler.f_in) compiler.f_in = stdin;
    if (!compiler.f_asm) compiler.f_asm = fopen_or_die("/dev/null", "r");
    if (!compiler.f_err) compiler.f_err = stdout;
    if (!compiler.f_dbg) compiler.f_dbg = fopen_or_die("/dev/null", "r");

    yyin = compiler.f_in;
    yyout = stderr;

    int ret = yyparse();

    fclose(compiler.f_in);
    fclose(compiler.f_asm);
    fclose(compiler.f_err);
    fclose(compiler.f_dbg);

    exit(0 == ret ? EXIT_SUCCESS : EXIT_FAILURE);
}
