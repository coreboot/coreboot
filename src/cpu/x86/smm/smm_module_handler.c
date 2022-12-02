/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <commonlib/bsd/compiler.h>
#include <commonlib/region.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <rmodule.h>
#include <types.h>

#if CONFIG(SPI_FLASH_SMM)
#include <spi-generic.h>
#endif

static int do_driver_init = 1;

typedef enum { SMI_LOCKED, SMI_UNLOCKED } smi_semaphore;

/* SMI multiprocessing semaphore */
static volatile
__attribute__((aligned(4))) smi_semaphore smi_handler_status = SMI_UNLOCKED;

static const volatile
__attribute((aligned(4), __section__(".module_parameters"))) struct smm_runtime smm_runtime;

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

void smm_get_cbmemc_buffer(void **buffer_out, size_t *size_out)
{
	*buffer_out = smm_runtime.cbmemc;
	*size_out = smm_runtime.cbmemc_size;
}

void io_trap_handler(int smif)
{
	/* If a handler function handled a given IO trap, it
	 * shall return a non-zero value
	 */
	printk(BIOS_DEBUG, "SMI function trap 0x%x: ", smif);

	if (mainboard_io_trap_handler(smif))
		return;

	printk(BIOS_DEBUG, "Unknown function\n");
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

struct global_nvs *gnvs;

void *smm_get_save_state(int cpu)
{
	if (cpu > smm_runtime.num_cpus)
		return NULL;

	return (void *)(smm_runtime.save_state_top[cpu] - smm_runtime.save_state_size);
}

uint32_t smm_revision(void)
{
	const uintptr_t save_state = (uintptr_t)(smm_get_save_state(0));

	return *(uint32_t *)(save_state + smm_runtime.save_state_size
			     - SMM_REVISION_OFFSET_FROM_TOP);
}

bool smm_region_overlaps_handler(const struct region *r)
{
	const struct region r_smm = {smm_runtime.smbase, smm_runtime.smm_size};
	const struct region r_aseg = {SMM_BASE, SMM_DEFAULT_SIZE};

	return region_overlap(&r_smm, r) || region_overlap(&r_aseg, r);
}

asmlinkage void smm_handler_start(void *arg)
{
	const struct smm_module_params *p;
	int cpu;
	uintptr_t actual_canary;
	uintptr_t expected_canary;

	p = arg;
	cpu = p->cpu;
	expected_canary = (uintptr_t)p->canary;

	/* Make sure to set the global runtime. It's OK to race as the value
	 * will be the same across CPUs as well as multiple SMIs. */
	gnvs = (void *)(uintptr_t)smm_runtime.gnvs_ptr;

	if (cpu >= CONFIG_MAX_CPUS) {
		/* Do not log messages to console here, it is not thread safe */
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

	smm_soc_early_init();

	console_init();

	printk(BIOS_SPEW, "\nSMI# #%d\n", cpu);

	/* Allow drivers to initialize variables in SMM context. */
	if (do_driver_init) {
#if CONFIG(SPI_FLASH_SMM)
		spi_init();
#endif
		do_driver_init = 0;
	}

	cpu_smi_handler();
	northbridge_smi_handler();
	southbridge_smi_handler();

	smi_restore_pci_address();

	actual_canary = *p->canary;

	if (actual_canary != expected_canary) {
		printk(BIOS_DEBUG, "canary 0x%lx != 0x%lx\n", actual_canary,
		       expected_canary);

		// Don't die if we can't indicate an error.
		if (CONFIG(DEBUG_SMI))
			die("SMM Handler caused a stack overflow\n");
	}

	smm_soc_exit();

	smi_release_lock();

	/* De-assert SMI# signal to allow another SMI */
	southbridge_smi_set_eos();
}

RMODULE_ENTRY(smm_handler_start);

/* Provide a default implementation for all weak handlers so that relocation
 * entries in the modules make sense. Without default implementations the
 * weak relocations w/o a symbol have a 0 address which is where the modules
 * are linked at. */
int __weak mainboard_io_trap_handler(int smif) { return 0; }
void __weak cpu_smi_handler(void) {}
void __weak northbridge_smi_handler(void) {}
void __weak southbridge_smi_handler(void) {}
void __weak mainboard_smi_gpi(u32 gpi_sts) {}
int __weak mainboard_smi_apmc(u8 data) { return 0; }
void __weak mainboard_smi_sleep(u8 slp_typ) {}
void __weak mainboard_smi_finalize(void) {}

void __weak smm_soc_early_init(void) {}
void __weak smm_soc_exit(void) {}
