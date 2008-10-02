/*
 * Copyright 2004 Tyan Computer
 *  by yhlu@tyan.com
 */

	.section ".romstrap", "a", @progbits


	.globl __romstrap_start
__romstrap_start:
rstables:
	.long 0x2b16d065
	.long 0x0
	.long 0x0
	.long linkedlist

linkedlist:
	.long 0x0003001C			// 10h
	.long 0x08000000			// 14h
	.long 0x00000000			// 18h
	.long 0xFFFFFFFF			// 1Ch

	.long 0xFFFFFFFF			// 20h
	.long 0xFFFFFFFF			// 24h
	.long 0xFFFFFFFF			// 28h
	.long 0xFFFFFFFF			// 2Ch

	.long 0x81543266			// 30h, MAC address low 4 byte ---> keep it in 0xffffffd0
	.long 0x000000E0			// 34h, MAC address high 4 byte

	.long 0x002309CE			// 38h, UUID low 4 byte
	.long 0x00E08100			// 3Ch, UUID high 4 byte

rspointers:
	.long rstables				// It will be 0xffffffe0
	.long rstables
	.long rstables
	.long rstables

	.globl __romstrap_end

__romstrap_end:
.previous
