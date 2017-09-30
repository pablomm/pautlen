#include "generacion.h"

/**
 * @file generacion.c
 * @author Manuel Blanc
 * @author Pablo Marcos
 * @date 30 Sep 2017
 * @brief Modulo para generar codigo nasm en un fichero.
 *
 * @see https://moodle.uam.es/pluginfile.php/1459150/mod_resource/content/1/manual_nasm.pdf
 */

/**********************************************************************************/

void escribir_cabecera_compatibilidad(FILE* fpasm)
{
/* FUNCIÓN PARA PODER HACER EL CÓDIGO MULTIPLATAFORMA U OTROS PARÁMETROS GENERALES TAL VEZ SE PUEDA QUEDAR VACÍA */
	/* Aqui iria la cabecera de compatibilidad */

	fprintf(fpasm, "\n;; Paulen 2017\n");
	fprintf(fpasm, ";; \t Manuel Blanc\n");
	fprintf(fpasm, ";; \t Pablo Marcos\n");

}
/**********************************************************************************/

void escribir_subseccion_data(FILE* fpasm)
{
/* FUNCIÓN PARA ESCRIBIR LA SECCIÓN .data:
	MENSAJES GENERALES (TEXTOS)
	VARIABLES AUXILIARES NECESARIAS EN EL COMPILADOR QUE DEBAN TENER UN VALOR CONCRETO */
	/* Variables auxiliares para mensajes de errores en tiempo de ejecución */

	fprintf(fpasm,"\nsegment .data\n");

}
/**********************************************************************************/
void escribir_cabecera_bss(FILE* fpasm)
{
/* FUNCIÓN PARA ESCRIBIR EL INICIO DE LA SECCIÓN .bss:
	AL MENOS HAY QUE DECLARAR LA VARIABLE AUXILIAR PARA GUARDAR EL PUNTERO DE PILA __esp 
*/
	fprintf(fpasm, "\nsegment .bss\n");
	fprintf(fpasm,"\t__esp resd 1\n");

}
/**********************************************************************************/
void declarar_variable(FILE* fpasm, char * nombre,  int tipo,  int tamano)
{
/* GENERA EL CÓDIGO ASOCIADO EN LA SECCIÓN .bss PARA DECLARAR UNA VARIABLE CON 
	SU NOMBRE (HAY QUE PRECEDER DE _)
	EL TAMANO (1 PARA VARIABLES QUE NO SEAN VECTORES O SU TAMANO EN OTRO CASO )
	TIPO NOSOTROS USAREMOS ESTE AÑO ENTERO O BOOLEANO

*/
	(void) tipo; /* Evitar warning unused */
	/* Aunque sea entero o booleano se declara como resd */
	fprintf(fpasm, "\t_%s resd %d\n", nombre, tamano);	
}

/************************************************************************************/

void escribir_segmento_codigo(FILE* fpasm)
{
/* ESCRIBE EL INICIO DE LA SECCIÓN DE CÓDIGO 
	DECLARACIONES DE FUNCIONES QUE SE TOMARAN DE OTROS MODULOS
	DECLARACION DE main COMO ETIQUETA VISIBLE DESDE EL EXTERIOR
*/
	fprintf(fpasm, "\nsegment .text\n");
	fprintf(fpasm, "\tglobal main\n");

	/* Declaracion de funciones externas libreria alfalib */
	fprintf(fpasm, "\textern scan_int, print_int, scan_float, print_float, scan_boolean, print_boolean\n");
	fprintf(fpasm, "\textern print_endofline, print_blank, print_string\n");
	fprintf(fpasm, "\textern alfa_malloc, alfa_free, ld_float\n");
}

/**********************************************************************************/

