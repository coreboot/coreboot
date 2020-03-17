/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <arch/io.h>
#include <stdint.h>
#include <console/console.h>
#include <amdblocks/alink.h>

void alink_ab_indx(u32 reg_space, u32 reg_addr, u32 mask, u32 val)
{
	u32 tmp;

	outl((reg_space & 0x7) << 29 | reg_addr, AB_INDX);
	tmp = inl(AB_DATA);
	/* rpr 4.2
	 * For certain revisions of the chip, the ABCFG registers,
	 * with an address of 0x100NN (where 'N' is any hexadecimal
	 * number), require an extra programming step.*/
	outl(0, AB_INDX);

	tmp &= ~mask;
	tmp |= val;

	// printk(BIOS_DEBUG, "about write %x, index=%x", tmp,
	//				(reg_space&0x3)<<29 | reg_addr);

	/* probably we don't have to do it again. */
	outl((reg_space & 0x7) << 29 | reg_addr, AB_INDX);
	outl(tmp, AB_DATA);
	outl(0, AB_INDX);
}

void alink_rc_indx(u32 reg_space, u32 reg_addr, u32 port, u32 mask, u32 val)
{
	u32 tmp;

	outl((reg_space & 0x7) << 29 | (port & 3) << 24 | reg_addr, AB_INDX);
	tmp = inl(AB_DATA);
	/* rpr 4.2
	 * For certain revisions of the chip, the ABCFG registers,
	 * with an address of 0x100NN (where 'N' is any hexadecimal
	 * number), require an extra programming step.*/
	outl(0, AB_INDX);

	tmp &= ~mask;
	tmp |= val;

	//printk(BIOS_DEBUG, "about write %x, index=%x", tmp,
	//		(reg_space&0x3)<<29 | (port&3) << 24 | reg_addr);

	/* probably we don't have to do it again. */
	outl((reg_space & 0x7) << 29 | (port & 3) << 24 | reg_addr, AB_INDX);
	outl(tmp, AB_DATA);
	outl(0, AB_INDX);
}

/*
 * space = 0: AX_INDXC, AX_DATAC
 * space = 1: AX_INDXP, AX_DATAP
 */
void alink_ax_indx(u32 space /*c or p? */, u32 axindc, u32 mask, u32 val)
{
	u32 tmp;

	/* read axindc to tmp */
	outl(space << 29 | space << 3 | 0x30, AB_INDX);
	outl(axindc, AB_DATA);
	outl(0, AB_INDX);
	outl(space << 29 | space << 3 | 0x34, AB_INDX);
	tmp = inl(AB_DATA);
	outl(0, AB_INDX);

	tmp &= ~mask;
	tmp |= val;

	/* write tmp */
	outl(space << 29 | space << 3 | 0x30, AB_INDX);
	outl(axindc, AB_DATA);
	outl(0, AB_INDX);
	outl(space << 29 | space << 3 | 0x34, AB_INDX);
	outl(tmp, AB_DATA);
	outl(0, AB_INDX);
}
