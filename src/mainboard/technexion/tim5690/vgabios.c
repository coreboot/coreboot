/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009 coresystems GmbH
 * Copyright (C) 2009 Libra Li <libra.li@technexion.com>
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
#include "vgabios.h"


int tim5690_int15_handler(struct eregs *regs);

static rs690_vbios_regs vbios_regs_local;

/* Initialization interrupt function */
static void vbios_fun_init(rs690_vbios_regs *vbios_regs)
{
        vbios_regs_local.int15_regs.fun00_panel_id = vbios_regs->int15_regs.fun00_panel_id;
        vbios_regs_local.int15_regs.fun05_tv_standard = vbios_regs->int15_regs.fun05_tv_standard;
}
/* BIOS int15 function */
int tim5690_int15_handler(struct eregs *regs)
{
        int res = -1;

        printk(BIOS_DEBUG, "tim5690_int15_handler\n");

        switch (regs->eax & 0xffff) {
        case AMD_RS690_INT15:
                switch (regs->ebx & 0xff) {
                case 0x00:
                        regs->eax &= ~(0xff);
                        regs->ebx = (regs->ebx & ~(0xff)) | vbios_regs_local.int15_regs.fun00_panel_id;
                        res = 0;
                        break;
                case 0x05:
                        regs->eax &= ~(0xff);
                        regs->ebx = (regs->ebx & ~(0xff)) | vbios_regs_local.int15_regs.fun05_tv_standard;
                        res = 0;
                        break;
                }
                break;
        default:
                printk(BIOS_DEBUG, "Unknown INT15 function %04x!\n",
                                regs->eax & 0xffff);
		break;
        }

        return res;
}

/* Initialization VBIOS function */
void vgabios_init(rs690_vbios_regs *vbios_regs)
{
	printk(BIOS_DEBUG, "vgabios_init\n");

	mainboard_interrupt_handlers(0x15, &tim5690_int15_handler);
	vbios_fun_init(vbios_regs);
}
