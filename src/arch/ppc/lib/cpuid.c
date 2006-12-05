/*
 * This file is part of the LinuxBIOS project.
 *
 * Copyright (C) 2000 AG Electronics Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
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

#include "ppc.h"
#include "ppcreg.h"
#include <device/device.h>
#include <console/console.h>

void display_cpuid(struct device *cpu)
{
	unsigned type    = cpu->device >> 16;
	unsigned version = cpu->device & 0xffff;
	const char *cpu_string = 0;
	switch(type) {
        case 0x0001:
		cpu_string = "601";
		break;
        case 0x0003:
		cpu_string = "603";
		break;
        case 0x0004:
		cpu_string = "604";
		break;
        case 0x0006:
		cpu_string = "603e";
		break;
        case 0x0007:
		cpu_string = "603ev";
		break;
        case 0x0008:
		cpu_string = "750";
		break;
        case 0x0009:
		cpu_string = "604e";
		break;
        case 0x000a:
		cpu_string = "604ev5 (MachV)";
		break;
        case 0x000c:
		cpu_string = "7400";
		break;
        case 0x0032:
		cpu_string = "821";
		break;
        case 0x0050:
		cpu_string = "860";
		break;
	case 0x4011:
		cpu_string = "405GP";
		break;
	case 0x5091:
		cpu_string = "405GPr";
		break;
	case 0x5121:
		cpu_string = "405EP";
		break;
	case 0x800c:
		cpu_string = "7410";
		break;
	}
	if (cpu_string)
		printk_info("PowerPC %s", cpu_string);       
	else
		printk_info("PowerPC unknown (0x%x)", type);
	printk_info(" CPU, version %d.%d\n", version >> 8, version & 0xff);       
}

