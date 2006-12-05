/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

#include <ppc.h>
#include <ppcreg.h>
#include <string.h>
#include <console/console.h>

#define ONEMEG  0x00100000
#define HALFMEG 0x00080000

unsigned long memory_base = 0;
unsigned long memory_top = 0;
unsigned long memory_size = 0;

//extern char __heap_end[];
extern unsigned mpc107_config_memory(void);

unsigned config_memory(unsigned offset)
{
    //extern char __start[];
    //extern char __bss_start[];
    //unsigned rom_image = (unsigned) __start & 0xfff00000;
    //unsigned physical = rom_image + offset;
    //unsigned codesize = (unsigned) __bss_start - rom_image;

#if 0    
    /* At this point, DBAT 0 is memory, 1 is variable, 2 is the rom image,
       and 3 is IO. */
    ppc_set_io_dbat_reloc(2, rom_image, physical, ONEMEG);
    ppc_set_io_dbat (3, 0xf0000000, 0x10000000);
    if ( rom_image != physical )
	    ppc_set_ibats_reloc(rom_image, physical, ONEMEG);
    else
	    ppc_set_ibats(physical, ONEMEG);

    printk_debug("bsp_init_memory...\n");
#endif

    ppc_setup_cpu(1); /* icache enable = 1 */
    //ppc_enable_mmu();

    memory_size = mpc107_config_memory();
        
    /* If we have some working RAM, we copy the code and rodata into it.
     * This allows us to reprogram the flash later. */
#if 0
    if (memory_size)
    {
	unsigned onemeg = memory_size - ONEMEG;
	ppc_set_mem_dbat_reloc(1, onemeg, onemeg, ONEMEG);
        memcpy((void *)onemeg, (void *)rom_image, codesize);
	memset((void *)(onemeg + codesize), 0, ONEMEG - codesize);
        ppc_set_ibats_reloc2(rom_image, physical, onemeg, ONEMEG);
        ppc_set_mem_dbat_reloc(2, rom_image, onemeg, ONEMEG);
        make_coherent((void *)onemeg, ONEMEG);
    }

    ppc_set_memory_dbat (memory_size);
#endif

    //ppc_enable_dcache ();

    return memory_size;
}
