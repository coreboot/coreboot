/* SPDX-License-Identifier: GPL-2.0-only */

#include "assert.h"
#include <acpi/acpi_gnvs.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <rmodule.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <commonlib/helpers.h>
#include <console/console.h>
#include <security/intel/stm/SmmStm.h>

#define FXSAVE_SIZE 512
#define SMM_CODE_SEGMENT_SIZE 0x10000
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

/*
 * The stub is the entry point that sets up protected mode and stacks for each
 * CPU. It then calls into the SMM handler module. It is encoded as an rmodule.
 */
extern unsigned char _binary_smmstub_start[];

/* Per CPU minimum stack size. */
#define SMM_MINIMUM_STACK_SIZE 32

struct cpu_smm_info {
	uint8_t active;
	uintptr_t smbase;
	uintptr_t ss_start;
	uintptr_t ss_top;
	uintptr_t code_start;
	uintptr_t code_end;
};
struct cpu_smm_info cpus[CONFIG_MAX_CPUS] = { 0 };

/*
 * This method creates a map of all the CPU entry points, save state locations
 * and the beginning and end of code segments for each CPU. This map is used
 * during relocation to properly align as many CPUs that can fit into the SMRAM
 * region. For more information on how SMRAM works, refer to the latest Intel
 * developer's manuals (volume 3, chapter 34). SMRAM is divided up into the
 * following regions:
 * +-----------------+ Top of SMRAM
 * |                 | <- MSEG, FXSAVE
 * +-----------------+
 * |    common       |
 * |  smi handler    | 64K
 * |                 |
 * +-----------------+
 * | CPU 0 code  seg |
 * +-----------------+
 * | CPU 1 code seg  |
 * +-----------------+
 * | CPU x code seg  |
 * +-----------------+
 * |                 |
 * |                 |
 * +-----------------+
 * |    stacks       |
 * +-----------------+ <- START of SMRAM
 *
 * The code below checks when a code segment is full and begins placing the remainder
 * CPUs in the lower segments. The entry point for each CPU is smbase + 0x8000
 * and save state is smbase + 0x8000 + (0x8000 - state save size). Save state
 * area grows downward into the CPUs entry point.  Therefore staggering too many
 * CPUs in one 32K block will corrupt CPU0's entry code as the save states move
 * downward.
 * input : smbase of first CPU (all other CPUs
 *         will go below this address)
 * input : num_cpus in the system. The map will
 *         be created from 0 to num_cpus.
 */
static int smm_create_map(const uintptr_t smbase, const unsigned int num_cpus,
			  const struct smm_loader_params *params)
{
	struct rmodule smm_stub;

	if (ARRAY_SIZE(cpus) < num_cpus) {
		printk(BIOS_ERR, "%s: increase MAX_CPUS in Kconfig\n", __func__);
		return 0;
	}

	if (rmodule_parse(&_binary_smmstub_start, &smm_stub)) {
		printk(BIOS_ERR, "%s: unable to get SMM module size\n", __func__);
		return 0;
	}

	/*
	 * How many CPUs can fit into one 64K segment?
	 * Make sure that the first stub does not overlap with the last save state of a segment.
	 */
	const size_t stub_size = rmodule_memory_size(&smm_stub);
	const size_t needed_ss_size = MAX(params->cpu_save_state_size, stub_size);
	const size_t cpus_per_segment =
		(SMM_CODE_SEGMENT_SIZE - SMM_ENTRY_OFFSET - stub_size) / needed_ss_size;

	if (cpus_per_segment == 0) {
		printk(BIOS_ERR, "%s: CPUs won't fit in segment. Broken stub or save state size\n",
		       __func__);
		return 0;
	}

	for (unsigned int i = 0; i < num_cpus; i++) {
		if (i % cpus_per_segment == 0)
			printk(BIOS_DEBUG, "-------------NEW CODE SEGMENT --------------\n");
		printk(BIOS_DEBUG, "CPU 0x%x\n", i);
		/* We copy the same stub for each CPU so they all need the same 'smbase'. */
		const size_t segment_number = i / cpus_per_segment;
		cpus[i].smbase = smbase - SMM_CODE_SEGMENT_SIZE * segment_number
			- needed_ss_size * (i % cpus_per_segment);
		cpus[i].code_start = cpus[i].smbase + SMM_ENTRY_OFFSET;
		cpus[i].code_end = cpus[i].code_start + stub_size;
		cpus[i].ss_top = cpus[i].smbase + SMM_CODE_SEGMENT_SIZE;
		cpus[i].ss_start = cpus[i].ss_top - params->cpu_save_state_size;
		printk(BIOS_DEBUG, "  Stub       [0x%lx-0x%lx[\n", cpus[i].code_start,
		       cpus[i].code_end);
		printk(BIOS_DEBUG, "  Save state [0x%lx-0x%lx[\n", cpus[i].ss_start,
		       cpus[i].ss_top);
		cpus[i].active = 1;
	}

	return 1;
}

