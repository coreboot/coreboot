
#include "cpu/amd/car/disable_cache_as_ram.c"

#include "cpu/amd/car/clear_1m_ram.c"

static inline void print_debug_pcar(const char *strval, uint32_t val)
{
#if CONFIG_USE_INIT
        printk_debug("%s%08x\r\n", strval, val);
#else
        print_debug(strval); print_debug_hex32(val); print_debug("\r\n");
#endif
}



static void post_cache_as_ram(unsigned cpu_reset)
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

        print_debug_pcar("cpu_reset = ",cpu_reset);

        if(cpu_reset == 0) {
                print_debug("Clearing initial memory region: ");
        }
        print_debug("No cache as ram now - ");

        /* store cpu_reset to ebx */
        __asm__ volatile (
                "movl %0, %%ebx\n\t"
                ::"a" (cpu_reset)
        );

	disable_cache_as_ram();

        if(cpu_reset==0) { // cpu_reset don't need to clear it 
		clear_1m_ram();
        }

#if 0
	int i;
	for(i=0;i<0x800000;i++) {
		outb(0x66, 0x80);
	}
#endif

        __asm__ volatile (
                /* set new esp */ /* before _RAMBASE */
                "subl   %0, %%ebp\n\t"
                "subl   %0, %%esp\n\t"
                ::"a"( (DCACHE_RAM_BASE + DCACHE_RAM_SIZE)- _RAMBASE )
        );

       {
                unsigned new_cpu_reset;

                /* get back cpu_reset from ebx */
                __asm__ volatile (
                        "movl %%ebx, %0\n\t"
                        :"=a" (new_cpu_reset)
                );

                print_debug("Use Ram as Stack now - "); /* but We can not go back any more, we lost old stack data in cache as ram*/

                if(new_cpu_reset==0) {
                        print_debug("done\r\n");
                } else
                {
                        print_debug("\r\n");
                }

                print_debug_pcar("new_cpu_reset = ", new_cpu_reset);


                /*copy and execute linuxbios_ram */
                copy_and_run(new_cpu_reset);
                /* We will not return */
        }

	print_debug("should not be here -\r\n");

}

