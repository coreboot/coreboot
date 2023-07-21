/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <stdint.h>
#include <northbridge/intel/sandybridge/raminit.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <northbridge/intel/sandybridge/sandybridge.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_fill_pei_data(struct pei_data *const pei_data)
{
	const uint8_t spdaddr[] = {0xA0, 0x00, 0xA4, 0x00};

	memcpy(pei_data->spd_addresses, &spdaddr, sizeof(pei_data->spd_addresses));

	/* TODO: Confirm if need to enable peg10 in devicetree */
	pei_data->pcie_init = 1;
}

const struct southbridge_usb_port mainboard_usb_ports[] = {
	/* Enabled / Power / OC PIN */
	{ 1, 0, 0 }, /* P00: 1st            USB3 (OC #0) */
	{ 1, 0, 4 }, /* P01: 2nd            USB3 (OC #4) */
	{ 1, 1, 1 }, /* P02: 1st Multibay   USB3 (OC #1) */
	{ 1, 1, 2 }, /* P03: 2nd Multibay   USB3 (OC #2) */
	{ 1, 0, 8 }, /* P04: MiniPCIe 1     USB2 (no OC) */
	{ 1, 0, 8 }, /* P05: MiniPCIe 2     USB2 (no OC) */
	{ 1, 0, 8 }, /* P06: MiniPCIe 3     USB2 (no OC) */
	{ 1, 0, 8 }, /* P07: GPS            USB2 (no OC) */
	{ 1, 0, 8 }, /* P08: MiniPCIe 4     USB2 (no OC) */
	{ 1, 0, 3 }, /* P09: Express Card   USB2 (OC #3) */
	{ 1, 0, 8 }, /* P10: SD card reader USB2 (no OC) */
	{ 1, 0, 8 }, /* P11: Sensors Hub?   USB2 (no OC) */
	{ 1, 0, 8 }, /* P12: Touch Screen   USB2 (no OC) */
	{ 1, 0, 5 }, /* P13: reserved?      USB2 (OC #5) */
};

void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
	read_spd(&spd[0], 0x50, id_only);
	read_spd(&spd[2], 0x52, id_only);
}
