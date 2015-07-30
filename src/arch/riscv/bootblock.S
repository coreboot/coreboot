/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2013Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */
// See LICENSE for license details. relating to the _start code in this file.
#include <arch/encoding.h>

.section ".text._start", "ax", %progbits
// Maybe there's a better way.
.space 0x200
.globl _start
_start:

	// pending figuring out this f-ing toolchain. Hardcode what we know works.
//  la sp, 0x4ef0 // .stacktop
//  la sp, 0x40000 // from src/mainboard/emulation/qemu-riscv
  la sp, 0x7FF00 // stack start + stack size

  // make room for HLS
  addi sp, sp, -64 // MENTRY_FRAME_SIZE


  //poison the stack
  la t1, 0x40000
  li t0, 0xdeadbeef
  sd t0, 0(t1)

//  la gp, _gp

  # clear any pending interrupts
#if __GNUC__ < 5
  csrwi clear_ipi, 0
#else
  csrwi sip, 0
#endif

  call main
.=0x4000
.stack:
.align 8
	.space 0xf00
.stacktop:
	.quad 0
.align 3
.stack_size:
	.quad 0xf00

reset:
init_stack_loop:

	.word CONFIG_STACK_SIZE
	.section ".id", "a", %progbits

	.section ".id", "a", @progbits

	.globl __id_start
	// fix this bs later. What's wrong with the riscv gcc?
__id_start:
ver:
	.asciz "1" //COREBOOT_VERSION
vendor:
	.asciz "ucb" //CONFIG_MAINBOARD_VENDOR
part:
	.asciz "1" //CONFIG_MAINBOARD_PART_NUMBER
.long __id_end + CONFIG_ID_SECTION_OFFSET - ver  /* Reverse offset to the vendor id */
.long __id_end + CONFIG_ID_SECTION_OFFSET - vendor  /* Reverse offset to the vendor id */
.long __id_end + CONFIG_ID_SECTION_OFFSET - part    /* Reverse offset to the part number */
.long CONFIG_ROM_SIZE                               /* Size of this romimage */
	.globl __id_end

__id_end:
.previous
