/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2006 Jon Dufresne <jon.dufresne@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */
 
#include <sdram_mode.h>

#define dumpnorth() dump_pci_device(PCI_DEV(0, 0, 1))
#define VG85X_MODE (SDRAM_BURST_4 | SDRAM_BURST_INTERLEAVED | SDRAM_CAS_2_5)

/**
  * Set only what I need until it works, then make it figure things out on boot
  * assumes only one dimm is populated
  */
 
static void sdram_set_registers(const struct mem_controller *ctrl)
{
	/*
	print_debug("Before configuration:\n");
	dump_pci_devices();
	*/
}

static void spd_set_row_attributes(const struct mem_controller *ctrl)
{
	uint16_t dra_reg;
	
	dra_reg = 0x7733;
	pci_write_config16(ctrl->d0, 0x50, dra_reg);
}

static void spd_set_dram_controller_mode(const struct mem_controller *ctrl)
{
	uint32_t drc_reg;
	
	/* drc_reg = 0x00009101; */
	drc_reg = 0x00009901;
	pci_write_config32(ctrl->d0, 0x70, drc_reg);
}

static void spd_set_dram_timing(const struct mem_controller *ctrl)
{
	uint32_t drt_reg;
	
	drt_reg = 0x2a004405;
	pci_write_config32(ctrl->d0, 0x60, drt_reg);
}

static void spd_set_dram_size(const struct mem_controller *ctrl)
{
	uint32_t drb_reg;

	drb_reg = 0x20202010;
	pci_write_config32(ctrl->d0, 0x40, drb_reg);
}

static void spd_set_dram_pwr_management(const struct mem_controller *ctrl)
{
	uint32_t pwrmg_reg;
	
	pwrmg_reg = 0x10f10430;
	pci_write_config32(ctrl->d0, 0x68, pwrmg_reg);
}

static void spd_set_dram_throttle_control(const struct mem_controller *ctrl)
{
	uint32_t dtc_reg;

	dtc_reg = 0x300aa2ff;
	pci_write_config32(ctrl->d0, 0xa0, dtc_reg);
}

void udelay(int usecs)
{
        int i;
        for(i = 0; i < usecs; i++)
                outb(i & 0xff, 0x80);
}

#define delay() udelay(200)

/* if ram still doesn't work do this function */
static void spd_set_undocumented_registers(const struct mem_controller *ctrl)
{
	uint16_t word;
	
	/* 0:0.0 */
	/*
	pci_write_config32(PCI_DEV(0, 0, 0), 0x10, 0xe0000008);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x2c, 0x35808086);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x48, 0xfec10000);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x50, 0x00440100);
	
	pci_write_config32(PCI_DEV(0, 0, 0), 0x58, 0x11111000);
	
	pci_write_config16(PCI_DEV(0, 0, 0), 0x52, 0x0002);
	*/
	pci_write_config16(PCI_DEV(0, 0, 0), 0x52, 0x0044);
	/*
	pci_write_config16(PCI_DEV(0, 0, 0), 0x52, 0x0000);
	*/
	pci_write_config32(PCI_DEV(0, 0, 0), 0x58, 0x33333000);	
	pci_write_config32(PCI_DEV(0, 0, 0), 0x5c, 0x33333333);
	/*
	pci_write_config32(PCI_DEV(0, 0, 0), 0x60, 0x0000390a);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x74, 0x02006056);
	pci_write_config32(PCI_DEV(0, 0, 0), 0x78, 0x00800001);
	*/
	pci_write_config32(PCI_DEV(0, 0, 0), 0xa8, 0x00000001);	
	
	pci_write_config32(PCI_DEV(0, 0, 0), 0xbc, 0x00001020);
	/*
	pci_write_config32(PCI_DEV(0, 0, 0), 0xfc, 0x00000109);
	*/

	/* 0:0.1 */
	pci_write_config32(ctrl->d0, 0x74, 0x00000001);	
	pci_write_config32(ctrl->d0, 0x78, 0x001fe974);
	pci_write_config32(ctrl->d0, 0x80, 0x00af0039);
	pci_write_config32(ctrl->d0, 0x84, 0x0000033c);
	pci_write_config32(ctrl->d0, 0x88, 0x00000010);
	pci_write_config32(ctrl->d0, 0x98, 0xde5a868c);
	pci_write_config32(ctrl->d0, 0x9c, 0x404e0046);
	pci_write_config32(ctrl->d0, 0xa8, 0x00020e1a);
	pci_write_config32(ctrl->d0, 0xb4, 0x0044cdac);
	pci_write_config32(ctrl->d0, 0xb8, 0x000055d4);
	pci_write_config32(ctrl->d0, 0xbc, 0x024acd38);
	pci_write_config32(ctrl->d0, 0xc0, 0x00000003);
	
	/* 0:0.3 */
	/*
	pci_write_config32(PCI_DEV(0, 0, 3), 0x2c, 0x35858086);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x44, 0x11110000);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x48, 0x09614a3c);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x4c, 0x4b09604a);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x50, 0x00000962);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x5c, 0x0b023331);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x6c, 0x0000402e);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x78, 0xe7c70f7f);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x7c, 0x0284007f);
	pci_write_config32(PCI_DEV(0, 0, 3), 0x84, 0x000000ef);
	*/
	
	/*
	pci_write_config16(PCI_DEV(0, 0, 3), 0xc0, 0x0200);
	pci_write_config16(PCI_DEV(0, 0, 3), 0xc0, 0x0400);
	*/
	
	/*
	pci_write_config32(PCI_DEV(0, 0, 3), 0xc4, 0x00000000);
	pci_write_config32(PCI_DEV(0, 0, 3), 0xd8, 0xff00c308);
	pci_write_config32(PCI_DEV(0, 0, 3), 0xdc, 0x00000025);
	pci_write_config32(PCI_DEV(0, 0, 3), 0xe0, 0x001f002a);
	pci_write_config32(PCI_DEV(0, 0, 3), 0xe4, 0x009f0098);
	pci_write_config32(PCI_DEV(0, 0, 3), 0xec, 0x00000400);
	pci_write_config32(PCI_DEV(0, 0, 3), 0xf0, 0xc0000000);
	*/
}

