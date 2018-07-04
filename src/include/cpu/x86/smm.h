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
 */

/* AMD64 SMM State-Save Area
 * starts @ 0x7e00
 */

#ifndef CPU_X86_SMM_H
#define CPU_X86_SMM_H

#include <arch/cpu.h>
#include <types.h>
#include <compiler.h>

#define SMM_DEFAULT_BASE 0x30000
#define SMM_DEFAULT_SIZE 0x10000

/* used only by C programs so far */
#define SMM_BASE 0xa0000

#define SMM_ENTRY_OFFSET 0x8000
#define SMM_SAVE_STATE_BEGIN(x) (SMM_ENTRY_OFFSET + (x))

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


/* Intel Core 2 (EM64T) SMM State-Save Area
 * starts @ 0x7c00
 */
#define SMM_EM64T_ARCH_OFFSET 0x7c00
#define SMM_EM64T_SAVE_STATE_OFFSET \
	SMM_SAVE_STATE_BEGIN(SMM_EM64T_ARCH_OFFSET)
typedef struct {
	u8	reserved0[256];
	u8	reserved1[208];

	u32	gdtr_upper_base;
	u32	ldtr_upper_base;
	u32	idtr_upper_base;

	u8	reserved2[4];

	u64	io_rdi;
	u64	io_rip;
	u64	io_rcx;
	u64	io_rsi;
	u64	cr4;

	u8	reserved3[68];

	u64	gdtr_base;
	u64	idtr_base;
	u64	ldtr_base;

	u8	reserved4[84];

	u32	smm_revision;
	u32	smbase;

	u16	io_restart;
	u16	autohalt_restart;

	u8	reserved5[24];

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
} __packed em64t_smm_state_save_area_t;


/* Intel Revision 30100 SMM State-Save Area
 * The following processor architectures use this:
 * - Bay Trail
 */
#define SMM_EM64T100_ARCH_OFFSET 0x7c00
#define SMM_EM64T100_SAVE_STATE_OFFSET \
	SMM_SAVE_STATE_BEGIN(SMM_EM64T100_ARCH_OFFSET)
typedef struct {
	u8	reserved0[256];
	u8	reserved1[208];

	u32	gdtr_upper_base;
	u32	ldtr_upper_base;
	u32	idtr_upper_base;

	u8	reserved2[4];

	u64	io_rdi;
	u64	io_rip;
	u64	io_rcx;
	u64	io_rsi;

	u8	reserved3[64];
	u32	cr4;

	u8	reserved4[72];

	u32	gdtr_base;
	u8	reserved5[4];
	u32	idtr_base;
	u8	reserved6[4];
	u32	ldtr_base;

	u8	reserved7[88];

	u32	smbase;
	u32	smm_revision;

	u16	io_restart;
	u16	autohalt_restart;

	u8	reserved8[24];

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
} __packed em64t100_smm_state_save_area_t;

/* Intel Revision 30101 SMM State-Save Area
 * The following processor architectures use this:
 * - SandyBridge
 * - IvyBridge
 * - Haswell
 */
#define SMM_EM64T101_ARCH_OFFSET 0x7c00
#define SMM_EM64T101_SAVE_STATE_OFFSET \
	SMM_SAVE_STATE_BEGIN(SMM_EM64T101_ARCH_OFFSET)
typedef struct {
	u8	reserved0[256];
	u8	reserved1[208];

	u32	gdtr_upper_base;
	u32	ldtr_upper_base;
	u32	idtr_upper_base;

	u32	io_cf8;

	u64	io_rdi;
	u64	io_rip;
	u64	io_rcx;
	u64	io_rsi;

	u8	reserved2[52];
	u32	shutdown_auto_restart;
	u8	reserved3[8];
	u32	cr4;

	u8	reserved4[72];

	u32	gdtr_base;
	u8	reserved5[4];
	u32	idtr_base;
	u8	reserved6[4];
	u32	ldtr_base;

	u8	reserved7[56];
	/* EPTP fields are only on Haswell according to BWGs, but Intel was
	 * wise and reused the same revision number. */
	u64	eptp;
	u32	eptp_en;
	u32	cs_base;
	u8	reserved8[4];
	u32	iedbase;

	u8	reserved9[8];

	u32	smbase;
	u32	smm_revision;

	u16	io_restart;
	u16	autohalt_restart;

	u8	reserved10[24];

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
} __packed em64t101_smm_state_save_area_t;


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

typedef enum {
	AMD64,
	EM64T,
	EM64T101,
	LEGACY
} save_state_type_t;


typedef struct {
	save_state_type_t type;
	union {
	amd64_smm_state_save_area_t *amd64_state_save;
	em64t_smm_state_save_area_t *em64t_state_save;
	em64t101_smm_state_save_area_t *em64t101_state_save;
	legacy_smm_state_save_area_t *legacy_state_save;
	};
} smm_state_save_area_t;