void escribir_inicio_main(FILE* fpasm)
{
/* ESCRIBE EL PRINCIPIO DEL CÓDIGO PROPIAMENTE DICHO
	ETIQUETA DE INICIO
	SALVAGUARDA DEL PUNTERO DE PILA (esp) EN LA VARIABLE A TAL EFECTO (__esp)

*/
	fprintf(fpasm, "\nmain:\n");
	fprintf(fpasm, "\tmov dword [__esp], esp\n");
}

/**********************************************************************************/

void escribir_fin(FILE* fpasm)
{
/* ESCRITURA DEL FINAL DEL PROGRAMA
	GESTIÓN DE ERROR EN TIEMPO DE EJECUCIÓN (DIVISION POR 0)
	RESTAURACION DEL PUNTERO DE PILA A PARTIR DE LA VARIABLE __esp
	SENTENCIA DE RETORNO DEL PROGRAMA
*/
	fprintf(fpasm, "\tmov dword esp, [__esp]\n");
	fprintf(fpasm, "\tret\n");
}

/**********************************************************************************/

void escribir_operando(FILE * fpasm, char * nombre, int es_var)
{
/* SE INTRODUCE EL OPERANDO nombre EN LA PILA
	SI ES UNA VARIABLE (es_var == 1) HAY QUE PRECEDER EL NOMBRE DE _
	EN OTRO CASO, SE ESCRIBE TAL CUAL

*/
	fprintf(fpasm,"\tpush dword %s%s\n", (es_var == 1) ? "_" : "" , nombre);
}

void asignar(FILE * fpasm, char * nombre, int es_referencia)
{
/* ESCRIBE EL CÓDIGO PARA REALIZAR UNA ASIGNACIÓN DE LO QUE ESTÉ EN LA CIMA DE LA PILA A LA VARIABLE nombre
	SE RECUPERA DE LA PILA LO QUE HAYA POR EJEMPLO EN EL REGISTRO eax
	SI es_referencia == 0 (ES UN VALOR) DIRECTAMENTE SE ASIGNA A LA VARIABLE _nombre  
	EN OTRO CASO es_referencia == 1 (ES UNA DIRECCIÓN, UN NOMBRE DE VARIABLE) HAY QUE OBTENER SU VALOR DESREFERENCIANDO
EL VALOR ES [eax]
*/
	/* B.4.156 MOV : Move Data */
	/* Caso MOV r/m32,reg32  */
	
	fprintf(fpasm,"\tpop dword eax\n");

	if(es_referencia)
		fprintf(fpasm,"\tmov eax,dword [eax]\n"); /* Seguro que el cast va aqui?? */

	fprintf(fpasm,"\tmov dword [_%s], eax\n", nombre);
}


void sumar(FILE * fpasm, int es_referencia_1, int es_referencia_2)
{
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

		fprintf(fpasm, "\tpop dword eax\n");
		fprintf(fpasm, "\tpop dword ebx\n");
		fprintf(fpasm, "\tadd eax, [ebx]\n");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		fprintf(fpasm, "\tpop dword ebx\n");

		fprintf(fpasm, "\tpop dword eax\n");
		if(es_referencia_1)
			fprintf(fpasm, "\tmov dword eax, [eax]\n");

		fprintf(fpasm, "\tadd eax, %s\n", es_referencia_2 ? "[ebx]" : "ebx");
	}

	fprintf(fpasm, "\tpush dword eax\n");
}

void cambiar_signo(FILE * fpasm, int es_referencia)
{
/* GENERA EL CÓDIGO PARA CAMBIAR DE SIGNO LO QUE HAYA EN LA CIMA DE LA PILA
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATO O UNA REFERENCIA
A UN VALOR INMEDIATO (VER ASIGNAR) 

*/
	/* Nota: Si es referencia NO se modifica el valor de la memoria */
	/* Doc: B.4.189 NEG , NOT : Two’s and One’s Complement */
	fprintf(fpasm, "\tpop dword eax\n");

	if(es_referencia)
		fprintf(fpasm, "\tmov dword eax, [eax]\n");

	fprintf(fpasm, "\tneg dword eax\n");
	fprintf(fpasm, "\tpush dword eax\n");
}

