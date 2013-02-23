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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <stdint.h>
#include <stdlib.h>
#include <console/console.h>
#include <arch/interrupt.h>
#include <x86emu/regs.h>
#include "int15_func.h"

int sbios_INT15_handler(void);
/*extern*/ unsigned long vgainfo_addr;

static INT15_function_extensions __int15_func;

/* System BIOS int15 function */
int sbios_INT15_handler(void)
{
    int res = -1;

    printk(BIOS_DEBUG, "System BIOS INT 15h\n");

    switch (X86_EAX & 0xffff) {
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
		X86_EAX = 0x5f;
		X86_ECX = BOOT_DISPLAY_DEFAULT;
		res = 0;
		break;
	case 0x5f40:
		X86_EAX = 0x5f;
		X86_ECX = 3; // This is mainboard specific
		printk(BIOS_DEBUG, "DISPLAY=%x\n", X86_ECX);
		res = 0;
		break;
    case 0x4e08:
        switch (X86_EBX & 0xff) {
        case 0x00:
            X86_EAX &= ~(0xff);
            X86_EBX = (X86_EBX & ~(0xff)) | __int15_func.regs.func00_LCD_panel_id;
			printk(BIOS_DEBUG, "DISPLAY = %x\n", X86_EBX & 0xff);
            res = 0;
			break;
		case 0x02:
			break;
        case 0x05:
            X86_EAX &= ~(0xff);
            X86_EBX = (X86_EBX & ~(0xff)) | __int15_func.regs.func05_TV_standard;
			printk(BIOS_DEBUG, "TV = %x\n", X86_EBX & 0xff);
            res = 0;
			break;
		case 0x80:
			X86_EAX &= ~(0xff);
			X86_EBX &= ~(0xff);
			printk(BIOS_DEBUG, "Integrated System Information = %x:%x\n", X86_EDX, X86_EDI);
			vgainfo_addr = (X86_EDX * 16) + X86_EDI;
			res = 0;
			break;
		case 0x89:
			X86_EAX &= ~(0xff);
			X86_EBX &= ~(0xff);
			printk(BIOS_DEBUG, "Get supported display device information\n");
			res = 0;
			break;
		default:
			break;
        }
        break;
	default:
        printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n", X86_EAX & 0xffff);
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
