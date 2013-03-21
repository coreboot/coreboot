/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 ChromeOS Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>

typedef enum { SMI_LOCKED, SMI_UNLOCKED } smi_semaphore;

/* SMI multiprocessing semaphore */
static volatile
smi_semaphore smi_handler_status __attribute__ ((aligned (4))) = SMI_UNLOCKED;

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

static void smi_release_lock(void)
{
	asm volatile (
		"movb %1, %%al\n"
		"xchgb %%al, %0\n"
		: "=m" (smi_handler_status)
		: "g" (SMI_UNLOCKED)
		: "eax"
	);
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


static const struct smm_runtime *smm_runtime;

void *smm_get_save_state(int cpu)
{
	char *base;

	/* This function assumes all save states start at top of default
	 * SMRAM size space and are staggered down by save state size. */
	base = (void *)smm_runtime->smbase;
	base += SMM_DEFAULT_SIZE;
	base -= (cpu + 1) * smm_runtime->save_state_size;

	return base;
}

void smm_handler_start(void *arg, int cpu, const struct smm_runtime *runtime)
{
	/* Make sure to set the global runtime. It's OK to race as the value
	 * will be the same across CPUs as well as multiple SMIs. */
	if (smm_runtime == NULL)
		smm_runtime = runtime;

	if (cpu >= CONFIG_MAX_CPUS) {
		console_init();
		printk(BIOS_CRIT,
		       "Invalid CPU number assigned in SMM stub: %d\n", cpu);
		return;
	}

	/* Are we ok to execute the handler? */
	if (!smi_obtain_lock()) {
		/* For security reasons we don't release the other CPUs
		 * until the CPU with the lock is actually done */
		while (smi_handler_status == SMI_LOCKED) {
			asm volatile (
				".byte 0xf3, 0x90\n" /* PAUSE */
			);
		}
		return;
	}

	smi_backup_pci_address();

	console_init();

	printk(BIOS_SPEW, "\nSMI# #%d\n", cpu);

	cpu_smi_handler();
	northbridge_smi_handler();
	southbridge_smi_handler();

	smi_restore_pci_address();

	smi_release_lock();

	/* De-assert SMI# signal to allow another SMI */
	smi_set_eos();
}

/* Provide a default implementation for all weak handlers so that relocation
 * entries in the modules make sense. Without default implementations the
 * weak relocations w/o a symbol have a 0 address which is where the modules
 * are linked at. */
int __attribute__((weak)) mainboard_io_trap_handler(int smif) { return 0; }
void __attribute__((weak)) cpu_smi_handler(void) {}
void __attribute__((weak)) northbridge_smi_handler() {}
void __attribute__((weak)) southbridge_smi_handler() {}
void __attribute__((weak)) mainboard_smi_gpi(u16 gpi_sts) {}
int __attribute__((weak)) mainboard_smi_apmc(u8 data) { return 0; }
void __attribute__((weak)) mainboard_smi_sleep(u8 slp_typ) {}
