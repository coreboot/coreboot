/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdint.h>
#include <ec/acpi/ec.h>
#include <device/smbus_host.h>
#include <northbridge/intel/ironlake/ironlake.h>

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* Enabled, Current table lookup index, OC map */
	{ 1, IF1_557, 0 },
	{ 1, IF1_55F, 1 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_557, 3 },
	{ 1, IF1_14B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 3 },
	{ 1, IF1_74B, 4 },
	{ 1, IF1_74B, 5 },
	{ 1, IF1_55F, 7 },
	{ 1, IF1_55F, 7 },
	{ 1, IF1_557, 7 },
	{ 1, IF1_55F, 7 },
};

static void set_fsb_frequency(void)
{
	u8 block[5];
	u16 fsbfreq = 62879;
	smbus_block_read(0x69, 0, 5, block);
	block[0] = fsbfreq;
	block[1] = fsbfreq >> 8;

	smbus_block_write(0x69, 0, 5, block);
}

void mainboard_pre_raminit(void)
{
	set_fsb_frequency();
}

void mainboard_get_spd_map(u8 *spd_addrmap)
{
	spd_addrmap[0] = 0x50;
	spd_addrmap[2] = 0x51;
}
