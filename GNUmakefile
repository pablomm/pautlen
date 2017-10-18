.DEFAULT_GOAL = all
.PHONY: all test debug clean zip help

## Definiciones de carpetas
BDIR := .
SDIR := src
IDIR := include
TDIR := test
ODIR := obj
EDIR := 

## Configuracion de las herramientas
CC       := gcc
CFLAGS   := -std=c99 -Iinclude -pedantic -Wall -Wextra
LDFLAGS  :=
RM	     := rm -f

## Definiciones de objetivos
SRCS := $(wildcard $(SDIR)/*.c)
SOBJ := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRCS))

## Exe para practica 2
EXES := 
EOBJ := $(patsubst $(EDIR)/%.c,$(ODIR)/%.o,$(EXES))
EBIN := $(patsubst $(EDIR)/%.c,$(BDIR)/%,$(EXES))

TEST := $(wildcard $(TDIR)/*.c)
TOBJ := $(patsubst $(TDIR)/%.c,$(ODIR)/%.o,$(TEST))
TBIN := $(patsubst $(TDIR)/%.c,$(BDIR)/%,$(TEST))

## Flags adicionales
all: CFLAGS += -O3 -DNDEBUG
test: CFLAGS += -O3 -DNDEBUG
debug: CFLAGS += -g

## Objetivos
all: $(EBIN)
test: $(TBIN)


## Reglas de compilacion

## Compilacion de .c de src
$(SOBJ):$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

## Compilacion de .c de tests
$(TOBJ):$(ODIR)/%.o: $(TDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

## Compilacion de .c de exes
$(EOBJ):$(ODIR)/%.o: $(EDIR)/%.c
	$(CC) -c -o $@ $< $(CFLAGS) 

## Linkado de tests
$(TBIN):$(BDIR)/%: $(ODIR)/%.o $(SOBJ)
	$(CC) -o $@ $? $(LDFLAGS) $(CFLAGS)

## Linkado de exes
$(EBIN):$(BDIR)/%: $(ODIR)/%.o $(SOBJ)
	$(CC) -o $@ $? $(LDFLAGS)


clean:
	@$(RM) $(SOBJ) $(EOBJ) $(EBIN) $(TOBJ) $(TBIN)

zip: clean
	@git archive

help:
	@echo "Posibles comandos:"
	@echo "    all      - construye todos los ejecutables"
	@echo "    test     - genera ejecutables para las pruebas"
	@echo "    debug    - compila todo usando con simbolos de depuracion"
	@echo "    clean    - borra todos los ficheros generados"
	@echo "    zip      - comprime el la rama local activa del repositorio de git"
	@echo "    help     - muestra esta ayuda"
