/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
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

#include <delay.h>

static void set_htic_bit(u8 i, u32 val, u8 bit)
{
	u32 dword;
	dword = pci_read_config32(NODE_PCI(i, 0), HT_INIT_CONTROL);
	dword &= ~(1<<bit);
	dword |= ((val & 1) <<bit);
	pci_write_config32(NODE_PCI(i, 0), HT_INIT_CONTROL, dword);
}

#ifdef UNUSED_CODE
static u32 get_htic_bit(u8 i, u8 bit)
{
	u32 dword;
	dword = pci_read_config32(NODE_PCI(i, 0), HT_INIT_CONTROL);
	dword &= (1<<bit);
	return dword;
}

static void wait_till_sysinfo_in_ram(void)
{
	while(1) {
		/* give the NB a break, many CPUs spinning on one bit makes a
		 * lot of traffic and time is not too important to APs.
		 */
		udelay(1000);
		if(get_htic_bit(0, 9)) return;
	}
}
#endif

static void set_sysinfo_in_ram(u32 val)
{
	set_htic_bit(0, val, 9);
}

static void fill_mem_ctrl(u32 controllers, struct mem_controller *ctrl_a, const u8 *spd_addr)
{
	int i;
	int j;
	int index = 0;
	struct mem_controller *ctrl;
	for(i=0;i<controllers; i++) {
		ctrl = &ctrl_a[i];
		ctrl->node_id = i;
		ctrl->f0 = NODE_PCI(i, 0);
		ctrl->f1 = NODE_PCI(i, 1);
		ctrl->f2 = NODE_PCI(i, 2);
		ctrl->f3 = NODE_PCI(i, 3);
		ctrl->f4 = NODE_PCI(i, 4);
		ctrl->f5 = NODE_PCI(i, 5);

		if(spd_addr == (void *)0) continue;

		ctrl->spd_switch_addr = spd_addr[index++];

		for(j=0; j < 8; j++) {
			ctrl->spd_addr[j] = spd_addr[index++];

		}
	}
}

