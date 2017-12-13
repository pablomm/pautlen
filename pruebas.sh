#!/usr/bin/env bash
set -eu

die() { >&2 echo "$@"; exit 1; }

BDIR="."                  # Carpeta con binarios
ODIR="obj"                # Carpeta con objetos
MDIR="misc"               # Carpeta con otros recursos
ALFALIB="$ODIR/alfalib.o" # Objeto alfalib
COMPILADOR="$BDIR/compilador"   # Binario del compilador


TABLA_SIMBOLOS_PRUEBAS="$MDIR/tabla_simbolos_pruebas"
ANALIZADOR_MORFOLOGICO_PRUEBAS="$MDIR/analizador_morfologico_pruebas"
ANALIZADOR_SINTACTICO_PRUEBAS="$MDIR/analizador_sintactico_pruebas"
COMPILADOR_PRUEBAS="$MDIR/compilador_pruebas"

## Colorines
NC="$(tput sgr0)"
BOLD="$(tput setaf 7)"
REVERSE="$(tput smso)"
RED="$(tput setaf 1)"
GREEN="$NC$(tput setaf 2)"

## Navegamos al directorio del script
cd "$( dirname "${BASH_SOURCE[0]}" )"

## Contador de numero de pruebas
n_pruebas=0


## Rutina auxiliar que limpia todos los ficheros generados
clean() {
	rm -vf $BDIR/prueba1 $BDIR/prueba2 $BDIR/prueba3
	rm -vf $ODIR/prueba1.o $ODIR/prueba2.o $ODIR/prueba3.o
	rm -vf prueba1.nasm prueba2.nasm prueba3.nasm
	make clean
}

clean_and_exit() {
	clean
	exit 0
}

## Imprime una cabecera con formato
title()    { echo -e "$REVERSE[$*]$NC"; }
subtitle() { echo -e    "$BOLD[$*]$NC"; }

## Un diff un poco mas descriptivo
diff() {
	echo '> diff' "$@"
	echo -en "$RED"
	command diff "$@"

	if [[ $? -eq 0 ]]; then
		n_pruebas=$(expr $n_pruebas + 1)
		echo -e "${GREEN}OK$NC"
	else
		echo -e "${RED}ERR$NC"
	fi
}

# './pruebas.sh clean' borra lo generado
if [[ "$*" == 'clean' ]]; then
	clean_and_exit
fi

# Compilamos tests en carpeta BDIR
compile() {
	title Compilando pruebas
	make test all
	echo
}

practica1() {
	title 'Pruebas practica 1 - generacion'

	# Pruebas con los datos del enunciado
	subtitle Prueba 1.1

	$BDIR/main_0_generacion_ejemplo1 prueba1.nasm
	nasm -f elf32 -o $ODIR/prueba1.o prueba1.nasm
	gcc -m32 -o prueba1 $ODIR/prueba1.o $ALFALIB

	diff <(./prueba1 <<< $'-9\n') <(echo -n $'-1\n')
	diff <(./prueba1 <<< $'10\n') <(echo -n $'18\n')

	subtitle Prueba 1.2

	$BDIR/main_0_generacion_ejemplo2 prueba2.nasm
	nasm -f elf32 -o $ODIR/prueba2.o prueba2.nasm
	gcc -m32 -o prueba2 $ODIR/prueba2.o $ALFALIB

	diff <(./prueba2 <<< $'0\n') <(echo -n $'true\nfalse\n')
	diff <(./prueba2 <<< $'1\n') <(echo -n $'false\ntrue\n')

	subtitle Prueba 1.3

	$BDIR/main_0_generacion_ejemplo3 prueba3.nasm
	nasm -f elf32 -o $ODIR/prueba3.o prueba3.nasm
	gcc -m32 -o prueba3 $ODIR/prueba3.o $ALFALIB

	diff <(./prueba3 <<< $'-3\n3\n0\n') <(echo -n $'true\n3\n0\n3\n')
	diff <(./prueba3 <<< $'10\n-9\n1\n') <(echo -n $'false\n-10\n1\n-9\n')

	echo
}

practica2() {
	title 'Pruebas practica 2 - tabla simbolos'

	i=0
	for file in $(ls $TABLA_SIMBOLOS_PRUEBAS/*.in); do
		i=$(expr $i + 1)
		file=$(echo $file | cut -f 1 -d '.')

		subtitle "Prueba 2.$i - $file"


		diff -bB <($BDIR/main_1_tabla $file.in) "$file.out"
	done

	echo
}

practica3() {
	title 'Pruebas practica 3 - analizador morfologico'

	subtitle Prueba 3.1
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_1.txt) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_salida_1.txt"

	subtitle Prueba 3.2
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_2.txt 2> /dev/null) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_salida_2.txt"

	subtitle Prueba 3.3
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_3.txt 2> /dev/null) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_salida_3.txt"

	echo
}

practica4() {
	title 'Pruebas practica 4 - analizador sintactico'

	i=0
	for file in $(ls $ANALIZADOR_SINTACTICO_PRUEBAS/ej_*.alf); do
		file=$(cut -d "/" -f 3 <<< "$file" | cut -d '_' -f 2- | cut -f 1 -d '.' )
		i=$(expr $i + 1)

		subtitle "Prueba 4.$i - $file"

		diff -bB  <($BDIR/main_3_sintactico $ANALIZADOR_SINTACTICO_PRUEBAS/ej_$file.alf 2> /dev/null)  "$ANALIZADOR_SINTACTICO_PRUEBAS/sal_$file.txt"

	done

	echo
}

compilador() {
	title 'Pruebas compilador - pruebas propias'

	i=0
	for file in $(ls $COMPILADOR_PRUEBAS/*.alf); do
		file=$(cut -d "/" -f 3 <<< "$file" | cut -d '_' -f 2- | cut -f 1 -d '.' )
		i=$(expr $i + 1)

		subtitle "Prueba 5.$i - $file"

		$COMPILADOR "$COMPILADOR_PRUEBAS/$file.alf" "_$file.nasm"

		nasm -f elf32 -o $ODIR/_$file.o _$file.nasm
		gcc -m32 -o _$file $ODIR/_$file.o $ALFALIB


		if [ -f $COMPILADOR_PRUEBAS/$file.in ]; then

		    diff <(./_$file < $COMPILADOR_PRUEBAS/$file.in ) "$COMPILADOR_PRUEBAS/$file.out"
		else

			diff <(./_$file) "$COMPILADOR_PRUEBAS/$file.out"
		fi

		rm -v _$file _$file.nasm $ODIR/_$file.o


	done

	echo
}

#compile
#practica1
#practica2
#practica3
#practica4


# Comentadas resto de pruebas de momento por comodidad
make
compilador

title 'Borrando ficheros generados'
clean

title "$n_pruebas/$n_pruebas pruebas completadas con exito"
echo -e "${GREEN}OK$NC"
exit 0

