/* 
    freebios/src/northbridge/nsc/gx1/northbridge.c

    Copyright (c) 2002 Christer Weinigel <wingel@hack.org>

    Do chipset setup for a National Semiconductor GX1 CPU.
*/

#include <mem.h>
#include <part/sizeram.h>
#include <printk.h>
#include <pci.h>
#include <pci_ids.h>

#include "gx1def.h"

#define __io_virt(addr) (addr)

#define readb(addr) (*(volatile unsigned char *) __io_virt(addr))
#define readw(addr) (*(volatile unsigned short *) __io_virt(addr))
#define readl(addr) (*(volatile unsigned int *) __io_virt(addr))

#define writeb(b,addr) (*(volatile unsigned char *) __io_virt(addr) = (b))
#define writew(b,addr) (*(volatile unsigned short *) __io_virt(addr) = (b))
#define writel(b,addr) (*(volatile unsigned int *) __io_virt(addr) = (b))

static unsigned calc_dimm(int index, unsigned cfg)
{
	if (((cfg >> 4) & 7) == 7) {
		printk_info("DIMM%d: empty\n", index);
		return 0;
	} else {
		unsigned page_size = 1 << ((cfg >> 4) & 7);
		unsigned dimm_size = 4 << ((cfg >> 8) & 7);
		unsigned comp_bank = 2 << ((cfg >> 12) & 1);
		unsigned module_bank = 1 << ((cfg >> 14) & 1);
		
		printk_info("DIMM%d: %uMB (%ukB page size, %d component banks, %u module banks)\n",
			     index, dimm_size, page_size, comp_bank, module_bank);
		return dimm_size * 1024;
	}
}

static unsigned long adjust_video_memory(unsigned long mem_size)
{
	unsigned video_size = 4096;

	printk_info("Reserving %dkB for video memory\n", video_size);

	/* Adjust for video memory size */
	mem_size -= video_size;

	/* Set up the registers */
	writel(mem_size * 1024 - 1, GX_BASE + BC_DRAM_TOP);
	writel(mem_size / 512, GX_BASE + MC_GBASE_ADD);

	printk_debug("BC_DRAM_TOP = 0x%08x\n", readl(GX_BASE + BC_DRAM_TOP));
	printk_debug("MC_GBASE_ADD = 0x%08x\n", readl(GX_BASE + MC_GBASE_ADD));
	
	return mem_size;
}

struct mem_range *sizeram(void)
{
	static struct mem_range mem[3];
	u32 mem_bank_cfg;
	unsigned mem_size;

	mem_bank_cfg = *(volatile unsigned long *)(GX_BASE+0x8408);

	mem_size = 0;
	mem_size += calc_dimm(0, mem_bank_cfg);
	mem_size += calc_dimm(1, mem_bank_cfg >> 16);

	mem_size = adjust_video_memory(mem_size);

	mem[0].basek = 0;
	mem[0].sizek = 640;
	mem[1].basek = 1024;
	mem[1].sizek = mem_size - mem[1].basek;
	mem[2].basek = 0;
	mem[2].sizek = 0;
	return &mem;
}

/*
    Local variables:
        compile-command: "make -C /export/bios/voyager2"
        c-basic-offset: 8
    End:
*/
