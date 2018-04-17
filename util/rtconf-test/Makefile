SOURCE_DIR=../../src/mainboard/pcengines/apu2

CC=gcc -Wall -pedantic --std=c99 -Dbool=_Bool -Iinc -I${SOURCE_DIR} 
DEF=-Dbool=_Bool -DCONFIG_USE_OPTION_TABLE_CBFS
KNOBS_C=${SOURCE_DIR}/bios_knobs.c
KNOBS_H=${SOURCE_DIR}/bios_knobs.h
OPT_H=../../src/include/option.h
OPT_CBFS_H=../../src/commonlib/include/commonlib/option_cbfs.h
OPT_CBFS_C=../../src/commonlib/option_cbfs.c

all: header test
	./test

header:
	@echo "CC           =   ${CC}"
	@echo "DEF          =   ${DEF}"
	@echo "SOURCE_DIR   =   ${SOURCE_DIR}"
	@echo

cases.c: ${KNOBS_H} cases.awk
	awk -f cases.awk ${KNOBS_H} > cases.c

test: test.c ${KNOBS_C} cases.c ${OPT_H} ${OPT_CBFS_C}
	${CC} ${DEF} test.c ${KNOBS_C} ${OPT_CBFS_C} -o test
 
 clean:
	rm test cases.c

.PHONY: all header
