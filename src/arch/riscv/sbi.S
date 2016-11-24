/*
 * RISC-V supervisor binary interface (SBI) trampoline page
 *
 * Copyright 2016 Jonathan Neuschäfer <j.neuschaefer@gmx.net>
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
 */

#define __ASSEMBLY__
#include <arch/encoding.h>
#include <arch/sbi.h>

.section ".text.sbi", "ax", %progbits

/* align to a page boundary */
.align RISCV_PGSHIFT

.globl sbi_page
sbi_page:

	/*
	 * None of the SBI entry points is located in the first half of the
	 * page
	 */
	.skip 0x800

	/* -2048: size_t sbi_hart_id(void); */
	li a7, SBI_ECALL_HART_ID
	ecall
	jr ra
	.align 4

	/* -2032: size_t sbi_num_harts(void); */
	li a7, SBI_ECALL_NUM_HARTS
	ecall
	jr ra
	.align 4

	/* -2016: unsigned long sbi_query_memory(unsigned long id,
			memory_block_info *p); */
	li a7, SBI_ECALL_QUERY_MEMORY
	ecall
	jr ra
	.align 4

	/* -2000: int sbi_console_putchar(uint8_t ch); */
	li a7, SBI_ECALL_CONSOLE_PUT
	ecall
	jr ra
	.align 4

	/* -1984: int sbi_console_getchar(void); */
	li a0, -1	/* failure: Coreboot doesn't support console input */
	jr ra
	.align 4

	/* -1968: Not allocated */
	ebreak
	.align 4

	/* -1952: int sbi_send_ipi(size_t hart_id); */
	ebreak
	.align 4

	/* -1936: int bool sbi_clear_ipi(void); */
	ebreak
	.align 4

	/* -1920: unsigned long sbi_timebase(void); */
	li a0, 1000000000 /* I have no idea. */
	jr ra
	.align 4

	/* -1904: void sbi_shutdown(void); */
	li a7, SBI_ECALL_SHUTDOWN
	ecall
	jr ra
	.align 4

	/* -1888: void sbi_set_timer(unsigned long long stime_value); */
	li a7, SBI_ECALL_SET_TIMER
	ecall
	jr ra
	.align 4

	/* -1872: int sbi_mask_interrupt(int which); */
	li a0, 0 # dummy
	jr ra
	.align 4

	/* -1856: int sbi_unmask_interrupt(int which); */
	li a0, 0 # dummy
	jr ra
	.align 4

	/* -1840: void sbi_remote_sfence_vm(const uintptr_t* harts,
			size_t asid); */
	ebreak
	.align 4

	/* -1824: void sbi_remote_sfence_vm_range(const uintptr_t* harts,
			size_t asid, uintptr_t start, uintptr_t size); */
	ebreak
	.align 4

	/* -1808: void sbi_remote_fence_i(const uintptr_t* harts); */
	ebreak
	.align 4

/* Fill the remainder of the page */
.align RISCV_PGSHIFT
