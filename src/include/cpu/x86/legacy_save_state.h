/*
 * This file is part of the coreboot project.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __X86_LEGACY_SAVE_STATE_H__
#define __X86_LEGACY_SAVE_STATE_H__

#include <types.h>

/* Legacy x86 SMM State-Save Area
 * starts @ 0x7e00
 */
#define SMM_LEGACY_ARCH_OFFSET 0x7e00

typedef struct {
	u8	reserved0[248];
	u32	smbase;
	u32	smm_revision;
	u16	io_restart;
	u16	autohalt_restart;
	u8	reserved1[132];
	u32	gdtbase;
	u8	reserved2[8];
	u32	idtbase;
	u8	reserved3[16];
	u32	es;
	u32	cs;
	u32	ss;
	u32	ds;
	u32	fs;
	u32	gs;
	u32	ldtbase;
	u32	tr;
	u32	dr7;
	u32	dr6;
	u32	eax;
	u32	ecx;
	u32	edx;
	u32	ebx;
	u32	esp;
	u32	ebp;
	u32	esi;
	u32	edi;
	u32	eip;
	u32	eflags;
	u32	cr3;
	u32	cr0;
} __packed legacy_smm_state_save_area_t;

#endif
