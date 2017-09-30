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
/* Con esta estructura podremos controlar si el codigo ensamblador generado tendra comentarios */
#if 1
#	define PUT_COMMENT(...) _put_asm(fpasm, "\t; [DEBUG] ", " [/DEBUG]\n", __VA_ARGS__)
#else
#	define PUT_COMMENT(...) ((void)0)
#endif

/* Funcion auxiliar privada. Es necesaria porque en una macro de aridad variable,
 * __VA_ARGS__ debe contener al menos un argumento. Como queremos que funcione como
 * un printf, el argumento obligatorio debe ser el formato. Esto tiene la pega de
 * que no nos deja anyadir un sufijo facilmente a la cadena del formato. */
static void _put_asm( FILE* fpasm, char* prefix, char* suffix, char* fmt, ...)
{
	va_list ap;
	va_start(ap, fmt);
	if (prefix) fputs(prefix, fpasm);
	vfprintf(fpasm, fmt, ap);
	if (suffix) fputs(suffix, fpasm);
	va_end(ap);
}

/**********************************************************************************/

void escribir_cabecera_compatibilidad(FILE* fpasm)
{
	PUT_COMMENT("Cabecera de compatibilidad");
/* FUNCIÓN PARA PODER HACER EL CÓDIGO MULTIPLATAFORMA U OTROS PARÁMETROS GENERALES TAL VEZ SE PUEDA QUEDAR VACÍA */
	/* Aqui iria la cabecera de compatibilidad */

	/* Pese a ser comentarios, queremos que salgan siempre */
	PUT_DIRECTIVE(";; Paulen 2017");
	PUT_DIRECTIVE(";; -w Manuel Blanc");
	PUT_DIRECTIVE(";; - Pablo Marcos");

}
/**********************************************************************************/

void escribir_subseccion_data(FILE* fpasm)
{
	PUT_COMMENT("Seccion data");
/* FUNCIÓN PARA ESCRIBIR LA SECCIÓN .data:
	MENSAJES GENERALES (TEXTOS)
	VARIABLES AUXILIARES NECESARIAS EN EL COMPILADOR QUE DEBAN TENER UN VALOR CONCRETO */
	/* Variables auxiliares para mensajes de errores en tiempo de ejecución */

	fputc('\n', fpasm);
	PUT_DIRECTIVE("segment .data");

}
/**********************************************************************************/
void escribir_cabecera_bss(FILE* fpasm)
{
	PUT_COMMENT("Seccion BSS");
/* FUNCIÓN PARA ESCRIBIR EL INICIO DE LA SECCIÓN .bss:
	AL MENOS HAY QUE DECLARAR LA VARIABLE AUXILIAR PARA GUARDAR EL PUNTERO DE PILA __esp
*/
	fputc('\n', fpasm);
	PUT_DIRECTIVE("segment .bss");
	PUT_ASM("__esp resd 1");

}
/**********************************************************************************/
void declarar_variable(FILE* fpasm, char * nombre,  int tipo,  int tamano)
{
	PUT_COMMENT("Declaracion de la variable '%s', de tipo %i", nombre, tipo);
/* GENERA EL CÓDIGO ASOCIADO EN LA SECCIÓN .bss PARA DECLARAR UNA VARIABLE CON
	SU NOMBRE (HAY QUE PRECEDER DE _)
	EL TAMANO (1 PARA VARIABLES QUE NO SEAN VECTORES O SU TAMANO EN OTRO CASO )
	TIPO NOSOTROS USAREMOS ESTE AÑO ENTERO O BOOLEANO

*/
	(void) tipo; /* Evitar warning unused */
	/* Aunque sea entero o booleano se declara como resd */
	PUT_ASM("_%s resd %d", nombre, tamano);
}

/************************************************************************************/