void no(FILE * fpasm, int es_referencia, int cuantos_no)
{
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
	fprintf(fpasm, "\tpop eax\n");
	fprintf(fpasm, "\tcmp %s, 0\n", es_referencia ? "[eax]" : "eax");
	fprintf(fpasm, "\tje _one_%d\n", cuantos_no);
	fprintf(fpasm, "\tpush dword 0\n");
	fprintf(fpasm, "\tjmp _end_not_%d\n", cuantos_no);
	fprintf(fpasm, "_one_%d:\n\tpush dword 1\n", cuantos_no);
	fprintf(fpasm, "_end_not_%d:\n", cuantos_no);
}


void leer(FILE * fpasm, char * nombre, int tipo)
{
/* GENERA EL CÓDIGO PARA LEER UNA VARIABLE DE NOMBRE nombre Y TIPO tipo (ESTE
AÑO SÓLO USAREMOS ENTERO Y BOOLEANO) DE CONSOLA LLAMANDO A LAS CORRESPONDIENTES
FUNCIONES DE ALFALIB (scan_int Y scan_boolean)
*/

	fprintf(fpasm, "\tpop dword _%s\n",nombre);

	/* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a scan_boolean */
	fprintf(fpasm, "\tcall %s\n", (tipo == ENTERO) ? "scan_int" : "scan_boolean");
	fprintf(fpasm, "\tadd esp, 4\n");
	fprintf(fpasm, "\tpush dword eax\n");
}


void escribir(FILE * fpasm, int es_referencia, int tipo)
{
/* GENERA EL CÓDIGO PARA ESCRIBIR POR PANTALLA LO QUE HAYA EN LA CIMA DE LA PILA 
TENIENDO EN CUENTA QUE PUEDE SER UN VALOR INMEDIATNO (es_referencia == 0) O UNA
REFERENCIA A UN VALOR INMEDIANTO (es_referencia == 1) Y QUE HAY QUE LLAMAR A LA CORRESPONDIENTE
FUNCIÓN DE ALFALIB (print_int O print_boolean) DEPENDIENTO DEL TIPO (tipo == BOOLEANO
O ENTERO )
*/
	if(es_referencia) {
		fprintf(fpasm, "\tpop eax\n");
		fprintf(fpasm, "\tpush [eax]\n");
	}

	/* Si tipo no es ENTERO o BOLEANO habria error, pero simplemente llama a print_boolean */
	fprintf(fpasm, "\tcall %s\n", (tipo == ENTERO) ? "print_int" : "print_boolean");
	fprintf(fpasm, "\tadd esp, 4\n");
}



void restar(FILE * fpasm, int es_referencia_1, int es_referencia_2)
{

	/* B.4.305 SUB : Subtract Integers */
	/* Dividimos los casos para aprovechar que 
	   se puede operar con un registro y una posicion 
	   de memoria y evitarnos un moc
	*/

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		fprintf(fpasm, "\tpop dword eax\n");
		fprintf(fpasm, "\tpop dword ebx\n");
		fprintf(fpasm, "\tsub eax, [ebx]\n");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		fprintf(fpasm, "\tpop dword ebx\n");

		fprintf(fpasm, "\tpop dword eax\n");
		if(es_referencia_1)
			fprintf(fpasm, "\tmov dword eax, [eax]\n");

		fprintf(fpasm, "\tsub eax, %s\n", es_referencia_2 ? "[ebx]" : "ebx");
	}

	fprintf(fpasm, "\tpush dword eax\n");

}

