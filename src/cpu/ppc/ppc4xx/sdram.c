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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <ppc_asm.tmpl>
#include <ppc.h>
#include <ppc4xx.h>
#include <timer.h>
#include <clock.h>

#define CONFIG_SDRAM_BANK0
#ifdef CONFIG_SDRAM_BANK0

#define mtsdram0(reg, data)  mtdcr(memcfga,reg);mtdcr(memcfgd,data)

/*-----------------------------------------------------------------------
 */
void sdram_init(void)
{
	unsigned long speed;
	unsigned long sdtr1;
	unsigned long rtr;

	/*
	 * Determine SDRAM speed
	 */
	speed = get_bus_freq(); /* parameter not used on ppc4xx */

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

        /*
	 * Disable memory controller.
	 */
/* TODO: work out why this trashes cache ram */
	//mtsdram0(mem_mcopt1, 0x00000000);

#if EMBEDDED_RAM_SIZE==128*1024*1024
	/*
	 * Set MB0CF for bank 0. (0-128MB) Address Mode 3 since 13x10(4)
	 */
	mtsdram0(mem_mb0cf, 0x000A4001);

	mtsdram0(mem_sdtr1, sdtr1);
	mtsdram0(mem_rtr, rtr);
#elif EMBEDDED_RAM_SIZE==64*1024*1024
	/*
	 * Set MB0CF for bank 0. (0-64MB) Address Mode 3 since 13x9(4)
	 */
	mtsdram0(mem_mb0cf, 0x00084001);

	mtsdram0(mem_sdtr1, sdtr1);
	mtsdram0(mem_rtr, rtr);
#elif EMBEDDED_RAM_SIZE==32*1024*1024
	/*
	 * Set MB0CF for bank 0. (0-32MB) Address Mode 2 since 12x9(4)
	 */
	mtsdram0(mem_mb0cf, 0x00062001);
#elif EMBEDDED_RAM_SIZE==16*1024*1024
	/*
	 * Set MB0CF for bank 0. (0-16MB) Address Mode 4 since 12x8(4)
	 */
	mtsdram0(mem_mb0cf, 0x00046001);
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
	mtsdram0(mem_mcopt1, 0x80800000);

	/*
	 * Wait for 10ms
	 */
	udelay(10000);
}

#endif /* CONFIG_SDRAM_BANK0 */
