/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <rmodule.h>
#include <stdio.h>
#include <string.h>
#include <types.h>

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
	struct region ss;
	struct region stub_code;
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
		const size_t segment_number = i / cpus_per_segment;
		cpus[i].smbase = smbase - SMM_CODE_SEGMENT_SIZE * segment_number
			- needed_ss_size * (i % cpus_per_segment);
		cpus[i].stub_code.offset = cpus[i].smbase + SMM_ENTRY_OFFSET;
		cpus[i].stub_code.size = stub_size;
		cpus[i].ss.offset = cpus[i].smbase + SMM_CODE_SEGMENT_SIZE
			- params->cpu_save_state_size;
		cpus[i].ss.size = params->cpu_save_state_size;
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
	size_t size;

	/* start at 1, the first CPU stub code is already there */
	size = region_sz(&cpus[0].stub_code);
	for (i = 1; i < num_cpus; i++) {
		printk(BIOS_DEBUG,
		       "SMM Module: placing smm entry code at %zx,  cpu # 0x%x\n",
		       region_offset(&cpus[i].stub_code), i);
		memcpy((void *)region_offset(&cpus[i].stub_code),
		       (void *)region_offset(&cpus[0].stub_code), size);
		printk(BIOS_SPEW, "%s: copying from %zx to %zx 0x%zx bytes\n",
		       __func__, region_offset(&cpus[0].stub_code),
		       region_offset(&cpus[i].stub_code), size);
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
		mod_params->save_state_top[i] = region_end(&cpus[i].ss);
}

static void print_region(const char *name, const struct region region)
{
	printk(BIOS_DEBUG, "%-12s [0x%zx-0x%zx]\n", name, region_offset(&region),
	       region_end(&region));
}

/* STM + FX_SAVE + Handler + (Stub + Save state) * CONFIG_MAX_CPUS + stacks */
#define SMM_REGIONS_ARRAY_SIZE (1 + 1 + 1 + CONFIG_MAX_CPUS * 2 + 1)

static int append_and_check_region(const struct region smram,
				   const struct region region,
				   struct region *region_list,
				   const char *name)
{
	unsigned int region_counter = 0;
	for (; region_counter < SMM_REGIONS_ARRAY_SIZE; region_counter++)
		if (region_list[region_counter].size == 0)
			break;

	if (region_counter >= SMM_REGIONS_ARRAY_SIZE) {
		printk(BIOS_ERR, "Array used to check regions too small\n");
		return 1;
	}

	if (!region_is_subregion(&smram, &region)) {
		printk(BIOS_ERR, "%s not in SMM\n", name);
		return 1;
	}

	print_region(name, region);
	for (unsigned int i = 0; i < region_counter; i++) {
		if (region_overlap(&region_list[i], &region)) {
			printk(BIOS_ERR, "%s overlaps with a previous region\n", name);
			return 1;
		}
	}

	region_list[region_counter] = region;

	return 0;
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
 *
 * With CONFIG(SMM_TSEG) the stubs will be placed in the same segment as the
 * permanent handler and the stacks.
 */
int smm_load_module(const uintptr_t smram_base, const size_t smram_size,
		    struct smm_loader_params *params)
{
	if (CONFIG(SMM_ASEG) && (smram_base != SMM_BASE || smram_size != SMM_CODE_SEGMENT_SIZE)) {
		printk(BIOS_ERR, "SMM base & size are 0x%lx, 0x%zx, but must be 0x%x, 0x%x\n",
		       smram_base, smram_size, SMM_BASE, SMM_CODE_SEGMENT_SIZE);
		return -1;
	}
	/*
	 * Place in .bss to reduce stack usage.
	 * TODO: once CPU_INFO_V2 is used everywhere, use smaller stack for APs and move
	 * this back to the BSP stack.
	 */
	static struct region region_list[SMM_REGIONS_ARRAY_SIZE] = {};

	struct rmodule smi_handler;
	if (rmodule_parse(&_binary_smm_start, &smi_handler))
		return -1;

	const struct region smram = { .offset = smram_base, .size = smram_size };
	const uintptr_t smram_top = region_end(&smram);

	const size_t stm_size =
		CONFIG(STM) ? CONFIG_MSEG_SIZE + CONFIG_BIOS_RESOURCE_LIST_SIZE : 0;

	if (CONFIG(STM)) {
		struct region stm = {};
		stm.offset = smram_top - stm_size;
		stm.size = stm_size;
		if (append_and_check_region(smram, stm, region_list, "STM"))
			return -1;
		printk(BIOS_DEBUG, "MSEG size     0x%x\n", CONFIG_MSEG_SIZE);
		printk(BIOS_DEBUG, "BIOS res list 0x%x\n", CONFIG_BIOS_RESOURCE_LIST_SIZE);
	}

	const size_t fx_save_area_size = CONFIG(SSE) ? FXSAVE_SIZE * params->num_cpus : 0;
	struct region fx_save = {};
	if (CONFIG(SSE)) {
		fx_save.offset = smram_top - stm_size - fx_save_area_size;
		fx_save.size = fx_save_area_size;
		if (append_and_check_region(smram, fx_save, region_list, "FX_SAVE"))
			return -1;
	}

	const size_t handler_size = rmodule_memory_size(&smi_handler);
	const size_t handler_alignment = rmodule_load_alignment(&smi_handler);
	const uintptr_t handler_base =
		ALIGN_DOWN(smram_top - stm_size - fx_save_area_size - handler_size,
			   handler_alignment);
	struct region handler = {
		.offset = handler_base,
		.size = handler_size
	};
	if (append_and_check_region(smram, handler, region_list, "HANDLER"))
		return -1;

	uintptr_t stub_segment_base = handler_base - SMM_CODE_SEGMENT_SIZE;

	if (!smm_create_map(stub_segment_base, params->num_concurrent_save_states, params)) {
		printk(BIOS_ERR, "%s: Error creating CPU map\n", __func__);
		return -1;
	}
	for (unsigned int i = 0; i < params->num_concurrent_save_states; i++) {
		printk(BIOS_DEBUG, "\nCPU %u\n", i);
		char string[13];
		snprintf(string, sizeof(string), "  ss%d", i);
		if (append_and_check_region(smram, cpus[i].ss, region_list, string))
			return -1;
		snprintf(string, sizeof(string), "  stub%d", i);
		if (append_and_check_region(smram, cpus[i].stub_code, region_list, string))
			return -1;
	}

	struct region stacks = {
		.offset = smram_base,
		.size = params->num_concurrent_save_states * CONFIG_SMM_MODULE_STACK_SIZE
	};
	printk(BIOS_DEBUG, "\n");
	if (append_and_check_region(smram, stacks, region_list, "stacks"))
		return -1;

	if (rmodule_load((void *)handler_base, &smi_handler))
		return -1;

	struct smm_runtime *smihandler_params = rmodule_parameters(&smi_handler);
	params->handler = rmodule_entry(&smi_handler);
	setup_smihandler_params(smihandler_params, smram_base, smram_size, params);

	return smm_module_setup_stub(stub_segment_base, smram_size, params,
				     (void *)region_offset(&fx_save));
}
