VERSION:=0.34
RELEASE_DATE:=4 July 2003
PACKAGE:=romcc


# Move the configuration defines to makefile.conf
CC=gcc
CPPFLAGS=-DVERSION='"$(VERSION)"' -DRELEASE_DATE='"$(RELEASE_DATE)"'
CFLAGS= -g -Wall $(CPPFLAGS)
CPROF_FLAGS=-pg -fprofile-arcs

all: romcc test

romcc: romcc.c Makefile
	$(CC) $(CFLAGS) -o $@ $<

romcc_pg: romcc.c Makefile
	$(CC) $(CFLAGS) $(CPROF_FLAGS) -o $@ $<

TESTS=\
	hello_world.c \
	hello_world2.c \
	simple_test.c \
	simple_test2.c \
	simple_test3.c \
	simple_test4.c \
	simple_test5.c \
	simple_test6.c \
	simple_test7.c \
	simple_test8.c \
	simple_test9.c \
	simple_test10.c \
	simple_test11.c \
	simple_test12.c \
	simple_test13.c \
	simple_test14.c \
	simple_test15.c \
	simple_test16.c \
	simple_test17.c \
	simple_test18.c \
	simple_test19.c \
	simple_test20.c \
	simple_test21.c \
	simple_test22.c \
	simple_test23.c \
	simple_test24.c \
	simple_test25.c \
	simple_test26.c \
	simple_test27.c \
	simple_test28.c \
	simple_test29.c \
	simple_test30.c \
	simple_test31.c \
	simple_test32.c \
	simple_test33.c \
	simple_test34.c \
	simple_test35.c \
	simple_test36.c \
	simple_test37.c \
	simple_test38.c \
	simple_test39.c \
	simple_test40.c \
	simple_test41.c \
	simple_test43.c \
	simple_test45.c \
	simple_test46.c \
	simple_test47.c \
	simple_test48.c \
	simple_test49.c \
	simple_test50.c \
	simple_test51.c \
	simple_test52.c \
	simple_test53.c \
	simple_test54.c \
	simple_test55.c \
	simple_test56.c \
	simple_test59.c \
	raminit_test.c \
	raminit_test2.c \
	raminit_test3.c \
	raminit_test4.c \
	raminit_test5.c

FAIL_TESTS = \
	fail_test1.c \
	fail_test2.c \
	fail_test3.c

TEST_SRCS:=$(patsubst %, tests/%, $(TESTS))
TEST_ASM:=$(patsubst %.c, tests/%.S, $(TESTS))
TEST_OBJ:=$(patsubst %.c, tests/%.o, $(TESTS))
TEST_ELF:=$(patsubst %.c, tests/%.elf, $(TESTS))

FAIL_SRCS:=$(patsubst %, tests/%, $(FAIL_TESTS))
FAIL_OUT:=$(patsubst %.c, tests/%.out, $(FAIL_TESTS))


$(TEST_ASM): %.S: %.c romcc
	export ALLOC_CHECK_=2; ./romcc -O -mcpu=k8 -o $@ $< > $*.debug

$(FAIL_OUT): %.out: %.c romcc
	export ALLOC_CHECK_=2; if ./romcc -O -o $*.S $< > $*.debug 2> $@ ; then exit 1 ; else exit 0 ; fi

$(TEST_OBJ): %.o: %.S
	as $< -o $@

$(TEST_ELF): %.elf: %.o tests/ldscript.ld
	ld -T tests/ldscript.ld $< -o $@

test: $(TEST_ELF) $(FAIL_OUT)

echo:
	echo "TEST_SRCS=$(TEST_SRCS)"
	echo "TEST_ASM=$(TEST_ASM)"
	echo "TEST_OBJ=$(TEST_OBJ)"
	echo "TEST_ELF=$(TEST_ELF)"
	echo ""
	echo "FAIL_SRCS=$(FAIL_SRCS)"
	echo "FAIL_ASM=$(FAIL_ASM)"

clean:
	rm -f romcc romcc_pg core $(TEST_ASM) $(TEST_OBJ) $(TEST_ELF) tests/*.debug tests/*.debug2 tests/*.gmon.out tests/*.out

