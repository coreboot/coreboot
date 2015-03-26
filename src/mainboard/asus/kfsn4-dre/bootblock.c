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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#include <pc80/mc146818rtc.h>

#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627thg/w83627thg.h>

#define GPIO_DEV PNP_DEV(0x2e, W83627THG_GPIO3)

#define WINBOND_ENTRY_KEY 0x87
#define WINBOND_EXIT_KEY 0xAA

/* Enable configuration: pass entry key '0x87' into index port dev. */
static void pnp_enter_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(WINBOND_ENTRY_KEY, port);
	outb(WINBOND_ENTRY_KEY, port);
}

/* Disable configuration: pass exit key '0xAA' into index port dev. */
static void pnp_exit_conf_state(pnp_devfn_t dev)
{
	u16 port = dev >> 8;
	outb(WINBOND_EXIT_KEY, port);
}

uint8_t bootblock_read_recovery_jumper(pnp_devfn_t dev)
{
	uint8_t recovery_enabled = 0;

	pnp_enter_conf_state(dev);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1);						/* Enable GPIO3 */
	pnp_write_config(dev, 0xf0, 0xff);				/* Set GPIO3 to input */
	recovery_enabled = !(pnp_read_config(dev, 0xf1) & 0x08);	/* Read GP33 */
	pnp_exit_conf_state(dev);

	return recovery_enabled;
}

void bootblock_mainboard_init(void)
{
	uint8_t recovery_enabled;
	unsigned char addr;
	unsigned char byte;

	recovery_enabled = bootblock_read_recovery_jumper(GPIO_DEV);
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