/*
 * This method expects the smm relocation map to be complete.
 * This method does not read any HW registers, it simply uses a
 * map that was created during SMM setup.
 * input: cpu_num - cpu number which is used as an index into the
 *       map to return the smbase
 */
u32 smm_get_cpu_smbase(unsigned int cpu_num)
{
	if (cpu_num < CONFIG_MAX_CPUS) {
		if (cpus[cpu_num].active)
			return cpus[cpu_num].smbase;
	}
	return 0;
}

/*
 * This method assumes that at least 1 CPU has been set up from
 * which it will place other CPUs below its smbase ensuring that
 * save state does not clobber the first CPUs init code segment. The init
 * code which is the smm stub code is the same for all CPUs. They enter
 * smm, setup stacks (based on their apic id), enter protected mode
 * and then jump to the common smi handler.  The stack is allocated
 * at the beginning of smram (aka tseg base, not smbase). The stack
 * pointer for each CPU is calculated by using its apic id
 * (code is in smm_stub.s)
 * Each entry point will now have the same stub code which, sets up the CPU
 * stack, enters protected mode and then jumps to the smi handler. It is
 * important to enter protected mode before the jump because the "jump to
 * address" might be larger than the 20bit address supported by real mode.
 * SMI entry right now is in real mode.
 * input: num_cpus - number of cpus that need relocation including
 *        the first CPU (though its code is already loaded)
 */

static void smm_place_entry_code(const unsigned int num_cpus)
{
	unsigned int i;
	unsigned int size;

	/* start at 1, the first CPU stub code is already there */
	size = cpus[0].code_end - cpus[0].code_start;
	for (i = 1; i < num_cpus; i++) {
		memcpy((int *)cpus[i].code_start, (int *)cpus[0].code_start, size);
		printk(BIOS_DEBUG,
			"SMM Module: placing smm entry code at %lx,  cpu # 0x%x\n",
			cpus[i].code_start, i);
		printk(BIOS_DEBUG, "%s: copying from %lx to %lx 0x%x bytes\n",
			__func__, cpus[0].code_start, cpus[i].code_start, size);
	}
}

static uintptr_t stack_top;
static size_t g_stack_size;

int smm_setup_stack(const uintptr_t perm_smbase, const size_t perm_smram_size,
		     const unsigned int total_cpus, const size_t stack_size)
{
	/* Need a minimum stack size and alignment. */
	if (stack_size <= SMM_MINIMUM_STACK_SIZE || (stack_size & 3) != 0) {
		printk(BIOS_ERR, "%s: need minimum stack size\n", __func__);
		return -1;
	}

	const size_t total_stack_size = total_cpus * stack_size;
	if (total_stack_size >= perm_smram_size) {
		printk(BIOS_ERR, "%s: Stack won't fit smram\n", __func__);
		return -1;
	}
	stack_top = perm_smbase + total_stack_size;
	g_stack_size = stack_size;
	return 0;
}

/*
 * Place the staggered entry points for each CPU. The entry points are
 * staggered by the per CPU SMM save state size extending down from
 * SMM_ENTRY_OFFSET.
 */
static void smm_stub_place_staggered_entry_points(const struct smm_loader_params *params)
{
	if (params->num_concurrent_save_states > 1)
		smm_place_entry_code(params->num_concurrent_save_states);
}

/*
 * The stub setup code assumes it is completely contained within the
 * default SMRAM size (0x10000) for the default SMI handler (entry at
 * 0x30000), but no assumption should be made for the permanent SMI handler.
 * The placement of CPU entry points for permanent handler are determined
 * by the number of CPUs in the system and the amount of SMRAM.
 * There are potentially 2 regions to place
 * within the default SMRAM size:
 * 1. Save state areas
 * 2. Stub code
 *
 * The save state always lives at the top of the CPUS smbase (and the entry
 * point is at offset 0x8000). This allows only a certain number of CPUs with
 * staggered entry points until the save state area comes down far enough to
 * overwrite/corrupt the entry code (stub code). Therefore, an SMM map is
 * created to avoid this corruption, see smm_create_map() above.
 * This module setup code works for the default (0x30000) SMM handler setup and the
 * permanent SMM handler.
 * The CPU stack is decided at runtime in the stub and is treaded as a continuous
 * region. As this might not fit the default SMRAM region, the same region used
 * by the permanent handler can be used during relocation.
 */
