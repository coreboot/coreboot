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

#ifndef RAMINIT_H
#define RAMINIT_H

#define MAX_DIMM_SOCKETS_PER_CHANNEL 4
#define MAX_NUM_CHANNELS 2
#define MAX_DIMM_SOCKETS (MAX_NUM_CHANNELS * MAX_DIMM_SOCKETS_PER_CHANNEL)

struct mem_controller {
	pci_devfn_t d0, d0f1; // PCI bus/device/fcns of E7501 memory controller

	// SMBus addresses of DIMM slots for each channel,
	// in order from closest to MCH to furthest away
	// 0 == not present
	uint16_t channel0[MAX_DIMM_SOCKETS_PER_CHANNEL];
	uint16_t channel1[MAX_DIMM_SOCKETS_PER_CHANNEL];
};

void e7505_mch_init(const struct mem_controller *memctrl);
void e7505_mch_scrub_ecc(unsigned long ret_addr);
void e7505_mch_done(const struct mem_controller *memctrl);
int e7505_mch_is_ready(void);


/* Mainboard exports this. */
int spd_read_byte(unsigned device, unsigned address);

#endif /* RAMINIT_H */
