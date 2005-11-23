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
#if CONFIG_USE_INIT
        printk_debug("v_esp=%08x\r\n", v_esp);
#else
        print_debug("v_esp="); print_debug_hex32(v_esp); print_debug("\r\n");
#endif
        }
#endif

#if CONFIG_USE_INIT
        printk_debug("cpu_reset = %08x\r\n",cpu_reset);
#else
        print_debug("cpu_reset = "); print_debug_hex32(cpu_reset); print_debug("\r\n");
#endif

        if(cpu_reset == 0) {
                print_debug("Clearing initial memory region: ");
        }
        print_debug("No cache as ram now - ");

        /* store cpu_reset to ebx */
        __asm__ volatile (
                "movl %0, %%ebx\n\t"
                ::"a" (cpu_reset)
        );

#include "cpu/amd/car/disable_cache_as_ram.c"

        if(cpu_reset==0) { // cpu_reset don't need to clear it 
#include "cpu/amd/car/clear_1m_ram.c"
        }

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

#if CONFIG_USE_INIT
                printk_debug("new_cpu_reset = %08x\r\n", new_cpu_reset);
#else
                print_debug("new_cpu_reset = "); print_debug_hex32(new_cpu_reset); print_debug("\r\n");
#endif
                /*copy and execute linuxbios_ram */
                copy_and_run(new_cpu_reset);
                /* We will not return */
        }

	print_debug("should not be here -\r\n");

}

