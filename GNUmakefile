.DEFAULT_GOAL = all
.PHONY: all test debug clean zip help graph nasm astyle pruebas
.SUFFIXES:

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
MDIR := misc
NDIR := .

# Fichero con configuracion de astyle
# Respeta la variable de entorno en caso de estar definida
ARTISTIC_STYLE_OPTIONS ?= .astylerc

# Script para ejecutar pruebas
PRUEBAS_SCRIPT := pruebas.bash

## Nombre fichero compresion
ZIP := ../BlancManuel_MarcosPablo_alfa.zip

## Configuracion de las herramientas
CC       ?= gcc
LEX      ?= flex
BISON    ?= bison
CFLAGS   := -std=c99 -Iinclude -pedantic -Wall -Wextra
LDFLAGS  :=
LFLAGS   :=
BFLAGS   := -d -y -v -g
RM       := rm -fv
NASM     := nasm
NFLAGS   := -f elf32
CCNASMFLAGS := -m32

ALFALIB     := $(ODIR)/alfalib.o

## Mains objetivos de make all
EXES := alfa.c

EXES := $(patsubst %,$(EDIR)/%,$(EXES))
EOBJ := $(patsubst $(EDIR)/%.c,$(ODIR)/%.o,$(EXES))
EBIN := $(patsubst $(EDIR)/%.c,$(BDIR)/%,$(EXES))