void escribir_segmento_codigo(FILE* fpasm)
{
	PUT_COMMENT("Seccion codigo");
/* ESCRIBE EL INICIO DE LA SECCIÓN DE CÓDIGO
	DECLARACIONES DE FUNCIONES QUE SE TOMARAN DE OTROS MODULOS
	DECLARACION DE main COMO ETIQUETA VISIBLE DESDE EL EXTERIOR
*/
	fputc('\n', fpasm);
	PUT_DIRECTIVE("segment .text");
	PUT_DIRECTIVE("global main");

	/* Declaracion de funciones externas libreria alfalib */
	PUT_DIRECTIVE("extern scan_int, print_int, scan_float, print_float, scan_boolean, print_boolean");
	PUT_DIRECTIVE("extern print_endofline, print_blank, print_string");
	PUT_DIRECTIVE("extern alfa_malloc, alfa_free, ld_float");
}

/**********************************************************************************/

void escribir_inicio_main(FILE* fpasm)
{
	PUT_COMMENT("Inicio del main()");
/* ESCRIBE EL PRINCIPIO DEL CÓDIGO PROPIAMENTE DICHO
	ETIQUETA DE INICIO
	SALVAGUARDA DEL PUNTERO DE PILA (esp) EN LA VARIABLE A TAL EFECTO (__esp)

*/
	fputc('\n', fpasm);
	PUT_DIRECTIVE("main:");
	PUT_ASM("mov dword [__esp], esp");
}

/**********************************************************************************/

void escribir_fin(FILE* fpasm)
{
	PUT_COMMENT("Fin del programa");
/* ESCRITURA DEL FINAL DEL PROGRAMA
	GESTIÓN DE ERROR EN TIEMPO DE EJECUCIÓN (DIVISION POR 0)
	RESTAURACION DEL PUNTERO DE PILA A PARTIR DE LA VARIABLE __esp
	SENTENCIA DE RETORNO DEL PROGRAMA
*/
	PUT_ASM("mov dword esp, [__esp]");
	PUT_ASM("ret");
}

/**********************************************************************************/

void escribir_operando(FILE* fpasm, char * nombre, int es_var)
{
	PUT_COMMENT("Guardar el operando %s en la pila", nombre);
/* SE INTRODUCE EL OPERANDO nombre EN LA PILA
	SI ES UNA VARIABLE (es_var == 1) HAY QUE PRECEDER EL NOMBRE DE _
	EN OTRO CASO, SE ESCRIBE TAL CUAL

*/
	PUT_ASM("push dword %s%s", (es_var == 1) ? "_" : "" , nombre);
}

void asignar(FILE* fpasm, char * nombre, int es_referencia)
{
	PUT_COMMENT("Asignacion de la pila a %s", nombre);
/* ESCRIBE EL CÓDIGO PARA REALIZAR UNA ASIGNACIÓN DE LO QUE ESTÉ EN LA CIMA DE LA PILA A LA VARIABLE nombre
	SE RECUPERA DE LA PILA LO QUE HAYA POR EJEMPLO EN EL REGISTRO eax
	SI es_referencia == 0 (ES UN VALOR) DIRECTAMENTE SE ASIGNA A LA VARIABLE _nombre
	EN OTRO CASO es_referencia == 1 (ES UNA DIRECCIÓN, UN NOMBRE DE VARIABLE) HAY QUE OBTENER SU VALOR DESREFERENCIANDO
EL VALOR ES [eax]
*/
	/* B.4.156 MOV : Move Data */
	/* Caso MOV r/m32,reg32  */

	PUT_ASM("pop dword eax");

	if(es_referencia)
		PUT_ASM("mov eax,dword [eax]"); /* Seguro que el cast va aqui?? */

	PUT_ASM("mov dword [_%s], eax", nombre);
}


