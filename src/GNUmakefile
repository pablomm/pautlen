.DEFAULT_GOAL = all
.PHONY: all debug test clean help

## Configuracion de las herramientas
CC         := gcc
CFLAGS     += -std=c99 -pedantic -Wall -Wextra

## Definiciones de objetivos
TESTS := main_0_generacion main_0_generacion_ejemplo1 main_0_generacion_ejemplo2 main_0_generacion_ejemplo3
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

## Dependencias de los ejecutables
$(EXES): generacion.o

$(TESTS): generacion.o

## Deteccion automatica de dependencias (_solo_ entre ficheros .c y .h)
.depend:
	$(CC) $(CFLAGS) -E -MM $(wildcard *.c) > .depend
ifneq ($(MAKECMDGOALS),clean)
include .depend
endif

all: $(EXES)
all: CFLAGS += -O3 -DNDEBUG

#Requisito entrega practica 1
all:
	$(CC) $(CFLAGS) -o generacion.o -c generacion.c

test: $(TESTS)
test: CFLAGS += -O3 -DNDEBUG


debug: $(EXES)
debug: CFLAGS += -g

clean:
	$(RM) $(EXES) $(TESTS) $(OBJS) .depend

help:
	@echo "Posibles comandos:"
	@echo "    all      - construye todos los ejecutables"
	@echo "    test     - genera ejecutables para las pruebas"
	@echo "    debug    - compila todo usando con simbolos de depuracion"
	@echo "    clean    - borra todos los ficheros generados"
	@echo "    help     - muestra esta ayuda"
