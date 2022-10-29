## SPDX-License-Identifier: GPL-2.0-only
all:
	afl-gcc -g -m32 -I ../../src/lib -o jpeg-test jpeg-test.c ../../src/lib/jpeg.c

run:
	afl-fuzz -i jpeg-test-cases -o jpeg-results ./jpeg-test @@