static int smm_module_setup_stub(const uintptr_t smbase, const size_t smm_size,
				 struct smm_loader_params *params,
				 void *const fxsave_area)
{
	struct rmodule smm_stub;
	if (rmodule_parse(&_binary_smmstub_start, &smm_stub)) {
		printk(BIOS_ERR, "%s: unable to parse smm stub\n", __func__);
		return -1;
	}
	const size_t stub_size = rmodule_memory_size(&smm_stub);

	/* Some sanity check */
	if (stub_size >= SMM_ENTRY_OFFSET) {
		printk(BIOS_ERR, "%s: Stub too large\n", __func__);
		return -1;
	}

	const uintptr_t smm_stub_loc = smbase + SMM_ENTRY_OFFSET;
	if (rmodule_load((void *)smm_stub_loc, &smm_stub)) {
		printk(BIOS_ERR, "%s: load module failed\n", __func__);
		return -1;
	}

	struct smm_stub_params *stub_params = rmodule_parameters(&smm_stub);
	stub_params->stack_top = stack_top;
	stub_params->stack_size = g_stack_size;
	stub_params->c_handler = (uintptr_t)params->handler;
	stub_params->fxsave_area = (uintptr_t)fxsave_area;
	stub_params->fxsave_area_size = FXSAVE_SIZE;

	/* Initialize the APIC id to CPU number table to be 1:1 */
	for (int i = 0; i < params->num_cpus; i++)
		stub_params->apic_id_to_cpu[i] = i;

	/* Allow the initiator to manipulate SMM stub parameters. */
	params->stub_params = stub_params;

	printk(BIOS_DEBUG, "%s: stack_top = 0x%x\n", __func__, stub_params->stack_top);
	printk(BIOS_DEBUG, "%s: per cpu stack_size = 0x%x\n", __func__,
	       stub_params->stack_size);
	printk(BIOS_DEBUG, "%s: runtime.start32_offset = 0x%x\n", __func__,
	       stub_params->start32_offset);
	printk(BIOS_DEBUG, "%s: runtime.smm_size = 0x%zx\n", __func__, smm_size);

	smm_stub_place_staggered_entry_points(params);

	printk(BIOS_DEBUG, "SMM Module: stub loaded at %lx. Will call %p\n", smm_stub_loc,
	       params->handler);
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
	uintptr_t smram = SMM_DEFAULT_BASE;
	printk(BIOS_SPEW, "%s: enter\n", __func__);
	/* There can't be more than 1 concurrent save state for the relocation
	 * handler because all CPUs default to 0x30000 as SMBASE. */
	if (params->num_concurrent_save_states > 1)
		return -1;

	/* A handler has to be defined to call for relocation. */
	if (params->handler == NULL)
		return -1;

	/* Since the relocation handler always uses stack, adjust the number
	 * of concurrent stack users to be CONFIG_MAX_CPUS. */
	if (params->num_cpus == 0)
		params->num_cpus = CONFIG_MAX_CPUS;

	printk(BIOS_SPEW, "%s: exit\n", __func__);
	return smm_module_setup_stub(smram, SMM_DEFAULT_SIZE,
				     params, fxsave_area_relocation);
}

static void setup_smihandler_params(struct smm_runtime *mod_params,
				    uintptr_t smram_base,
				    uintptr_t smram_size,
				    struct smm_loader_params *loader_params)
{
	mod_params->smbase = smram_base;
	mod_params->smm_size = smram_size;
	mod_params->save_state_size = loader_params->cpu_save_state_size;
	mod_params->num_cpus = loader_params->num_cpus;
	mod_params->gnvs_ptr = (uint32_t)(uintptr_t)acpi_get_gnvs();
	const struct cbmem_entry *cbmemc;
	if (CONFIG(CONSOLE_CBMEM) && (cbmemc = cbmem_entry_find(CBMEM_ID_CONSOLE))) {
		mod_params->cbmemc = cbmem_entry_start(cbmemc);
		mod_params->cbmemc_size = cbmem_entry_size(cbmemc);
	} else {
		mod_params->cbmemc = 0;
		mod_params->cbmemc_size = 0;
	}

