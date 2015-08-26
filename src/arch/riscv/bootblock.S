/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 *
 * Copyright 2013 Google Inc.
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
# machine mode handler when in supervisor mode
.space 0x140
supervisor_machine_handler:
  j supervisor_trap_entry

# handler for when
.space 0x7c
.globl machine_handler
machine_handler:
#  call trap_handler
  j trap_entry

.space 0x3c
.globl _start
_start:

	// pending figuring out this f-ing toolchain. Hardcode what we know works.
  la sp, 0x80FFF0 // stack start + stack size

  # make room for HLS and initialize it
  addi sp, sp, -64 // MENTRY_FRAME_SIZE
  csrr a0, mhartid
  call hls_init

  //poison the stack
  la t1, 0x800000
  li t0, 0xdeadbeef
  sd t0, 0(t1)

  la t0, exception_handler
  csrw stvec, t0

  # clear any pending interrupts
  csrwi sip, 0

  # set up the mstatus register for VM
  call mstatus_init
  call main
.=0x2000
    .space 0x800
# sbi interface lives here

# hart_id
.align 5
li a7, 0
ecall
ret

# num_harts
.align 4
li a0, 1
ret

# query_memory
.align 4
li a7, 8
ecall
ret

# console_putchar
.align 4
li a7, 1
ecall
ret

# send_device_request
.align 4
li a7, 2
ecall
ret

# receive_device_response
.align 4
li a7, 3
ecall
ret

# send ipi
.align 4
li a7, 4
ecall
ret

# clear ipi
.align 4
li a7, 5
ecall
ret

# timebase
.align 4
li a0, 10000000 # temporary, we should provide the correct answer
ret

# shutdown
.align 4
li a7, 6
ecall

# set_timer
.align 4
li a7, 7
ecall
ret

# end of SBI trampolines
.=0x4000
.stack:
.align 8
	.space 0xf00
.stacktop:
	.quad 0
.align 3
.stack_size:
	.quad 0xf00
.globl test_trap
exception_handler:
        call trap_handler
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
