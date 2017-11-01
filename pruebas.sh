#!/usr/bin/env bash
set -eu

die() { >&2 echo "$@"; exit 1; }

BDIR="."                  # Carpeta con binarios
ODIR="obj"                # Carpeta con objetos
MDIR="misc"               # Carpeta con otros recursos
ALFALIB="$ODIR/alfalib.o" # Objeto alfalib

## Colorines
NC="$(tput sgr0)"
BOLD="$(tput setaf 7)"
REVERSE="$(tput smso)"
RED="$(tput setaf 1)"
GREEN="$NC$(tput setaf 2)"

## Navegamos al directorio del script
cd "$( dirname "${BASH_SOURCE[0]}" )"

## Rutina auxiliar que limpia todos los ficheros generados
clean_and_exit() {
	#make clean
	rm -f prueba1 prueba2 prueba3
	rm -f prueba1.o prueba2.o prueba3.o
	rm -f prueba1.nasm prueba2.nasm prueba3.nasm
	exit 0
}

## Imrprime una cabecera con formato
title()    { echo -e "$REVERSE[$*]$NC"; }
subtitle() { echo -e    "$BOLD[$*]$NC"; }

## Un diff un poco mas descriptivo
diff() {
	echo '> diff' "$@"
	echo -en "$RED"
	command diff "$@"

	if [[ $? -eq 0 ]]; then
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

	diff <($BDIR/main_1_tabla $MDIR/p2_entrada_1.txt) "$MDIR/p2_salida_1.txt"

	echo
}

practica3() {
	title 'Pruebas practica 3 - analizador morfologico'

	subtitle Prueba 3.1
	diff <($BDIR/pruebaMorfo $MDIR/p3_entrada_1.txt) "$MDIR/p3_salida_1.txt"

	subtitle Prueba 3.2
	diff <($BDIR/pruebaMorfo $MDIR/p3_entrada_2.txt 2> /dev/null) "$MDIR/p3_salida_2.txt"

	subtitle Prueba 3.3
	diff <($BDIR/pruebaMorfo $MDIR/p3_entrada_3.txt 2> /dev/null) "$MDIR/p3_salida_3.txt"

	echo
}

compile
practica1
practica2
practica3

title 'Borrando ficheros generados'
clean_and_exit
