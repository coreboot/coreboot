#define ASM
#include "ppcreg.h"
#include <ppc_asm.tmpl>

#define NUM_CACHE_LINES 128*8
#define L1_CACHE_LINE_SIZE 32
#define cache_flush_buffer 0x1000

/*
 * Flush data cache
 * Do this by just reading lots of stuff into the cache.
 */
.globl flush_dcache
flush_dcache:
	lis     r3,cache_flush_buffer@h
	ori     r3,r3,cache_flush_buffer@l
	li      r4,NUM_CACHE_LINES
	mtctr   r4
0:	lwz     r4,0(r3)
	addi    r3,r3,L1_CACHE_LINE_SIZE
	bdnz    0b
	blr

