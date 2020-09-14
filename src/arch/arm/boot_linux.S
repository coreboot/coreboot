/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/asm.h>

/* Required to jump to kernel in ARM state */
.arm
/* void boot_linux(void *kernel_ptr, void *fdt_ptr); */
ENTRY(boot_linux)
	/* Save kernel ptr */
	mov r3, r0
	/* Set R2 = fdt */
	mov r2, r1
	/* Set R0 = 0x00000000 as expected by Linux ABI */
	mov r0, #0
	/* Set R1 = 0xffffffff as expected by Linux ABI */
	mov r1, #-1
	/* Linux ABI expects SVC mode (0x13) with IRQ(7) and FIQ(6) disabled. */
	msr cpsr_cxf, #0xd3
	/* Jump to kernel */
	mov pc, r3
ENDPROC(boot_linux)