	for (int i = 0; i < loader_params->num_cpus; i++)
		mod_params->save_state_top[i] = cpus[i].ss_top;
}

/*
 *The SMM module is placed within the provided region in the following
 * manner:
 * +-----------------+ <- smram + size
 * | BIOS resource   |
 * | list (STM)      |
 * +-----------------+
 * |  fxsave area    |
 * +-----------------+
 * |  smi handler    |
 * |      ...        |
 * +-----------------+ <- cpu0
 * |    stub code    | <- cpu1
 * |    stub code    | <- cpu2
 * |    stub code    | <- cpu3, etc
 * |                 |
 * |                 |
 * |                 |
 * |    stacks       |
 * +-----------------+ <- smram start

 * It should be noted that this algorithm will not work for
 * SMM_DEFAULT_SIZE SMRAM regions such as the A segment. This algorithm
 * expects a region large enough to encompass the handler and stacks
 * as well as the SMM_DEFAULT_SIZE.
 */
static int smm_load_module_tseg(const uintptr_t smram_base, const size_t smram_size,
				struct smm_loader_params *params)
{
	if (smram_size <= SMM_DEFAULT_SIZE)
		return -1;

	struct rmodule smi_handler;
	if (rmodule_parse(&_binary_smm_start, &smi_handler))
		return -1;

	const uintptr_t smram_top = smram_base + smram_size;

	const size_t stm_size =
		CONFIG(STM) ? CONFIG_MSEG_SIZE - CONFIG_BIOS_RESOURCE_LIST_SIZE : 0;
	const uintptr_t stm_base = CONFIG(STM) ? smram_top - stm_size : 0;
	if (stm_size) {
		printk(BIOS_DEBUG,     "STM          [0x%lx-0x%lx[\n", stm_base,
		       stm_base + stm_size);

		printk(BIOS_DEBUG, "MSEG size     0x%x\n", CONFIG_MSEG_SIZE);
		printk(BIOS_DEBUG, "BIOS res list 0x%x\n", CONFIG_BIOS_RESOURCE_LIST_SIZE);
	}
	const size_t fx_save_area_size = CONFIG(SSE) ? FXSAVE_SIZE * params->num_cpus : 0;
	const uintptr_t fx_save_area_base =
		CONFIG(SSE) ? smram_top - stm_size - fx_save_area_size : 0;
	if (fx_save_area_size)
		printk(BIOS_DEBUG, "fx_save      [0x%lx-0x%lx[\n", fx_save_area_base,
		       fx_save_area_base + fx_save_area_size);
	const size_t handler_size = rmodule_memory_size(&smi_handler);
	const size_t handler_alignment = rmodule_load_alignment(&smi_handler);
	const uintptr_t handler_base =
		ALIGN_DOWN(smram_top - stm_size - fx_save_area_size - handler_size,
			   handler_alignment);
	printk(BIOS_DEBUG, "smihandler   [0x%lx-0x%lx[\n", handler_base,
	       handler_base + handler_size);

	if (handler_base <= smram_base) {
		printk(BIOS_ERR, "Permanent handler won't FIT smram\n");
		return -1;
	}

	const uintptr_t stub_segment_base = handler_base - SMM_CODE_SEGMENT_SIZE;
	if (!smm_create_map(stub_segment_base, params->num_concurrent_save_states, params)) {
		printk(BIOS_ERR, "%s: Error creating CPU map\n", __func__);
		return -1;
	}

	const uintptr_t lowest_stub = cpus[params->num_concurrent_save_states - 1].code_start;
	const uintptr_t smm_stack_top =
		smram_base + params->num_concurrent_save_states * CONFIG_SMM_MODULE_STACK_SIZE;
	printk(BIOS_DEBUG, "cpu stacks   [0x%lx-0x%lx[\n", smram_base, smm_stack_top);
	if (lowest_stub < smm_stack_top) {
		printk(BIOS_ERR, "SMM stubs won't fit in SMRAM 0x%lx\n",
		       cpus[params->num_concurrent_save_states - 1].code_start);
		return -1;
	}

	if (rmodule_load((void *)handler_base, &smi_handler))
		return -1;

	struct smm_runtime *smihandler_params = rmodule_parameters(&smi_handler);
	params->handler = rmodule_entry(&smi_handler);
	setup_smihandler_params(smihandler_params, smram_base, smram_size, params);

