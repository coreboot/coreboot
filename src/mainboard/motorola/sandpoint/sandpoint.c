/* $Id$ */
/* Copyright 2000  AG Electronics Ltd. */
/* This code is distributed without warranty under the GPL v2 (see COPYING) */

#include <ppc.h>
#include <bsp.h>
#include <ppcreg.h>
#include <types.h>
#include <string.h>
#include <printk.h>
#include <pci.h>
#include "nvram.h"

#define ONEMEG  0x00100000
#define HALFMEG 0x00080000

int memory_has_failed = 0;

//extern char __heap_end[];
extern nvram_device bsp_nvram;
extern int init_flash_amd800(char *, unsigned, unsigned);

#if 0
void bsp_relocate(void)
{
	extern unsigned _iseg[];
	extern unsigned _liseg[]; 
	extern unsigned _eiseg[]; 
	unsigned *to;
	unsigned *from;

	from = _liseg;
	to = _iseg;  

	while ( from < _eiseg )
		*to++ = *from++;    
}

void bsp_init_post_reloc(unsigned memory)
{
    extern char __stack_end[];
    physical_memory_size = memory;
    memory_top = memory - ONEMEG;
    memory_base = 0;
    /* Use tiny default heap */
    malloc_add_pool(__stack_end, __heap_end - __stack_end);
}

void bsp_init_post_hello(void)
{    
    init_flash_amd800("BOOT", 0xff000000, 1);
    init_flash_amd800("BOOT", 0xff800000, 1);
    pci_configure();
    printk_info("Memory from 0x%08lx to 0x%08lx\n", memory_base, memory_top);
    nvram_init(&bsp_nvram);
    
    if (!memory_has_failed)
    {
	printk_info("Clearing memory...");
	//memset(0, 0, memory_top); trashes dink
	//block_add_pool(0, memory_top);    
	printk_info("Done\n");
    }
}

void bsp_indicate_dead(void)
{    
    for(;;)
    {
    }       
}

void bsp_identify(void)
{    
    printk_info("Sandpoint BSP\n");

    ppc_identify();
    //net_init();
}
#endif

unsigned bsp_clock_speed(void)
{
    return 100000000 / 4;
}

