/* 2005.6 by yhlu 
 * 2006.3 yhlu add copy data from CAR to ram
 */
#include "cpu/amd/car/disable_cache_as_ram.c"

static inline void print_debug_pcar(const char *strval, uint32_t val)
{
        printk_debug("%s%08x\r\n", strval, val);
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

#if 1
        {
        /* Check value of esp to verify if we have enough rom for stack in Cache as RAM */
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
	
	/* So we can access RAM from [1M, CONFIG_RAMTOP) */
	set_var_mtrr(0, 0x00000000, CONFIG_RAMTOP, MTRR_TYPE_WRBACK);

//	dump_mem(CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x8000, CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x7c00);
	print_debug("Copying data from cache to RAM -- switching to use RAM as stack... ");

	/* from here don't store more data in CAR */
	vErrata343();

        memcopy((void *)((CONFIG_RAMTOP)-CONFIG_DCACHE_RAM_SIZE), (void *)CONFIG_DCACHE_RAM_BASE, CONFIG_DCACHE_RAM_SIZE); //inline
//        dump_mem((CONFIG_RAMTOP) - 0x8000, (CONFIG_RAMTOP) - 0x7c00);

        __asm__ volatile (
                /* set new esp */ /* before CONFIG_RAMBASE */
                "subl   %0, %%esp\n\t"
                ::"a"( (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)- (CONFIG_RAMTOP) )
		/* discard all registers (eax is used for %0), so gcc redo everything
		   after the stack is moved */
		: "cc", "memory", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%ebp"
        );

	/* We can put data to stack again */

        /* only global variable sysinfo in cache need to be offset */
        print_debug("Done\r\n");
        print_debug_pcar("testx = ", testx);

	print_debug("Disabling cache as ram now \r\n");
	disable_cache_as_ram_bsp();  

        print_debug("Clearing initial memory region: ");
#if CONFIG_HAVE_ACPI_RESUME == 1
	/* clear only coreboot used region of memory. Note: this may break ECC enabled boards */
	memset((void*) CONFIG_RAMBASE, (CONFIG_RAMTOP) - CONFIG_RAMBASE - CONFIG_DCACHE_RAM_SIZE, 0);
#else
        memset((void*)0, ((CONFIG_RAMTOP) - CONFIG_DCACHE_RAM_SIZE), 0);
#endif
        print_debug("Done\r\n");

//	dump_mem((CONFIG_RAMTOP) - 0x8000, (CONFIG_RAMTOP) - 0x7c00);

        set_sysinfo_in_ram(1); // So other core0 could start to train mem

#if CONFIG_MEM_TRAIN_SEQ == 1
//	struct sys_info *sysinfox = ((CONFIG_RAMTOP) - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);

        // wait for ap memory to trained
//        wait_all_core0_mem_trained(sysinfox); // moved to lapic_init_cpus.c
#endif
        /*copy and execute coreboot_ram */
        copy_and_run();
        /* We will not return */

        print_debug("should not be here -\r\n");

}

