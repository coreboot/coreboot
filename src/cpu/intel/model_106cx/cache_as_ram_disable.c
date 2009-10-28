/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2007-2008 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "cpu/x86/car/copy_and_run.c"

void real_main(unsigned long bist);

void stage1_main(unsigned long bist)
{
	unsigned int cpu_reset = 0;

#if CONFIG_USE_FALLBACK_IMAGE == 1
        /* Is this a deliberate reset by the bios */
        if (bios_reset_detected() && last_boot_normal()) {
                goto normal_image;
        } else {
        	/* This is the primary cpu how should I boot? */
		check_cmos_failed();
		if (do_normal_boot()) {
        	        goto normal_image;
	        }
        	else {
	                goto fallback_image;
        	}
	}
 normal_image:
        __asm__ volatile ("jmp __normal_image"
                : /* outputs */
                : "a" (bist) /* inputs */
                );
 fallback_image:
#endif

	real_main(bist);

	/* No servicable parts below this line .. */

        {
        	/* Check value of esp to verify if we have enough rom for stack in Cache as RAM */
	        unsigned v_esp;
	        __asm__ volatile (
        	        "movl   %%esp, %0\n\t"
	                : "=a" (v_esp)
	        );
	        printk_spew("v_esp=%08x\r\n", v_esp);
        }

cpu_reset_x:

        printk_spew("cpu_reset = %08x\r\n",cpu_reset);

	if(cpu_reset == 0) {
	        print_spew("Clearing initial memory region: ");
	}
	print_spew("No cache as ram now - ");

	/* store cpu_reset to ebx */
        __asm__ volatile (
                "movl %0, %%ebx\n\t"
                ::"a" (cpu_reset)
        );

	if(cpu_reset==0) {
#define CLEAR_FIRST_1M_RAM 1
#include "cache_as_ram_post.c"
	} else {
#undef CLEAR_FIRST_1M_RAM 
#include "cache_as_ram_post.c"
	}

	__asm__ volatile (
                /* set new esp */ /* before _RAMBASE */
                "subl   %0, %%ebp\n\t"
                "subl   %0, %%esp\n\t"
                ::"a"( (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE)- CONFIG_RAMBASE )
	);

	{
		unsigned new_cpu_reset;

		/* get back cpu_reset from ebx */
		__asm__ volatile (
			"movl %%ebx, %0\n\t"
			:"=a" (new_cpu_reset)
		);

#ifdef CONFIG_DEACTIVATE_CAR
		print_debug("Deactivating CAR");
#include CONFIG_DEACTIVATE_CAR_FILE
		print_debug(" - Done.\r\n");
#endif
		/* Copy and execute coreboot_ram */
		copy_and_run(new_cpu_reset);
		/* We will not return */
	}

	print_debug("sorry. parachute did not open.\r\n");
}