	return smm_module_setup_stub(stub_segment_base, smram_size, params,
				     (void *)fx_save_area_base);
}

/*
 *The SMM module is placed within the provided region in the following
 * manner:
 * +-----------------+ <- smram + size == 0xB0000
 * |  save states    |
 * +-----------------+
 * |  fxsave area    |
 * +-----------------+
 * |  smi handler    | (or below the stubs if there is more space there)
 * |      ...        |
 * +-----------------+ <- cpu0
 * |    stub code    | <- cpu1
 * |    stub code    | <- cpu2
 * |    stub code    | <- cpu3, etc
 * |                 |
 * |                 |
 * |                 |
 * |    stacks       |
 * +-----------------+ <- smram start = 0xA0000
 */
static int smm_load_module_aseg(const uintptr_t smram_base, const size_t smram_size,
				struct smm_loader_params *params)
{
	if (smram_size != SMM_DEFAULT_SIZE)
		return -1;

	struct rmodule smi_handler;
	if (rmodule_parse(&_binary_smm_start, &smi_handler))
		return -1;

	if (!smm_create_map(smram_base, params->num_concurrent_save_states, params)) {
		printk(BIOS_ERR, "%s: Error creating CPU map\n", __func__);
		return -1;
	}

	const uintptr_t smm_stack_top =
		smram_base + params->num_concurrent_save_states * CONFIG_SMM_MODULE_STACK_SIZE;
	printk(BIOS_DEBUG, "cpu stacks   [0x%lx-0x%lx[\n", smram_base, smm_stack_top);
	if (smm_stack_top > cpus[params->num_concurrent_save_states - 1].code_start) {
		printk(BIOS_ERR, "stack won't fit in smram\n");
		return -1;
	}

	const uintptr_t save_state_bottom =
		cpus[params->num_concurrent_save_states - 1].ss_start;
	const size_t fx_save_area_size = CONFIG(SSE) ? FXSAVE_SIZE * params->num_cpus : 0;
	const uintptr_t fx_save_area_base =
		CONFIG(SSE) ? save_state_bottom - fx_save_area_size : 0;
	if (fx_save_area_size) {
		printk(BIOS_DEBUG, "fx_save      [0x%lx-0x%lx[\n", fx_save_area_base,
		       fx_save_area_base + fx_save_area_size);

		if (fx_save_area_base < cpus[0].code_end) {
			printk(BIOS_ERR, "fxsave won't fit in smram\n");
			return -1;
		}
	}

	const size_t top_space = save_state_bottom - fx_save_area_size - cpus[0].code_end;
	const size_t bottom_space =
		cpus[params->num_concurrent_save_states - 1].code_start - smm_stack_top;
	const bool use_top = top_space >= bottom_space;

	const size_t handler_size = rmodule_memory_size(&smi_handler);
	const size_t handler_alignment = rmodule_load_alignment(&smi_handler);
	uintptr_t handler_base;
	if (use_top) {
		handler_base = ALIGN_DOWN(save_state_bottom - fx_save_area_size - handler_size,
					  handler_alignment);
		if (handler_base < cpus[0].code_end) {
			printk(BIOS_ERR, "handler won't fit in top of smram\n");
			return -1;
		}
	} else {
		handler_base = ALIGN_UP(stack_top, handler_alignment);
		const uintptr_t handler_top = handler_base + handler_size;
		if (handler_top > cpus[params->num_concurrent_save_states - 1].code_start) {
			printk(BIOS_ERR, "handler won't fit in bottom of smram\n");
			return -1;
		}
	}
	printk(BIOS_DEBUG, "handler      [0x%lx-0x%lx[\n", handler_base,
	       handler_base + handler_size);

	if (rmodule_load((void *)handler_base, &smi_handler))
		return -1;

	struct smm_runtime *smihandler_params = rmodule_parameters(&smi_handler);
	params->handler = rmodule_entry(&smi_handler);
	setup_smihandler_params(smihandler_params, smram_base, smram_size, params);

	return smm_module_setup_stub(smram_base, smram_size, params,
				     (void *)fx_save_area_base);
}


int smm_load_module(const uintptr_t smram_base, const size_t smram_size,
		    struct smm_loader_params *params)
{
	if (CONFIG(SMM_ASEG))
		return smm_load_module_aseg(smram_base, smram_size, params);
	else if (CONFIG(SMM_TSEG))
		return smm_load_module_tseg(smram_base, smram_size, params);

	return -1;
}
