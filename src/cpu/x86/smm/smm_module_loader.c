/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 ChromeOS Authors
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

#include <compiler.h>
#include <string.h>
#include <rmodule.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/cache.h>
#include <console/console.h>

#define FXSAVE_SIZE 512

/* FXSAVE area during relocation. While it may not be strictly needed the
   SMM stub code relies on the FXSAVE area being non-zero to enable SSE
   instructions within SMM mode. */
static uint8_t fxsave_area_relocation[CONFIG_MAX_CPUS][FXSAVE_SIZE]
__attribute__((aligned(16)));

/*
 * Components that make up the SMRAM:
 * 1. Save state - the total save state memory used
 * 2. Stack - stacks for the CPUs in the SMM handler
 * 3. Stub - SMM stub code for calling into handler
 * 4. Handler - C-based SMM handler.
 *
 * The components are assumed to consist of one consecutive region.
 */

/* These parameters are used by the SMM stub code. A pointer to the params
 * is also passed to the C-base handler. */
struct smm_stub_params {
	u32 stack_size;
	u32 stack_top;
	u32 c_handler;
	u32 c_handler_arg;
	u32 fxsave_area;
	u32 fxsave_area_size;
	struct smm_runtime runtime;
} __packed;

/*
 * The stub is the entry point that sets up protected mode and stacks for each
 * CPU. It then calls into the SMM handler module. It is encoded as an rmodule.
 */
extern unsigned char _binary_smmstub_start[];

/* Per CPU minimum stack size. */
#define SMM_MINIMUM_STACK_SIZE 32

/*
 * The smm_entry_ins consists of 3 bytes. It is used when staggering SMRAM entry
 * addresses across CPUs.
 *
 * 0xe9 <16-bit relative target> ; jmp <relative-offset>
 */
struct smm_entry_ins {
	char jmp_rel;
	uint16_t rel16;
} __packed;

/*
 * Place the entry instructions for num entries beginning at entry_start with
 * a given stride. The entry_start is the highest entry point's address. All
 * other entry points are stride size below the previous.
 */
static void smm_place_jmp_instructions(void *entry_start, size_t stride,
		size_t num, void *jmp_target)
{
	size_t i;
	char *cur;
	struct smm_entry_ins entry = { .jmp_rel = 0xe9 };

	/* Each entry point has an IP value of 0x8000. The SMBASE for each
	 * CPU is different so the effective address of the entry instruction
	 * is different. Therefore, the relative displacement for each entry
	 * instruction needs to be updated to reflect the current effective
	 * IP. Additionally, the IP result from the jmp instruction is
	 * calculated using the next instruction's address so the size of
	 * the jmp instruction needs to be taken into account. */
	cur = entry_start;
	for (i = 0; i < num; i++) {
		uint32_t disp = (uintptr_t)jmp_target;

		disp -= sizeof(entry) + (uintptr_t)cur;
		printk(BIOS_DEBUG,
		       "SMM Module: placing jmp sequence at %p rel16 0x%04x\n",
		       cur, disp);
		entry.rel16 = disp;
		memcpy(cur, &entry, sizeof(entry));
		cur -= stride;
	}
}

/* Place stacks in base -> base + size region, but ensure the stacks don't
 * overlap the staggered entry points. */
static void *smm_stub_place_stacks(char *base, size_t size,
				   struct smm_loader_params *params)
{
	size_t total_stack_size;
	char *stacks_top;

	if (params->stack_top != NULL)
		return params->stack_top;

	/* If stack space is requested assume the space lives in the lower
	 * half of SMRAM. */
	total_stack_size = params->per_cpu_stack_size *
			   params->num_concurrent_stacks;

	/* There has to be at least one stack user. */
	if (params->num_concurrent_stacks < 1)
		return NULL;

	/* Total stack size cannot fit. */
	if (total_stack_size > size)
		return NULL;

	/* Stacks extend down to SMBASE */
	stacks_top = &base[total_stack_size];

	return stacks_top;
}

/* Place the staggered entry points for each CPU. The entry points are
 * staggered by the per CPU SMM save state size extending down from
 * SMM_ENTRY_OFFSET. */
