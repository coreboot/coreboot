VERSION:=0.21
RELEASE_DATE:=7 April 2003
PACKAGE:=romcc


# Move the configuration defines to makefile.conf
CC=gcc
CPPFLAGS=-DVERSION='"$(VERSION)"' -DRELEASE_DATE='"$(RELEASE_DATE)"'
CFLAGS=-O -g -Wall $(CPPFLAGS)

all: romcc test

romcc: romcc.c Makefile
	$(CC) $(CFLAGS) -o $@ $<

TESTS=\
	hello_world.c \
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
	raminit_test.c \
	raminit_test2.c

TEST_SRCS:=$(patsubst %, tests/%, $(TESTS))
TEST_ASM:=$(patsubst %.c, tests/%.S, $(TESTS))
TEST_OBJ:=$(patsubst %.c, tests/%.o, $(TESTS))
TEST_ELF:=$(patsubst %.c, tests/%.elf, $(TESTS))

$(TEST_ASM): %.S: %.c romcc
	export ALLOC_CHECK_=2; ./romcc -O $< > $@

$(TEST_OBJ): %.o: %.S
	as $< -o $@

$(TEST_ELF): %.elf: %.o
	ld -Ttext 0x1000 $< -o $@

test: $(TEST_ELF)

echo:
	echo "TEST_SRCS=$(TEST_SRCS)"
	echo "TEST_ASM=$(TEST_ASM)"
	echo "TEST_OBJ=$(TEST_OBJ)"
	echo "TEST_ELF=$(TEST_ELF)"

clean:
	rm -f romcc core $(TEST_ASM) $(TEST_OBJ) $(TEST_ELF)