#define APM_CNT		0xb2
#define APM_CNT_CST_CONTROL	0x85
#define APM_CNT_PST_CONTROL	0x80
#define APM_CNT_ACPI_DISABLE	0x1e
#define APM_CNT_ACPI_ENABLE	0xe1
#define APM_CNT_MBI_UPDATE	0xeb
#define APM_CNT_GNVS_UPDATE	0xea
#define APM_CNT_FINALIZE	0xcb
#define APM_CNT_LEGACY		0xcc
#define APM_STS		0xb3

/* SMI handler function prototypes */
void smi_handler(u32 smm_revision);

void io_trap_handler(int smif);
int southbridge_io_trap_handler(int smif);
int mainboard_io_trap_handler(int smif);

void southbridge_smi_set_eos(void);

#if IS_ENABLED(CONFIG_SMM_TSEG)
void cpu_smi_handler(void);
void northbridge_smi_handler(void);
void southbridge_smi_handler(void);
#else
void cpu_smi_handler(unsigned int node, smm_state_save_area_t *state_save);
void northbridge_smi_handler(unsigned int node,
	smm_state_save_area_t *state_save);
void southbridge_smi_handler(unsigned int node,
	smm_state_save_area_t *state_save);
#endif /* CONFIG_SMM_TSEG */
void mainboard_smi_gpi(u32 gpi_sts);
int  mainboard_smi_apmc(u8 data);
void mainboard_smi_sleep(u8 slp_typ);

#if !IS_ENABLED(CONFIG_SMM_TSEG)
void smi_release_lock(void);
#endif

/* This is the SMM handler. */
extern unsigned char _binary_smm_start[];
extern unsigned char _binary_smm_end[];

/* Get PMBASE address */
u16 smm_get_pmbase(void);

struct smm_runtime {
	u32 smbase;
	u32 save_state_size;
	/* The apic_id_to_cpu provides a mapping from APIC id to CPU number.
	 * The CPU number is indicated by the index into the array by matching
	 * the default APIC id and value at the index. The stub loader
	 * initializes this array with a 1:1 mapping. If the APIC ids are not
	 * contiguous like the 1:1 mapping it is up to the caller of the stub
	 * loader to adjust this mapping. */
	u8 apic_id_to_cpu[CONFIG_MAX_CPUS];
} __packed;

struct smm_module_params {
	void *arg;
	int cpu;
	const struct smm_runtime *runtime;
	/* A canary value that has been placed at the end of the stack.
	 * If (uintptr_t)canary != *canary then a stack overflow has occurred.
	 */
	const uintptr_t *canary;
};

/* smm_handler_t is called with arg of smm_module_params pointer. */
typedef asmlinkage void (*smm_handler_t)(void *);

#ifdef __SMM__
/* SMM Runtime helpers. */

/* Entry point for SMM modules. */
asmlinkage void smm_handler_start(void *params);

/* Retrieve SMM save state for a given CPU. WARNING: This does not take into
 * account CPUs which are configured to not save their state to RAM. */
void *smm_get_save_state(int cpu);
#endif /* __SMM__ */

/* SMM Module Loading API */

/* The smm_loader_params structure provides direction to the SMM loader:
 * - stack_top - optional external stack provided to loader. It must be at
 *               least per_cpu_stack_size * num_concurrent_stacks in size.
 * - per_cpu_stack_size - stack size per CPU for smm modules.
 * - num_concurrent_stacks - number of concurrent cpus in handler needing stack
 *                           optional for setting up relocation handler.
 * - per_cpu_save_state_size - the SMM save state size per cpu
 * - num_concurrent_save_states - number of concurrent cpus needing save state
 *                                space
 * - handler - optional handler to call. Only used during SMM relocation setup.
 * - handler_arg - optional argument to handler for SMM relocation setup. For
 *                 loading the SMM module, the handler_arg is filled in with
 *                 the address of the module's parameters (if present).
 * - runtime - this field is a result only. The SMM runtime location is filled
 *             into this field so the code doing the loading can manipulate the
 *             runtime's assumptions. e.g. updating the APIC id to CPU map to
 *             handle sparse APIC id space.
 */
struct smm_loader_params {
	void *stack_top;
	size_t per_cpu_stack_size;
	size_t num_concurrent_stacks;

	size_t per_cpu_save_state_size;
	size_t num_concurrent_save_states;

	smm_handler_t handler;
	void *handler_arg;

	struct smm_runtime *runtime;
};

/* Both of these return 0 on success, < 0 on failure. */
int smm_setup_relocation_handler(struct smm_loader_params *params);
int smm_load_module(void *smram, size_t size, struct smm_loader_params *params);

/* Backup and restore default SMM region. */
void *backup_default_smm_area(void);
void restore_default_smm_area(void *smm_save_area);

#endif /* CPU_X86_SMM_H */