static void sdram_set_spd_registers(const struct mem_controller *ctrl)
{
	spd_set_row_attributes(ctrl);
	spd_set_dram_controller_mode(ctrl);
	spd_set_dram_timing(ctrl);
	spd_set_dram_size(ctrl);
	spd_set_dram_pwr_management(ctrl);
	spd_set_dram_throttle_control(ctrl);
	spd_set_undocumented_registers(ctrl);
}

static void ram_command(const struct mem_controller *ctrl,
			uint8_t command,
			uint32_t addr)
{
	uint32_t drc_reg;
	
	drc_reg = pci_read_config32(ctrl->d0, 0x70);
	drc_reg &= ~(7 << 4);
	drc_reg |= (command << 4);
	pci_write_config8(ctrl->d0, 0x70, drc_reg);
	read32(addr);
}

static void ram_command_mrs(const struct mem_controller *ctrl,
							uint8_t command,
							uint32_t mode,
							uint32_t addr)
{
	uint32_t drc_reg;
	uint32_t adjusted_mode;
	
	drc_reg = pci_read_config32(ctrl->d0, 0x70);
	drc_reg &= ~(7 << 4);
	drc_reg |= (command << 4);
	pci_write_config8(ctrl->d0, 0x70, drc_reg);
	/* Host address lines [13:3] map to DIMM address lines [11, 9:0] */	
	adjusted_mode = ((mode & 0x800) << (13 - 11)) | ((mode & 0x3ff) << (12 - 9));
	print_debug("Setting mode: ");
	print_debug_hex32(adjusted_mode + addr);
	print_debug("\n");
	read32(adjusted_mode + addr);
}

static void set_initialize_complete(const struct mem_controller *ctrl)
{
	uint32_t drc_reg;
	
	drc_reg = pci_read_config32(ctrl->d0, 0x70);
	drc_reg |= (1 << 29);
	pci_write_config32(ctrl->d0, 0x70, drc_reg);
}

static void sdram_enable(int controllers, const struct mem_controller *ctrl)
{
	int i;
	uint32_t rank1 = (1 << 30) / 2;
	print_debug("Ram enable 1\n");
	delay();
	delay();
	
	print_debug("Ram enable 2\n");
	ram_command(ctrl, 1, 0);
	ram_command(ctrl, 1, rank1);
	delay();
	delay();

	print_debug("Ram enable 3\n");
	ram_command(ctrl, 2, 0);
	ram_command(ctrl, 2, rank1);
	delay();
	delay();

	print_debug("Ram enable 4\n");
	ram_command_mrs(ctrl, 4, SDRAM_EXTMODE_DLL_ENABLE, 0);
	ram_command_mrs(ctrl, 4, SDRAM_EXTMODE_DLL_ENABLE, rank1);
	delay();
	delay();
	
	print_debug("Ram enable 5\n");
	ram_command_mrs(ctrl, 3, VG85X_MODE | SDRAM_MODE_DLL_RESET, 0);
	ram_command_mrs(ctrl, 3, VG85X_MODE | SDRAM_MODE_DLL_RESET, rank1);
	
	print_debug("Ram enable 6\n");
	ram_command(ctrl, 2, 0);
	ram_command(ctrl, 2, rank1);
	delay();
	delay();
	
	print_debug("Ram enable 7\n");
	for(i = 0; i < 8; i++) {
		ram_command(ctrl, 6, 0);
		ram_command(ctrl, 6, rank1);
		delay();
		delay();
	}

	print_debug("Ram enable 8\n");
	ram_command_mrs(ctrl, 3, VG85X_MODE | SDRAM_MODE_NORMAL, 0);
	ram_command_mrs(ctrl, 3, VG85X_MODE | SDRAM_MODE_NORMAL, rank1);

	print_debug("Ram enable 9\n");
	ram_command(ctrl, 7, 0);
	ram_command(ctrl, 7, rank1);
	delay();
	delay();
	
	print_debug("Ram enable 9\n");
	set_initialize_complete(ctrl);
	
	delay();
	delay();
	delay();
	
	print_debug("After configuration:\n");
	/* dump_pci_devices(); */
	
	/*
	print_debug("\n\n***** RAM TEST *****\n");	
	ram_check(0, 0xa0000);
	ram_check(0x100000, 0x40000000);
	*/
}
