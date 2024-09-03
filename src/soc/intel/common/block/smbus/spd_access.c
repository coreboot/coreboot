/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/smbus_host.h>
#include <spd_bin.h>

int spd_read_byte(u8 slave_addr, u8 bus_addr)
{
	return smbus_read_byte(slave_addr, bus_addr);
}

int spd_read_word(u8 slave_addr, u8 bus_addr)
{
	return smbus_read_word(slave_addr, bus_addr);
}

void spd_write_byte(u8 slave_addr, u8 bus_addr, u8 value)
{
	smbus_write_byte(slave_addr, bus_addr, value);
}
