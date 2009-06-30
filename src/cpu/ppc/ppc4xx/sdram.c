/*
 * (C) Copyright 2002
 * Stefan Roese, esd gmbh germany, stefan.roese@esd-electronics.com
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
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

#include <ppc_asm.tmpl>
#include <ppc.h>
#include <ppc4xx.h>
#include <timer.h>
#include <clock.h>
#include <stdint.h>

#define CONFIG_SDRAM_BANK0
#ifdef CONFIG_SDRAM_BANK0

/*
 * According to the PPC405GPr Users Manual, only non-reserved
 * bits of SDRAM registers can be set. This means reading the
 * contents and masking off bits to be set.
 */
#define CMD_BITS	0x80C00000
#define CMD_MASK	0xFFE00000
#define TR_BITS		0x010E8016
#define TR_MASK		0x018FC01F
#define B0CR_BITS	0x00084001
#define B0CR_MASK	0xFFCEE001
#define RTR_BITS	0x08080000
#define RTR_MASK	0xFFFF0000
#define ECCCF_BITS	0x00000000
#define ECCCF_MASK	0x00F00000
#define PMIT_BITS	0x0F000000
#define PMIT_MASK	0xFFC00000

#define mfsdram0(reg, data)  mtdcr(memcfga,reg);data = mfdcr(memcfgd)
#define mtsdram0(reg, data)  mtdcr(memcfga,reg);mtdcr(memcfgd,data)

#define set_sdram0(reg, val) \
	mfsdram0(reg, reg32); \
	reg32 &= ~(val##_MASK); \
	reg32 |= (val##_BITS); \
	mtsdram0(reg, reg32)

/*-----------------------------------------------------------------------
 */
void memory_init(void)
{
#if 0
	unsigned long speed;
	unsigned long sdtr1;
#endif
	uint32_t reg32;

#if 0
	/*
	 * Determine SDRAM speed
	 */
	speed = get_pci_bus_freq(); /* parameter not used on ppc4xx */

	/*
	 * Support for 100MHz and 133MHz SDRAM
	 */
	if (speed > 100000000) {
		/*
		 * 133 MHz SDRAM
		 */
		sdtr1 = 0x01074015;
		rtr = 0x07f00000;
	} else {
		/*
		 * default: 100 MHz SDRAM
		 */
		sdtr1 = 0x0086400d;
		rtr = 0x05f00000;
	}
#endif

        /*
	 * Disable memory controller.
	 */
/* TODO: work out why this trashes cache ram */
	//mtsdram0(mem_mcopt1, 0x00000000);

#if CONFIG_EMBEDDED_RAM_SIZE==128*1024*1024
	/* TODO */
#elif CONFIG_EMBEDDED_RAM_SIZE==64*1024*1024
	set_sdram0(mem_sdtr1, TR);
	set_sdram0(mem_mb0cf, B0CR);
	set_sdram0(mem_rtr, RTR);
	set_sdram0(mem_ecccf, ECCCF);
	set_sdram0(mem_pmit, PMIT);
#elif CONFIG_EMBEDDED_RAM_SIZE==32*1024*1024
	/* TODO */
#elif CONFIG_EMBEDDED_RAM_SIZE==16*1024*1024
	/* TODO */
#endif

	/*
	 * Wait for 200us
	 */
	udelay(200);

	/*
	 * Set memory controller options reg, MCOPT1.
	 * Set DC_EN to '1' and BRD_PRF to '01' for 16 byte PLB Burst
	 * read/prefetch.
	 */
	set_sdram0(mem_mcopt1, CMD);

	/*
	 * Wait for 10ms
	 */
	udelay(10000);
}

#endif /* CONFIG_SDRAM_BANK0 */
