/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
 * Copyright (C) 2010 Siemens AG, Inc.
 * (Written by Josef Kellermann <joseph.kellermann@heitec.de> for Siemens AG, Inc.)
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
 
#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/interrupt.h>
#include "int15_func.h"

int sbios_INT15_handler(struct eregs *);
/*extern*/ unsigned long vgainfo_addr;

static INT15_function_extensions __int15_func;

/* System BIOS int15 function */
int sbios_INT15_handler(struct eregs *regs)
{
    int res = -1;

    printk(BIOS_DEBUG, "System BIOS INT 15h\n");

    switch (regs->eax & 0xffff) {
#define BOOT_DISPLAY_DEFAULT    0
#define BOOT_DISPLAY_CRT        (1 << 0)
#define BOOT_DISPLAY_TV         (1 << 1)
#define BOOT_DISPLAY_EFP        (1 << 2)
#define BOOT_DISPLAY_LCD        (1 << 3)
#define BOOT_DISPLAY_CRT2       (1 << 4)
#define BOOT_DISPLAY_TV2        (1 << 5)
#define BOOT_DISPLAY_EFP2       (1 << 6)
#define BOOT_DISPLAY_LCD2       (1 << 7)
	case 0x5f35:
		regs->eax = 0x5f;
		regs->ecx = BOOT_DISPLAY_DEFAULT;
		res = 0;
		break;
	case 0x5f40:
		regs->eax = 0x5f;
		regs->ecx = 3; // This is mainboard specific
		printk(BIOS_DEBUG, "DISPLAY=%x\n", regs->ecx);
		res = 0;
		break;
    case 0x4e08:
        switch (regs->ebx & 0xff) {
        case 0x00:
            regs->eax &= ~(0xff);
            regs->ebx = (regs->ebx & ~(0xff)) | __int15_func.regs.func00_LCD_panel_id;
			printk(BIOS_DEBUG, "DISPLAY = %x\n", regs->ebx & 0xff);
            res = 0;
			break;
		case 0x02:
			break;
        case 0x05:
            regs->eax &= ~(0xff);
            regs->ebx = (regs->ebx & ~(0xff)) | __int15_func.regs.func05_TV_standard;
			printk(BIOS_DEBUG, "TV = %x\n", regs->ebx & 0xff);
            res = 0;
			break;
		case 0x80:
			regs->eax &= ~(0xff);
			regs->ebx &= ~(0xff);
			printk(BIOS_DEBUG, "Integrated System Information = %x:%x\n", regs->edx, regs->edi);
			vgainfo_addr = (regs->edx * 16) + regs->edi; 
			res = 0;
			break;
		case 0x89:
			regs->eax &= ~(0xff);
			regs->ebx &= ~(0xff);
			printk(BIOS_DEBUG, "Get supported display device information\n");
			res = 0;
			break;
		default:
			break;
        }
        break;
	default:
        printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", regs->eax & 0xffff);
		break;
    }

    return res;
}

/* Initialization VBIOS function extensions */
void install_INT15_function_extensions(INT15_function_extensions *int15_func)
{
	printk(BIOS_DEBUG, "Initialize function extensions for Callback function number 04E08h ..\n");
	__int15_func.regs.func00_LCD_panel_id = int15_func->regs.func00_LCD_panel_id;
	__int15_func.regs.func05_TV_standard = int15_func->regs.func05_TV_standard;
	mainboard_interrupt_handlers(0x15, &sbios_INT15_handler);
}
