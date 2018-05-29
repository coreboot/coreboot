/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <compiler.h>
#include <console/console.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/smm.h>

#if IS_ENABLED(CONFIG_SPI_FLASH_SMM)
#include <spi-generic.h>
#endif

static int do_driver_init = 1;

typedef enum { SMI_LOCKED, SMI_UNLOCKED } smi_semaphore;

/* SMI multiprocessing semaphore */
static __attribute__((aligned(4))) volatile smi_semaphore smi_handler_status
	= SMI_UNLOCKED;

static int smi_obtain_lock(void)
{
	u8 ret = SMI_LOCKED;

	asm volatile (
		"movb %2, %%al\n"
		"xchgb %%al, %1\n"
		"movb %%al, %0\n"
		: "=g" (ret), "=m" (smi_handler_status)
		: "g" (SMI_LOCKED)
		: "eax"
	);

	return (ret == SMI_UNLOCKED);
}

void smi_release_lock(void)
{
	asm volatile (
		"movb %1, %%al\n"
		"xchgb %%al, %0\n"
		: "=m" (smi_handler_status)
		: "g" (SMI_UNLOCKED)
		: "eax"
	);
}

#define LAPIC_ID 0xfee00020
static inline __attribute__((always_inline)) unsigned long nodeid(void)
{
	return (*((volatile unsigned long *)(LAPIC_ID)) >> 24);
}

void io_trap_handler(int smif)
{
	/* If a handler function handled a given IO trap, it
	 * shall return a non-zero value
	 */
	printk(BIOS_DEBUG, "SMI function trap 0x%x: ", smif);

	if (southbridge_io_trap_handler(smif))
		return;

	if (mainboard_io_trap_handler(smif))
		return;

	printk(BIOS_DEBUG, "Unknown function\n");
}

/**
 * @brief Set the EOS bit
 */
static void smi_set_eos(void)
{
	southbridge_smi_set_eos();
}

static u32 pci_orig;

/**
 * @brief Backup PCI address to make sure we do not mess up the OS
 */
static void smi_backup_pci_address(void)
{
	pci_orig = inl(0xcf8);
}

/**
 * @brief Restore PCI address previously backed up
 */
static void smi_restore_pci_address(void)
{
	outl(pci_orig, 0xcf8);
}

static inline void *smm_save_state(uintptr_t base, int arch_offset, int node)
{
	base += SMM_SAVE_STATE_BEGIN(arch_offset) - (node * 0x400);
	return (void *)base;
}

/**
 * @brief Interrupt handler for SMI#
 *
 * @param smm_revision revision of the smm state save map
 */

void smi_handler(u32 smm_revision)
{
	unsigned int node;
	smm_state_save_area_t state_save;
	u32 smm_base = 0xa0000; /* ASEG */

	/* Are we ok to execute the handler? */
	if (!smi_obtain_lock()) {
		/* For security reasons we don't release the other CPUs
		 * until the CPU with the lock is actually done
		 */
		while (smi_handler_status == SMI_LOCKED) {
			asm volatile (
				".byte 0xf3, 0x90\n"  /* hint a CPU we are in
						       * spinlock (PAUSE
						       * instruction, REP NOP)
						       */
			);
		}
		return;
	}

	smi_backup_pci_address();

	node = nodeid();

	console_init();

	printk(BIOS_SPEW, "\nSMI# #%d\n", node);

	switch (smm_revision) {
	case 0x00030002:
	case 0x00030007:
		state_save.type = LEGACY;
		state_save.legacy_state_save =
			smm_save_state(smm_base,
				       SMM_LEGACY_ARCH_OFFSET, node);
		break;
	case 0x00030100:
		state_save.type = EM64T;
		state_save.em64t_state_save =
			smm_save_state(smm_base,
				       SMM_EM64T_ARCH_OFFSET, node);
		break;
	case 0x00030101: /* SandyBridge, IvyBridge, and Haswell */
		state_save.type = EM64T101;
		state_save.em64t101_state_save =
			smm_save_state(smm_base,
				       SMM_EM64T101_ARCH_OFFSET, node);
		break;
	case 0x00030064:
		state_save.type = AMD64;
		state_save.amd64_state_save =
			smm_save_state(smm_base,
				       SMM_AMD64_ARCH_OFFSET, node);
		break;
	default:
		printk(BIOS_DEBUG, "smm_revision: 0x%08x\n", smm_revision);
		printk(BIOS_DEBUG, "SMI# not supported on your CPU\n");
		/* Don't release lock, so no further SMI will happen,
		 * if we don't handle it anyways.
		 */
		return;
	}

	/* Allow drivers to initialize variables in SMM context. */
	if (do_driver_init) {
#if IS_ENABLED(CONFIG_SPI_FLASH_SMM)
		spi_init();
#endif
		do_driver_init = 0;
	}

	/* Call chipset specific SMI handlers. */
	cpu_smi_handler(node, &state_save);
	northbridge_smi_handler(node, &state_save);
	southbridge_smi_handler(node, &state_save);

	smi_restore_pci_address();

	smi_release_lock();

	/* De-assert SMI# signal to allow another SMI */
	smi_set_eos();
}

/* Provide a default implementation for all weak handlers so that relocation
 * entries in the modules make sense. Without default implementations the
 * weak relocations w/o a symbol have a 0 address which is where the modules
 * are linked at. */
int __weak mainboard_io_trap_handler(int smif) { return 0; }
void __weak cpu_smi_handler(unsigned int node,
	smm_state_save_area_t *state_save) {}
void __weak northbridge_smi_handler(unsigned int node,
	smm_state_save_area_t *state_save) {}
void __weak southbridge_smi_handler(unsigned int node,
	smm_state_save_area_t *state_save) {}
void __weak mainboard_smi_gpi(u32 gpi_sts) {}
int __weak mainboard_smi_apmc(u8 data) { return 0; }
void __weak mainboard_smi_sleep(u8 slp_typ) {}
