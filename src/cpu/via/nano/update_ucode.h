/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012  Alexandru Gagniuc <mr.nuke.me@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __UPDATE_UCODE_C
#define __UPDATE_UCODE_C

#include <console/console.h>
#include <cpu/cpu.h>

#define UCODE_UPDATE_NOT_ATTEMPTED	0x0
#define UCODE_UPDATE_SUCCESS		0x1
#define UCODE_UPDATE_FAIL		0x2
#define UCODE_UPDATE_WRONG_CPU		0x3
#define UCODE_INVALID_UPDATE_BLOCK	0x4

#define MSR_IA32_BIOS_UPDT_TRIG	0x00000079
#define MSR_IA32_BIOS_SIGN_ID	0x0000008b
#define MSR_UCODE_UPDATE_STATUS	0x00001205

#if defined(CONFIG_UPDATE_CPU_MICROCODE) && CONFIG_UPDATE_CPU_MICROCODE != 0
void nano_update_ucode(void);
#else
static void nano_update_ucode(void)
{
	printk(BIOS_INFO, "CPU Microcode updating disabled.\n");
}
#endif
#endif /* __UPDATE_UCODE_C */