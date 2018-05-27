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

#include <arch/io.h>

#define NODE_ID		0x60
#define HT_INIT_CONTROL	0x6c

#define NODE_NUMS 8

#define DIMM_SOCKETS 4
struct mem_controller {
	unsigned node_id;
	pci_devfn_t f0, f1, f2, f3;
	uint16_t channel0[DIMM_SOCKETS];
	uint16_t channel1[DIMM_SOCKETS];
};

struct sys_info;

void exit_from_self(int controllers, const struct mem_controller *ctrl, struct sys_info *sysinfo);
void setup_resource_map(const unsigned int *register_values, int max);
void set_hw_mem_hole(int controllers, const struct mem_controller *ctrl);
int spd_read_byte(unsigned device, unsigned address);
void activate_spd_rom(const struct mem_controller *ctrl);
void memreset(int controllers, const struct mem_controller *ctrl);
void set_sysinfo_in_ram(unsigned int val);

#define TIMEOUT_LOOPS 300000

#if defined(__PRE_RAM__) && IS_ENABLED(CONFIG_RAMINIT_SYSINFO)
void sdram_initialize(int controllers, const struct mem_controller *ctrl, void *sysinfo);
void sdram_enable(int controllers, const struct mem_controller *ctrl,
		struct sys_info *sysinfo);
void sdram_set_registers(const struct mem_controller *ctrl, struct sys_info *sysinfo);
void sdram_set_spd_registers(const struct mem_controller *ctrl,
			struct sys_info *sysinfo);
void dump_spd_registers(const struct mem_controller *ctrl);
void dump_smbus_registers(void);
#else
void sdram_set_registers(const struct mem_controller *ctrl);
void sdram_set_spd_registers(const struct mem_controller *ctrl);
void sdram_enable(int controllers, const struct mem_controller *ctrl);
void sdram_initialize(int controllers, const struct mem_controller *ctrl);
#endif

#endif /* RAMINIT_H */
