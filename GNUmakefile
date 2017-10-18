.DEFAULT_GOAL = all
.PHONY: all test debug clean zip help

## Definiciones de carpetas
BDIR := .
SDIR := src
IDIR := include
TDIR := test
ODIR := obj
EDIR := src

## Nombre compresion
ZIP := ../BiancManuel_MarcosPablo_TABLA.zip

## Configuracion de las herramientas
CC       := gcc
CFLAGS   := -std=c99 -Iinclude -pedantic -Wall -Wextra
LDFLAGS  :=
RM	     := rm -f

## Exe para practica 2
EXES := $(EDIR)/prueba_tabla.c
EOBJ := $(patsubst $(EDIR)/%.c,$(ODIR)/%.o,$(EXES))
EBIN := $(patsubst $(EDIR)/%.c,$(BDIR)/%,$(EXES))

## Nota: Por comodidad permitimos que los exes con main
##       esten en la carpeta src tambien

## Definiciones de objetivos
SRCS := $(filter-out $(EXES), $(wildcard $(SDIR)/*.c))
SOBJ := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRCS))

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
debug: $(EBIN)

## Reglas de compilacion

## Compilacion de .c de src
$(SOBJ):$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

## Compilacion de .c de tests
$(TOBJ):$(ODIR)/%.o: $(TDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

## Compilacion de .c de exes
$(EOBJ):$(ODIR)/%.o: $(EDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

## Linkado de tests
$(TBIN):$(BDIR)/%: $(ODIR)/%.o $(SOBJ)
	$(CC) -o $@ $? $(LDFLAGS) $(CFLAGS)

## Linkado de exes
$(EBIN):$(BDIR)/%: $(ODIR)/%.o $(SOBJ)
	$(CC) -o $@ $? $(LDFLAGS)


clean:
	@$(RM) $(SOBJ) $(EOBJ) $(EBIN) $(TOBJ) $(TBIN)

zip: clean
	@git archive --format zip -o $(ZIP) HEAD

help:
	@echo "Posibles comandos:"
	@echo "    all      - construye todos los ejecutables"
	@echo "    test     - genera ejecutables para las pruebas"
	@echo "    debug    - compila todo usando con simbolos de depuracion"
	@echo "    clean    - borra todos los ficheros generados"
	@echo "    zip      - comprime la rama activa del repositorio"
	@echo "    help     - muestra esta ayuda"