void sumar(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
	PUT_COMMENT("Suma");
/* GENERA EL CÓDIGO PARA SUMAR LO QUE HAYA EN LAS DOS PRIMERAS (DESDE LA CIMA)
POSICIONES DE LA PILA, TENIENDO EN CUENTA QUE HAY QUE INDICAR SI SON
REFERENCIAS A VALORES O NO (VER ASIGNAR) ¡¡¡CUIDADO CON EL ORDEN !! EL ARGUMENTO
QUE SE REFIERE AL SEGUNDO OPERANDO SERÁ EL QUE NOS ENCONTREMOS EN LA PILA PRIMERO
*/
	/* B.4.3 ADD : Add Integers */
	/* Dividimos los casos para aprovechar que
	   se puede operar con un registro y una posicion
	   de memoria y evitarnos un moc
	*/

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		PUT_ASM("pop dword eax");
		PUT_ASM("pop dword ebx");
		PUT_ASM("add eax, [ebx]");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		PUT_ASM("pop dword ebx");

		PUT_ASM("pop dword eax");
		if(es_referencia_1)
			PUT_ASM("mov dword eax, [eax]");

		PUT_ASM("add eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
	}

	PUT_ASM("push dword eax");
}

void cambiar_signo(FILE* fpasm, int es_referencia)
{
	PUT_COMMENT("Cambio de signo");
/* GENERA EL CÓDIGO PARA CAMBIAR DE SIGNO LO QUE HAYA EN LA CIMA DE LA PILA
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATO O UNA REFERENCIA
A UN VALOR INMEDIATO (VER ASIGNAR)

*/
	/* Nota: Si es referencia NO se modifica el valor de la memoria */
	/* Doc: B.4.189 NEG , NOT : Two’s and One’s Complement */
	PUT_ASM("pop dword eax");

	if(es_referencia)
		PUT_ASM("mov dword eax, [eax]");

	PUT_ASM("neg dword eax");
	PUT_ASM("push dword eax");
}

void no(FILE* fpasm, int es_referencia, int cuantos_no)
{
	PUT_COMMENT("Negacion logica");
/* GENERA EL CÓDIGO PARA NEGAR COMO VALOR LÓGICO LO QUE HAYA EN LA CIMA DE LA PILA
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATO O UNA REFERENCIA A UN VALOR
INMEDIATO (VER ASIGNAR)
COMO ES NECESARIO UTILIZAR ETIQUETAS, SE SUPONE QUE LA VARIABLE cuantos_no ES UN
CONTADOR QUE ASEGURA QUE UTILIZARLO COMO AÑADIDO AL NOMBRE DE LAS ETIQUETAS QUE
USEMOS (POR EJEMPLO cierto: O falso: ) NOS ASEGURARÁ QUE CADA LLAMADA A no
UTILIZA UN JUEGO DE ETIQUETAS ÚNICO
*/

	/*
	   	cmp eax, 0
	   	je _one_x
	   	push dword 0
		jmp _end_not_x
	_one_x:
		push dword 1
	_end_not_x:
	*/
	PUT_ASM("pop eax");
	PUT_ASM("cmp %s, 0", es_referencia ? "[eax]" : "eax");
	PUT_ASM("je _one_%d", cuantos_no);
	PUT_ASM("push dword 0");
	PUT_ASM("jmp _end_not_%d", cuantos_no);
	PUT_ASM("_one_%d:\n\tpush dword 1", cuantos_no);
	PUT_ASM("_end_not_%d:", cuantos_no);
}


void leer(FILE* fpasm, char * nombre, int tipo)
{
	PUT_COMMENT("Lectura a la variable %s", nombre);
/* GENERA EL CÓDIGO PARA LEER UNA VARIABLE DE NOMBRE nombre Y TIPO tipo (ESTE
AÑO SÓLO USAREMOS ENTERO Y BOOLEANO) DE CONSOLA LLAMANDO A LAS CORRESPONDIENTES
FUNCIONES DE ALFALIB (scan_int Y scan_boolean)
*/

	PUT_ASM("pop dword _%s",nombre);

	/* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a scan_boolean */
	PUT_ASM("call %s", (tipo == ENTERO) ? "scan_int" : "scan_boolean");
	PUT_ASM("add esp, 4");
	PUT_ASM("push dword eax");
}


