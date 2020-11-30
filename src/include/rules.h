/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _RULES_H
#define _RULES_H

#if defined(__TEST__)
#define ENV_TEST 1
#else
#define ENV_TEST 0
#endif

#if defined(__TIMELESS__)
#define ENV_TIMELESS 1
#else
#define ENV_TIMELESS 0
#endif

/* Useful helpers to tell whether the code is executing in bootblock,
 * romstage, ramstage or SMM.
 */

#if defined(__DECOMPRESSOR__)
#define ENV_DECOMPRESSOR 1
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "decompressor"

#elif defined(__BOOTBLOCK__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 1
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "bootblock"

#elif defined(__ROMSTAGE__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 1
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "romstage"

#elif defined(__SMM__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 1
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "smm"

/*
 * NOTE: "verstage" code may either run as a separate stage or linked into the
 * bootblock/romstage, depending on the setting of the VBOOT_SEPARATE_VERSTAGE
 * kconfig option. The ENV_SEPARATE_VERSTAGE macro will only return true for
 * "verstage" code when CONFIG(VBOOT_SEPARATE_VERSTAGE) is true, otherwise that
 * code will have ENV_BOOTBLOCK or ENV_ROMSTAGE set (depending on the
 * "VBOOT_STARTS_IN_"... kconfig options).
 */
#elif defined(__VERSTAGE__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 1
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#if CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)
#define ENV_STRING "verstage-before-bootblock"
#else
#define ENV_STRING "verstage"
#endif

#elif defined(__RAMSTAGE__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 1
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "ramstage"

#elif defined(__RMODULE__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 1
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "rmodule"

#elif defined(__POSTCAR__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 1
#define ENV_LIBAGESA 0
#define ENV_STRING "postcar"

#elif defined(__LIBAGESA__)
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 1
#define ENV_STRING "libagesa"

#else
/*
 * Default case of nothing set for random blob generation using
 * create_class_compiler that isn't bound to a stage.
 */
#define ENV_DECOMPRESSOR 0
#define ENV_BOOTBLOCK 0
#define ENV_ROMSTAGE 0
#define ENV_RAMSTAGE 0
#define ENV_SMM 0
#define ENV_SEPARATE_VERSTAGE 0
#define ENV_RMODULE 0
#define ENV_POSTCAR 0
#define ENV_LIBAGESA 0
#define ENV_STRING "UNKNOWN"
#endif

/* Define helpers about the current architecture, based on toolchain.inc. */

#if defined(__ARCH_arm__)
#define ENV_ARM 1
#define ENV_ARM64 0
#if __COREBOOT_ARM_ARCH__ == 4
#define ENV_ARMV4 1
#define ENV_ARMV7 0
#elif __COREBOOT_ARM_ARCH__ == 7
#define ENV_ARMV4 0
#define ENV_ARMV7 1
#if defined(__COREBOOT_ARM_V7_A__)
#define ENV_ARMV7_A 1
#define ENV_ARMV7_M 0
#define ENV_ARMV7_R 0
#elif defined(__COREBOOT_ARM_V7_M__)
#define ENV_ARMV7_A 0
#define ENV_ARMV7_M 1
#define ENV_ARMV7_R 0
#elif defined(__COREBOOT_ARM_V7_R__)
#define ENV_ARMV7_A 0
#define ENV_ARMV7_M 0
#define ENV_ARMV7_R 1
#endif
#else
#define ENV_ARMV4 0
#define ENV_ARMV7 0
#endif
#define ENV_ARMV8 0
#define ENV_RISCV 0
#define ENV_X86 0
#define ENV_X86_32 0
#define ENV_X86_64 0

#elif defined(__ARCH_arm64__)
#define ENV_ARM 0
#define ENV_ARM64 1
#define ENV_ARMV4 0
#define ENV_ARMV7 0
#if __COREBOOT_ARM_ARCH__ == 8
#define ENV_ARMV8 1
#else
#define ENV_ARMV8 0
#endif
#define ENV_RISCV 0
#define ENV_X86 0
#define ENV_X86_32 0
#define ENV_X86_64 0

#elif defined(__ARCH_riscv__)
#define ENV_ARM 0
#define ENV_ARM64 0
#define ENV_ARMV4 0
#define ENV_ARMV7 0
#define ENV_ARMV8 0
#define ENV_RISCV 1
#define ENV_X86 0
#define ENV_X86_32 0
#define ENV_X86_64 0

#elif defined(__ARCH_x86_32__)
#define ENV_ARM 0
#define ENV_ARM64 0
#define ENV_ARMV4 0
#define ENV_ARMV7 0
#define ENV_ARMV8 0
#define ENV_RISCV 0
#define ENV_X86 1
#define ENV_X86_32 1
#define ENV_X86_64 0

