#!/usr/bin/env bash

set -eu

num=$(ls misc/compilador_pruebas/ | tail -1)
num=${num%%-*}

(( num += 1 ))

my_touch() {
	filename="$(printf "misc/compilador_pruebas/%02g-$1" $num)"
	echo "Touching $filename"
	touch "$filename"
}

#my_touch $1.in
my_touch $1.err
#my_touch $1.out
my_touch $1.alf