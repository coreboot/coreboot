/*
 * This file is part of the coreboot project.
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

#include <device/pnp_def.h>

#define SCH4307_CONFIG_PORT     0x162e
static inline void shc4307_enter_ext_func_mode(device_t dev)
{
	unsigned port = dev >> 8;
	outb(0x55, port);
}

static inline void shc4307_exit_ext_func_mode(device_t dev)
{
	unsigned port = dev >> 8;
	outb(0xaa, port);
}

#define CMOS_DEV PNP_DEV(SCH4307_CONFIG_PORT, 0x6)
#define KBD_DEV  PNP_DEV(SCH4307_CONFIG_PORT, 0x7)
#define DBG_DEV  PNP_DEV(SCH4307_CONFIG_PORT, 0x3)
#define REGS_DEV PNP_DEV(SCH4307_CONFIG_PORT, 0xa)

/* FIXME: This appears to be a super-io initialisation,
 *        placed in the mainboard directory.
 */
void shc4307_init(void)
{
	shc4307_enter_ext_func_mode(CMOS_DEV);
	pnp_set_logical_device(CMOS_DEV); /* CMOS/RTC */
	pnp_set_iobase(CMOS_DEV, PNP_IDX_IO0, 0x70);
	pnp_set_iobase(CMOS_DEV, PNP_IDX_IO1, 0x72);
	pnp_set_irq(CMOS_DEV, PNP_IDX_IRQ0, 8);
	/* pnp_set_enable(CMOS_DEV, 3); */
	pnp_write_config(CMOS_DEV, 0x30, 3);

	pnp_set_logical_device(KBD_DEV); /* Keyboard */
	pnp_set_irq(KBD_DEV, PNP_IDX_IRQ0, 1);
	pnp_set_enable(KBD_DEV, 1);

	pnp_set_logical_device(DBG_DEV); /* Debug */
	pnp_set_iobase(DBG_DEV, PNP_IDX_IO0, 0x80);
	pnp_set_enable(DBG_DEV, 1);

	pnp_set_logical_device(REGS_DEV);
	pnp_set_iobase(REGS_DEV, PNP_IDX_IO0, 0x600);
	pnp_set_enable(REGS_DEV, 1);

	shc4307_exit_ext_func_mode(CMOS_DEV);
}

static void bootblock_mainboard_init(void)
{
	bootblock_northbridge_init();
	bootblock_southbridge_init();
	shc4307_init();
}
