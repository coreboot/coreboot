/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
 * Copyright (C) 2014 Edward O'Callaghan <eocallaghan@alterapraxis.com>
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

#include <arch/io.h>
#include <pc80/mc146818rtc.h>

void bootblock_mainboard_init(void)
{
	uint8_t recovery_enabled;
	unsigned char addr;
	unsigned char byte;

	bootblock_northbridge_init();
	bootblock_southbridge_init();

	/* Recovery jumper is connected to SP5100 GPIO61, and clears the GPIO when placed in the Recovery position */
	byte = pci_io_read_config8(PCI_DEV(0, 0x14, 0), 0x56);
	byte |= 0x1 << 4;	/*  Set GPIO61 to input mode */
	pci_io_write_config8(PCI_DEV(0, 0x14, 0), 0x56, byte);
	recovery_enabled = (!(pci_io_read_config8(PCI_DEV(0, 0x14, 0), 0x57) & 0x1));
	if (recovery_enabled) {
#if CONFIG_USE_OPTION_TABLE
		/* Clear NVRAM checksum */
		for (addr = LB_CKS_RANGE_START; addr <= LB_CKS_RANGE_END; addr++) {
			cmos_write(0x0, addr);
		}

		/* Set fallback boot */
		byte = cmos_read(RTC_BOOT_BYTE);
		byte &= 0xfc;
		cmos_write(byte, RTC_BOOT_BYTE);
#else
		/* FIXME
		 * Figure out how to recover if the option table is not available
		 */
#endif
	}
}
