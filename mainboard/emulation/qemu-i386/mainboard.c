/*
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.
	
	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.
	
	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA

*/
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <arch/types.h>
// #include "dtc.h"

char *mainboard_vendor = "emulation";
char *mainboard_part_number = "qemu-x86";

static void enable_dev(struct device *dev){
	printk(BIOS_INFO, "qemu-i386 enable_dev done\n");
}

struct chip_operations mainboard_emulation_qemu_i386_ops = {
	.name = "QEMU Mainboard",
	.enable_dev = enable_dev
};
