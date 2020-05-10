/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __AMD64_SAVE_STATE_H__
#define __AMD64_SAVE_STATE_H__
#endif

#include <types.h>
#include <cpu/x86/smm.h>

/* AMD64 x86 SMM State-Save Area
 * starts @ 0x7e00
 */
#define SMM_AMD64_ARCH_OFFSET 0x7e00
#define SMM_AMD64_SAVE_STATE_OFFSET \
	SMM_SAVE_STATE_BEGIN(SMM_AMD64_ARCH_OFFSET)
typedef struct {
	u16	es_selector;
	u16	es_attributes;
	u32	es_limit;
	u64	es_base;

	u16	cs_selector;
	u16	cs_attributcs;
	u32	cs_limit;
	u64	cs_base;

	u16	ss_selector;
	u16	ss_attributss;
	u32	ss_limit;
	u64	ss_base;

	u16	ds_selector;
	u16	ds_attributds;
	u32	ds_limit;
	u64	ds_base;

	u16	fs_selector;
	u16	fs_attributfs;
	u32	fs_limit;
	u64	fs_base;

	u16	gs_selector;
	u16	gs_attributgs;
	u32	gs_limit;
	u64	gs_base;

	u8	reserved0[4];
	u16	gdtr_limit;
	u8	reserved1[2];
	u64	gdtr_base;

	u16	ldtr_selector;
	u16	ldtr_attributes;
	u32	ldtr_limit;
	u64	ldtr_base;

	u8	reserved2[4];
	u16	idtr_limit;
	u8	reserved3[2];
	u64	idtr_base;

	u16	tr_selector;
	u16	tr_attributes;
	u32	tr_limit;
	u64	tr_base;

	u64	io_restart_rip;
	u64	io_restart_rcx;
	u64	io_restart_rsi;
	u64	io_restart_rdi;
	u32	smm_io_trap_offset;
	u32	local_smi_status;

	u8	io_restart;
	u8	autohalt_restart;

	u8	reserved5[6];

	u64	efer;

	u8	reserved6[36];

	u32	smm_revision;
	u32	smbase;

	u8	reserved7[68];

	u64	cr4;
	u64	cr3;
	u64	cr0;
	u64	dr7;
	u64	dr6;

	u64	rflags;
	u64	rip;
	u64	r15;
	u64	r14;
	u64	r13;
	u64	r12;
	u64	r11;
	u64	r10;
	u64	r9;
	u64	r8;

	u64	rdi;
	u64	rsi;
	u64	rpb;
	u64	rsp;
	u64	rbx;
	u64	rdx;
	u64	rcx;
	u64	rax;
} __packed amd64_smm_state_save_area_t;
