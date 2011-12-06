# Move the configuration defines to makefile.conf
CC=gcc
CPPFLAGS=
CFLAGS= -g -Wall -Werror $(CPPFLAGS)
CPROF_FLAGS=-pg -fprofile-arcs

all: romcc test

romcc: romcc.c Makefile
	$(CC) $(CFLAGS) -o $@ $<

romcc_pg: romcc.c Makefile
	$(CC) $(CFLAGS) $(CPROF_FLAGS) -o $@ $<

LINUX_TESTS=\
	linux_test1.c \
	linux_test2.c \
	linux_test3.c \
	linux_test4.c \
	linux_test5.c \
	linux_test6.c \
	linux_test7.c \
	linux_test8.c \
	linux_test9.c \
	linux_test10.c \
	linux_test11.c \
	linux_test12.c \
	linux_test13.c \

TESTS=\
	hello_world1.c \
	hello_world2.c \
	simple_test1.c \
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
	simple_test23.c \
	simple_test24.c \
	simple_test25.c \
	simple_test26.c \
	simple_test27.c \
	simple_test28.c \
	simple_test29.c \
	simple_test31.c \
	simple_test32.c \
	simple_test33.c \
	simple_test34.c \
	simple_test35.c \
	simple_test36.c \
	simple_test37.c \
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
	simple_test55.c \
	simple_test56.c \
	simple_test57.c \
	simple_test58.c \
	simple_test60.c \
	simple_test61.c \
	simple_test62.c \
	simple_test63.c \
	simple_test64.c \
	simple_test65.c \
	simple_test66.c \
	simple_test67.c \
	simple_test68.c \
	simple_test69.c \
	simple_test70.c \
	simple_test71.c \
	simple_test72.c \
	simple_test73.c \
	simple_test74.c \
	simple_test75.c \
	simple_test76.c \
	simple_test77.c \
	simple_test78.c \
	simple_test79.c \
	simple_test80.c \
	simple_test81.c \
	simple_test82.c \
	simple_test83.c \
	simple_test84.c \
	simple_test85.c \
	simple_test86.c \
	simple_test87.c \
	simple_test88.c \
	simple_test89.c \
	simple_test90.c \
	raminit_test1.c \
	raminit_test2.c \
	raminit_test3.c \
	raminit_test4.c \
	raminit_test5.c \
	raminit_test6.c \
	$(LINUX_TESTS)

FAIL_TESTS = \
	fail_test1.c \
	fail_test2.c \
	fail_test3.c \
	fail_test4.c \
	fail_test5.c \
	fail_test6.c \
	fail_test7.c \
	fail_test8.c \
	fail_test9.c \
	fail_test10.c \
	fail_test11.c \

TEST_SRCS:=$(patsubst %, tests/%, $(TESTS))
TEST_ASM:=$(patsubst %.c, tests/%.S, $(TESTS))
TEST_ASM_O:=$(patsubst %.c, tests/%.S-O, $(TESTS))
TEST_ASM_O2:=$(patsubst %.c, tests/%.S-O2, $(TESTS))
TEST_ASM_mmmx     :=$(patsubst %.c, tests/%.S-mmmx, $(TESTS))
TEST_ASM_msse     :=$(patsubst %.c, tests/%.S-msse, $(TESTS))
TEST_ASM_mmmx_msse:=$(patsubst %.c, tests/%.S-mmmx-msse, $(TESTS))
TEST_ASM_O_mmmx     :=$(patsubst %.c, tests/%.S-O-mmmx, $(TESTS))
TEST_ASM_O_msse     :=$(patsubst %.c, tests/%.S-O-msse, $(TESTS))
TEST_ASM_O_mmmx_msse:=$(patsubst %.c, tests/%.S-O-mmmx-msse, $(TESTS))
TEST_ASM_O2_mmmx     :=$(patsubst %.c, tests/%.S-O2-mmmx, $(TESTS))
TEST_ASM_O2_msse     :=$(patsubst %.c, tests/%.S-O2-msse, $(TESTS))
TEST_ASM_O2_mmmx_msse:=$(patsubst %.c, tests/%.S-O2-mmmx-msse, $(TESTS))
TEST_ASM_O2_mmmx_call     :=$(patsubst %.c, tests/%.S-O2-mmmx-call, $(TESTS))
TEST_ASM_O2_mmmx_msse_call:=$(patsubst %.c, tests/%.S-O2-mmmx-msse-call, $(TESTS))
TEST_ASM_ALL:= $(TEST_ASM) $(TEST_ASM_O) $(TEST_ASM_O2) $(TEST_ASM_mmmx) $(TEST_ASM_msse) $(TEST_ASM_mmmx_msse) $(TEST_ASM_O_mmmx) $(TEST_ASM_O_msse) $(TEST_ASM_O_mmmx_msse) $(TEST_ASM_O2_mmmx) $(TEST_ASM_O2_msse) $(TEST_ASM_O2_mmmx_msse) $(TEST_ASM_O2_mmmx_call) $(TEST_ASM_O2_mmmx_msse_call)
TEST_ASM_MOST:= $(TEST_ASM_O) $(TEST_ASM_O_mmmx) $(TEST_ASM_O_msse) $(TEST_ASM_O_mmmx_msse) $(TEST_ASM_O2) $(TEST_ASM_O2_mmmx) $(TEST_ASM_O2_msse) $(TEST_ASM_O2_mmmx_msse)
# $(TEST_ASM_O2_mmmx_call) $(TEST_ASM_O2_mmmx_msse_call)
TEST_OBJ:=$(patsubst %.c, tests/%.o, $(TESTS))
TEST_ELF:=$(patsubst %.c, tests/%.elf, $(TESTS))
LINUX_ELF:=$(patsubst %.c, tests/%.elf, $(LINUX_TESTS))
LINUX_OUT:=$(patsubst %.c, tests/%.out, $(LINUX_TESTS))

