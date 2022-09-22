/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <commonlib/region.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/smi_deprecated.h>
#include <cpu/amd/amd64_save_state.h>
#include <cpu/intel/em64t100_save_state.h>
#include <cpu/intel/em64t101_save_state.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/legacy_save_state.h>

#if CONFIG(SPI_FLASH_SMM)
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

static inline void *smm_save_state(uintptr_t base, int arch_offset, int node)
{
	base += SMM_SAVE_STATE_BEGIN(arch_offset) - (node * 0x400);
	return (void *)base;
}

/* This returns the SMM revision from the savestate of CPU0,
   which is assumed to be the same for all CPU's. See the memory
   map in smmhandler.S */
uint32_t smm_revision(void)
{
	return *(uint32_t *)(SMM_BASE + SMM_ENTRY_OFFSET * 2 - SMM_REVISION_OFFSET_FROM_TOP);
}

void *smm_get_save_state(int cpu)
{
	switch (smm_revision()) {
	case 0x00030002:
	case 0x00030007:
		return smm_save_state(SMM_BASE, SMM_LEGACY_ARCH_OFFSET, cpu);
	case 0x00030100:
		return smm_save_state(SMM_BASE, SMM_EM64T100_ARCH_OFFSET, cpu);
	case 0x00030101: /* SandyBridge, IvyBridge, and Haswell */
		return smm_save_state(SMM_BASE, SMM_EM64T101_ARCH_OFFSET, cpu);
	case 0x00020064:
	case 0x00030064:
		return smm_save_state(SMM_BASE, SMM_AMD64_ARCH_OFFSET, cpu);
	}

	return NULL;
}

bool smm_region_overlaps_handler(const struct region *r)
{
	const struct region r_smm = {SMM_BASE, SMM_DEFAULT_SIZE};

	return region_overlap(&r_smm, r);
}

/**
 * @brief Interrupt handler for SMI#
 *
 * @param smm_revision revision of the smm state save map
 */

void smi_handler(void)
{
	unsigned int node;

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

	node = lapicid();

	console_init();

	printk(BIOS_SPEW, "\nSMI# #%d\n", node);

	/* Use smm_get_save_state() to see if the smm revision is supported */
	if (smm_get_save_state(node) == NULL) {
		printk(BIOS_WARNING, "smm_revision: 0x%08x\n", smm_revision());
		printk(BIOS_WARNING, "SMI# not supported on your CPU\n");
		/* Don't release lock, so no further SMI will happen,
		 * if we don't handle it anyways.
		 */
		return;
	}

	/* Allow drivers to initialize variables in SMM context. */
	if (do_driver_init) {
#if CONFIG(SPI_FLASH_SMM)
		spi_init();
#endif
		do_driver_init = 0;
	}

	/* Call chipset specific SMI handlers. */
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
int __weak mainboard_io_trap_handler(int smif) { return 0; }
void __weak southbridge_smi_handler(void) {}
void __weak mainboard_smi_gpi(u32 gpi_sts) {}
int __weak mainboard_smi_apmc(u8 data) { return 0; }
void __weak mainboard_smi_sleep(u8 slp_typ) {}
void __weak mainboard_smi_finalize(void) {}