void multiplicar(FILE * fpasm, int es_referencia_1, int es_referencia_2)
{
/* SIMILAR A SUMAR (CUIDADO CON edx PORQUE LA MULTIPILICACIÓN DEJA EL RESULTADO
EN edx:eax) */

	/* B.4.118 IMUL : Signed Integer Multiply */
	/* Estamos en el caso  IMUL r/m32 */

	fprintf(fpasm, "\tpop dword ebx\n");
	fprintf(fpasm, "\tpop dword eax\n");

	if(es_referencia_1)
		fprintf(fpasm, "mov eax, [eax]\n");

	fprintf(fpasm, "\timul %s\n", es_referencia_2 ? "[ebx]" : "ebx");

	/* No estoy seguro del orden de apilar argumentos */
	fprintf(fpasm, "\tpush dword edx\n");
	fprintf(fpasm, "\tpush dword eax\n");

}

void dividir(FILE * fpasm, int es_referencia_1, int es_referencia_2)
{
/* SIMILAR A MULTIPLICAR (CUIDADO CON LA EXTENSIÓN DE SIGNO PREVIA
QUE IMPLICA EL USO DE edx YA QUE cdq EXTIENDE EL SIGNO A edx:eax)
Y CUIDADO TAMBIÉN CON CONTROLAR EN TIEMPO DE EJECUCIÓN LA
DIVISIÓN ENTRE 0 */

	/* B.4.117 IDIV : Signed Integer Divide */
	/* B.4.19 CBW , CWD , CDQ , CWDE : Sign Extensions */

	/* Falta comprobacion de division por 0 */
	/* Salvar registros antes de usarlos? */

	fprintf(fpasm, "\tpop dword ebx\n");
	fprintf(fpasm, "\tpop dword eax\n");

	if(es_referencia_1)
		fprintf(fpasm, "mov eax, [eax]\n");

	fprintf(fpasm, "\tcdq\n");

	fprintf(fpasm, "\tidiv %s\n", es_referencia_2 ? "[ebx]" : "ebx");

	/* Apilamos unicamente el cociente */
	fprintf(fpasm, "\tpush dword eax\n");
}

void o(FILE * fpasm, int es_referencia_1, int es_referencia_2)
{
	/* B.4.191 OR : Bitwise OR */
	/* Dividimos los casos para aprovechar que 
	   se puede operar con un registro y una posicion 
	   de memoria y evitarnos un moc
	*/

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		fprintf(fpasm, "\tpop dword eax\n");
		fprintf(fpasm, "\tpop dword ebx\n");
		fprintf(fpasm, "\tor eax, [ebx]\n");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		fprintf(fpasm, "\tpop dword ebx\n");

		fprintf(fpasm, "\tpop dword eax\n");
		if(es_referencia_1)
			fprintf(fpasm, "\tmov dword eax, [eax]\n");

		fprintf(fpasm, "\tor eax, %s\n", es_referencia_2 ? "[ebx]" : "ebx");
	}

	fprintf(fpasm, "\tpush dword eax\n");

}


void y(FILE * fpasm, int es_referencia_1, int es_referencia_2)
{
	/* B.4.8 AND : Bitwise AND */
	/* Dividimos los casos para aprovechar que 
	   se puede operar con un registro y una posicion 
	   de memoria y evitarnos un moc
	*/

	/* Caso solo es referencia el primer operando */
	if(es_referencia_1 && !es_referencia_2) {

		fprintf(fpasm, "\tpop dword eax\n");
		fprintf(fpasm, "\tpop dword ebx\n");
		fprintf(fpasm, "\tand eax, [ebx]\n");

	/* Caso ambos referencia, solo el segundo o ninguno */
	} else {

		fprintf(fpasm, "\tpop dword ebx\n");

		fprintf(fpasm, "\tpop dword eax\n");
		if(es_referencia_1)
			fprintf(fpasm, "\tmov dword eax, [eax]\n");

		fprintf(fpasm, "\tand eax, %s\n", es_referencia_2 ? "[ebx]" : "ebx");
	}

	fprintf(fpasm, "\tpush dword eax\n");
}
