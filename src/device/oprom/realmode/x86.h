/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2009-2010 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __DEVICE_OPROM_REALMODE_X86_H__
#define __DEVICE_OPROM_REALMODE_X86_H__

#include <arch/cpu.h>

#define REALMODE_BASE ((void *)0x600)

struct realmode_idt {
	u16 offset, cs;
};

void x86_exception(struct eregs *info);

/* From x86_asm.S */
extern unsigned char __idt_handler;
extern unsigned int __idt_handler_size;
extern unsigned char __realmode_code;
extern unsigned int __realmode_code_size;

extern void (*realmode_call)(u32 addr, u32 eax, u32 ebx, u32 ecx, u32 edx,
		u32 esi, u32 edi) asmlinkage;

extern void (*realmode_interrupt)(u32 intno, u32 eax, u32 ebx, u32 ecx, u32 edx,
		u32 esi, u32 edi) asmlinkage;

#define FAKE_MEMORY_SIZE (1024*1024) // only 1MB
#define INITIAL_EBDA_SEGMENT 0xF600
#define INITIAL_EBDA_SIZE 0x400

int int10_handler(void);
int int12_handler(void);
int int16_handler(void);
int int1a_handler(void);

#endif
