/* 2005.6 by yhlu 
 * 2006.3 yhlu add copy data from CAR to ram
 */
#include "cpu/amd/car/disable_cache_as_ram.c"

#include "cpu/amd/car/clear_init_ram.c"

static inline void print_debug_pcar(const char *strval, uint32_t val)
{
#if CONFIG_USE_PRINTK_IN_CAR
        printk_debug("%s%08x\r\n", strval, val);
#else
        print_debug(strval); print_debug_hex32(val); print_debug("\r\n");
#endif
}

static void inline __attribute__((always_inline))  memcopy(void *dest, const void *src, unsigned long bytes)
{
        __asm__ volatile(
                "cld\n\t"
                "rep; movsl\n\t"
                : /* No outputs */
                : "S" (src), "D" (dest), "c" ((bytes)>>2)
                );
}
/* Disable Erratum 343 Workaround, see RevGuide for Fam10h, Pub#41322 Rev 3.33 */

static void vErrata343(void)
{
    msr_t msr;
    unsigned int uiMask = 0xFFFFFFF7;

#ifdef BU_CFG2_MSR
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
		ram need to set CONFIG_LB_MEM_TOPK to 2048 and use var mtrr instead.
	 */
#if CONFIG_LB_MEM_TOPK <= 1024
        #error "You need to set CONFIG_LB_MEM_TOPK greater than 1024"
#endif
	
	set_init_ram_access(); /* So we can access RAM from [1M, CONFIG_LB_MEM_TOPK) */

//	dump_mem(CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x8000, CONFIG_DCACHE_RAM_BASE+CONFIG_DCACHE_RAM_SIZE-0x7c00);
	print_debug("Copying data from cache to RAM -- switching to use RAM as stack... ");

	/* from here don't store more data in CAR */
	vErrata343();

#if 0
        __asm__ volatile (
        	"pushl  %eax\n\t"
        );
#endif

        memcopy((void *)((CONFIG_LB_MEM_TOPK<<10)-CONFIG_DCACHE_RAM_SIZE), (void *)CONFIG_DCACHE_RAM_BASE, CONFIG_DCACHE_RAM_SIZE); //inline
//        dump_mem((CONFIG_LB_MEM_TOPK<<10) - 0x8000, (CONFIG_LB_MEM_TOPK<<10) - 0x7c00);

        __asm__ volatile (
                /* set new esp */ /* before CONFIG_RAMBASE */
                "subl   %0, %%ebp\n\t"
                "subl   %0, %%esp\n\t"
                ::"a"( (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)- (CONFIG_LB_MEM_TOPK<<10) )
        ); // We need to push %eax to the stack (CAR) before copy stack and pop it later after copy stack and change esp
#if 0
        __asm__ volatile (
	        "popl   %eax\n\t"
        );
#endif


	/* We can put data to stack again */

        /* only global variable sysinfo in cache need to be offset */
        print_debug("Done\r\n");
        print_debug_pcar("testx = ", testx);

	print_debug("Disabling cache as ram now \r\n");
	disable_cache_as_ram_bsp();  

        print_debug("Clearing initial memory region: ");
        clear_init_ram(); //except the range from [(CONFIG_LB_MEM_TOPK<<10) - CONFIG_DCACHE_RAM_SIZE, (CONFIG_LB_MEM_TOPK<<10))
        print_debug("Done\r\n");

//	dump_mem((CONFIG_LB_MEM_TOPK<<10) - 0x8000, (CONFIG_LB_MEM_TOPK<<10) - 0x7c00);

#ifndef CONFIG_MEM_TRAIN_SEQ
#define CONFIG_MEM_TRAIN_SEQ 0
#endif
        set_sysinfo_in_ram(1); // So other core0 could start to train mem

#if CONFIG_MEM_TRAIN_SEQ == 1
//	struct sys_info *sysinfox = ((CONFIG_LB_MEM_TOPK<<10) - CONFIG_DCACHE_RAM_GLOBAL_VAR_SIZE);

        // wait for ap memory to trained
//        wait_all_core0_mem_trained(sysinfox); // moved to lapic_init_cpus.c
#endif
        /*copy and execute coreboot_ram */
        copy_and_run();
        /* We will not return */

        print_debug("should not be here -\r\n");

}

