#include "comun.h"
#include "generacion.h"
#include <stdarg.h>

/**
 * @file generacion.c
 * @author Manuel Blanc
 * @author Pablo Marcos
 * @date 30 Sep 2017
 * @brief Modulo para generar codigo nasm en un fichero.
 *
 * @see https://moodle.uam.es/pluginfile.php/1459150/mod_resource/content/1/manual_nasm.pdf
 */

/* Esta serie de macros sirven para desacoplar la escritura la generacion de codigo
 * de la entrada/salida. Tambien tienen la ventaja de que eliminan codigo repetido,
 * haciendo mas facil cambiarlo (eg, el tabulador que prefija cada linea de ensamblador)
 */
#define PUT_ASM(...) _put_asm(fpasm, "\t", "\n", __VA_ARGS__)
#define PUT_DIRECTIVE(...) _put_asm(fpasm, "", "\n", __VA_ARGS__)
#define PUT_LABEL(...) _put_asm(fpasm, "", ":\n", __VA_ARGS__)
/* Con esta estructura podremos controlar si el codigo ensamblador generado tendra comentarios */


#if !defined(NDEBUG) || NDEBUG == 0
#   define PUT_COMMENT(...) _put_asm(fpasm, ";; [DEBUG] ", " [/DEBUG]\n", __VA_ARGS__)
#else
#   define PUT_COMMENT(...) ((void)0)
#endif

/* Funcion auxiliar privada. Es necesaria porque en una macro de aridad variable,
 * __VA_ARGS__ debe contener al menos un argumento. Como queremos que funcione como
 * un printf, el argumento obligatorio debe ser el formato. Esto tiene la pega de
 * que no nos deja anyadir un sufijo facilmente a la cadena del formato. */
static void _put_asm(FILE* fpasm, const char* prefix, const char* suffix, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    if (prefix) fputs(prefix, fpasm);
    vfprintf(fpasm, fmt, ap);
    if (suffix) fputs(suffix, fpasm);
    va_end(ap);
}


#define es_ref_to_str(es_ref)   ((es_ref) ? "variable" : "constante")



/**********************************************************************************/

void escribir_cabecera_compatibilidad(FILE* fpasm)
{
    PUT_COMMENT("Cabecera de compatibilidad");


    /* Pese a ser comentarios, queremos que salgan siempre */
    PUT_DIRECTIVE(";; Paulen 2017");
    PUT_DIRECTIVE(";; - Manuel Blanc");
    PUT_DIRECTIVE(";; - Pablo Marcos");

    fputc('\n', fpasm);

    PUT_DIRECTIVE("%%ifdef TIPO_MSVC");
    PUT_DIRECTIVE("%%define main _main");
    PUT_DIRECTIVE("%%endif");

}
/**********************************************************************************/

void escribir_subseccion_data(FILE* fpasm)
{
    PUT_COMMENT("Seccion data");

    /* Variables auxiliares para mensajes de errores en tiempo de ejecución */
    fputc('\n', fpasm);
    PUT_DIRECTIVE("segment .data");
    PUT_ASM("__msg_error_division\tdb \"Error division por 0\", 0");
    PUT_ASM("__msg_error_vector\tdb \"Error acceso a vector fuera de rango\", 0");

}
/**********************************************************************************/
void escribir_cabecera_bss(FILE* fpasm)
{
    PUT_COMMENT("Seccion BSS");
    fputc('\n', fpasm);
    PUT_DIRECTIVE("segment .bss");
    PUT_ASM("__esp resd 1");

}
/**********************************************************************************/
void declarar_variable(FILE* fpasm, char* nombre,  int tipo,  int tamano)
{
    PUT_COMMENT("Declaracion de la variable '%s', de tipo %i", nombre, tipo);

    UNUSED(tipo); /* Evitar warning unused */
    /* Aunque sea entero o booleano se declara como resd */
    PUT_ASM("_%s resd %d", nombre, tamano);
}

/************************************************************************************/

void escribir_segmento_codigo(FILE* fpasm)
{
    PUT_COMMENT("Seccion codigo");

    fputc('\n', fpasm);
    PUT_DIRECTIVE("segment .text");
    PUT_ASM("global main");

    /* Declaracion de funciones externas libreria alfalib */
    PUT_ASM("extern scan_int, print_int, scan_float, print_float, scan_boolean, print_boolean");
    PUT_ASM("extern print_endofline, print_blank, print_string");
    PUT_ASM("extern alfa_malloc, alfa_free, ld_float");
}

