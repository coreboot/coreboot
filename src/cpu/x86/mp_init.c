/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <string.h>
#include <rmodule.h>
#include <commonlib/helpers.h>
#include <cpu/cpu.h>
#include <cpu/intel/microcode.h>
#include <cpu/x86/cache.h>
#include <cpu/x86/gdt.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/name.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/mtrr.h>
#include <cpu/x86/smm.h>
#include <cpu/x86/mp.h>
#include <delay.h>
#include <device/device.h>
#include <device/path.h>
#include <smp/atomic.h>
#include <smp/spinlock.h>
#include <symbols.h>
#include <timer.h>
#include <thread.h>
#include <types.h>

#include <security/intel/stm/SmmStm.h>

struct mp_callback {
	void (*func)(void *);
	void *arg;
	int logical_cpu_number;
};

static char processor_name[49];

/*
 * A mp_flight_record details a sequence of calls for the APs to perform
 * along with the BSP to coordinate sequencing. Each flight record either
 * provides a barrier for each AP before calling the callback or the APs
 * are allowed to perform the callback without waiting. Regardless, each
 * record has the cpus_entered field incremented for each record. When
 * the BSP observes that the cpus_entered matches the number of APs
 * the bsp_call is called with bsp_arg and upon returning releases the
 * barrier allowing the APs to make further progress.
 *
 * Note that ap_call() and bsp_call() can be NULL. In the NULL case the
 * callback will just not be called.
 */
struct mp_flight_record {
	atomic_t barrier;
	atomic_t cpus_entered;
	void (*ap_call)(void);
	void (*bsp_call)(void);
} __aligned(CACHELINE_SIZE);

#define _MP_FLIGHT_RECORD(barrier_, ap_func_, bsp_func_) \
	{							\
		.barrier = ATOMIC_INIT(barrier_),		\
		.cpus_entered = ATOMIC_INIT(0),			\
		.ap_call = ap_func_,				\
		.bsp_call = bsp_func_,				\
	}

#define MP_FR_BLOCK_APS(ap_func_, bsp_func_) \
	_MP_FLIGHT_RECORD(0, ap_func_, bsp_func_)

#define MP_FR_NOBLOCK_APS(ap_func_, bsp_func_) \
	_MP_FLIGHT_RECORD(1, ap_func_, bsp_func_)

/* The mp_params structure provides the arguments to the mp subsystem
 * for bringing up APs. */
struct mp_params {
	int num_cpus; /* Total cpus include BSP */
	int parallel_microcode_load;
	const void *microcode_pointer;
	/* Flight plan  for APs and BSP. */
	struct mp_flight_record *flight_plan;
	int num_records;
};

/* This needs to match the layout in the .module_parametrs section. */
struct sipi_params {
	uint16_t gdtlimit;
	uint32_t gdt;
	uint16_t unused;
	uint32_t idt_ptr;
	uint32_t per_cpu_segment_descriptors;
	uint32_t per_cpu_segment_selector;
	uint32_t stack_top;
	uint32_t stack_size;
	uint32_t microcode_lock; /* 0xffffffff means parallel loading. */
	uint32_t microcode_ptr;
	uint32_t msr_table_ptr;
	uint32_t msr_count;
	uint32_t c_handler;
	atomic_t ap_count;
} __packed;

/* This also needs to match the assembly code for saved MSR encoding. */
struct saved_msr {
	uint32_t index;
	uint32_t lo;
	uint32_t hi;
} __packed;

/* The sipi vector rmodule is included in the ramstage using 'objdump -B'. */
extern char _binary_sipi_vector_start[];

/* The SIPI vector is loaded at the SMM_DEFAULT_BASE. The reason is that the
 * memory range is already reserved so the OS cannot use it. That region is
 * free to use for AP bringup before SMM is initialized. */
static const uintptr_t sipi_vector_location = SMM_DEFAULT_BASE;
static const int sipi_vector_location_size = SMM_DEFAULT_SIZE;

struct mp_flight_plan {
	int num_records;
	struct mp_flight_record *records;
};

static int global_num_aps;
static struct mp_flight_plan mp_info;

static inline void barrier_wait(atomic_t *b)
{
	while (atomic_read(b) == 0)
		asm ("pause");
	mfence();
}

static inline void release_barrier(atomic_t *b)
{
	mfence();
	atomic_set(b, 1);
}

static enum cb_err wait_for_aps(atomic_t *val, int target, int total_delay,
			int delay_step)
{
	int delayed = 0;
	while (atomic_read(val) != target) {
		udelay(delay_step);
		delayed += delay_step;
		if (delayed >= total_delay) {
			/* Not all APs ready before timeout */
			return CB_ERR;
		}
	}

	/* APs ready before timeout */
	printk(BIOS_SPEW, "APs are ready after %dus\n", delayed);
	return CB_SUCCESS;
}

static void ap_do_flight_plan(void)
{
	int i;

	for (i = 0; i < mp_info.num_records; i++) {
		struct mp_flight_record *rec = &mp_info.records[i];

		atomic_inc(&rec->cpus_entered);
		barrier_wait(&rec->barrier);

		if (rec->ap_call != NULL)
			rec->ap_call();
	}
}

