#include <stdio.h>
#include "generacion.h"

int main (int argc, char** argv)
{
    FILE * salida;
    int num_nots = 0;

    if (argc != 2) {fprintf (stdout, "ERROR POCOS ARGUMENTOS\n"); return -1;}
    

    salida = fopen(argv[1],"w");


    escribir_cabecera_compatibilidad(salida);
    escribir_subseccion_data(salida);
    escribir_cabecera_bss(salida);
    declarar_variable(salida, "b1", BOOLEANO, 1);


    escribir_segmento_codigo(salida);
    escribir_inicio_main(salida);

    leer(salida,"b1",BOOLEANO);
    escribir_operando(salida,"b1",1); 

    no(salida,1,num_nots++);
    escribir(salida,0,BOOLEANO);


    escribir_operando(salida,"b1",1);
    no(salida,1,num_nots++);
    no(salida,0,num_nots++);
    escribir(salida,0,BOOLEANO);

    escribir_fin(salida);

    fclose(salida);
    return 0;

}