/**********************************************************************************/

void escribir_inicio_main(FILE* fpasm)
{
    PUT_COMMENT("Inicio del main()");

    fputc('\n', fpasm);
    PUT_LABEL("main");
    PUT_ASM("mov dword [__esp], esp");
}

/**********************************************************************************/

void escribir_fin(FILE* fpasm)
{
    PUT_COMMENT("Fin del programa");
    PUT_ASM("jmp near fin");

    /* Error division por 0 */
    PUT_LABEL("error_division");
    PUT_ASM("push __msg_error_division");
    PUT_ASM("jmp near __salida_mensaje_error");

    PUT_LABEL("__error_vector");
    PUT_ASM("push __msg_error_vector");

    PUT_LABEL("__salida_mensaje_error");
    PUT_ASM("call print_string");
    PUT_ASM("call print_endofline");

    /* No balanceamos la pila, se restaura a continuacion */




    PUT_LABEL("fin");
    PUT_ASM("mov dword esp, [__esp]");
    PUT_ASM("ret");
}

/**********************************************************************************/

void escribir_operando(FILE* fpasm, char* nombre, int es_var)
{
    PUT_COMMENT("Guardar el operando %s en la pila", nombre);

    PUT_ASM("push dword %s%s", (es_var == 1) ? "_" : "", nombre);
}

void escribir_valor_operando(FILE* fpasm, char* nombre, int es_var)
{
    PUT_COMMENT("Guardar el operando %s en la pila", nombre);

    PUT_ASM("push dword [%s%s]", (es_var == 1) ? "_" : "", nombre);
}

void asignar(FILE* fpasm, char* nombre, int es_referencia)
{
    PUT_COMMENT("Asignacion de la pila a %s", nombre);

    /* B.4.156 MOV : Move Data */
    /* Caso MOV r/m32,reg32  */

    PUT_ASM("pop dword eax");

    if (es_referencia)
        PUT_ASM("mov eax,dword [eax]");

    PUT_ASM("mov dword [_%s], eax", nombre);
}