FAIL_SRCS:=$(patsubst %, tests/%, $(FAIL_TESTS))
FAIL_OUT:=$(patsubst %.c, tests/%.out, $(FAIL_TESTS))


ROMCC_OPTS=-fmax-allocation-passes=8 -fdebug-live-range-conflicts -Itests/include

$(TEST_ASM): %.S: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -o $@ $< > $*.debug

$(TEST_ASM_O): %.S-O: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O -o $@ $< > $*.debug

$(TEST_ASM_O2): %.S-O2: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O2 -o $@ $< > $*.debug


$(TEST_ASM_mmmx): %.S-mmmx: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -mmmx -o $@ $< > $*.debug

$(TEST_ASM_msse): %.S-msse: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -msse -o $@ $< > $*.debug

$(TEST_ASM_mmmx_msse): %.S-mmmx-msse: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -mmmx -msse -o $@ $< > $*.debug


$(TEST_ASM_O_mmmx): %.S-O-mmmx: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O  -mmmx -o $@ $< > $*.debug

$(TEST_ASM_O_msse): %.S-O-msse: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O  -msse -o $@ $< > $*.debug

$(TEST_ASM_O_mmmx_msse): %.S-O-mmmx-msse: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O  -mmmx -msse -o $@ $< > $*.debug


$(TEST_ASM_O2_mmmx): %.S-O2-mmmx: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O2  -mmmx -o $@ $< > $*.debug

$(TEST_ASM_O2_msse): %.S-O2-msse: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O2  -msse -o $@ $< > $*.debug

$(TEST_ASM_O2_mmmx_msse): %.S-O2-mmmx-msse: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O2 -mmmx -msse -o $@ $< > $*.debug

$(TEST_ASM_O2_mmmx_call): %.S-O2-mmmx-call: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O2 -mmmx -fno-always-inline -o $@ $< > $*.debug

$(TEST_ASM_O2_mmmx_msse_call): %.S-O2-mmmx-msse-call: %.c romcc
	export ALLOC_CHECK_=2; ./romcc $(ROMCC_OPTS) -O2 -mmmx -msse -fno-always-inline -o $@ $< > $*.debug


$(FAIL_OUT): %.out: %.c romcc
	export ALLOC_CHECK_=2; if ./romcc $(ROMCC_OPTS) -O2  -o $*.S $< > $*.debug 2> $@ ; then exit 1 ; else exit 0 ; fi

#$(TEST_OBJ): %.o: %.S-O2-mmmx-call

$(TEST_OBJ): %.o: %.S-O2-mmmx
	as $< -o $@

$(TEST_ELF): %.elf: %.o tests/ldscript.ld
	ld -T tests/ldscript.ld $< -o $@

$(LINUX_OUT): %.out: %.elf
	./$< > $@

test: $(TEST_ELF) $(FAIL_OUT) $(TEST_ASM_MOST)

run_linux: $(LINUX_OUT)

echo:
	echo "TEST_SRCS=$(TEST_SRCS)"
	echo "TEST_ASM=$(TEST_ASM)"
	echo "TEST_ASM=$(TEST_ASM_MOST)"
	echo "TEST_OBJ=$(TEST_OBJ)"
	echo "TEST_ELF=$(TEST_ELF)"
	echo ""
	echo "FAIL_SRCS=$(FAIL_SRCS)"
	echo "FAIL_ASM=$(FAIL_ASM)"

clean:
	rm -f romcc romcc_pg core $(TEST_ASM_ALL) $(TEST_OBJ) $(TEST_ELF) tests/*.debug tests/*.debug2 tests/*.gmon.out tests/*.out

