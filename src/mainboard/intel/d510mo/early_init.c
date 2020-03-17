/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <northbridge/intel/pineview/pineview.h>
#include <superio/winbond/w83627thg/w83627thg.h>
#include <superio/winbond/common/winbond.h>

#define SERIAL_DEV PNP_DEV(0x4e, W83627THG_SP1)

void bootblock_mainboard_early_init(void)
{
	/* Disable Serial IRQ */
	pci_write_config8(PCI_DEV(0, 0x1f, 0), SERIRQ_CNTL, 0x00);
	/* Decode range */
	pci_or_config16(PCI_DEV(0, 0x1f, 0), LPC_IO_DEC, 0x0010);
	pci_write_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, CNF1_LPC_EN
			| CNF2_LPC_EN | KBC_LPC_EN | COMA_LPC_EN
			| COMB_LPC_EN);

	pci_write_config32(PCI_DEV(0, 0x1f, 0), GEN2_DEC, 0x7c0291);

	winbond_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);
}

void mb_pirq_setup(void)
{
	/* dev irq route register */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x0146;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3201;
	RCBA16(D27IR) = 0x0146;

	/* Does not belong here, but is it needed? */
	RCBA32(FD) |= FD_INTLAN;
}

void get_mb_spd_addrmap(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[1] = 0x51;
}
