#!/usr/bin/env bash
set -eu

die() { >&2 echo "$@"; exit 1; }

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

# Nos aseguramos de que exista el generador y generamos el ejecutable
make test
./main_0_generacion_ejemplo1 prueba1.nasm
./main_0_generacion_ejemplo2 prueba2.nasm
./main_0_generacion_ejemplo3 prueba3.nasm
nasm -f elf32 -o prueba1.o prueba1.nasm
nasm -f elf32 -o prueba2.o prueba2.nasm
nasm -f elf32 -o prueba3.o prueba3.nasm
gcc -m32 -o prueba1 prueba1.o alfalib.o
gcc -m32 -o prueba2 prueba2.o alfalib.o
gcc -m32 -o prueba3 prueba3.o alfalib.o

# Pruebas con los datos del enunciado
echo prueba1
diff <(./prueba1 <<< $'-9\n') <(echo -n $'-1\n')
diff <(./prueba1 <<< $'10\n') <(echo -n $'18\n')

echo prueba2
diff <(./prueba2 <<< $'0\n') <(echo -n $'true\nfalse\n')
diff <(./prueba2 <<< $'1\n') <(echo -n $'false\ntrue\n')

echo prueba3
diff <(./prueba3 <<< $'-3\n3\n0\n') <(echo -n $'true\n3\n0\n3\n')
diff <(./prueba3 <<< $'10\n-9\n1\n') <(echo -n $'false\n-10\n1\n-9\n')

