/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Early initialization code for aarch64 (a.k.a. armv8)
 */

#include <arch/asm.h>
#include <mainboard/addressmap.h>

ENTRY(_start)
	/* Get core ID */
	mrs	x0, MPIDR_EL1
	and	x1, x0, # 0xfffff // Aff2:Aff1:Aff0

	/* APs wait for instructions from BL31 */
	cbnz	x1, _wait_for_bl31

	/* Setup CPU. */
	bl	arm64_init_cpu

	/* Get code positions. */
	ldr	x1, =_flash
	ldr	x0, =_bootblock

	/* Calculate bootblock size. */
	ldr	x2, =_ebootblock
	sub	x2, x2, x0

	/* Call memcpy in arch/arm64/memcpy.S */
	bl	memcpy
	dmb	sy

	/* Calculate relocation offset between bootblock in flash and in DRAM. */
	ldr	x0, =_flash
	ldr	x1, =_bootblock
	sub	x1, x1, x0

	/* Jump to main() in DRAM. */
	adr	x0, main
	add	x0, x0, x1
	blr	x0
ENDPROC(_start)

#define HOLD_BOOT_MAGIC1	0xCAFECAFE
#define HOLD_BOOT_MAGIC2	0xBEEFBEEF
#define HOLD_STATE_WAIT	-1
#define HOLD_SLOT_SIZE	64
#define HOLD_SLOT_ENTRY	0
#define HOLD_SLOT_MAGIC1	8
#define HOLD_SLOT_MAGIC2	16
#define PLATFORM_CPU_PER_CLUSTER_SHIFT	4

/**
 * Secondary cores spin here until BL31 takes control over them.
 * ATF has a per core mailbox at address VIRT_SECRAM_BASE which needs
 * the magic1 and magic2 to be written before the core jumps to the
 * address requested by BL31.
 */
ENTRY(_wait_for_bl31)
	mrs	x1, mpidr_el1
	and	x2, x1, #0xff
	ubfx	x1, x1, #8, #8
	add	x1, x2, x1, lsl #PLATFORM_CPU_PER_CLUSTER_SHIFT
	ldr	x0, =VIRT_SECRAM_BASE
	mov	x2, #HOLD_SLOT_SIZE
	madd	x0, x1, x2, x0
	// Reset slot to defaults. Assume BSP wasn't fast enough to enter BL31 yet.
	mov	x2, #HOLD_STATE_WAIT
	str	x2, [x0, #HOLD_SLOT_ENTRY]
	ldr	x3, =HOLD_BOOT_MAGIC1
	ldr	x4, =HOLD_BOOT_MAGIC2
1:
	ldr	x1, [x0, #HOLD_SLOT_MAGIC1]
	cmp	x1, x3
	b.ne	1b
	ldr	x1, [x0, #HOLD_SLOT_MAGIC2]
	cmp	x1, x4
	b.ne	1b
	dmb	sy
	ldr	x1, [x0, #HOLD_SLOT_ENTRY]
	cmp	x1, #HOLD_STATE_WAIT
	b.eq	1b
	mov	x2, #HOLD_STATE_WAIT
	str	x2, [x0, #HOLD_SLOT_ENTRY]
	dsb	sy
	br	x1
ENDPROC(_wait_for_bl31)