void sumar(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
    PUT_COMMENT("Suma");

    /* B.4.3 ADD : Add Integers */
    /* Dividimos los casos para aprovechar que
       se puede operar con un registro y una posicion
       de memoria y evitarnos un moc
    */

    /* Caso solo es referencia el primer operando */
    if (es_referencia_1 && !es_referencia_2) {

        PUT_ASM("pop dword eax");
        PUT_ASM("pop dword ebx");
        PUT_ASM("add eax, [ebx]");

        /* Caso ambos referencia, solo el segundo o ninguno */
    }
    else {

        PUT_ASM("pop dword ebx");

        PUT_ASM("pop dword eax");
        if (es_referencia_1)
            PUT_ASM("mov dword eax, [eax]");

        PUT_ASM("add eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
    }

    PUT_ASM("push dword eax");
}

void cambiar_signo(FILE* fpasm, int es_referencia)
{
    PUT_COMMENT("Cambio de signo");

    /* Nota: Si es referencia NO se modifica el valor de la memoria */
    /* Doc: B.4.189 NEG , NOT : Two’s and One’s Complement */
    PUT_ASM("pop dword eax");

    if (es_referencia)
        PUT_ASM("mov dword eax, [eax]");

    PUT_ASM("neg dword eax");
    PUT_ASM("push dword eax");
}

void no(FILE* fpasm, int es_referencia, int cuantos_no)
{
    PUT_COMMENT("Negacion logica");

    PUT_ASM("pop eax");
    PUT_ASM("cmp dword %s, 0", es_referencia ? "[eax]" : "eax");
    PUT_ASM("je _one_%d", cuantos_no);
    PUT_ASM("push dword 0");
    PUT_ASM("jmp _end_not_%d", cuantos_no);
    PUT_LABEL("_one_%d", cuantos_no);
    PUT_ASM("push dword 1");
    PUT_LABEL("_end_not_%d", cuantos_no);
}


void leer(FILE* fpasm, char* nombre, int tipo)
{
    PUT_COMMENT("Lectura de un %s a %s", (tipo == ENTERO) ? "entero" : "booleano", nombre);

    PUT_ASM("push dword _%s",nombre);

    /* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a scan_boolean */

    PUT_ASM("call %s", (tipo == ENTERO) ? "scan_int" : "scan_boolean");


    PUT_ASM("add esp, 4");
}

void leer_ya_apilado(FILE* fpasm, int tipo)
{
    PUT_COMMENT("Lectura de un %s a direccion ya apilada", (tipo == ENTERO) ? "entero" : "booleano");

    /* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a scan_boolean */

    PUT_ASM("call %s", (tipo == ENTERO) ? "scan_int" : "scan_boolean");


    PUT_ASM("add esp, 4");
}



void escribir(FILE* fpasm, int es_referencia, int tipo)
{
    PUT_COMMENT("Escritura");

    if (es_referencia) {
        PUT_ASM("pop eax");
        PUT_ASM("push dword [eax]");
    }


    /* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a print_boolean */
    PUT_ASM("call %s", (tipo == ENTERO) ? "print_int" : "print_boolean");
    PUT_ASM("call print_endofline");


    PUT_ASM("add esp, 4");
}



void restar(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
    PUT_COMMENT("Resta");

    /* B.4.305 SUB : Subtract Integers */
    /* Dividimos los casos para aprovechar que
       se puede operar con un registro y una posicion
       de memoria y evitarnos un moc
    */

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov dword eax, [eax]");

    PUT_ASM("sub eax, %s", es_referencia_2 ? "[ebx]" : "ebx");

    PUT_ASM("push dword eax");

}

void multiplicar(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
    PUT_COMMENT("Multiplicacion");

    /* B.4.118 IMUL : Signed Integer Multiply */
    /* Estamos en el caso  IMUL r/m32 */

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov eax, [eax]");

    PUT_ASM("imul dword %s", es_referencia_2 ? "[ebx]" : "ebx");

    PUT_ASM("push dword eax");

}

void dividir(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
    PUT_COMMENT("Division");

    /* B.4.117 IDIV : Signed Integer Divide */
    /* B.4.19 CBW , CWD , CDQ , CWDE : Sign Extensions */

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov eax, dword [eax]");

    if (es_referencia_2)
        PUT_ASM("mov ebx, dword [ebx]");

    PUT_ASM("cmp ebx, 0");
    PUT_ASM("je error_division");

    PUT_ASM("cdq");
    PUT_ASM("idiv ebx");

    /* Apilamos unicamente el cociente */
    PUT_ASM("push dword eax");
}

void o(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
    PUT_COMMENT("Disyuncion logica");
    /* B.4.191 OR : Bitwise OR */
    /* Dividimos los casos para aprovechar que
       se puede operar con un registro y una posicion
       de memoria y evitarnos un mov
    */

    /* Caso solo es referencia el primer operando */
    if (es_referencia_1 && !es_referencia_2) {

        PUT_ASM("pop dword eax");
        PUT_ASM("pop dword ebx");
        PUT_ASM("or eax, [ebx]");

        /* Caso ambos referencia, solo el segundo o ninguno */
    }
    else {

        PUT_ASM("pop dword ebx");

        PUT_ASM("pop dword eax");
        if (es_referencia_1)
            PUT_ASM("mov dword eax, [eax]");

        PUT_ASM("or eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
    }

    PUT_ASM("push dword eax");

}


void y(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
    PUT_COMMENT("Conjuncion logica");
    /* B.4.8 AND : Bitwise AND */
    /* Dividimos los casos para aprovechar que
       se puede operar con un registro y una posicion
       de memoria y evitarnos un mov
    */

    /* Caso solo es referencia el primer operando */
    if (es_referencia_1 && !es_referencia_2) {

        PUT_ASM("pop dword eax");
        PUT_ASM("pop dword ebx");
        PUT_ASM("and eax, [ebx]");

        /* Caso ambos referencia, solo el segundo o ninguno */
    }
    else {

        PUT_ASM("pop dword ebx");

        PUT_ASM("pop dword eax");
        if (es_referencia_1)
            PUT_ASM("mov dword eax, [eax]");

        PUT_ASM("and eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
    }

    PUT_ASM("push dword eax");
}

void apilar_constante(FILE* fpasm, int valor)
{
    PUT_COMMENT("Apilar la constante %i", valor);
    PUT_ASM("push dword %i", valor);
}

void apilar_valor(FILE* fpasm, int es_referencia)
{
    if(es_referencia) {
        PUT_COMMENT("Apilar el valor para pasar parametro a funcion");
        PUT_ASM("pop dword ebx");
        PUT_ASM("mov ebx, [ebx]");
        PUT_ASM("push dword ebx");
    }
    /* Si no es referencia no se hace nada pues ya esta en pila */
}

void igual(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta)
{
    PUT_COMMENT("Comparacion igualdad");

    if (es_referencia_1 && !es_referencia_2) {

        PUT_ASM("pop dword eax");
        PUT_ASM("pop dword ebx");
        PUT_ASM("cmp eax, [ebx]");

        /* Caso ambos referencia, solo el segundo o ninguno */
    }
    else {

        PUT_ASM("pop dword ebx");

        PUT_ASM("pop dword eax");
        if (es_referencia_1)
            PUT_ASM("mov dword eax, [eax]");

        PUT_ASM("cmp eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
    }

    /* Comprobamos igualdad */
    PUT_ASM("je _igual_%d", etiqueta);

    /* Caso no se cumple la condicion */
    PUT_ASM("push dword 0");
    PUT_ASM("jmp _end_igual_%d", etiqueta);

    PUT_LABEL("_igual_%d", etiqueta);
    PUT_ASM("push dword 1");
    PUT_LABEL("_end_igual_%d", etiqueta);
}

void distinto(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta)
{
    PUT_COMMENT("Comparacion distinto");

    if (es_referencia_1 && !es_referencia_2) {

        PUT_ASM("pop dword eax");
        PUT_ASM("pop dword ebx");
        PUT_ASM("cmp eax, [ebx]");

        /* Caso ambos referencia, solo el segundo o ninguno */
    }
    else {

        PUT_ASM("pop dword ebx");

        PUT_ASM("pop dword eax");
        if (es_referencia_1)
            PUT_ASM("mov dword eax, [eax]");

        PUT_ASM("cmp eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
    }

    /* Comprobamos igualdad */
    PUT_ASM("je _distinto_%d", etiqueta);

    /* Caso se cumple la condicion */
    PUT_ASM("push dword 1");
    PUT_ASM("jmp _end_distinto_%d", etiqueta);

    PUT_LABEL("_distinto_%d", etiqueta);
    PUT_ASM("push dword 0");
    PUT_LABEL("_end_distinto_%d", etiqueta);
}


void menorigual(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta)
{
    PUT_COMMENT("Menor o igual");

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov eax, dword [eax]");

    if (es_referencia_2)
        PUT_ASM("mov ebx, dword [ebx]");

    PUT_ASM("cmp eax, ebx");

    /* Comprobamos menor o igual */
    /* JLE Jump Less or equal para signed */
    PUT_ASM("jle _menorigual_%d", etiqueta);

    /* Caso se cumple la condicion */
    PUT_ASM("push dword 0");
    PUT_ASM("jmp _end_menorigual_%d", etiqueta);

    PUT_LABEL("_menorigual_%d", etiqueta);
    PUT_ASM("push dword 1");
    PUT_LABEL("_end_menorigual_%d", etiqueta);
}

void mayorigual(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta)
{
    PUT_COMMENT("Mayor o igual");

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov eax, dword [eax]");

    if (es_referencia_2)
        PUT_ASM("mov ebx, dword [ebx]");

    PUT_ASM("cmp eax, ebx");

    /* Comprobamos menor o igual */
    /* JGE Jump greater or equal para signed */
    PUT_ASM("jge _mayorigual_%d", etiqueta);

    /* Caso se cumple la condicion */
    PUT_ASM("push dword 0");
    PUT_ASM("jmp _end_mayorigual_%d", etiqueta);

    PUT_LABEL("_mayorigual_%d", etiqueta);
    PUT_ASM("push dword 1");
    PUT_LABEL("_end_mayorigual_%d", etiqueta);
}

void menor(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta)
{
    PUT_COMMENT("Menor");

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov eax, dword [eax]");

    if (es_referencia_2)
        PUT_ASM("mov ebx, dword [ebx]");

    PUT_ASM("cmp eax, ebx");

    /* Comprobamos menor o igual */
    /* JL Jump Less para signed */
    PUT_ASM("jl _menor_%d", etiqueta);

    /* Caso se cumple la condicion */
    PUT_ASM("push dword 0");
    PUT_ASM("jmp _end_menor_%d", etiqueta);

    PUT_LABEL("_menor_%d", etiqueta);
    PUT_ASM("push dword 1");
    PUT_LABEL("_end_menor_%d", etiqueta);
}

void mayor(FILE* fpasm, int es_referencia_1, int es_referencia_2, int etiqueta)
{
    PUT_COMMENT("Mayor");

    PUT_ASM("pop dword ebx");
    PUT_ASM("pop dword eax");

    if (es_referencia_1)
        PUT_ASM("mov eax, dword [eax]");

    if (es_referencia_2)
        PUT_ASM("mov ebx, dword [ebx]");

    PUT_ASM("cmp eax, ebx");

    /* Comprobamos menor o igual */
    /* JG Jump Less or equal para signed */
    PUT_ASM("jg _mayor_%d", etiqueta);

    /* Caso se cumple la condicion */
    PUT_ASM("push dword 0");
    PUT_ASM("jmp _end_mayor_%d", etiqueta);

    PUT_LABEL("_mayor_%d", etiqueta);
    PUT_ASM("push dword 1");
    PUT_LABEL("_end_mayor_%d", etiqueta);
}

void generar_if_then(FILE* fpasm, int es_referencia, int etiqueta)
{
    PUT_COMMENT("if (%i)", etiqueta);

    PUT_ASM("pop eax");
    if (es_referencia)
        PUT_ASM("mov eax, [eax]");

    PUT_ASM("cmp eax, 0");

    // Si es igual a 0, es falso y saltamos al else
    PUT_ASM("je near _else_%d", etiqueta);

    // En caso contrario, caemos al then
    PUT_COMMENT("then (%i)", etiqueta);
}

void generar_else(FILE* fpasm, int etiqueta)
{
    PUT_COMMENT("else (%i)", etiqueta);
    PUT_ASM("jmp near _endif_%d", etiqueta);
    PUT_LABEL("_else_%d", etiqueta);
}

void generar_endif(FILE* fpasm, int etiqueta)
{
    PUT_COMMENT("endif (%i)", etiqueta);
    PUT_LABEL("_endif_%d", etiqueta);
}

void generar_while(FILE* fpasm, int etiqueta)
{
    PUT_COMMENT("while (%i)", etiqueta);
    PUT_LABEL("_while_%d", etiqueta);
}

void generar_do(FILE* fpasm, int es_referencia, int etiqueta)
{
    PUT_COMMENT("do (%i)", etiqueta);

    PUT_ASM("pop eax");
    if (es_referencia)
        PUT_ASM("mov eax, [eax]");

    PUT_ASM("cmp eax, 0");

    // Si es igual a 0, es falso y terminamos el bucle
    PUT_ASM("je near _endwhile_%d", etiqueta);

    // En caso contrario, caemos al then
}

void generar_endwhile(FILE* fpasm, int etiqueta)
{
    PUT_COMMENT("endwhile (%i)", etiqueta);
    PUT_ASM("jmp near _while_%d", etiqueta);
    PUT_LABEL("_endwhile_%d", etiqueta);
}

void generar_prologo_funcion(FILE* fpasm, const char* nombre, int num_locales)
{
    PUT_COMMENT("Prologo de la funcion %s", nombre);

    PUT_LABEL("_%s", nombre);
    PUT_ASM("push ebp");
    PUT_ASM("mov ebp, esp");
    PUT_ASM("sub esp, %i", 4*num_locales);

    // Las locales estan en ebp[1:locales] (Notacion Python)
    // Los argumentos estan en ebp[-1:-aridad:-1]
}

void generar_retorno_funcion(FILE* fpasm, int es_referencia)
{
    PUT_COMMENT("Retorno de la funcion");

    PUT_ASM("pop eax");
    if(es_referencia)
        PUT_ASM("mov eax, [eax]");

    // Reestablecemos los punteros de pila
    PUT_ASM("mov esp, ebp");
    PUT_ASM("pop ebp");

    // Devolvemos de la funcion
    PUT_ASM("ret");
}

void generar_llamada_funcion(FILE* fpasm, const char* nombre, int aridad)
{
    PUT_COMMENT("Llamada a la funcion %s con %i argumentos", nombre, aridad);

    PUT_ASM("call _%s", nombre);

    PUT_ASM("add esp, %i", 4*aridad);
    PUT_ASM("push dword eax");
}


void apilar_variable_local(FILE *fpasm, int direccion, int posicion_variable)
{

    PUT_COMMENT("Apilando %s de variable local %d", (direccion) ? "direccion" : "valor", posicion_variable);
    /* Usamos la formula que  el parametro se localiza en
     [ebp - <4*posición de la variable en declaración>] */
    
    if(direccion){
        PUT_ASM("lea eax, [ebp - %d]",  4*posicion_variable);
        PUT_ASM("push dword eax");
     } else {

        PUT_ASM("mov eax, dword [ebp - %d]", 4*posicion_variable);
        PUT_ASM("push eax");
    }
/*
    PUT_ASM("lea eax, [ebp - %d]", 4*posicion_variable);
    PUT_ASM("push dword %s", (direccion) ? "eax" : "[eax]");*/

}

void apilar_parametro(FILE *fpasm, int direccion, int posicion_parametro, int numero_parametro)
{
    PUT_COMMENT("Apilando %s de parametro %d de %d parametros", (direccion) ? "direccion" : "valor", posicion_parametro, numero_parametro);

    /* Usamos la formula que  el parametro se localiza en
     [ebp + <4 + 4*(numero de parametros -posicion del parametro en declaracion)>] */
    
    if(direccion) {
        PUT_ASM("lea eax, [ebp + %d]", 4 + 4*(numero_parametro - posicion_parametro));
        PUT_ASM("push dword eax");
    }
    else {
        PUT_ASM("mov eax, dword [ebp + %d]", 4 + 4*(numero_parametro - posicion_parametro));
        PUT_ASM("push dword eax");
    }
/*
    PUT_ASM("lea eax, [ebp + %d]", 4 + 4*(numero_parametro - posicion_parametro));
    PUT_ASM("push dword %s", (direccion) ? "eax" : "[eax]");*/
    
}


void asignar_parametro(FILE *fpasm, int es_referencia, int posicion_parametro, int numero_parametro)
{
    PUT_COMMENT("Asignacionando valor al parametro %d", posicion_parametro);

    /* Obtenemos valor de la expresion */
        /* Lo desreferenciamos si hace falta */
    PUT_ASM("pop dword eax");


    if (es_referencia)
        PUT_ASM("mov eax, dword [eax]");

    /* Calculamos la direccion del parametro */
    PUT_ASM("lea ebx, [ebp + %d]", 4 + 4*(numero_parametro - posicion_parametro));

    PUT_ASM("mov [ebx], eax", 4 + 4*(numero_parametro - posicion_parametro) );
}

void asignar_variable_local(FILE *fpasm, int es_referencia, int posicion_variable)
{
    PUT_COMMENT("Asignacionando valor a variable local %d", posicion_variable);

    /* Obtenemos valor de la expresion */
    PUT_ASM("pop dword eax");

    /* Lo desreferenciamos si hace falta */
    if (es_referencia)
        PUT_ASM("mov eax, dword [eax]");

    PUT_ASM("lea ebx, [ebp - %d]", 4*posicion_variable);
    PUT_ASM("mov [ebx], eax",  4*posicion_variable);
}

void comprobar_acceso_vector(FILE *fpasm, int longitud, const char *nombre, int es_referencia) {

    PUT_COMMENT("Comprobando acceso a vector de longitud %d", longitud);
    PUT_ASM("pop eax");

    if(es_referencia)
        PUT_ASM("mov eax, [eax]");

    PUT_ASM("cmp eax, 0");
    PUT_ASM("jl __error_vector");

    PUT_ASM("cmp eax, %d", longitud);
    PUT_ASM("jge __error_vector");

    PUT_ASM("lea eax, [4*eax + _%s]", nombre);
    PUT_ASM("push eax");

}


void asignar_elemento_vector(FILE* fpasm, int es_referencia)
{
    PUT_COMMENT("Asignacion a elemento del vector");

    /* B.4.156 MOV : Move Data */
    /* Caso MOV r/m32,reg32  */

    /* Pop del valor a asignar */
    PUT_ASM("pop dword eax");

    if (es_referencia)
        PUT_ASM("mov eax,dword [eax]");

    /* Posicion del vector */
    PUT_ASM("pop dword ebx");

    PUT_ASM("mov dword [ebx], eax");
}

void apilar_valor_vector(FILE *fpasm) {

    PUT_COMMENT("Apilando valor del vector");
    PUT_ASM("pop dword eax");
    PUT_ASM("mov eax, dword [eax]");
    PUT_ASM("push dword eax");
}