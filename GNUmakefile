.DEFAULT_GOAL = all
.PHONY: all test debug clean zip help

##########################################################
#                                                        #
#  Makefile Proyecto de automatas y lenguajes            #
#  23-oct-2017                                           #
#                                                        #
# Carpetas del proyecto (definidas en las variables):    #
#                                                        #
#  BDIR - Carpeta donde se crearan los ejecutables       #
#         tanto del objetivo all como de test            #
#                                                        #
#  SDIR - Carpeta con los ficheros fuente con el         #
#         codigo de los modulos                          #
#                                                        #
#  IDIR - Carpeta con todas las cabeceras de los         #
#         modulos                                        #
#                                                        #
#  TDIR - Carpeta con todos los mains con tests          #
#         Por como esta hecho el makefile no puede ser   #
#         la misma carpeta que la de los ficheros        #
#         fuente (SDIR). Los binarios de esta carpeta    #
#         se compilaran al hacer make test               #
#                                                        #
#  ODIR - Carpeta con todos los objetos .o compilados    #
#         y los .o adicionales (alfalib.o)               #
#                                                        #
#  EDIR - Carpeta con los mains que se van a generar     #
#         al hacer make all, por comodidad se            #
#         permite que esta carpeta sea la misma que      #
#         la de los ficheros fuente (SDIR)               #
#                                                        #
#  MDIR - Carpeta con otros ficheros. Ej: entrada para   #
#         las pruebas. (Carpeta no utilizada en el       #
#         makefile, en el proyecto es misc)              #
#                                                        #
#  Tan solo hay que definir las variables de las         #
#  carpetas y la variable EXES con los mains que         #
#  generaran los binarios al hacer make all. El resto    #
#  lo hace solo el makefile. Tiene en cuenta si un       #
#  .c esta actualizado para no recompilarlo.             #
#  Para definir alguna de las carpetas como el           #
#  directorio raiz bastara darle el valor ' . '          #
#  Make clean elimina exclusivamente los ficheros        #
#  generados por el propio makefile.                     #
#  Make zip utiliza la utilidad git archive para añadir  #
#  a un zip la version en el commit local del branch     #
#  activo mas reciente (HEAD). En la carpeta raiz del    #
#  proyecto debe estar ubicado el repositorio. junto     #
#  al makefile.                                          #
#                                                        #
##########################################################

## Definiciones de carpetas
BDIR := .
SDIR := src
IDIR := include
TDIR := test
ODIR := obj
EDIR := src

## Nombre fichero compresion
ZIP := ../BlancManuel_MarcosPablo_TABLA.zip

## Configuracion de las herramientas
CC       := gcc
CFLAGS   := -std=c99 -Iinclude -pedantic -Wall -Wextra
LDFLAGS  :=
RM	     := rm -f

## Mains objetivos de make all
EXES := prueba_tabla.c

EXES := $(patsubst %,$(EDIR)/%,$(EXES))
EOBJ := $(patsubst $(EDIR)/%.c,$(ODIR)/%.o,$(EXES))
EBIN := $(patsubst $(EDIR)/%.c,$(BDIR)/%,$(EXES))

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
