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
BOLD="$(tput bold)"
REVERSE="$(tput smso)"
RED="$(tput setaf 1)"
GREEN="$NC$(tput setaf 2)"

## Navegamos al directorio del script
cd "$( dirname "${BASH_SOURCE[0]}" )"

## Contador de numero de pruebas
n_pruebas=0

## Compilamos tests en carpeta BDIR
compile() {
	title Compilando pruebas
	echo
	make debug test
	echo
}

## Rutina auxiliar que limpia todos los ficheros generados
clean() {
	rm -f $BDIR/prueba1 $BDIR/prueba2 $BDIR/prueba3
	rm -f $ODIR/prueba1.o $ODIR/prueba2.o $ODIR/prueba3.o
	rm -f prueba1.nasm prueba2.nasm prueba3.nasm _*.nasm
	rm -f $ODIR/_*.o $BDIR/_*
	make clean > /dev/null
}

## Imprime una cabecera con formato
title()    { printf "$REVERSE[%s]$NC" "$*"; }
subtitle() { printf "\n$BOLD%-60s$NC" "$*"; }

## Un diff un poco mas descriptivo
diff() {
	#echo '$ diff' "$@"
	echo -en "$RED"
	command diff "$@"

	if [[ $? -eq 0 ]]; then
		n_pruebas=$(expr $n_pruebas + 1 )
		echo -en "${GREEN}OK$NC "
		return 0
	else
		echo -en "${RED}ERR$NC "
		return 1
	fi
}

# './pruebas.sh clean' borra lo generado
if [[ "$*" == 'clean' ]]; then
	clean
	exit 0
fi

make_nasm_exe() {
	local file_nasm="$1"
	nasm -f elf32 -o "$ODIR/${file_nasm%.*}.o" "$file_nasm"
	gcc -m32 -o "${file_nasm%.*}" "$ODIR/${file_nasm%.*}.o" "$ALFALIB"
}

practica1() {
	title 'Pruebas practica 1 - generacion'

	# Pruebas con los datos del enunciado
	subtitle Prueba 1.1

	$BDIR/main_0_generacion_ejemplo1 prueba1.nasm
	make_nasm_exe prueba1.nasm

	diff <(./prueba1 <<< $'-9\n') <(echo -n $'-1\n')
	diff <(./prueba1 <<< $'10\n') <(echo -n $'18\n')

	subtitle Prueba 1.2

	$BDIR/main_0_generacion_ejemplo2 prueba2.nasm
	make_nasm_exe prueba2.nasm

	diff <(./prueba2 <<< $'0\n') <(echo -n $'true\nfalse\n')
	diff <(./prueba2 <<< $'1\n') <(echo -n $'false\ntrue\n')

	subtitle Prueba 1.3

	$BDIR/main_0_generacion_ejemplo3 prueba3.nasm
	make_nasm_exe prueba3.nasm

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


		diff <($BDIR/main_1_tabla $file.in) "$file.out"
	done

	echo
}

practica3() {
	title 'Pruebas practica 3 - analizador morfologico'

	subtitle 'Prueba 3.1 - entrada valida'
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_1.txt) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_salida_1.txt"

	subtitle 'Prueba 3.2 - identificador largo'
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_2.txt 2> /dev/null) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_salida_2.txt"
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_2.txt 2>&1 > /dev/null) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_error_2.txt"

	subtitle 'Prueba 3.3 - simbolo no permitido'
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_3.txt 2> /dev/null) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_salida_3.txt"
	diff <($BDIR/main_2_morfo $ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_entrada_3.txt 2>&1 > /dev/null) "$ANALIZADOR_MORFOLOGICO_PRUEBAS/p3_error_3.txt"

	echo
}

practica4() {
	title 'Pruebas practica 4 - analizador sintactico'

	i=0
	for file in $(ls $ANALIZADOR_SINTACTICO_PRUEBAS/ej_*.alf); do
		file=$(basename -s .alf "$file")
		file=${file#*_}

		i=$(expr $i + 1)

		subtitle "Prueba 4.$i - $file"

		diff <($BDIR/main_3_sintactico $ANALIZADOR_SINTACTICO_PRUEBAS/ej_$file.alf 2> /dev/null)  "$ANALIZADOR_SINTACTICO_PRUEBAS/sal_$file.txt"

	done

	echo
}

compilador() {
	title "Pruebas compilador - $2"

	local COMPILADOR_PRUEBAS
	COMPILADOR_PRUEBAS=$3

	local i=0
	for file in $(ls $COMPILADOR_PRUEBAS/*.alf); do
		i=$(expr $i + 1)

		file=$(basename -s .alf "$file")

		# Borramos los generados de la prueba anterior
		rm -f "_$file" "_$file.nasm" "$ODIR/_$file.o"


		subtitle "Prueba $1.$i - $file"

		# Si existe un fichero .err, tiene errores semanticos
		if [[ -f "$COMPILADOR_PRUEBAS/$file.err" ]]; then
			diff <($COMPILADOR "$COMPILADOR_PRUEBAS/$file.alf" "_$file.nasm") "$COMPILADOR_PRUEBAS/$file.err"
			continue
		fi

		$COMPILADOR "$COMPILADOR_PRUEBAS/$file.alf" "_$file.nasm"
		make_nasm_exe _$file.nasm

		# Si hay ficheros que acaben en _N, ejecutamos para cada par (in, out)
		if [[ -f "$COMPILADOR_PRUEBAS/${file}_1.input" ]]; then
			# Buscamos por ficheros IN porque no tiene sentido comprobar salidas
			# diferentes si no hay entrada/es la misma (los programas son deterministas).
			for in_file in $(find "$COMPILADOR_PRUEBAS" -iname "${file}_?.input" -type f); do
				diff <(./_$file < "$in_file") "${in_file%.*}.output"
			done

		# En caso contrario, buscamos un out y opcionalmente un in
		elif [[ -f "$COMPILADOR_PRUEBAS/$file.output" ]]; then
			if [[ -f "$COMPILADOR_PRUEBAS/$file.input" ]]; then
				diff <("./_$file" < "$COMPILADOR_PRUEBAS/$file.input") "$COMPILADOR_PRUEBAS/$file.output"
			else
				diff <(./_$file) "$COMPILADOR_PRUEBAS/$file.output"
			fi

		# Nos quejamos si no hay salida
		else
			echo "error: No existen $file.out ni $file.err"
			exit 1
		fi

	done
	echo
}

compile
practica1
practica2
practica3
practica4
compilador 5 'Pruebas propias' "$COMPILADOR_PRUEBAS"
compilador 6 'Ficheros de correccion' "$MDIR/ficheros_correccion"

title 'Borrando ficheros generados'
echo
clean

title "$n_pruebas/$n_pruebas pruebas completadas con exito"
echo
exit 0

