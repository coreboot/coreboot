/* SPDX-License-Identifier: GPL-2.0-only */

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
	uintptr_t entry;
	uintptr_t ss_start;
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
static int smm_create_map(uintptr_t smbase, unsigned int num_cpus,
			const struct smm_loader_params *params)
{
	unsigned int i;
	struct rmodule smm_stub;
	unsigned int ss_size = params->per_cpu_save_state_size, stub_size;
	unsigned int smm_entry_offset = SMM_ENTRY_OFFSET;
	unsigned int seg_count = 0, segments = 0, available;
	unsigned int cpus_in_segment = 0;
	unsigned int base = smbase;

	if (rmodule_parse(&_binary_smmstub_start, &smm_stub)) {
		printk(BIOS_ERR, "%s: unable to get SMM module size\n", __func__);
		return 0;
	}

	stub_size = rmodule_memory_size(&smm_stub);
	/* How many CPUs can fit into one 64K segment? */
	available = 0xFFFF - smm_entry_offset - ss_size - stub_size;
	if (available > 0) {
		cpus_in_segment = available / ss_size;
		/* minimum segments needed will always be 1 */
		segments = num_cpus / cpus_in_segment + 1;
		printk(BIOS_DEBUG,
			"%s: cpus allowed in one segment %d\n", __func__, cpus_in_segment);
		printk(BIOS_DEBUG,
			"%s: min # of segments needed %d\n", __func__, segments);
	} else {
		printk(BIOS_ERR, "%s: not enough space in SMM to setup all CPUs\n", __func__);
		printk(BIOS_ERR, "    save state & stub size need to be reduced\n");
		printk(BIOS_ERR, "    or increase SMRAM size\n");
		return 0;
	}

	if (ARRAY_SIZE(cpus) < num_cpus) {
		printk(BIOS_ERR,
			"%s: increase MAX_CPUS in Kconfig\n", __func__);
		return 0;
	}

	for (i = 0; i < num_cpus; i++) {
		cpus[i].smbase = base;
		cpus[i].entry = base + smm_entry_offset;
		cpus[i].ss_start = cpus[i].entry + (smm_entry_offset - ss_size);
		cpus[i].code_start = cpus[i].entry;
		cpus[i].code_end = cpus[i].entry + stub_size;
		cpus[i].active = 1;
		base -= ss_size;
		seg_count++;
		if (seg_count >= cpus_in_segment) {
			base -= smm_entry_offset;
			seg_count = 0;
		}
	}

	if (CONFIG_DEFAULT_CONSOLE_LOGLEVEL >= BIOS_DEBUG) {
		seg_count = 0;
		for (i = 0; i < num_cpus; i++) {
			printk(BIOS_DEBUG, "CPU 0x%x\n", i);
			printk(BIOS_DEBUG,
				"    smbase %lx  entry %lx\n",
				cpus[i].smbase, cpus[i].entry);
			printk(BIOS_DEBUG,
				"           ss_start %lx  code_end %lx\n",
				cpus[i].ss_start, cpus[i].code_end);
			seg_count++;
			if (seg_count >= cpus_in_segment) {
				printk(BIOS_DEBUG,
					"-------------NEW CODE SEGMENT --------------\n");
				seg_count = 0;
			}
		}
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
 * input: smbase - this is the smbase of the first cpu not the smbase
 *        where tseg starts (aka smram_start). All CPUs code segment
 *        and stack will be below this point except for the common
 *        SMI handler which is one segment above
 * input: num_cpus - number of cpus that need relocation including
 *        the first CPU (though its code is already loaded)
 * input: top of stack (stacks work downward by default in Intel HW)
 * output: return -1, if runtime smi code could not be installed. In
 *         this case SMM will not work and any SMI's generated will
 *         cause a CPU shutdown or general protection fault because
 *         the appropriate smi handling code was not installed
 */

static int smm_place_entry_code(uintptr_t smbase, unsigned int num_cpus,
				uintptr_t stack_top, const struct smm_loader_params *params)
{
	unsigned int i;
	unsigned int size;

	/*
	 * Ensure there was enough space and the last CPUs smbase
	 * did not encroach upon the stack. Stack top is smram start
	 * + size of stack.
	 */
	if (cpus[num_cpus].active) {
		if (cpus[num_cpus - 1].smbase + SMM_ENTRY_OFFSET < stack_top) {
			printk(BIOS_ERR, "%s: stack encroachment\n", __func__);
				printk(BIOS_ERR, "%s: smbase %lx, stack_top %lx\n",
				       __func__, cpus[num_cpus].smbase, stack_top);
				return 0;
		}
	}

	printk(BIOS_INFO, "%s: smbase %lx, stack_top %lx\n",
		__func__, cpus[num_cpus-1].smbase, stack_top);

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
	return 1;
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
static int smm_stub_place_staggered_entry_points(char *base,
	const struct smm_loader_params *params, const struct rmodule *smm_stub)
{
	size_t stub_entry_offset;
	int rc = 1;
	stub_entry_offset = rmodule_entry_offset(smm_stub);
	/* Each CPU now has its own stub code, which enters protected mode,
	 * sets up the stack, and then jumps to common SMI handler
	 */
	if (params->num_concurrent_save_states > 1 || stub_entry_offset != 0) {
		rc = smm_place_entry_code((uintptr_t)base,
					  params->num_concurrent_save_states,
					  stack_top, params);
	}
	return rc;
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
	size_t total_save_state_size;
	size_t smm_stub_size;
	uintptr_t smm_stub_loc;
	size_t size;
	uintptr_t base;
	size_t i;
	struct smm_stub_params *stub_params;
	struct rmodule smm_stub;
	base = smbase;
	size = smm_size;

	/* The number of concurrent stacks cannot exceed CONFIG_MAX_CPUS. */
	if (params->num_cpus > CONFIG_MAX_CPUS) {
		printk(BIOS_ERR, "%s: not enough stacks\n", __func__);
		return -1;
	}

	/* Fail if can't parse the smm stub rmodule. */
	if (rmodule_parse(&_binary_smmstub_start, &smm_stub)) {
		printk(BIOS_ERR, "%s: unable to parse smm stub\n", __func__);
		return -1;
	}

	/* Adjust remaining size to account for save state. */
	total_save_state_size = params->per_cpu_save_state_size *
				params->num_concurrent_save_states;
	if (total_save_state_size > size) {
		printk(BIOS_ERR,
			"%s: more state save space needed:need -> %zx:available->%zx\n",
			__func__, total_save_state_size, size);
		return -1;
	}

	size -= total_save_state_size;

	/* The save state size encroached over the first SMM entry point. */
	if (size <= SMM_ENTRY_OFFSET) {
		printk(BIOS_ERR, "%s: encroachment over SMM entry point\n", __func__);
		printk(BIOS_ERR, "%s: state save size: %zx : smm_entry_offset -> %zx\n",
		       __func__, size, (size_t)SMM_ENTRY_OFFSET);
		return -1;
	}

	smm_stub_size = rmodule_memory_size(&smm_stub);

	/* Put the stub at the main entry point */
	smm_stub_loc = base + SMM_ENTRY_OFFSET;

	/* Stub is too big to fit. */
	if (smm_stub_size > (size - SMM_ENTRY_OFFSET)) {
		printk(BIOS_ERR, "%s: stub is too big to fit\n", __func__);
		return -1;
	}

	if (stack_top == 0) {
		printk(BIOS_ERR, "%s: error assigning stacks\n", __func__);
		return -1;
	}
	/* Load the stub. */
	if (rmodule_load((void *)smm_stub_loc, &smm_stub)) {
		printk(BIOS_ERR, "%s: load module failed\n", __func__);
		return -1;
	}

	if (!smm_stub_place_staggered_entry_points((void *)base, params, &smm_stub)) {
		printk(BIOS_ERR, "%s: staggered entry points failed\n", __func__);
		return -1;
	}

	/* Setup the parameters for the stub code. */
	stub_params = rmodule_parameters(&smm_stub);
	stub_params->stack_top = stack_top;
	stub_params->stack_size = g_stack_size;
	stub_params->c_handler = (uintptr_t)params->handler;
	stub_params->fxsave_area = (uintptr_t)fxsave_area;
	stub_params->fxsave_area_size = FXSAVE_SIZE;

	printk(BIOS_DEBUG,
		"%s: stack_top = 0x%x\n", __func__, stub_params->stack_top);
	printk(BIOS_DEBUG, "%s: per cpu stack_size = 0x%x\n",
		__func__, stub_params->stack_size);
	printk(BIOS_DEBUG, "%s: runtime.start32_offset = 0x%x\n", __func__,
		stub_params->start32_offset);
	printk(BIOS_DEBUG, "%s: runtime.smm_size = 0x%zx\n",
		__func__, smm_size);

	/* Initialize the APIC id to CPU number table to be 1:1 */
	for (i = 0; i < params->num_cpus; i++)
		stub_params->apic_id_to_cpu[i] = i;

	/* Allow the initiator to manipulate SMM stub parameters. */
	params->stub_params = stub_params;

	printk(BIOS_DEBUG, "SMM Module: stub loaded at %lx. Will call %p\n",
	       smm_stub_loc, params->handler);
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

static int smm_load_module_aseg(const uintptr_t smram_base, const size_t smram_size,
				struct smm_loader_params *params);

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
int smm_load_module(const uintptr_t smram_base, const size_t smram_size,
		    struct smm_loader_params *params)
{
	struct rmodule smm_mod;
	struct smm_runtime *handler_mod_params;
	size_t total_stack_size;
	size_t handler_size;
	size_t module_alignment;
	size_t alignment_size;
	size_t fxsave_size;
	void *fxsave_area;
	size_t total_size = 0;
	uintptr_t base; /* The base for the permanent handler */
	const struct cbmem_entry *cbmemc;

	if (CONFIG(SMM_ASEG))
		return smm_load_module_aseg(smram_base, smram_size, params);

	if (smram_size <= SMM_DEFAULT_SIZE)
		return -1;

	/* Load main SMI handler at the top of SMRAM
	 * everything else will go below
	 */
	base = smram_base;
	base += smram_size;

	/* Fail if can't parse the smm rmodule. */
	if (rmodule_parse(&_binary_smm_start, &smm_mod))
		return -1;

	/* Clear SMM region */
	if (CONFIG(DEBUG_SMI))
		memset((void *)smram_base, 0xcd, smram_size);

	total_stack_size = stack_top - smram_base;
	total_size += total_stack_size;
	/* Stacks are the base of SMRAM */

	/* MSEG starts at the top of SMRAM and works down */
	if (CONFIG(STM)) {
		base -= CONFIG_MSEG_SIZE + CONFIG_BIOS_RESOURCE_LIST_SIZE;
		total_size += CONFIG_MSEG_SIZE + CONFIG_BIOS_RESOURCE_LIST_SIZE;
	}

	/* FXSAVE goes below MSEG */
	if (CONFIG(SSE)) {
		fxsave_size = FXSAVE_SIZE * params->num_cpus;
		fxsave_area = (char *)base - fxsave_size;
		base -= fxsave_size;
		total_size += fxsave_size;
	} else {
		fxsave_size = 0;
		fxsave_area = NULL;
	}

	handler_size = rmodule_memory_size(&smm_mod);
	base -= handler_size;
	total_size += handler_size;
	module_alignment = rmodule_load_alignment(&smm_mod);
	alignment_size = module_alignment - (base % module_alignment);
	if (alignment_size != module_alignment) {
		handler_size += alignment_size;
		base += alignment_size;
	}

	printk(BIOS_DEBUG,
		"%s: total_smm_space_needed %zx, available -> %zx\n",
		 __func__, total_size, smram_size);

	/* Does the required amount of memory exceed the SMRAM region size? */
	if (total_size > smram_size) {
		printk(BIOS_ERR, "%s: need more SMRAM\n", __func__);
		return -1;
	}
	if (handler_size > SMM_CODE_SEGMENT_SIZE) {
		printk(BIOS_ERR, "%s: increase SMM_CODE_SEGMENT_SIZE: handler_size = %zx\n",
			__func__, handler_size);
		return -1;
	}

	if (rmodule_load((void *)base, &smm_mod))
		return -1;

	params->handler = rmodule_entry(&smm_mod);
	handler_mod_params = rmodule_parameters(&smm_mod);
	handler_mod_params->smbase = smram_base;
	handler_mod_params->smm_size = smram_size;
	handler_mod_params->save_state_size = params->real_cpu_save_state_size;
	handler_mod_params->num_cpus = params->num_cpus;
	handler_mod_params->gnvs_ptr = (uintptr_t)acpi_get_gnvs();

	if (CONFIG(CONSOLE_CBMEM) && (cbmemc = cbmem_entry_find(CBMEM_ID_CONSOLE))) {
		handler_mod_params->cbmemc = cbmem_entry_start(cbmemc);
		handler_mod_params->cbmemc_size = cbmem_entry_size(cbmemc);
	} else {
		handler_mod_params->cbmemc = 0;
		handler_mod_params->cbmemc_size = 0;
	}

	printk(BIOS_DEBUG, "%s: smram_start: 0x%lx\n",  __func__, smram_base);
	printk(BIOS_DEBUG, "%s: smram_end: %lx\n", __func__, smram_base + smram_size);
	printk(BIOS_DEBUG, "%s: handler start %p\n",
		 __func__, params->handler);
	printk(BIOS_DEBUG, "%s: handler_size %zx\n",
		 __func__, handler_size);
	printk(BIOS_DEBUG, "%s: fxsave_area %p\n",
		 __func__, fxsave_area);
	printk(BIOS_DEBUG, "%s: fxsave_size %zx\n",
		 __func__, fxsave_size);
	printk(BIOS_DEBUG, "%s: CONFIG_MSEG_SIZE 0x%x\n",
		 __func__, CONFIG_MSEG_SIZE);
	printk(BIOS_DEBUG, "%s: CONFIG_BIOS_RESOURCE_LIST_SIZE 0x%x\n",
		 __func__, CONFIG_BIOS_RESOURCE_LIST_SIZE);

	printk(BIOS_DEBUG, "%s: handler_mod_params.smbase = 0x%x\n", __func__,
	       handler_mod_params->smbase);
	printk(BIOS_DEBUG, "%s: per_cpu_save_state_size = 0x%x\n", __func__,
	       handler_mod_params->save_state_size);
	printk(BIOS_DEBUG, "%s: num_cpus = 0x%x\n", __func__, handler_mod_params->num_cpus);
	printk(BIOS_DEBUG, "%s: cbmemc = %p, cbmemc_size = %#x\n", __func__,
	       handler_mod_params->cbmemc, handler_mod_params->cbmemc_size);
	printk(BIOS_DEBUG, "%s: total_save_state_size = 0x%x\n", __func__,
	       (handler_mod_params->save_state_size * handler_mod_params->num_cpus));

	/* CPU 0 smbase goes first, all other CPUs
	 * will be staggered below
	 */
	base -= SMM_CODE_SEGMENT_SIZE;
	printk(BIOS_DEBUG, "%s: cpu0 entry: %lx\n",  __func__, base);

	if (!smm_create_map(base, params->num_concurrent_save_states, params)) {
		printk(BIOS_ERR, "%s: Error creating CPU map\n", __func__);
		return -1;
	}

	for (int i = 0; i < params->num_cpus; i++) {
		handler_mod_params->save_state_top[i] =
			cpus[i].ss_start + params->per_cpu_save_state_size;
	}

	return smm_module_setup_stub(base, smram_size, params, fxsave_area);
}

/*
 *The SMM module is placed within the provided region in the following
 * manner:
 * +-----------------+ <- smram + size == 0x10000
 * |  save states    |
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
 * +-----------------+ <- smram start = 0xA0000
 */
static int smm_load_module_aseg(const uintptr_t smram_base, const size_t smram_size,
				struct smm_loader_params *params)
{
	struct rmodule smm_mod;
	struct smm_runtime *handler_mod_params;

	if (smram_size != SMM_DEFAULT_SIZE)
		return -1;

	if (smram_base != SMM_BASE)
		return -1;

	/* Fail if can't parse the smm rmodule. */
	if (rmodule_parse(&_binary_smm_start, &smm_mod))
		return -1;

	if (!smm_create_map(smram_base, params->num_concurrent_save_states, params)) {
		printk(BIOS_ERR, "%s: Error creating CPU map\n", __func__);
		return -1;
	}

	const uintptr_t entry0_end = cpus[0].code_end;
	const uintptr_t save_state_base = cpus[params->num_cpus - 1].ss_start;
	const size_t fxsave_size = FXSAVE_SIZE * params->num_cpus;
	const uintptr_t fxsave_base = ALIGN_DOWN(save_state_base - fxsave_size, 16);

	if (fxsave_base <= entry0_end) {
		printk(BIOS_ERR, "%s, fxsave %lx won't fit smram\n", __func__, fxsave_base);
		return -1;
	}

	const size_t handler_size = rmodule_memory_size(&smm_mod);
	const size_t module_alignment = rmodule_load_alignment(&smm_mod);
	const uintptr_t module_base = ALIGN_DOWN(fxsave_base - handler_size, module_alignment);

	if (module_base <= entry0_end) {
		printk(BIOS_ERR, "%s, module won't fit smram\n", __func__);
		return -1;
	}

	if (rmodule_load((void *)module_base, &smm_mod))
		return -1;

	params->handler = rmodule_entry(&smm_mod);
	handler_mod_params = rmodule_parameters(&smm_mod);
	handler_mod_params->smbase = smram_base;
	handler_mod_params->smm_size = smram_size;
	handler_mod_params->save_state_size = params->real_cpu_save_state_size;
	handler_mod_params->num_cpus = params->num_cpus;
	handler_mod_params->gnvs_ptr = (uintptr_t)acpi_get_gnvs();

	for (int i = 0; i < params->num_cpus; i++) {
		handler_mod_params->save_state_top[i] =
			cpus[i].ss_start + params->per_cpu_save_state_size;
	}

	printk(BIOS_DEBUG, "%s: smram_start: 0x%lx\n",  __func__, smram_base);
	printk(BIOS_DEBUG, "%s: smram_end: %lx\n", __func__, smram_base + smram_size);
	printk(BIOS_DEBUG, "%s: handler start %p\n", __func__, params->handler);
	printk(BIOS_DEBUG, "%s: handler_size %zx\n", __func__, handler_size);
	printk(BIOS_DEBUG, "%s: fxsave_area %lx\n", __func__, fxsave_base);
	printk(BIOS_DEBUG, "%s: fxsave_size %zx\n", __func__, fxsave_size);

	printk(BIOS_DEBUG, "%s: handler_mod_params.smbase = 0x%x\n", __func__,
	       handler_mod_params->smbase);
	printk(BIOS_DEBUG, "%s: per_cpu_save_state_size = 0x%x\n", __func__,
	       handler_mod_params->save_state_size);
	printk(BIOS_DEBUG, "%s: num_cpus = 0x%x\n", __func__, handler_mod_params->num_cpus);
	printk(BIOS_DEBUG, "%s: total_save_state_size = 0x%x\n", __func__,
	       (handler_mod_params->save_state_size * handler_mod_params->num_cpus));

	return smm_module_setup_stub(smram_base, smram_size, params, (void *)fxsave_base);
}
