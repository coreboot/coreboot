/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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
#include <arch/io.h>
#include <cpu/cpu.h>

void smm_setup_structures(void *gnvs, void *tcg, void *smi1)
{
	/* The GDT or coreboot table is going to live here. But a long time
	 * after we relocated the GNVS, so this is not troublesome.
	 */
	*(u32 *)0x500 = (u32)gnvs;
	*(u32 *)0x504 = (u32)tcg;
	*(u32 *)0x508 = (u32)smi1;
	outb(0xea, 0xb2);
}
