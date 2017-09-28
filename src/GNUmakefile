.DEFAULT_GOAL = help
.PHONY: all debug clean clobber help

CC    	:= gcc
CFLAGS	+= -std=c99 -pedantic -Wall -Wextra

EXES := main_0_generacion
OBJS := generacion.o
$(EXES): $(OBJS)

all: $(EXES)
all: CFLAGS += -O3

# Para debugging
debug: $(EXES)
debug: CFLAGS += -g

.depend:
	$(CC) $(CFLAGS) -E -MM $(wildcard *.c) > .depend
ifneq ($(MAKECMDGOALS),clean)
include .depend
endif

clean:
	$(RM) $(EXES) $(OBJS) .depend

help:
	@echo "Posibles comandos:"
	@echo "    all      - construye todos los ejecutables"
	@echo "    debug    - compila todo usando con simbolos de depuracion"
	@echo "    clean    - borra todos los ficheros generados"
	@echo "    help     - muestra esta ayuda"
