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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
// See LICENSE for license details. relating to the _start code in this file.
#include <arch/encoding.h>

.section ".start", "ax", %progbits
// Maybe there's a better way.
.space 0x2000
.globl _start
_start:

	// pending figuring out this f-ing toolchain. Hardcode what we know works.
  la sp, 0x4ef0 // .stacktop
//  la a0, trap_entry
//  la gp, _gp
//  csrw evec, a0

  # clear any pending interrupts
  csrwi clear_ipi, 0

  li a0, SR_S | SR_PS | SR_EI | SR_S64 | SR_U64
  or a1, a0, SR_EF | SR_EA
  csrw status, a1
  csrr a1, status
  csrw status, a0

//  and a2, a1, SR_EF
//  sw a2, have_fp, t0

//  and   a2, a1, SR_EA
//  sw a2, have_accelerator, t0
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

.globl _cbfs_master_header
_cbfs_master_header:
	.balignl 16,0xdeadbeef
	.align 8
	// this assembler SUCKS
	.long  0x4F524243
	.long 0xdeadbeef
	.long 0xdeadbeef
	.long 0xdeadbeef
	.long 0xdeadbeef
	.long 0xdeadbeef
	.long 0xdeadbeef
	/* The CBFS master header is inserted by cbfstool at the first
	 * aligned offset after the above anchor string is found.
	 * Hence, we leave some space for it.
	 * Assumes 64-byte alignment.
	 */
	.space 128

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
