/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 The Chromium OS Authors
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
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

#if 0
/*
 * Set/clear program flow prediction and return the previous state.
 */
static int config_branch_prediction(int set_cr_z)
{
	unsigned int cr;

	/* System Control Register: 11th bit Z Branch prediction enable */
	cr = get_cr();
	set_cr(set_cr_z ? cr | CR_Z : cr & ~CR_Z);

	return cr & CR_Z;
}
#endif

void bootblock_cpu_init(void);
void bootblock_cpu_init(void)
{
	/*
	 * FIXME: this is a stub for now. It should eventually copy
	 * romstage data (and maybe more) from SPI to SRAM.
	 */
#if 0
	volatile unsigned long *addr = (unsigned long *)0x1004330c;
	*addr |= 0x100;
	while (1) ;
#endif
}
