#!/usr/bin/env bash
set -eu

die() { >&2 echo "$@"; exit 1; }

## Carpeta con binarios
BDIR="."

## Carpeta con objetos
ODIR="obj"

## Carpeta con otros recursos
MDIR="misc"

## Objeto alfalib
ALFALIB="$ODIR/alfalib.o"

## Colorines
NC='\033[0m'
GREEN='\033[0;32m'
RED='\033[0;31m'

# Navegamos al directorio del script
cd "$( dirname "${BASH_SOURCE[0]}" )"

# './pruebas.sh clean' borra lo generado
if [ $# -eq 1 ] && [ $1 = clean ]; then

    make clean
    rm -f prueba1 prueba2 prueba3
    rm -f prueba1.o prueba2.o prueba3.o
    rm -f prueba1.nasm prueba2.nasm prueba3.nasm
    exit 0
fi

# Compilamos tests en carpeta BDIR

echo [Compilando pruebas]
make test

echo [Pruebas practica 1]
# Pruebas con los datos del enunciado
echo [Prueba1]
echo -e -n $RED

$BDIR/main_0_generacion_ejemplo1 prueba1.nasm
nasm -f elf32 -o $ODIR/prueba1.o prueba1.nasm
gcc -m32 -o prueba1 $ODIR/prueba1.o $ALFALIB

diff <(./prueba1 <<< $'-9\n') <(echo -n $'-1\n')
diff <(./prueba1 <<< $'10\n') <(echo -n $'18\n')
echo -e -n $NC

echo [Prueba2]
echo -e -n $RED

$BDIR/main_0_generacion_ejemplo2 prueba2.nasm
nasm -f elf32 -o $ODIR/prueba2.o prueba2.nasm
gcc -m32 -o prueba2 $ODIR/prueba2.o $ALFALIB

diff <(./prueba2 <<< $'0\n') <(echo -n $'true\nfalse\n')
diff <(./prueba2 <<< $'1\n') <(echo -n $'false\ntrue\n')
echo -e -n $NC

echo [Prueba3]
echo -e -n $RED

$BDIR/main_0_generacion_ejemplo3 prueba3.nasm
nasm -f elf32 -o $ODIR/prueba3.o prueba3.nasm
gcc -m32 -o prueba3 $ODIR/prueba3.o $ALFALIB

diff <(./prueba3 <<< $'-3\n3\n0\n') <(echo -n $'true\n3\n0\n3\n')
diff <(./prueba3 <<< $'10\n-9\n1\n') <(echo -n $'false\n-10\n1\n-9\n')
echo -e -n $NC

echo [Pruebas practica 2]
echo -e -n $RED

diff <($BDIR/main_1_simbolos $MDIR/entrada_simbolos.txt) <(cat $MDIR/salida_simbolos.txt)

echo -e -n $NC

echo [Borrando ficheros generados]
make clean
rm -f prueba1 prueba2 prueba3
rm -f $ODIR/prueba1.o $ODIR/prueba2.o $ODIR/prueba3.o
rm -f prueba1.nasm prueba2.nasm prueba3.nasm
