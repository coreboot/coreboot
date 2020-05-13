/* SPDX-License-Identifier: GPL-2.0-only */

/* This file is included inside a SECTIONS block */

_STACK_SIZE = CONFIG_EARLYRAM_BSP_STACK_SIZE;
_ = ASSERT(_STACK_SIZE > 0x0, "EARLYRAM_BSP_STACK_SIZE is not configured");

_CONSOLE_SIZE = CONFIG_PRERAM_CBMEM_CONSOLE_SIZE;
_ = ASSERT(_CONSOLE_SIZE > 0x0, "PRERAM_CBMEM_CONSOLE_SIZE is not configured");

_TIMESTAMPS_SIZE = 0x200;
#if !CONFIG(NO_FMAP_CACHE)
_FMAP_SIZE = FMAP_SIZE;
#else
_FMAP_SIZE = 0;
#endif

/*
 * The PRERAM_CBMEM_CONSOLE, TIMESTAMP, and FMAP_CACHE regions are shared
 * between the pre-ram stages (bootblock, romstage, etc). We need to assign a
 * fixed size and consistent link address so they can be shared between stages.
 *
 * The stack area is not shared between stages, but is defined here for
 * convenience.
 */
. = CONFIG_X86_RESET_VECTOR - ARCH_STACK_ALIGN_SIZE - _STACK_SIZE - _CONSOLE_SIZE - _TIMESTAMPS_SIZE - _FMAP_SIZE - VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE;

_ = ASSERT(. > _eprogram, "Not enough room for .earlyram.data. Try increasing C_ENV_BOOTBLOCK_SIZE, or decreasing either EARLYRAM_BSP_STACK_SIZE or PRERAM_CBMEM_CONSOLE_SIZE.");

.stack ALIGN(ARCH_STACK_ALIGN_SIZE) (NOLOAD) : {
	EARLYRAM_STACK(., _STACK_SIZE)
}

.persistent ALIGN(ARCH_POINTER_ALIGN_SIZE) (NOLOAD) : {
	PRERAM_CBMEM_CONSOLE(., _CONSOLE_SIZE)
	TIMESTAMP(., _TIMESTAMPS_SIZE)
	#if !CONFIG(NO_FMAP_CACHE)
		FMAP_CACHE(., FMAP_SIZE)
	#endif

	#if CONFIG(VBOOT_STARTS_IN_BOOTBLOCK)
		ALIGN_COUNTER(16);
		VBOOT2_WORK(., VB2_FIRMWARE_WORKBUF_RECOMMENDED_SIZE)
	#endif
}

_ = ASSERT(. <= CONFIG_X86_RESET_VECTOR, "Earlyram data regions don't fit below the reset vector!");