static void smm_stub_place_staggered_entry_points(char *base,
	const struct smm_loader_params *params, const struct rmodule *smm_stub)
{
	size_t stub_entry_offset;

	stub_entry_offset = rmodule_entry_offset(smm_stub);

	/* If there are staggered entry points or the stub is not located
	 * at the SMM entry point then jmp instructions need to be placed. */
	if (params->num_concurrent_save_states > 1 || stub_entry_offset != 0) {
		size_t num_entries;

		base += SMM_ENTRY_OFFSET;
		num_entries = params->num_concurrent_save_states;
		/* Adjust beginning entry and number of entries down since
		 * the initial entry point doesn't need a jump sequence. */
		if (stub_entry_offset == 0) {
			base -= params->per_cpu_save_state_size;
			num_entries--;
		}
		smm_place_jmp_instructions(base,
					   params->per_cpu_save_state_size,
					   num_entries,
					   rmodule_entry(smm_stub));
	}
}

/*
 * The stub setup code assumes it is completely contained within the
 * default SMRAM size (0x10000). There are potentially 3 regions to place
 * within the default SMRAM size:
 * 1. Save state areas
 * 2. Stub code
 * 3. Stack areas
 *
 * The save state and stack areas are treated as contiguous for the number of
 * concurrent areas requested. The save state always lives at the top of SMRAM
 * space, and the entry point is at offset 0x8000.
 */
static int smm_module_setup_stub(void *smbase, struct smm_loader_params *params,
		void *fxsave_area)
{
	size_t total_save_state_size;
	size_t smm_stub_size;
	size_t stub_entry_offset;
	char *smm_stub_loc;
	void *stacks_top;
	size_t size;
	char *base;
	int i;
	struct smm_stub_params *stub_params;
	struct rmodule smm_stub;

	base = smbase;
	size = SMM_DEFAULT_SIZE;

	/* The number of concurrent stacks cannot exceed CONFIG_MAX_CPUS. */
	if (params->num_concurrent_stacks > CONFIG_MAX_CPUS)
		return -1;

	/* Fail if can't parse the smm stub rmodule. */
	if (rmodule_parse(&_binary_smmstub_start, &smm_stub))
		return -1;

	/* Adjust remaining size to account for save state. */
	total_save_state_size = params->per_cpu_save_state_size *
				params->num_concurrent_save_states;
	size -= total_save_state_size;

	/* The save state size encroached over the first SMM entry point. */
	if (size <= SMM_ENTRY_OFFSET)
		return -1;

	/* Need a minimum stack size and alignment. */
	if (params->per_cpu_stack_size <= SMM_MINIMUM_STACK_SIZE ||
	    (params->per_cpu_stack_size & 3) != 0)
		return -1;

	smm_stub_loc = NULL;
	smm_stub_size = rmodule_memory_size(&smm_stub);
	stub_entry_offset = rmodule_entry_offset(&smm_stub);

	/* Assume the stub is always small enough to live within upper half of
	 * SMRAM region after the save state space has been allocated. */
	smm_stub_loc = &base[SMM_ENTRY_OFFSET];

	/* Adjust for jmp instruction sequence. */
	if (stub_entry_offset != 0) {
		size_t entry_sequence_size = sizeof(struct smm_entry_ins);
		/* Align up to 16 bytes. */
		entry_sequence_size = ALIGN_UP(entry_sequence_size, 16);
		smm_stub_loc += entry_sequence_size;
		smm_stub_size += entry_sequence_size;
	}

	/* Stub is too big to fit. */
	if (smm_stub_size > (size - SMM_ENTRY_OFFSET))
		return -1;

	/* The stacks, if requested, live in the lower half of SMRAM space. */
	size = SMM_ENTRY_OFFSET;

	/* Ensure stacks don't encroach onto staggered SMM
	 * entry points. The staggered entry points extend
	 * below SMM_ENTRY_OFFSET by the number of concurrent
	 * save states - 1 and save state size. */
	if (params->num_concurrent_save_states > 1) {
		size -= total_save_state_size;
		size += params->per_cpu_save_state_size;
	}

	/* Place the stacks in the lower half of SMRAM. */
	stacks_top = smm_stub_place_stacks(base, size, params);
	if (stacks_top == NULL)
		return -1;

	/* Load the stub. */
	if (rmodule_load(smm_stub_loc, &smm_stub))
		return -1;

	/* Place staggered entry points. */
	smm_stub_place_staggered_entry_points(base, params, &smm_stub);

	/* Setup the parameters for the stub code. */
	stub_params = rmodule_parameters(&smm_stub);
	stub_params->stack_top = (uintptr_t)stacks_top;
	stub_params->stack_size = params->per_cpu_stack_size;
	stub_params->c_handler = (uintptr_t)params->handler;
	stub_params->c_handler_arg = (uintptr_t)params->handler_arg;
	stub_params->fxsave_area = (uintptr_t)fxsave_area;
	stub_params->fxsave_area_size = FXSAVE_SIZE;
	stub_params->runtime.smbase = (uintptr_t)smbase;
	stub_params->runtime.save_state_size = params->per_cpu_save_state_size;

	/* Initialize the APIC id to CPU number table to be 1:1 */
	for (i = 0; i < params->num_concurrent_stacks; i++)
		stub_params->runtime.apic_id_to_cpu[i] = i;

	/* Allow the initiator to manipulate SMM stub parameters. */
	params->runtime = &stub_params->runtime;

	printk(BIOS_DEBUG, "SMM Module: stub loaded at %p. Will call %p(%p)\n",
	       smm_stub_loc, params->handler, params->handler_arg);

	return 0;
}

