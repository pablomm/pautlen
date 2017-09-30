#!/usr/bin/env bash
set -eu

die() { >&2 echo "$@"; exit 1 }

# Navegamos al directorio del script
cd "$( dirname "${BASH_SOURCE[0]}" )"

# Nos aseguramos de que exista el generador y generamos el ejecutable
make main_0_generacion2
./main_0_generacion2 prueba.nasm
nasm -f elf32 -o prueba.o prueba.nasm
gcc -m32 -o prueba prueba.o alfalib.o

# Pruebas con los datos del enunciado
diff <(./prueba <<< $'-3\n3\n0\n') <(echo $'true\n3\n0\n3')
diff <(./prueba <<< $'10\n-9\n1\n') <(echo $'false\n-10\n1\n-9')

rm -f prueba.nasm prueba.o prueba
