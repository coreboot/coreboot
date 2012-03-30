/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 The ChromiumOS Authors.  All rights reserved.
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

/*
 * This code tries to discover the SMSC LPC47N207 superio chip which can be
 * connected over an LPC dongle. The chip could be bootstrap mapped to one of
 * four LPC addresses: 0x2e, 0x4e, 0x162e, and 0x164e.
 *
 * Initializing the UART requires accesses to a few control registers. This
 * structure includes the register offset and the value to write (along with
 * the mask).
 */
typedef struct {
	u8 conf_reg;
	u8 value;
	u8 mask;
} uart_conf;

/* All regs/values to write to initialize the LPC47N207 UART */
static const uart_conf uart_conf_data [] = {
	{2, (1 << 3), (1 << 3)},    /* cr02, enable Primary UART power */
	{0xc, (1 << 6), (1 << 6)},  /* cr0c, enable Primary UART high speed */
	{0x24, (CONFIG_TTYS0_BASE >> 3) << 1, 0xff},  /* cr24, base addr */
};

void try_enabling_LPC47N207_uart(void)
{
	u8 reg_value;
	const uart_conf* conf_item;
	u16 lpc_ports[] = {0x2e, 0x4e, 0x162e, 0x164e};
	u16 lpc_port;
	int i, j;

#define CONFIG_ENABLE    0x55
#define CONFIG_DISABLE   0xaa

	for (j = 0; j < ARRAY_SIZE(lpc_ports); j++) {
		lpc_port = lpc_ports[j];

		/* enable CONFIG mode */
		outb(CONFIG_ENABLE, lpc_port);
		reg_value=inb(lpc_port);
		if (reg_value != CONFIG_ENABLE) {
			continue; /* There is no LPC device at this address */
		}

		do {
			/*
			 * Registers 12 and 13 hold config address, look for a
			 * match.
			 */
			outb(0x12, lpc_port);
			reg_value=inb(lpc_port + 1);
			if (reg_value != (lpc_port & 0xff))
			    break;

			outb(0x13, lpc_port);
			reg_value=inb(lpc_port + 1);
			if (reg_value != (lpc_port >> 8))
				break;

			/* This must be the SMSC LPC 47N207, enable the UART. */
			for (i = 0; i < ARRAY_SIZE(uart_conf_data); i++) {
				u8 reg, value, mask;

				conf_item = uart_conf_data + i;

				reg = conf_item->conf_reg;
				value = conf_item->value;
				mask = conf_item->mask;

				outb(reg, lpc_port);
				reg_value = inb(lpc_port + 1);
				reg_value &= ~mask;
				reg_value |= (value & mask);
				outb(reg_value, lpc_port + 1);
			}
		} while (0);
		outb(CONFIG_DISABLE, lpc_port);
	}
}
