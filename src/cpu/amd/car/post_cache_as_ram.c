/* 2005.6 by yhlu
 * 2006.3 yhlu add copy data from CAR to ram
 */
#include <string.h>
#include <arch/stages.h>
#include <cpu/x86/mtrr.h>
#include "cbmem.h"
#include "cpu/amd/car/disable_cache_as_ram.c"
#include "cpu/x86/mtrr/earlymtrr.c"

static inline void print_debug_pcar(const char *strval, uint32_t val)
{
	printk(BIOS_DEBUG, "%s%08x\n", strval, val);
}

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

#if CONFIG_HAVE_ACPI_RESUME

static inline void *backup_resume(void) {
	void *resume_backup_memory;
	int suspend = acpi_is_wakeup_early();

	if (!suspend)
		return NULL;

	if (!cbmem_reinit())
		return NULL;

	resume_backup_memory = cbmem_find(CBMEM_ID_RESUME);

	/* copy 1MB - 64K to high tables ram_base to prevent memory corruption
	 * through stage 2. We could keep stuff like stack and heap in high tables
	 * memory completely, but that's a wonderful clean up task for another
	 * day.
	 */

	if (resume_backup_memory) {
		print_debug_pcar("Will copy coreboot region to: ", (uint32_t) resume_backup_memory);
		/* copy only backup only memory used for CAR */
		memcopy(resume_backup_memory+HIGH_MEMORY_SAVE-CONFIG_DCACHE_RAM_SIZE,
			(void *)((CONFIG_RAMTOP)-CONFIG_DCACHE_RAM_SIZE),
			 CONFIG_DCACHE_RAM_SIZE); //inline
	}

	return resume_backup_memory;
}
#endif

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

static void post_cache_as_ram(void)
{
#if CONFIG_HAVE_ACPI_RESUME
	void *resume_backup_memory;
#endif
#if 1
	{
	/* Check value of esp to verify if we have enough room for stack in Cache as RAM */
	unsigned v_esp;
	__asm__ volatile (
		"movl   %%esp, %0\n\t"
		: "=a" (v_esp)
	);
	print_debug_pcar("v_esp=", v_esp);
	}
#endif

	unsigned testx = 0x5a5a5a5a;
	print_debug_pcar("testx = ", testx);

	/* copy data from cache as ram to
		ram need to set CONFIG_RAMTOP to 2M and use var mtrr instead.
	 */
#if CONFIG_RAMTOP <= 0x100000
	#error "You need to set CONFIG_RAMTOP greater than 1M"
#endif

#if CONFIG_HAVE_ACPI_RESUME
 	resume_backup_memory = backup_resume();
#endif

	print_debug("Copying data from cache to RAM -- switching to use RAM as stack... ");

	/* from here don't store more data in CAR */
	vErrata343();

	memcopy((void *)((CONFIG_RAMTOP)-CONFIG_DCACHE_RAM_SIZE), (void *)CONFIG_DCACHE_RAM_BASE, CONFIG_DCACHE_RAM_SIZE); //inline

	__asm__ volatile (
		/* set new esp */ /* before CONFIG_RAMBASE */
		"subl   %0, %%esp\n\t"
		::"a"( (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)- (CONFIG_RAMTOP) )
		/* discard all registers (eax is used for %0), so gcc redoes everything
		   after the stack is moved */
		: "cc", "memory", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%ebp"
	);

	/* We can put data to stack again */

	/* only global variable sysinfo in cache need to be offset */
	print_debug("Done\n");
	print_debug_pcar("testx = ", testx);

	print_debug("Disabling cache as ram now \n");

	disable_cache_as_ram_bsp();

	disable_cache();
	set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);
	enable_cache();

#if CONFIG_HAVE_ACPI_RESUME
	/* now copy the rest of the area, using the WB method because we already
	   run normal RAM */
	if (resume_backup_memory) {
		memcopy(resume_backup_memory,
				(void *)(CONFIG_RAMBASE),
				(CONFIG_RAMTOP) - CONFIG_RAMBASE - CONFIG_DCACHE_RAM_SIZE);
	}
#endif

	print_debug("Clearing initial memory region: ");

#if CONFIG_HAVE_ACPI_RESUME
	/* clear only coreboot used region of memory. Note: this may break ECC enabled boards */
	memset((void*) CONFIG_RAMBASE, 0, (CONFIG_RAMTOP) - CONFIG_RAMBASE - CONFIG_DCACHE_RAM_SIZE);
#else
	memset((void*)0, 0, ((CONFIG_RAMTOP) - CONFIG_DCACHE_RAM_SIZE));
#endif
	print_debug("Done\n");

	set_sysinfo_in_ram(1); // So other core0 could start to train mem

	/*copy and execute coreboot_ram */
	copy_and_run();
	/* We will not return */

	print_debug("should not be here -\n");
}
