/*
 * Copyright (C) 2003, Greg Watson <gwatson@lanl.gov>
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * Do very early board initialization:
 */

#include <ppc_asm.tmpl>
#include <ppc.h>
#include <arch/io.h>
#include <console/console.h>

void
board_init(void)
{
        /*
         * Enable UART
         */
        uart8250_init(TTYS0_BASE, TTYS0_DIV, TTYS0_LCS);
        printk_info("briQ board initialized...\n");

}
