/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2008-2009 coresystems GmbH
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
 */

/* AMD64 SMM State-Save Area
 * starts @ 0x7e00
 */
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

	u8	reserved4[40];

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
} __attribute__((packed)) amd64_smm_state_save_area_t;


/* Intel Core 2 (EM64T) SMM State-Save Area
 * starts @ 0x7d00
 */
typedef struct {
	u8	reserved0[208];

	u32	gdtr_upper_base;
	u32	ldtr_upper_base;
	u32	idtr_upper_base;

	u8	reserved1[4];

	u64	io_rdi;
	u64	io_rip;
	u64	io_rcx;
	u64	io_rsi;
	u64	cr4;

	u8	reserved2[68];

	u64	gdtr_base;
	u64	idtr_base;
	u64	ldtr_base;

	u8	reserved3[84];

	u32	smm_revision;
	u32	smbase;

	u16	io_restart;
	u16	autohalt_restart;

	u8	reserved4[24];

	u64	r15;
	u64	r14;
	u64	r13;
	u64	r12;
	u64	r11;
	u64	r10;
	u64	r9;
	u64	r8;

	u64	rax;
	u64	rcx;
	u64	rdx;
	u64	rbx;

	u64	rsp;
	u64	rbp;
	u64	rsi;
	u64	rdi;


	u64	io_mem_addr;
	u32	io_misc_info;

	u32	es_sel;
	u32	cs_sel;
	u32	ss_sel;
	u32	ds_sel;
	u32	fs_sel;
	u32	gs_sel;

	u32	ldtr_sel;
	u32	tr_sel;

	u64	dr7;
	u64	dr6;
	u64	rip;
	u64	efer;
	u64	rflags;

	u64	cr3;
	u64	cr0;
} __attribute__((packed)) em64t_smm_state_save_area_t;


/* Legacy x86 SMM State-Save Area
 * starts @ 0x7e00
 */

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
} __attribute__((packed)) legacy_smm_state_save_area_t;

typedef enum {
	AMD64,
	EM64T,
	LEGACY
} save_state_type_t;


typedef struct {
	save_state_type_t type;
	union {
	amd64_smm_state_save_area_t *amd64_state_save;
	em64t_smm_state_save_area_t *em64t_state_save;
	legacy_smm_state_save_area_t *legacy_state_save;
	};
} smm_state_save_area_t;


/* SMI handler function prototypes */
void smi_handler(u32 smm_revision);

void io_trap_handler(int smif);
int southbridge_io_trap_handler(int smif);
int __attribute__((weak)) mainboard_io_trap_handler(int smif);

void southbridge_smi_set_eos(void);

void __attribute__((weak)) cpu_smi_handler(unsigned int node, smm_state_save_area_t *state_save);
void __attribute__((weak)) northbridge_smi_handler(unsigned int node, smm_state_save_area_t *state_save);
void __attribute__((weak)) southbridge_smi_handler(unsigned int node, smm_state_save_area_t *state_save);

void __attribute__((weak)) mainboard_smi_gpi(u16 gpi_sts);