static void park_this_cpu(void *unused)
{
	stop_this_cpu();
}

static struct bus *g_cpu_bus;

/* By the time APs call ap_init() caching has been setup, and microcode has
 * been loaded. */
static asmlinkage void ap_init(unsigned int index)
{
	/* Ensure the local APIC is enabled */
	enable_lapic();
	setup_lapic_interrupts();

	struct device *dev;
	int i = 0;
	for (dev = g_cpu_bus->children; dev; dev = dev->sibling)
		if (i++ == index)
			break;

	if (!dev) {
		printk(BIOS_ERR, "Could not find allocated device for index %u\n", index);
		return;
	}

	set_cpu_info(index, dev);

	struct cpu_info *info = cpu_info();
	cpu_add_map_entry(info->index);

	/* Fix up APIC id with reality. */
	info->cpu->path.apic.apic_id = lapicid();

	if (cpu_is_intel())
		printk(BIOS_INFO, "AP: slot %zu apic_id %x, MCU rev: 0x%08x\n", info->index,
		       info->cpu->path.apic.apic_id, get_current_microcode_rev());
	else
		printk(BIOS_INFO, "AP: slot %zu apic_id %x\n", info->index,
		       info->cpu->path.apic.apic_id);

	/* Walk the flight plan */
	ap_do_flight_plan();

	/* Park the AP. */
	park_this_cpu(NULL);
}

static void setup_default_sipi_vector_params(struct sipi_params *sp)
{
	sp->gdt = (uintptr_t)&gdt;
	sp->gdtlimit = (uintptr_t)&gdt_end - (uintptr_t)&gdt - 1;
	sp->idt_ptr = (uintptr_t)&idtarg;
	sp->per_cpu_segment_descriptors = (uintptr_t)&per_cpu_segment_descriptors;
	sp->per_cpu_segment_selector = per_cpu_segment_selector;
	sp->stack_size = CONFIG_STACK_SIZE;
	sp->stack_top = ALIGN_DOWN((uintptr_t)&_estack, CONFIG_STACK_SIZE);
}

static const unsigned int fixed_mtrrs[NUM_FIXED_MTRRS] = {
	MTRR_FIX_64K_00000, MTRR_FIX_16K_80000, MTRR_FIX_16K_A0000,
	MTRR_FIX_4K_C0000, MTRR_FIX_4K_C8000, MTRR_FIX_4K_D0000,
	MTRR_FIX_4K_D8000, MTRR_FIX_4K_E0000, MTRR_FIX_4K_E8000,
	MTRR_FIX_4K_F0000, MTRR_FIX_4K_F8000,
};

static inline struct saved_msr *save_msr(int index, struct saved_msr *entry)
{
	msr_t msr;

	msr = rdmsr(index);
	entry->index = index;
	entry->lo = msr.lo;
	entry->hi = msr.hi;

	/* Return the next entry. */
	entry++;
	return entry;
}

static int save_bsp_msrs(char *start, int size)
{
	int msr_count;
	int num_var_mtrrs;
	struct saved_msr *msr_entry;
	int i;

	/* Determine number of MTRRs need to be saved. */
	num_var_mtrrs = get_var_mtrr_count();

	/* 2 * num_var_mtrrs for base and mask. +1 for IA32_MTRR_DEF_TYPE. */
	msr_count = 2 * num_var_mtrrs + NUM_FIXED_MTRRS + 1;

	if ((msr_count * sizeof(struct saved_msr)) > size) {
		printk(BIOS_CRIT, "Cannot mirror all %d msrs.\n", msr_count);
		return -1;
	}

	fixed_mtrrs_expose_amd_rwdram();

	msr_entry = (void *)start;
	for (i = 0; i < NUM_FIXED_MTRRS; i++)
		msr_entry = save_msr(fixed_mtrrs[i], msr_entry);

	for (i = 0; i < num_var_mtrrs; i++) {
		msr_entry = save_msr(MTRR_PHYS_BASE(i), msr_entry);
		msr_entry = save_msr(MTRR_PHYS_MASK(i), msr_entry);
	}

	msr_entry = save_msr(MTRR_DEF_TYPE_MSR, msr_entry);

	fixed_mtrrs_hide_amd_rwdram();

	/* Tell static analysis we know value is left unused. */
	(void)msr_entry;

	return msr_count;
}

