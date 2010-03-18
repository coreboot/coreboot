/*
 * This file is part of the coreboot project.
 * 
 * Copyright (C) 2007-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston,
 * MA 02110-1301 USA
 */

#include "cpu/x86/car/copy_and_run.c"

void real_main(unsigned long bist);

void stage1_main(unsigned long bist)
{
	unsigned int cpu_reset = 0;

	real_main(bist);

	/* No servicable parts below this line .. */
#ifdef CAR_DEBUG
        /* Check value of esp to verify if we have enough rom for stack in Cache as RAM */
	unsigned v_esp;
	__asm__ volatile (
        	"movl   %%esp, %0\n"
		: "=a" (v_esp)
	);
	printk_spew("v_esp=%08x\n", v_esp);
#endif

cpu_reset_x:

        printk_spew("cpu_reset = %08x\n", cpu_reset);
	printk_spew("No cache as ram now - ");

	/* store cpu_reset to ebx */
        __asm__ volatile (
                "movl %0, %%ebx\n\t"
                ::"a" (cpu_reset)
        );

#undef CLEAR_FIRST_1M_RAM 
#include "cache_as_ram_post.c"

	/* For now: use rambase + 1MB - 64K (counting downwards) as stack. This
	 * makes sure that we stay completely within the 1M of memory we
	 * preserve with the memcpy above.
	 */

#ifndef HIGH_MEMORY_SAVE
#define HIGH_MEMORY_SAVE ( (1024 - 64) * 1024 )
#endif

	__asm__ volatile (
		"movl %0, %%ebp\n"
		"movl %0, %%esp\n"
		:: "a" (CONFIG_RAMBASE + HIGH_MEMORY_SAVE)
	);

	{
		unsigned new_cpu_reset;

		/* get back cpu_reset from ebx */
		__asm__ volatile (
			"movl %%ebx, %0\n"
			:"=a" (new_cpu_reset)
		);

		/* Copy and execute coreboot_ram */
		copy_and_run(new_cpu_reset);
	}

	/* We will not return */
	printk_debug("sorry. parachute did not open.\n");
}
