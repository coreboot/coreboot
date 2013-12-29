/* 2005.6 by yhlu
 * 2006.3 yhlu add copy data from CAR to ram
 */
#include <string.h>
#include <arch/stages.h>
#include <cpu/x86/mtrr.h>
#include <cpu/amd/mtrr.h>
#include "cbmem.h"
#include "cpu/amd/car/disable_cache_as_ram.c"

#if CONFIG_RAMTOP <= 0x100000
	#error "You need to set CONFIG_RAMTOP greater than 1M"
#endif

/* from linux kernel 2.6.32 asm/string_32.h */

static void inline __attribute__((always_inline))  memcopy(void *dest, const void *src, unsigned long bytes)
{
	int d0, d1, d2;
	asm volatile("cld ; rep ; movsl\n\t"
			"movl %4,%%ecx\n\t"
			"andl $3,%%ecx\n\t"
			"jz 1f\n\t"
			"rep ; movsb\n\t"
			"1:"
			: "=&c" (d0), "=&D" (d1), "=&S" (d2)
			: "0" (bytes / 4), "g" (bytes), "1" ((long)dest), "2" ((long)src)
			: "memory", "cc");
}

#define PRINTK_IN_CAR	1
#define ACTUAL_CAR_SIZE	1

#if PRINTK_IN_CAR
#define print_car_debug(x) print_debug(x)
#else
#define print_car_debug(x)
#endif

extern char _car_data_start[];
extern char _car_data_end[];

#if ACTUAL_CAR_SIZE
static size_t car_data_size(void)
{
	size_t car_size = &_car_data_end[0] - &_car_data_start[0];
	return ALIGN(64, car_size);
}

static size_t backup_size(void)
{
	size_t car_size = &_car_data_end[0] - &_car_data_start[0];
	return ALIGN(1024, car_size + 1024);
}
#else
#define car_data_size() 0x1000
#define backup_size() 0x2000
#endif

static void memcpy_(void *d, const void *s, size_t len)
{
#if PRINTK_IN_CAR
	printk(BIOS_SPEW, " Copy [%08x-%08x] to [%08x - %08x] ... ",
		(u32) s, (u32) (s + len), (u32) d, (u32) (d + len));
#endif
	memcpy(d, s, len);
}

static void memset_(void *d, int val, size_t len)
{
#if PRINTK_IN_CAR
	printk(BIOS_SPEW, " Fill [%08x-%08x] ... ", (u32) d, (u32) (d + len));
#endif
	memset(d, val, len);
}

static void prepare_romstage_ramstack(void *resume_backup_memory)
{
	size_t backup_top = backup_size();
	print_car_debug("Prepare CAR migration and stack regions...");
#if CONFIG_HAVE_ACPI_RESUME
	if (resume_backup_memory) {
		memcpy_(resume_backup_memory + HIGH_MEMORY_SAVE - backup_top,
			(void *)(CONFIG_RAMTOP - backup_top), backup_top);
	}
#endif
	memset_((void *)(CONFIG_RAMTOP - backup_top), 0, backup_top);
	print_car_debug("Done\n");
}

static void prepare_ramstage_region(void *resume_backup_memory)
{
	size_t backup_top = backup_size();
	print_car_debug("Prepare ramstage memory region... ");
#if CONFIG_HAVE_ACPI_RESUME
	if (resume_backup_memory) {
		memcpy_(resume_backup_memory, (void *) CONFIG_RAMBASE, HIGH_MEMORY_SAVE - backup_top);
		memset_((void*) CONFIG_RAMBASE, 0, HIGH_MEMORY_SAVE - backup_top);
	}
#endif
	if (!resume_backup_memory)
		memset_((void*)0, 0, CONFIG_RAMTOP - backup_top);
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

void cache_as_ram_switch_stack(void * new_stack);
void cache_as_ram_new_stack (void);

void post_cache_as_ram(void)
{
	void *resume_backup_memory = NULL;

#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_is_wakeup_early()) {
		cbmem_reinit();
		resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
	}
#endif
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

	print_car_debug("Done\n");

	print_car_debug("Disabling cache as ram now\n");
	disable_cache_as_ram_bsp();

	disable_cache();
	set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);
	enable_cache();

#if CONFIG_HAVE_ACPI_RESUME
	if (acpi_is_wakeup_early()) {
		resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);
	}
#endif
	prepare_ramstage_region(resume_backup_memory);

	set_sysinfo_in_ram(1); // So other core0 could start to train mem

	/*copy and execute coreboot_ram */
	copy_and_run();
	/* We will not return */

	print_car_debug("should not be here -\n");
}
