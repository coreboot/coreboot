SOURCE_DIR=../../src/mainboard/pcengines/apu2

CC=gcc -Wall -pedantic --std=c99 -Dbool=_Bool -Iinc -I${SOURCE_DIR}
SRC_C=${SOURCE_DIR}/bios_knobs.c
SRC_H=${SOURCE_DIR}/bios_knobs.h

all: header test
	./test

header:
	@echo "CC           =   ${CC}"
	@echo "SOURCE_DIR   =   ${SOURCE_DIR}"
	@echo

cases.c: ${SRC_H} cases.awk
	awk -f cases.awk ${SRC_H} > cases.c

test: test.c ${SRC_C} cases.c
	${CC} test.c ${SRC_C} -o test
 
 clean:
	rm test cases.c

.PHONY: all header
