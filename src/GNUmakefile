.DEFAULT_GOAL = all
.PHONY: all debug clean help

## Configuracion de las herramientas
CC         := gcc
CFLAGS     += -m32 -std=c99 -pedantic -Wall -Wextra
LDFLAGS    += -m32
NAS        := nasm
NASFLAGS   := -f elf32

## Definiciones de objetivos
EXES := main_0_generacion main_0_generacion2
SRCS := $(wildcard *.c)
OBJS := $(SRCS:.c=.o)

## Dependencias de los ejecutables
$(EXES): generacion.o

## Deteccion automatica de dependencias (_solo_ entre ficheros .c y .h)
.depend:
	$(CC) $(CFLAGS) -E -MM $(wildcard *.c) > .depend
ifneq ($(MAKECMDGOALS),clean)
include .depend
endif

# Regla implicita extra para poder compilar los .nasm
%.o: %.nasm
	$(NAS) $(NASFLAGS) -o $@ $^

all: $(EXES)
all: CFLAGS += -O3

debug: $(EXES)
debug: CFLAGS += -g

clean:
	$(RM) $(EXES) $(OBJS) .depend

help:
	@echo "Posibles comandos:"
	@echo "    all      - construye todos los ejecutables"
	@echo "    debug    - compila todo usando con simbolos de depuracion"
	@echo "    clean    - borra todos los ficheros generados"
	@echo "    help     - muestra esta ayuda"
