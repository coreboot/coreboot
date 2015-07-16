/* 2005.6 by yhlu
 * 2006.3 yhlu add copy data from CAR to ram
 */
#include <string.h>
#include <arch/stages.h>
#include <arch/early_variables.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/amd/car.h>
#include <arch/acpi.h>
#include "cbmem.h"
#include "cpu/amd/car/disable_cache_as_ram.c"

#if CONFIG_RAMTOP <= 0x100000
	#error "You need to set CONFIG_RAMTOP greater than 1M"
#endif

#define PRINTK_IN_CAR	1

#if PRINTK_IN_CAR
#define print_car_debug(x) printk(BIOS_DEBUG, x)
#else
#define print_car_debug(x)
#endif

static size_t backup_size(void)
{
	size_t car_size = car_data_size();
	return ALIGN(car_size + 1024, 1024);
}

static void memcpy_(void *d, const void *s, size_t len)
{
#if PRINTK_IN_CAR
	printk(BIOS_SPEW, " Copy [%08x-%08x] to [%08x - %08x] ... ",
		(u32) s, (u32) (s + len - 1), (u32) d, (u32) (d + len - 1));
#endif
	memcpy(d, s, len);
}

static void memset_(void *d, int val, size_t len)
{
#if PRINTK_IN_CAR
	printk(BIOS_SPEW, " Fill [%08x-%08x] ... ", (u32) d, (u32) (d + len - 1));
#endif
	memset(d, val, len);
}

static void prepare_romstage_ramstack(void *resume_backup_memory)
{
	size_t backup_top = backup_size();
	print_car_debug("Prepare CAR migration and stack regions...");

	if (resume_backup_memory) {
		memcpy_(resume_backup_memory + HIGH_MEMORY_SAVE - backup_top,
			(void *)(CONFIG_RAMTOP - backup_top), backup_top);
	}
	memset_((void *)(CONFIG_RAMTOP - backup_top), 0, backup_top);

	print_car_debug("Done\n");
}

static void prepare_ramstage_region(void *resume_backup_memory)
{
	size_t backup_top = backup_size();
	print_car_debug("Prepare ramstage memory region... ");

	if (resume_backup_memory) {
		memcpy_(resume_backup_memory, (void *) CONFIG_RAMBASE, HIGH_MEMORY_SAVE - backup_top);
		memset_((void*) CONFIG_RAMBASE, 0, HIGH_MEMORY_SAVE - backup_top);
	} else {
		memset_((void*)0, 0, CONFIG_RAMTOP - backup_top);
	}

	print_car_debug("Done\n");
}

/* Disable Erratum 343 Workaround, see RevGuide for Fam10h, Pub#41322 Rev 3.33 */

static void vErrata343(void)
{
#ifdef BU_CFG2_MSR
    msr_t msr;
    unsigned int uiMask = 0xFFFFFFF7;

    msr = rdmsr(BU_CFG2_MSR);
    msr.hi &= uiMask; // set bit 35 to 0
    wrmsr(BU_CFG2_MSR, msr);
#endif
}

void post_cache_as_ram(void)
{
	void *resume_backup_memory = NULL;

	int s3resume = acpi_is_wakeup_s3();
	if (s3resume) {
		cbmem_recovery(s3resume);
		resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
	}
	prepare_romstage_ramstack(resume_backup_memory);

	/* from here don't store more data in CAR */
	vErrata343();

	size_t car_size = car_data_size();
	void *migrated_car = (void *)(CONFIG_RAMTOP - car_size);

	print_car_debug("Copying data from cache to RAM... ");
	memcpy_(migrated_car, &_car_data_start[0], car_size);
	print_car_debug("Done\n");

	/* New stack grows right below migrated_car. */
	print_car_debug("Switching to use RAM as stack... ");
	cache_as_ram_switch_stack(migrated_car);

	/* We do not come back. */
}

void cache_as_ram_new_stack (void)
{
	void *resume_backup_memory = NULL;

#if PRINTK_IN_CAR
	printk(BIOS_DEBUG, "Top about %08x ... Done\n", (u32) &resume_backup_memory);
#endif
	print_car_debug("Disabling cache as ram now\n");
	disable_cache_as_ram_bsp();

	disable_cache();
	set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);
	enable_cache();

	if (acpi_is_wakeup_s3()) {
		resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
	}
	prepare_ramstage_region(resume_backup_memory);

	set_sysinfo_in_ram(1); // So other core0 could start to train mem

	/*copy and execute ramstage */
	copy_and_run();
	/* We will not return */

	print_car_debug("should not be here -\n");
}