static atomic_t *load_sipi_vector(struct mp_params *mp_params)
{
	struct rmodule sipi_mod;
	int module_size;
	int num_msrs;
	struct sipi_params *sp;
	char *mod_loc = (void *)sipi_vector_location;
	const int loc_size = sipi_vector_location_size;
	atomic_t *ap_count = NULL;

	if (rmodule_parse(&_binary_sipi_vector_start, &sipi_mod)) {
		printk(BIOS_CRIT, "Unable to parse sipi module.\n");
		return ap_count;
	}

	if (rmodule_entry_offset(&sipi_mod) != 0) {
		printk(BIOS_CRIT, "SIPI module entry offset is not 0!\n");
		return ap_count;
	}

	if (rmodule_load_alignment(&sipi_mod) != 4096) {
		printk(BIOS_CRIT, "SIPI module load alignment(%d) != 4096.\n",
		       rmodule_load_alignment(&sipi_mod));
		return ap_count;
	}

	module_size = rmodule_memory_size(&sipi_mod);

	/* Align to 4 bytes. */
	module_size = ALIGN_UP(module_size, 4);

	if (module_size > loc_size) {
		printk(BIOS_CRIT, "SIPI module size (%d) > region size (%d).\n",
		       module_size, loc_size);
		return ap_count;
	}

	num_msrs = save_bsp_msrs(&mod_loc[module_size], loc_size - module_size);

	if (num_msrs < 0) {
		printk(BIOS_CRIT, "Error mirroring BSP's msrs.\n");
		return ap_count;
	}

	if (rmodule_load(mod_loc, &sipi_mod)) {
		printk(BIOS_CRIT, "Unable to load SIPI module.\n");
		return ap_count;
	}

	sp = rmodule_parameters(&sipi_mod);

	if (sp == NULL) {
		printk(BIOS_CRIT, "SIPI module has no parameters.\n");
		return ap_count;
	}

	setup_default_sipi_vector_params(sp);
	/* Setup MSR table. */
	sp->msr_table_ptr = (uintptr_t)&mod_loc[module_size];
	sp->msr_count = num_msrs;
	/* Provide pointer to microcode patch. */
	sp->microcode_ptr = (uintptr_t)mp_params->microcode_pointer;
	/* Pass on ability to load microcode in parallel. */
	if (mp_params->parallel_microcode_load)
		sp->microcode_lock = ~0;
	else
		sp->microcode_lock = 0;
	sp->c_handler = (uintptr_t)&ap_init;
	ap_count = &sp->ap_count;
	atomic_set(ap_count, 0);

	return ap_count;
}

static int allocate_cpu_devices(struct bus *cpu_bus, struct mp_params *p)
{
	int i;
	int max_cpus;
	struct cpu_info *info;

	max_cpus = p->num_cpus;
	if (max_cpus > CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT, "CPU count(%d) exceeds CONFIG_MAX_CPUS(%d)\n",
		       max_cpus, CONFIG_MAX_CPUS);
		max_cpus = CONFIG_MAX_CPUS;
	}

	info = cpu_info();
	for (i = 1; i < max_cpus; i++) {
		/* Assuming linear APIC space allocation. AP will set its own
		   APIC id in the ap_init() path above. */
		struct device *new = add_cpu_device(cpu_bus, info->cpu->path.apic.apic_id + i, 1);
		if (new == NULL) {
			printk(BIOS_CRIT, "Could not allocate CPU device\n");
			max_cpus--;
			continue;
		}
		new->name = processor_name;
	}

	return max_cpus;
}

static enum cb_err apic_wait_timeout(int total_delay, int delay_step)
{
	int total = 0;

	while (lapic_busy()) {
		udelay(delay_step);
		total += delay_step;
		if (total >= total_delay) {
			/* LAPIC not ready before the timeout */
			return CB_ERR;
		}
	}

	/* LAPIC ready before the timeout */
	return CB_SUCCESS;
}

/* Send Startup IPI to APs */
static enum cb_err send_sipi_to_aps(int ap_count, atomic_t *num_aps, int sipi_vector)
{
	if (lapic_busy()) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...\n");
		if (apic_wait_timeout(1000 /* 1 ms */, 50) != CB_SUCCESS) {
			printk(BIOS_ERR, "timed out. Aborting.\n");
			return CB_ERR;
		}
		printk(BIOS_DEBUG, "done.\n");
	}

	lapic_send_ipi_others(LAPIC_INT_ASSERT | LAPIC_DM_STARTUP | sipi_vector);
	printk(BIOS_DEBUG, "Waiting for SIPI to complete...\n");
	if (apic_wait_timeout(10000 /* 10 ms */, 50 /* us */) != CB_SUCCESS) {
		printk(BIOS_ERR, "timed out.\n");
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "done.\n");
	return CB_SUCCESS;
}

static enum cb_err start_aps(struct bus *cpu_bus, int ap_count, atomic_t *num_aps)
{
	int sipi_vector;
	/* Max location is 4KiB below 1MiB */
	const int max_vector_loc = ((1 << 20) - (1 << 12)) >> 12;

	if (ap_count == 0)
		return CB_SUCCESS;

	/* The vector is sent as a 4k aligned address in one byte. */
	sipi_vector = sipi_vector_location >> 12;

	if (sipi_vector > max_vector_loc) {
		printk(BIOS_CRIT, "SIPI vector too large! 0x%08x\n",
		       sipi_vector);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "Attempting to start %d APs\n", ap_count);