DEPEND_FILES := $(wildcard $(ODIR)/*.d)

## Definiciones de objetivos
FLEX_SOURCES := $(wildcard $(SDIR)/*.l)
FLEX_GENERATED_FILES := $(FLEX_SOURCES:.l=.yy.c)
FLEX_OBJ := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(FLEX_GENERATED_FILES))

## Definiciones de objetivos
BISON_SOURCES := $(wildcard $(SDIR)/*.y)
BISON_GENERATED_FILES := $(BISON_SOURCES:.y=.tab.c)
BISON_OBJ := $(patsubst $(SDIR)/%.c, $(ODIR)/%.o, $(BISON_GENERATED_FILES))
BISON_HEADERS_ORIG := $(patsubst %.c,%.h, $(BISON_GENERATED_FILES))
BISON_HEADERS := $(patsubst $(SDIR)/%,$(IDIR)/%, $(BISON_HEADERS_ORIG))
BISON_OUTPUT_ORIG := $(patsubst %.tab.c,%.output, $(BISON_GENERATED_FILES))
BISON_OUTPUT := $(patsubst $(SDIR)/%,$(MDIR)/%, $(BISON_OUTPUT_ORIG))

BISON_GRAPH_ORIG := $(patsubst %.tab.c,%.dot, $(BISON_GENERATED_FILES))
BISON_GRAPH := $(patsubst $(SDIR)/%,$(MDIR)/%, $(BISON_GRAPH_ORIG))
DOT := dot
DOTFLAGS := -O -Tpdf

SRCS := $(filter-out $(EXES) $(FLEX_GENERATED_FILES) $(BISON_GENERATED_FILES), $(wildcard $(SDIR)/*.c))
SOBJ := $(patsubst $(SDIR)/%.c,$(ODIR)/%.o,$(SRCS) $(FLEX_GENERATED_FILES) $(BISON_GENERATED_FILES))

TEST := $(wildcard $(TDIR)/*.c)
TOBJ := $(patsubst $(TDIR)/%.c,$(ODIR)/%.o,$(TEST))
TBIN := $(patsubst $(TDIR)/%.c,$(BDIR)/%,$(TEST))


NASM_SOURCES := $(wildcard $(NDIR)/*.nasm)
NASM_OBJ := $(patsubst $(NDIR)/%.nasm, $(ODIR)/%.o, $(NASM_SOURCES))
NASM_BIN := $(patsubst %.nasm, %, $(NASM_SOURCES))


# Flags de compilacion extras para ficheros generados por flex
$(FLEX_OBJ): CFLAGS += -Wno-sign-compare -D_XOPEN_SOURCE=700

## Flags adicionales
all: CFLAGS += -O3 -DNDEBUG
test: CFLAGS += -g
debug: CFLAGS += -g

## Objetivos
all: $(EBIN)
test: $(TBIN)

nasm: $(NASM_BIN)

debug: $(EBIN)

## Deteccion automatica de dependencias (_solo_ entre ficheros .c y .h)
# La siguiente regla le dice a make como generar el fichero .depend
# Solo ejecuta el cpp sobre los fuentes, y acumula las dependencias
# (flag -MM) en un formato que puede procesar make
#CFLAGS += -MMD
# Lo incluimos en este make solo si no estamos haciendo un clean
# (para no crearlo y destruirlo inmediatamente)
# y lo prefijamos con un - para que no falle aunque no se pueda incluir
# (como la primera vez que se ejecuta este makefile)
#ifneq ($(MAKECMDGOALS),clean)
#-include $(patsubst $(wildcard $(SDIR)/*.c,$(ODIR)/%.o),:.c=.d)
#endif

## Reglas de compilacion

## Compilacion de .c de src
$(SOBJ):$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

## Generacion de .yy.c a partir de .l
## Debe estar generada la cabecera de bison
$(SDIR)/%.yy.c: $(SDIR)/%.l $(BISON_GENERATED_FILES)
	$(LEX) $(LFLAGS) -o $@ $<

## Generacion de .tab.c a partir de .y
# Permitimos que falle el mover el grafico ya que no
# es imprescindible
$(SDIR)/%.tab.c: $(SDIR)/%.y
	$(BISON) $(BFLAGS) -o $@ $<
	mv $(BISON_HEADERS_ORIG) $(IDIR)
	mv $(BISON_OUTPUT_ORIG) $(MDIR)
	-mv $(BISON_GRAPH_ORIG) $(MDIR)


## Compilacion de .c de tests
$(TOBJ):$(ODIR)/%.o: $(TDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $<

## Compilacion de .c de exes
$(EOBJ):$(ODIR)/%.o: $(EDIR)/%.c
	$(CC) $(CFLAGS) -o $@ -c $< 

## Linkado de tests
$(TBIN):$(BDIR)/%: $(ODIR)/%.o $(SOBJ)
	$(CC) -o $@ $^ $(LDFLAGS) $(CFLAGS)

## Linkado de exes
$(EBIN):$(BDIR)/%: $(ODIR)/%.o $(SOBJ)
	$(CC) -o $@ $^ $(LDFLAGS)


## Compilacion de .nasm
$(NASM_OBJ):$(ODIR)/%.o: $(NDIR)/%.nasm
	$(NASM) $(NFLAGS) -o $@ $<

## Linkado de los nasm junto con alfalib
$(NASM_BIN): $(BDIR)/%: $(ODIR)/%.o $(ALFALIB)
	$(CC) $(CCNASMFLAGS) -o $@ $^ $(CFLAGS)

clean:
	@$(RM) $(SOBJ) $(EOBJ) $(EBIN) $(TOBJ) $(TBIN) $(DEPEND_FILES)
	@$(RM) $(FLEX_GENERATED_FILES) $(BISON_GENERATED_FILES)
	@$(RM) $(BISON_HEADERS) $(BISON_HEADERS_ORIG) $(BISON_OUTPUT) $(BISON_OUTPUT_ORIG)
	@$(RM) $(BISON_GRAPH_ORIG) $(BISON_GRAPH)
	@$(RM) $(NASM_OBJ) $(NASM_BIN)

zip:
	git archive --format zip -o $(ZIP) HEAD

help:
	@echo "Posibles comandos:"
	@echo "    all      - construye el/los ejecutable $(EBIN)"
	@echo "    test     - genera los ejecutables para las pruebas situados en $(TDIR)/"
	@echo "    debug    - compila todo usando con simbolos de depuracion"
	@echo "    clean    - borra todos los ficheros generados"
	@echo "    pruebas  - ejecuta el script de pruebas $(PRUEBAS_SCRIPT)"
	@echo "    zip      - comprime la rama activa del repositorio"
	@echo "    astyle   - estiliza el codigo acorde al fichero fichero $(ARTISTIC_STYLE_OPTIONS)"
	@echo "    graph    - genera un diagrama utilizando $(DOT) a partir de la salida de bison"
	@echo "    nasm     - compila los nasm junto con alfalib situados en la carpeta $(NDIR)/"
	@echo "    help     - muestra esta ayuda"

astyle:
	astyle --options=$(ARTISTIC_STYLE_OPTIONS) $(IDIR)/*.h $(SDIR)/*.c $(TDIR)/*.c

graph: $(BISON_GENERATED_FILES)
	$(DOT) $(DOTFLAGS) $(BISON_GRAPH)

pruebas: $(PRUEBAS_SCRIPT)
	bash $(PRUEBAS_SCRIPT)

## Deteccion de dependencias automatica, v2
CFLAGS += -MMD
-include $(DEPEND_FILES)