#elif defined(__ARCH_x86_64__)
#define ENV_ARM 0
#define ENV_ARM64 0
#define ENV_ARMV4 0
#define ENV_ARMV7 0
#define ENV_ARMV8 0
#define ENV_RISCV 0
#define ENV_X86 1
#define ENV_X86_32 0
#define ENV_X86_64 1

#else
#define ENV_ARM 0
#define ENV_ARM64 0
#define ENV_ARMV4 0
#define ENV_ARMV7 0
#define ENV_ARMV8 0
#define ENV_RISCV 0
#define ENV_X86 0
#define ENV_X86_32 0
#define ENV_X86_64 0

#endif

#if CONFIG(RAMPAYLOAD)
/* ENV_PAYLOAD_LOADER is set to ENV_POSTCAR when CONFIG_RAMPAYLOAD is enabled */
#define ENV_PAYLOAD_LOADER ENV_POSTCAR
#else
/* ENV_PAYLOAD_LOADER is set when you are in a stage that loads the payload.
 * For now, that is the ramstage. */
#define ENV_PAYLOAD_LOADER ENV_RAMSTAGE
#endif

#define ENV_ROMSTAGE_OR_BEFORE \
	(ENV_DECOMPRESSOR || ENV_BOOTBLOCK || ENV_ROMSTAGE || \
	(ENV_SEPARATE_VERSTAGE && !CONFIG(VBOOT_STARTS_IN_ROMSTAGE)))

#if ENV_X86
/* Indicates memory layout is determined with arch/x86/car.ld. */
#define ENV_CACHE_AS_RAM		(ENV_ROMSTAGE_OR_BEFORE && !CONFIG(RESET_VECTOR_IN_RAM))
/* No .data sections with execute-in-place from ROM.  */
#define ENV_HAS_DATA_SECTION	!ENV_CACHE_AS_RAM
#else
/* Both .data and .bss, sometimes SRAM not DRAM. */
#define ENV_HAS_DATA_SECTION	1
#define ENV_CACHE_AS_RAM		0
#endif

/* Indicates if the stage uses the _bss region defined in arch/x86/car.ld */
#define ENV_SEPARATE_BSS	(ENV_CACHE_AS_RAM && (ENV_BOOTBLOCK || !CONFIG(NO_XIP_EARLY_STAGES)))

/* Currently rmodules, ramstage and smm have heap. */
#define ENV_HAS_HEAP_SECTION	(ENV_RMODULE || ENV_RAMSTAGE || ENV_SMM)

/* Set USER_SPACE in the makefile for the rare code that runs in userspace */
#if defined(__USER_SPACE__)
#define ENV_USER_SPACE		1
#else
#define ENV_USER_SPACE		0
#endif

/* Define the first stage to run */
#if CONFIG(VBOOT_STARTS_BEFORE_BOOTBLOCK)
#define ENV_INITIAL_STAGE		ENV_SEPARATE_VERSTAGE
#else
#define ENV_INITIAL_STAGE		ENV_BOOTBLOCK
#endif

#define ENV_CREATES_CBMEM	ENV_ROMSTAGE
#define ENV_HAS_CBMEM		(ENV_ROMSTAGE | ENV_POSTCAR | ENV_RAMSTAGE)
#define ENV_RAMINIT		ENV_ROMSTAGE

#if ENV_X86
#define ENV_HAS_SPINLOCKS		!ENV_ROMSTAGE_OR_BEFORE
#elif ENV_RISCV
#define ENV_HAS_SPINLOCKS		1
#else
#define ENV_HAS_SPINLOCKS		0
#endif

/* When set <arch/smp/spinlock.h> is included for the spinlock implementation. */
#define ENV_SUPPORTS_SMP		(CONFIG(SMP) && ENV_HAS_SPINLOCKS)

#if ENV_X86 && CONFIG(COOP_MULTITASKING) && (ENV_RAMSTAGE || ENV_ROMSTAGE)
/* TODO: Enable in all x86 stages */
#define ENV_SUPPORTS_COOP         1
#else
#define ENV_SUPPORTS_COOP         0
#endif

/**
 * For pre-DRAM stages and post-CAR always build with simple device model, ie.
 * PCI, PNP and CPU functions operate without use of devicetree. The reason
 * post-CAR utilizes __SIMPLE_DEVICE__ is for simplicity. Currently there's
 * no known requirement that devicetree would be needed during that stage.
 *
 * For ramstage individual source file may define __SIMPLE_DEVICE__
 * before including any header files to force that particular source
 * be built with simple device model.
 */

#if !ENV_RAMSTAGE
#define __SIMPLE_DEVICE__
#endif

#endif /* _RULES_H */