	if (lapic_busy()) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...\n");
		if (apic_wait_timeout(1000 /* 1 ms */, 50) != CB_SUCCESS) {
			printk(BIOS_ERR, "timed out. Aborting.\n");
			return CB_ERR;
		}
		printk(BIOS_DEBUG, "done.\n");
	}

	/* Send INIT IPI to all but self. */
	lapic_send_ipi_others(LAPIC_INT_ASSERT | LAPIC_DM_INIT);

	if (!CONFIG(X86_INIT_NEED_1_SIPI)) {
		printk(BIOS_DEBUG, "Waiting for 10ms after sending INIT.\n");
		mdelay(10);

		/* Send 1st Startup IPI (SIPI) */
		if (send_sipi_to_aps(ap_count, num_aps, sipi_vector) != CB_SUCCESS)
			return CB_ERR;

		/* Wait for CPUs to check in. */
		wait_for_aps(num_aps, ap_count, 200 /* us */, 15 /* us */);
	}

	/* Send final SIPI */
	if (send_sipi_to_aps(ap_count, num_aps, sipi_vector) != CB_SUCCESS)
		return CB_ERR;

	/* Wait for CPUs to check in. */
	if (wait_for_aps(num_aps, ap_count, 400000 /* 400 ms */, 50 /* us */) != CB_SUCCESS) {
		printk(BIOS_ERR, "Not all APs checked in: %d/%d.\n",
		       atomic_read(num_aps), ap_count);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

static enum cb_err bsp_do_flight_plan(struct mp_params *mp_params)
{
	int i;
	enum cb_err ret = CB_SUCCESS;
	/*
	 * Set time out for flight plan to a huge minimum value (>=1 second).
	 * CPUs with many APs may take longer if there is contention for
	 * resources such as UART, so scale the time out up by increments of
	 * 100ms if needed.
	 */
	const int timeout_us = MAX(1000000, 100000 * mp_params->num_cpus);
	const int step_us = 100;
	int num_aps = mp_params->num_cpus - 1;
	struct stopwatch sw;

	stopwatch_init(&sw);

	for (i = 0; i < mp_params->num_records; i++) {
		struct mp_flight_record *rec = &mp_params->flight_plan[i];

		/* Wait for APs if the record is not released. */
		if (atomic_read(&rec->barrier) == 0) {
			/* Wait for the APs to check in. */
			if (wait_for_aps(&rec->cpus_entered, num_aps,
					 timeout_us, step_us) != CB_SUCCESS) {
				printk(BIOS_ERR, "MP record %d timeout.\n", i);
				ret = CB_ERR;
			}
		}

		if (rec->bsp_call != NULL)
			rec->bsp_call();

		release_barrier(&rec->barrier);
	}

	printk(BIOS_INFO, "%s done after %lld msecs.\n", __func__,
	       stopwatch_duration_msecs(&sw));
	return ret;
}

static enum cb_err init_bsp(struct bus *cpu_bus)
{
	struct cpu_info *info;

	/* Print processor name */
	fill_processor_name(processor_name);
	printk(BIOS_INFO, "CPU: %s.\n", processor_name);

	/* Ensure the local APIC is enabled */
	enable_lapic();
	setup_lapic_interrupts();

	struct device *bsp = add_cpu_device(cpu_bus, lapicid(), 1);
	if (bsp == NULL) {
		printk(BIOS_CRIT, "Failed to find or allocate BSP struct device\n");
		return CB_ERR;
	}

	/* Find the device structure for the boot CPU. */
	set_cpu_info(0, bsp);
	info = cpu_info();
	info->cpu = bsp;
	info->cpu->name = processor_name;

	if (info->index != 0) {
		printk(BIOS_CRIT, "BSP index(%zd) != 0!\n", info->index);
		return CB_ERR;
	}

	/* Track BSP in cpu_map structures. */
	cpu_add_map_entry(info->index);
	return CB_SUCCESS;
}

/*
 * mp_init() will set up the SIPI vector and bring up the APs according to
 * mp_params. Each flight record will be executed according to the plan. Note
 * that the MP infrastructure uses SMM default area without saving it. It's
 * up to the chipset or mainboard to either e820 reserve this area or save this
 * region prior to calling mp_init() and restoring it after mp_init returns.
 *
 * At the time mp_init() is called the MTRR MSRs are mirrored into APs then
 * caching is enabled before running the flight plan.
 *
 * The MP initialization has the following properties:
 * 1. APs are brought up in parallel.
 * 2. The ordering of coreboot CPU number and APIC ids is not deterministic.
 *    Therefore, one cannot rely on this property or the order of devices in
 *    the device tree unless the chipset or mainboard know the APIC ids
 *    a priori.
 */
static enum cb_err mp_init(struct bus *cpu_bus, struct mp_params *p)
{
	int num_cpus;
	atomic_t *ap_count;

	g_cpu_bus = cpu_bus;

	if (init_bsp(cpu_bus) != CB_SUCCESS) {
		printk(BIOS_CRIT, "Setting up BSP failed\n");
		return CB_ERR;
	}

	if (p == NULL || p->flight_plan == NULL || p->num_records < 1) {
		printk(BIOS_CRIT, "Invalid MP parameters\n");
		return CB_ERR;
	}

	/* We just need to run things on the BSP */
	if (!CONFIG(SMP))
		return bsp_do_flight_plan(p);

	/* Default to currently running CPU. */
	num_cpus = allocate_cpu_devices(cpu_bus, p);

	if (num_cpus < p->num_cpus) {
		printk(BIOS_CRIT,
		       "ERROR: More cpus requested (%d) than supported (%d).\n",
		       p->num_cpus, num_cpus);
		return CB_ERR;
	}

	/* Copy needed parameters so that APs have a reference to the plan. */
	mp_info.num_records = p->num_records;
	mp_info.records = p->flight_plan;

	/* Load the SIPI vector. */
	ap_count = load_sipi_vector(p);
	if (ap_count == NULL)
		return CB_ERR;

	/* Make sure SIPI data hits RAM so the APs that come up will see
	 * the startup code even if the caches are disabled.  */
	wbinvd();

	/* Start the APs providing number of APs and the cpus_entered field. */
	global_num_aps = p->num_cpus - 1;
	if (start_aps(cpu_bus, global_num_aps, ap_count) != CB_SUCCESS) {
		mdelay(1000);
		printk(BIOS_DEBUG, "%d/%d eventually checked in?\n",
		       atomic_read(ap_count), global_num_aps);
		return CB_ERR;
	}

	/* Walk the flight plan for the BSP. */
	return bsp_do_flight_plan(p);
}

void smm_initiate_relocation_parallel(void)
{
	if (lapic_busy()) {
		printk(BIOS_DEBUG, "Waiting for ICR not to be busy...");
		if (apic_wait_timeout(1000 /* 1 ms */, 50) != CB_SUCCESS) {
			printk(BIOS_DEBUG, "timed out. Aborting.\n");
			return;
		}
		printk(BIOS_DEBUG, "done.\n");
	}

	lapic_send_ipi_self(LAPIC_INT_ASSERT | LAPIC_DM_SMI);

	if (lapic_busy()) {
		if (apic_wait_timeout(1000 /* 1 ms */, 100 /* us */) != CB_SUCCESS) {
			printk(BIOS_DEBUG, "SMI Relocation timed out.\n");
			return;
		}
	}
	printk(BIOS_DEBUG, "Relocation complete.\n");
}

DECLARE_SPIN_LOCK(smm_relocation_lock);

/* Send SMI to self with single user serialization. */
void smm_initiate_relocation(void)
{
	spin_lock(&smm_relocation_lock);
	smm_initiate_relocation_parallel();
	spin_unlock(&smm_relocation_lock);
}

struct mp_state {
	struct mp_ops ops;
	int cpu_count;
	uintptr_t perm_smbase;
	size_t perm_smsize;
	size_t smm_save_state_size;
	uintptr_t reloc_start32_offset;
	bool do_smm;
} mp_state;

static bool is_smm_enabled(void)
{
	return CONFIG(HAVE_SMI_HANDLER) && mp_state.do_smm;
}

static void smm_disable(void)
{
	mp_state.do_smm = false;
}

static void smm_enable(void)
{
	if (CONFIG(HAVE_SMI_HANDLER))
		mp_state.do_smm = true;
}

/*
 * This code is built as part of ramstage, but it actually runs in SMM. This
 * means that ENV_SMM is 0, but we are actually executing in the environment
 * setup by the smm_stub.
 */
static void asmlinkage smm_do_relocation(void *arg)
{
	const struct smm_module_params *p;
	int cpu;
	const uintptr_t curr_smbase = SMM_DEFAULT_BASE;
	uintptr_t perm_smbase;

	p = arg;
	cpu = p->cpu;

	if (cpu >= CONFIG_MAX_CPUS) {
		printk(BIOS_CRIT,
		       "Invalid CPU number assigned in SMM stub: %d\n", cpu);
		return;
	}

	/*
	 * The permanent handler runs with all cpus concurrently. Precalculate
	 * the location of the new SMBASE. If using SMM modules then this
	 * calculation needs to match that of the module loader.
	 */
	perm_smbase = smm_get_cpu_smbase(cpu);
	if (!perm_smbase) {
		printk(BIOS_ERR, "%s: bad SMBASE for CPU %d\n", __func__, cpu);
		return;
	}

	/* Setup code checks this callback for validity. */
	printk(BIOS_INFO, "%s : curr_smbase 0x%x perm_smbase 0x%x, cpu = %d\n",
		__func__, (int)curr_smbase, (int)perm_smbase, cpu);
	mp_state.ops.relocation_handler(cpu, curr_smbase, perm_smbase);

	if (CONFIG(STM)) {
		uintptr_t mseg;

		mseg = mp_state.perm_smbase +
			(mp_state.perm_smsize - CONFIG_MSEG_SIZE);

		stm_setup(mseg, p->cpu,
				perm_smbase,
				mp_state.perm_smbase,
				mp_state.reloc_start32_offset);
	}
}

static void adjust_smm_apic_id_map(struct smm_loader_params *smm_params)
{
	int i;
	struct smm_stub_params *stub_params = smm_params->stub_params;

	for (i = 0; i < CONFIG_MAX_CPUS; i++)
		stub_params->apic_id_to_cpu[i] = cpu_get_apic_id(i);
}

static enum cb_err install_relocation_handler(int num_cpus, size_t save_state_size)
{
	if (CONFIG(X86_SMM_SKIP_RELOCATION_HANDLER))
		return CB_SUCCESS;

	struct smm_loader_params smm_params = {
		.num_cpus = num_cpus,
		.cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = 1,
		.handler = smm_do_relocation,
	};

	if (smm_setup_relocation_handler(&smm_params)) {
		printk(BIOS_ERR, "%s: smm setup failed\n", __func__);
		return CB_ERR;
	}
	adjust_smm_apic_id_map(&smm_params);

	mp_state.reloc_start32_offset = smm_params.stub_params->start32_offset;

	return CB_SUCCESS;
}

static enum cb_err install_permanent_handler(int num_cpus, uintptr_t smbase,
				     size_t smsize, size_t save_state_size)
{
	/*
	 * All the CPUs will relocate to permanaent handler now. Set parameters
	 * needed for all CPUs. The placement of each CPUs entry point is
	 * determined by the loader. This code simply provides the beginning of
	 * SMRAM region, the number of CPUs who will use the handler, the stack
	 * size and save state size for each CPU.
	 */
	struct smm_loader_params smm_params = {
		.num_cpus = num_cpus,
		.cpu_save_state_size = save_state_size,
		.num_concurrent_save_states = num_cpus,
	};

	printk(BIOS_DEBUG, "Installing permanent SMM handler to 0x%08lx\n", smbase);

	if (smm_load_module(smbase, smsize, &smm_params))
		return CB_ERR;

	adjust_smm_apic_id_map(&smm_params);

	return CB_SUCCESS;
}

/* Load SMM handlers as part of MP flight record. */
static void load_smm_handlers(void)
{
	const size_t save_state_size = mp_state.smm_save_state_size;

	/* Do nothing if SMM is disabled.*/
	if (!is_smm_enabled())
		return;

	if (smm_setup_stack(mp_state.perm_smbase, mp_state.perm_smsize, mp_state.cpu_count,
			    CONFIG_SMM_MODULE_STACK_SIZE)) {
		printk(BIOS_ERR, "Unable to install SMM relocation handler.\n");
		smm_disable();
	}

	/* Install handlers. */
	if (install_relocation_handler(mp_state.cpu_count, save_state_size) != CB_SUCCESS) {
		printk(BIOS_ERR, "Unable to install SMM relocation handler.\n");
		smm_disable();
	}

	if (install_permanent_handler(mp_state.cpu_count, mp_state.perm_smbase,
				      mp_state.perm_smsize, save_state_size) != CB_SUCCESS) {
		printk(BIOS_ERR, "Unable to install SMM permanent handler.\n");
		smm_disable();
	}

	/* Ensure the SMM handlers hit DRAM before performing first SMI. */
	wbinvd();

	/*
	 * Indicate that the SMM handlers have been loaded and MP
	 * initialization is about to start.
	 */
	if (is_smm_enabled() && mp_state.ops.pre_mp_smm_init != NULL)
		mp_state.ops.pre_mp_smm_init();
}

/* Trigger SMM as part of MP flight record. */
static void trigger_smm_relocation(void)
{
	/* Do nothing if SMM is disabled.*/
	if (!is_smm_enabled() || mp_state.ops.per_cpu_smm_trigger == NULL)
		return;
	/* Trigger SMM mode for the currently running processor. */
	mp_state.ops.per_cpu_smm_trigger();
}

static struct mp_callback *ap_callbacks[CONFIG_MAX_CPUS];

enum AP_STATUS {
	/* AP takes the task but not yet finishes */
	AP_BUSY = 1,
	/* AP finishes the task or no task to run yet */
	AP_NOT_BUSY
};

static atomic_t ap_status[CONFIG_MAX_CPUS];

static struct mp_callback *read_callback(struct mp_callback **slot)
{
	struct mp_callback *ret;

	asm volatile ("mov	%1, %0\n"
		: "=r" (ret)
		: "m" (*slot)
		: "memory"
	);
	return ret;
}

static void store_callback(struct mp_callback **slot, struct mp_callback *val)
{
	asm volatile ("mov	%1, %0\n"
		: "=m" (*slot)
		: "r" (val)
		: "memory"
	);
}

static enum cb_err run_ap_work(struct mp_callback *val, long expire_us, bool wait_ap_finish)
{
	int i;
	int cpus_accepted, cpus_finish;
	struct stopwatch sw;
	int cur_cpu;

	if (!CONFIG(PARALLEL_MP_AP_WORK)) {
		printk(BIOS_ERR, "APs already parked. PARALLEL_MP_AP_WORK not selected.\n");
		return CB_ERR;
	}

	cur_cpu = cpu_index();

	if (cur_cpu < 0) {
		printk(BIOS_ERR, "Invalid CPU index.\n");
		return CB_ERR;
	}

	/* Signal to all the APs to run the func. */
	for (i = 0; i < ARRAY_SIZE(ap_callbacks); i++) {
		if (cur_cpu == i)
			continue;
		store_callback(&ap_callbacks[i], val);
	}
	mfence();

	/* Wait for all the APs to signal back that call has been accepted. */
	if (expire_us > 0)
		stopwatch_init_usecs_expire(&sw, expire_us);

	do {
		cpus_accepted = 0;
		cpus_finish = 0;

		for (i = 0; i < ARRAY_SIZE(ap_callbacks); i++) {
			if (cur_cpu == i)
				continue;

			if (read_callback(&ap_callbacks[i]) == NULL) {
				cpus_accepted++;
				/* Only increase cpus_finish if AP took the task and not busy */
				if (atomic_read(&ap_status[i]) == AP_NOT_BUSY)
					cpus_finish++;
			}
		}

		/*
		 * if wait_ap_finish is true, need to make sure all CPUs finish task and return
		 * else just need to make sure all CPUs take task
		 */
		if (cpus_accepted == global_num_aps)
			if (!wait_ap_finish || (cpus_finish == global_num_aps))
				return CB_SUCCESS;

	} while (expire_us <= 0 || !stopwatch_expired(&sw));

	printk(BIOS_CRIT, "CRITICAL ERROR: AP call expired. %d/%d CPUs accepted.\n",
		cpus_accepted, global_num_aps);
	return CB_ERR;
}

static void ap_wait_for_instruction(void)
{
	struct mp_callback lcb;
	struct mp_callback **per_cpu_slot;
	int cur_cpu;

	if (!CONFIG(PARALLEL_MP_AP_WORK))
		return;

	cur_cpu = cpu_index();

	if (cur_cpu < 0) {
		printk(BIOS_ERR, "Invalid CPU index.\n");
		return;
	}

	per_cpu_slot = &ap_callbacks[cur_cpu];

	/* Init ap_status[cur_cpu] to AP_NOT_BUSY and ready to take job */
	atomic_set(&ap_status[cur_cpu], AP_NOT_BUSY);

	while (1) {
		struct mp_callback *cb = read_callback(per_cpu_slot);

		if (cb == NULL) {
			asm ("pause");
			continue;
		}
		/*
		 * Set ap_status to AP_BUSY before store_callback(per_cpu_slot, NULL).
		 * it's to let BSP know APs take tasks and busy to avoid race condition.
		 */
		atomic_set(&ap_status[cur_cpu], AP_BUSY);

		/* Copy to local variable before signaling consumption. */
		memcpy(&lcb, cb, sizeof(lcb));
		mfence();
		store_callback(per_cpu_slot, NULL);

		if (lcb.logical_cpu_number == MP_RUN_ON_ALL_CPUS ||
				(cur_cpu == lcb.logical_cpu_number))
			lcb.func(lcb.arg);

		atomic_set(&ap_status[cur_cpu], AP_NOT_BUSY);
	}
}

enum cb_err mp_run_on_aps(void (*func)(void *), void *arg, int logical_cpu_num,
		long expire_us)
{
	struct mp_callback lcb = { .func = func, .arg = arg,
				.logical_cpu_number = logical_cpu_num};
	return run_ap_work(&lcb, expire_us, false);
}

static enum cb_err mp_run_on_aps_and_wait_for_complete(void (*func)(void *), void *arg,
		int logical_cpu_num, long expire_us)
{
	struct mp_callback lcb = { .func = func, .arg = arg,
				.logical_cpu_number = logical_cpu_num};
	return run_ap_work(&lcb, expire_us, true);
}

enum cb_err mp_run_on_all_aps(void (*func)(void *), void *arg, long expire_us,
			      bool run_parallel)
{
	int ap_index, bsp_index;

	if (run_parallel)
		return mp_run_on_aps(func, arg, MP_RUN_ON_ALL_CPUS, expire_us);

	bsp_index = cpu_index();

	const int total_threads = global_num_aps + 1; /* +1 for BSP */

	for (ap_index = 0; ap_index < total_threads; ap_index++) {
		/* skip if BSP */
		if (ap_index == bsp_index)
			continue;
		if (mp_run_on_aps(func, arg, ap_index, expire_us) != CB_SUCCESS)
			return CB_ERR;
	}

	return CB_SUCCESS;
}

enum cb_err mp_run_on_all_cpus(void (*func)(void *), void *arg)
{
	/* Run on BSP first. */
	func(arg);

	/* For up to 1 second for AP to finish previous work. */
	return mp_run_on_aps(func, arg, MP_RUN_ON_ALL_CPUS, 1000 * USECS_PER_MSEC);
}

enum cb_err mp_run_on_all_cpus_synchronously(void (*func)(void *), void *arg)
{
	/* Run on BSP first. */
	func(arg);

	/* For up to 1 second per AP (console can be slow) to finish previous work. */
	return mp_run_on_aps_and_wait_for_complete(func, arg, MP_RUN_ON_ALL_CPUS,
						   1000 * USECS_PER_MSEC * global_num_aps);
}

enum cb_err mp_park_aps(void)
{
	struct stopwatch sw;
	enum cb_err ret;
	long duration_msecs;

	stopwatch_init(&sw);

	ret = mp_run_on_aps(park_this_cpu, NULL, MP_RUN_ON_ALL_CPUS,
				1000 * USECS_PER_MSEC);

	duration_msecs = stopwatch_duration_msecs(&sw);

	if (ret == CB_SUCCESS)
		printk(BIOS_DEBUG, "%s done after %ld msecs.\n", __func__,
		       duration_msecs);
	else
		printk(BIOS_ERR, "%s failed after %ld msecs.\n", __func__,
		       duration_msecs);

	return ret;
}

static struct mp_flight_record mp_steps[] = {
	/* Once the APs are up load the SMM handlers. */
	MP_FR_BLOCK_APS(NULL, load_smm_handlers),
	/* Perform SMM relocation. */
	MP_FR_NOBLOCK_APS(trigger_smm_relocation, trigger_smm_relocation),
	/* Initialize each CPU through the driver framework. */
	MP_FR_BLOCK_APS(cpu_initialize, cpu_initialize),
	/* Wait for APs to finish then optionally start looking for work. */
	MP_FR_BLOCK_APS(ap_wait_for_instruction, NULL),
};

static void fill_mp_state_smm(struct mp_state *state, const struct mp_ops *ops)
{
	if (ops->get_smm_info != NULL)
		ops->get_smm_info(&state->perm_smbase, &state->perm_smsize,
				  &state->smm_save_state_size);

	/*
	 * Make sure there is enough room for the SMM descriptor
	 */
	if (CONFIG(STM)) {
		state->smm_save_state_size +=
			ALIGN_UP(sizeof(TXT_PROCESSOR_SMM_DESCRIPTOR), 0x100);
	}

	/*
	 * Default to smm_initiate_relocation() if trigger callback isn't
	 * provided.
	 */
	if (ops->per_cpu_smm_trigger == NULL)
		mp_state.ops.per_cpu_smm_trigger = smm_initiate_relocation;
}

static void fill_mp_state(struct mp_state *state, const struct mp_ops *ops)
{
	/*
	 * Make copy of the ops so that defaults can be set in the non-const
	 * structure if needed.
	 */
	memcpy(&state->ops, ops, sizeof(*ops));

	if (ops->get_cpu_count != NULL)
		state->cpu_count = ops->get_cpu_count();

	if (CONFIG(HAVE_SMI_HANDLER))
		fill_mp_state_smm(state, ops);
}

static enum cb_err do_mp_init_with_smm(struct bus *cpu_bus, const struct mp_ops *mp_ops)
{
	enum cb_err ret;
	void *default_smm_area;
	struct mp_params mp_params;

	if (mp_ops->pre_mp_init != NULL)
		mp_ops->pre_mp_init();

	fill_mp_state(&mp_state, mp_ops);

	memset(&mp_params, 0, sizeof(mp_params));

	if (mp_state.cpu_count <= 0) {
		printk(BIOS_ERR, "Invalid cpu_count: %d\n", mp_state.cpu_count);
		return CB_ERR;
	}

	/* Sanity check SMM state. */
	smm_enable();
	if (mp_state.perm_smsize == 0)
		smm_disable();
	if (mp_state.smm_save_state_size == 0)
		smm_disable();
	if (!CONFIG(X86_SMM_SKIP_RELOCATION_HANDLER) && mp_state.ops.relocation_handler == NULL)
		smm_disable();

	if (is_smm_enabled())
		printk(BIOS_INFO, "Will perform SMM setup.\n");

	mp_params.num_cpus = mp_state.cpu_count;
	/* Gather microcode information. */
	if (mp_state.ops.get_microcode_info != NULL)
		mp_state.ops.get_microcode_info(&mp_params.microcode_pointer,
			&mp_params.parallel_microcode_load);
	mp_params.flight_plan = &mp_steps[0];
	mp_params.num_records = ARRAY_SIZE(mp_steps);

	/* Perform backup of default SMM area when using SMM relocation handler. */
	if (!CONFIG(X86_SMM_SKIP_RELOCATION_HANDLER))
		default_smm_area = backup_default_smm_area();

	ret = mp_init(cpu_bus, &mp_params);

	if (!CONFIG(X86_SMM_SKIP_RELOCATION_HANDLER))
		restore_default_smm_area(default_smm_area);

	/* Signal callback on success if it's provided. */
	if (ret == CB_SUCCESS && mp_state.ops.post_mp_init != NULL)
		mp_state.ops.post_mp_init();

	return ret;
}

enum cb_err mp_init_with_smm(struct bus *cpu_bus, const struct mp_ops *mp_ops)
{
	enum cb_err ret = do_mp_init_with_smm(cpu_bus, mp_ops);

	if (ret != CB_SUCCESS)
		printk(BIOS_ERR, "MP initialization failure.\n");

	return ret;
}
