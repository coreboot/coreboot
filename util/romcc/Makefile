VERSION:=0.31
RELEASE_DATE:=25 June 2003
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
	raminit_test.c \
	raminit_test2.c \
	raminit_test3.c \
	raminit_test4.c

TEST_SRCS:=$(patsubst %, tests/%, $(TESTS))
TEST_ASM:=$(patsubst %.c, tests/%.S, $(TESTS))
TEST_OBJ:=$(patsubst %.c, tests/%.o, $(TESTS))
TEST_ELF:=$(patsubst %.c, tests/%.elf, $(TESTS))

$(TEST_ASM): %.S: %.c romcc
	export ALLOC_CHECK_=2; ./romcc -O -o $@ $< > $*.debug

$(TEST_OBJ): %.o: %.S
	as $< -o $@

$(TEST_ELF): %.elf: %.o tests/ldscript.ld
	ld -T tests/ldscript.ld $< -o $@

test: $(TEST_ELF)

echo:
	echo "TEST_SRCS=$(TEST_SRCS)"
	echo "TEST_ASM=$(TEST_ASM)"
	echo "TEST_OBJ=$(TEST_OBJ)"
	echo "TEST_ELF=$(TEST_ELF)"

clean:
	rm -f romcc romcc_pg core $(TEST_ASM) $(TEST_OBJ) $(TEST_ELF) tests/*.debug tests/*.debug2 tests/*.gmon.out

