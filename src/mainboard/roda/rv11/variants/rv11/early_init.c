/* SPDX-License-Identifier: GPL-2.0-only */

#include <northbridge/intel/sandybridge/raminit.h>
#include <southbridge/intel/bd82x6x/pch.h>

void mainboard_fill_pei_data(struct pei_data *pei_data)
{
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