/*
 * smm_setup_relocation_handler assumes the callback is already loaded in
 * memory. i.e. Another SMM module isn't chained to the stub. The other
 * assumption is that the stub will be entered from the default SMRAM
 * location: 0x30000 -> 0x40000.
 */
int smm_setup_relocation_handler(struct smm_loader_params *params)
{
	void *smram = (void *)SMM_DEFAULT_BASE;

	/* There can't be more than 1 concurrent save state for the relocation
	 * handler because all CPUs default to 0x30000 as SMBASE. */
	if (params->num_concurrent_save_states > 1)
		return -1;

	/* A handler has to be defined to call for relocation. */
	if (params->handler == NULL)
		return -1;

	/* Since the relocation handler always uses stack, adjust the number
	 * of concurrent stack users to be CONFIG_MAX_CPUS. */
	if (params->num_concurrent_stacks == 0)
		params->num_concurrent_stacks = CONFIG_MAX_CPUS;

	return smm_module_setup_stub(smram, params, fxsave_area_relocation);
}

/* The SMM module is placed within the provided region in the following
 * manner:
 * +-----------------+ <- smram + size
 * |    stacks       |
 * +-----------------+ <- smram + size - total_stack_size
 * |      ...        |
 * +-----------------+ <- smram + handler_size + SMM_DEFAULT_SIZE
 * |    handler      |
 * +-----------------+ <- smram + SMM_DEFAULT_SIZE
 * |    stub code    |
 * +-----------------+ <- smram
 *
 * It should be noted that this algorithm will not work for
 * SMM_DEFAULT_SIZE SMRAM regions such as the A segment. This algorithm
 * expects a region large enough to encompass the handler and stacks
 * as well as the SMM_DEFAULT_SIZE.
 */
int smm_load_module(void *smram, size_t size, struct smm_loader_params *params)
{
	struct rmodule smm_mod;
	size_t total_stack_size;
	size_t handler_size;
	size_t module_alignment;
	size_t alignment_size;
	size_t fxsave_size;
	void *fxsave_area;
	size_t total_size;
	char *base;

	if (size <= SMM_DEFAULT_SIZE)
		return -1;

	/* Fail if can't parse the smm rmodule. */
	if (rmodule_parse(&_binary_smm_start, &smm_mod))
		return -1;

	/* Clear SMM region */
	if (IS_ENABLED(CONFIG_DEBUG_SMI))
		memset(smram, 0xcd, size);

	total_stack_size = params->per_cpu_stack_size *
			   params->num_concurrent_stacks;

	/* Stacks start at the top of the region. */
	base = smram;
	base += size;
	params->stack_top = base;

	/* SMM module starts at offset SMM_DEFAULT_SIZE with the load alignment
	 * taken into account. */
	base = smram;
	base += SMM_DEFAULT_SIZE;
	handler_size = rmodule_memory_size(&smm_mod);
	module_alignment = rmodule_load_alignment(&smm_mod);
	alignment_size = module_alignment -
				((uintptr_t)base % module_alignment);
	if (alignment_size != module_alignment) {
		handler_size += alignment_size;
		base += alignment_size;
	}

	fxsave_size = 0;
	fxsave_area = NULL;
	if (IS_ENABLED(CONFIG_SSE)) {
		fxsave_size = FXSAVE_SIZE * params->num_concurrent_stacks;
		/* FXSAVE area below all the stacks stack. */
		fxsave_area = params->stack_top;
		fxsave_area -= total_stack_size + fxsave_size;
	}

	/* Does the required amount of memory exceed the SMRAM region size? */
	total_size = total_stack_size + handler_size;
	total_size += fxsave_size + SMM_DEFAULT_SIZE;
	if (total_size > size)
		return -1;

	if (rmodule_load(base, &smm_mod))
		return -1;

	params->handler = rmodule_entry(&smm_mod);
	params->handler_arg = rmodule_parameters(&smm_mod);

	return smm_module_setup_stub(smram, params, fxsave_area);
}