void escribir(FILE* fpasm, int es_referencia, int tipo)
{
	PUT_COMMENT("Escritura");
/* GENERA EL CÓDIGO PARA ESCRIBIR POR PANTALLA LO QUE HAYA EN LA CIMA DE LA PILA
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATNO (es_referencia == 0) O UNA
REFERENCIA A UN VALOR INMEDIANTO (es_referencia == 1) Y QUE HAY QUE LLAMAR A LA CORRESPONDIENTE
FUNCIÓN DE ALFALIB (print_int O print_boolean) DEPENDIENTO DEL TIPO (tipo == BOOLEANO
O ENTERO )
*/
	if(es_referencia) {
		PUT_ASM("pop eax");
		PUT_ASM("push [eax]");
	}

	/* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a print_boolean */
	PUT_ASM("call %s", (tipo == ENTERO) ? "print_int" : "print_boolean");
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

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		PUT_ASM("pop dword eax");
		PUT_ASM("pop dword ebx");
		PUT_ASM("sub eax, [ebx]");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		PUT_ASM("pop dword ebx");

		PUT_ASM("pop dword eax");
		if(es_referencia_1)
			PUT_ASM("mov dword eax, [eax]");

		PUT_ASM("sub eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
	}

	PUT_ASM("push dword eax");

}

void multiplicar(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
	PUT_COMMENT("Multiplicacion");
/* SIMILAR A SUMAR (CUIDADO CON edx PORQUE LA MULTIPILICACIÓN DEJA EL RESULTADO
EN edx:eax) */

	/* B.4.118 IMUL : Signed Integer Multiply */
	/* Estamos en el caso  IMUL r/m32 */

	PUT_ASM("pop dword ebx");
	PUT_ASM("pop dword eax");

	if(es_referencia_1)
		PUT_ASM("mov eax, [eax]");

	PUT_ASM("imul %s", es_referencia_2 ? "[ebx]" : "ebx");

	/* No estoy seguro del orden de apilar argumentos */
	PUT_ASM("push dword edx");
	PUT_ASM("push dword eax");

}

void dividir(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
	PUT_COMMENT("Division");
/* SIMILAR A MULTIPLICAR (CUIDADO CON LA EXTENSIÓN DE SIGNO PREVIA
QUE IMPLICA EL USO DE edx YA QUE cdq EXTIENDE EL SIGNO A edx:eax)
Y CUIDADO TAMBIÉN CON CONTROLAR EN TIEMPO DE EJECUCIÓN LA
DIVISIÓN ENTRE 0 */

	/* B.4.117 IDIV : Signed Integer Divide */
	/* B.4.19 CBW , CWD , CDQ , CWDE : Sign Extensions */

	/* Falta comprobacion de division por 0 */
	/* Salvar registros antes de usarlos? */

	PUT_ASM("pop dword ebx");
	PUT_ASM("pop dword eax");

	if(es_referencia_1)
		PUT_ASM("mov eax, [eax]");

	PUT_ASM("cdq");

	PUT_ASM("idiv %s", es_referencia_2 ? "[ebx]" : "ebx");

	/* Apilamos unicamente el cociente */
	PUT_ASM("push dword eax");
}

void o(FILE* fpasm, int es_referencia_1, int es_referencia_2)
{
	PUT_COMMENT("Disyuncion logica");
	/* B.4.191 OR : Bitwise OR */
	/* Dividimos los casos para aprovechar que
	   se puede operar con un registro y una posicion
	   de memoria y evitarnos un moc
	*/

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		PUT_ASM("pop dword eax");
		PUT_ASM("pop dword ebx");
		PUT_ASM("or eax, [ebx]");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		PUT_ASM("pop dword ebx");

		PUT_ASM("pop dword eax");
		if(es_referencia_1)
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
	   de memoria y evitarnos un moc
	*/

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		PUT_ASM("pop dword eax");
		PUT_ASM("pop dword ebx");
		PUT_ASM("and eax, [ebx]");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		PUT_ASM("pop dword ebx");

		PUT_ASM("pop dword eax");
		if(es_referencia_1)
			PUT_ASM("mov dword eax, [eax]");

		PUT_ASM("and eax, %s", es_referencia_2 ? "[ebx]" : "ebx");
	}

	PUT_ASM("push dword eax");
}
