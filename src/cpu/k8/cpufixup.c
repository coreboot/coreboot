/* Needed so the AMD K8 runs correctly.  */
#include <console/console.h>
#include <mem.h>
#include <cpu/p6/msr.h>
#include <cpu/k8/mtrr.h>
#include <device/device.h>
#include <device/chip.h>
#include <device/device.h>
#include <device/pci.h>
#include <smp/start_stop.h>
#include <string.h>
#include <cpu/p6/msr.h>
#include <cpu/p6/pgtbl.h>
#include <pc80/mc146818rtc.h>
#include <arch/smp/lapic.h>
#include "../../northbridge/amd/amdk8/amdk8.h"
#include "../../northbridge/amd/amdk8/cpu_rev.c"
#include "chip.h"

#define MCI_STATUS 0x401

static inline void disable_cache(void)
{
	unsigned int tmp;
	/* Disable cache */
	/* Write back the cache */
	asm volatile (
		"movl  %%cr0, %0\n\t"
		"orl  $0x40000000, %0\n\t"
		"wbinvd\n\t"
		"movl  %0, %%cr0\n\t"
		"wbinvd\n\t"
		:"=r" (tmp)
		::"memory");
}

static inline void enable_cache(void)
{
	unsigned int tmp;
	// turn cache back on. 
	asm volatile (
		"movl  %%cr0, %0\n\t"
		"andl  $0x9fffffff, %0\n\t"
		"movl  %0, %%cr0\n\t"
		:"=r" (tmp)
		::"memory");
}

static inline msr_t rdmsr_amd(unsigned index)
{
        msr_t result;
        __asm__ __volatile__ (
                "rdmsr"
                : "=a" (result.lo), "=d" (result.hi)
                : "c" (index), "D" (0x9c5a203a)
                );
        return result;
}

static inline void wrmsr_amd(unsigned index, msr_t msr)
{
        __asm__ __volatile__ (
                "wrmsr"
                : /* No outputs */
                : "c" (index), "a" (msr.lo), "d" (msr.hi), "D" (0x9c5a203a)
                );
}

#define MTRR_COUNT 8
#define ZERO_CHUNK_KB 0x800UL /* 2M */
#define TOLM_KB 0x400000UL

struct mtrr {
	msr_t base;
	msr_t mask;
};
struct mtrr_state {
	struct mtrr mtrrs[MTRR_COUNT];
	msr_t top_mem, top_mem2;
	msr_t def_type;
};

static void save_mtrr_state(struct mtrr_state *state)
{
	int i;
	for(i = 0; i < MTRR_COUNT; i++) {
		state->mtrrs[i].base = rdmsr(MTRRphysBase_MSR(i));
		state->mtrrs[i].mask = rdmsr(MTRRphysMask_MSR(i));
	}
	state->top_mem  = rdmsr(TOP_MEM);
	state->top_mem2 = rdmsr(TOP_MEM2);
	state->def_type = rdmsr(MTRRdefType_MSR);
}

static void restore_mtrr_state(struct mtrr_state *state)
{
	int i;
	disable_cache();

	for(i = 0; i < MTRR_COUNT; i++) {
		wrmsr(MTRRphysBase_MSR(i), state->mtrrs[i].base);
		wrmsr(MTRRphysMask_MSR(i), state->mtrrs[i].mask);
	}
	wrmsr(TOP_MEM,         state->top_mem);
	wrmsr(TOP_MEM2,        state->top_mem2);
	wrmsr(MTRRdefType_MSR, state->def_type);

	enable_cache();
}


#if 0
static void print_mtrr_state(struct mtrr_state *state)
{
	int i;
	for(i = 0; i < MTRR_COUNT; i++) {
		printk_debug("var mtrr %d: %08x%08x mask: %08x%08x\n",
			i,
			state->mtrrs[i].base.hi, state->mtrrs[i].base.lo,
			state->mtrrs[i].mask.hi, state->mtrrs[i].mask.lo);
	}
	printk_debug("top_mem:  %08x%08x\n",
		state->top_mem.hi, state->top_mem.lo);
	printk_debug("top_mem2: %08x%08x\n",
		state->top_mem2.hi, state->top_mem2.lo);
	printk_debug("def_type: %08x%08x\n",
		state->def_type.hi, state->def_type.lo);
}
#endif

static void set_init_ecc_mtrrs(void)
{
	msr_t msr;
	int i;

	disable_cache();

	/* First clear all of the msrs to be safe */
	for (i = 0; i < MTRR_COUNT; i++) {
		msr_t zero;
		zero.lo = zero.hi = 0;
		wrmsr(MTRRphysBase_MSR(i), zero);
		wrmsr(MTRRphysMask_MSR(i), zero);
	}

	/* Write back cache the first 1MB */
	msr.hi = 0x00000000;
	msr.lo = 0x00000000 | MTRR_TYPE_WRBACK;
	wrmsr(MTRRphysBase_MSR(0), msr);
	msr.hi = 0x000000ff;
	msr.lo = ~((CONFIG_LB_MEM_TOPK << 10) - 1) | 0x800;
	wrmsr(MTRRphysMask_MSR(0), msr);

	/* Set the default type to write combining */
	msr.hi = 0x00000000;
	msr.lo = 0xc00 | MTRR_TYPE_WRCOMB;
	wrmsr(MTRRdefType_MSR, msr);

	/* Set TOP_MEM to 4G */
	msr.hi = 0x00000001;
	msr.lo = 0x00000000;
	wrmsr(TOP_MEM, msr);

	enable_cache();
}

static void init_ecc_memory(void)
{
	unsigned long startk, begink, endk;
	unsigned long basek;
	struct mtrr_state mtrr_state;
	device_t f1_dev, f2_dev, f3_dev;
	int cpu_index, cpu_id, node_id;
	int enable_scrubbing;
	uint32_t dcl;
	cpu_id = this_processors_id();
	cpu_index = processor_index(cpu_id);
	/* For now there is a 1-1 mapping between node_id and cpu_id */
	node_id = cpu_id;

	f1_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 1));
	if (!f1_dev) {
		die("Cannot find cpu function 1\n");
	}
	f2_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 2));
	if (!f2_dev) {
		die("Cannot find cpu function 2\n");
	}
	f3_dev = dev_find_slot(0, PCI_DEVFN(0x18 + node_id, 3));
	if (!f3_dev) {
		die("Cannot find cpu function 3\n");
	}

	/* See if we scrubbing should be enabled */
	enable_scrubbing = 1;
	get_option(&enable_scrubbing, "hw_scrubber");

	/* Enable cache scrubbing at the lowest possible rate */
	if (enable_scrubbing) {
		pci_write_config32(f3_dev, SCRUB_CONTROL,
			(SCRUB_84ms << 16) | (SCRUB_84ms << 8) | (SCRUB_NONE << 0));
	} else {
		pci_write_config32(f3_dev, SCRUB_CONTROL,
			(SCRUB_NONE << 16) | (SCRUB_NONE << 8) | (SCRUB_NONE << 0));
		printk_debug("Scrubbing Disabled\n");
	}
	

	/* If ecc support is not enabled don't touch memory */
	dcl = pci_read_config32(f2_dev, DRAM_CONFIG_LOW);
	if (!(dcl & DCL_DimmEccEn)) {
		return;
	}

	startk = (pci_read_config32(f1_dev, 0x40 + (node_id*8)) & 0xffff0000) >> 2;
	endk   = ((pci_read_config32(f1_dev, 0x44 + (node_id*8)) & 0xffff0000) >> 2) + 0x4000;

	/* Don't start too early */
	begink = startk;
	if (begink < CONFIG_LB_MEM_TOPK) {
		begink = CONFIG_LB_MEM_TOPK;
	}
	printk_debug("Clearing memory %uK - %uK: ", startk, endk);

	/* Save the normal state */
	save_mtrr_state(&mtrr_state);

	/* Switch to the init ecc state */
	set_init_ecc_mtrrs();
	disable_lapic();

	/* Walk through 2M chunks and zero them */
	for(basek = begink; basek < endk; basek = ((basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1))) {
		unsigned long limitk;
		unsigned long size;
		void *addr;

		/* Report every 64M */
		if ((basek % (64*1024)) == 0) {
			/* Restore the normal state */
			map_2M_page(cpu_index, 0);
			restore_mtrr_state(&mtrr_state);
			enable_lapic();

			/* Print a status message */
			printk_debug("%c", (basek >= TOLM_KB)?'+':'-');

			/* Return to the initialization state */
			set_init_ecc_mtrrs();
			disable_lapic();
		}
		limitk = (basek + ZERO_CHUNK_KB) & ~(ZERO_CHUNK_KB - 1);
		if (limitk > endk) {
			limitk = endk;
		}
		size = (limitk - basek) << 10;
		addr = map_2M_page(cpu_index, basek >> 11);
		addr = (void *)(((uint32_t)addr) | ((basek & 0x7ff) << 10));
		if (addr == MAPPING_ERROR) {
			continue;
		}

		/* clear memory 2M (limitk - basek) */
		__asm__ volatile(
			"1: \n\t"
			"movl %0, (%1)\n\t"
			"addl $4,%1\n\t"
			"subl $4,%2\n\t"
			"jnz 1b\n\t"
			:
			: "a" (0), "D" (addr),	"c" (size)
			);
	}
	/* Restore the normal state */
	map_2M_page(cpu_index, 0);
	restore_mtrr_state(&mtrr_state);
	enable_lapic();

	/* Set the scrub base address registers */
	pci_write_config32(f3_dev, SCRUB_ADDR_LOW,  startk << 10);
	pci_write_config32(f3_dev, SCRUB_ADDR_HIGH, startk >> 22);

	/* Enable the scrubber? */
	if (enable_scrubbing) {
		/* Enable scrubbing at the lowest possible rate */
		pci_write_config32(f3_dev, SCRUB_CONTROL,
			(SCRUB_84ms << 16) | (SCRUB_84ms << 8) | (SCRUB_84ms << 0));
	}

	printk_debug(" done\n");
}

static void k8_errata(void)
{
	msr_t msr;

	if (is_cpu_pre_c0()) {
		/* Erratum 63... */
		msr = rdmsr(HWCR_MSR);
		msr.lo |= (1 << 6);
		wrmsr(HWCR_MSR, msr);

		/* Erratum 69... */
		msr = rdmsr_amd(BU_CFG_MSR);
		msr.hi |= (1 << (45 - 32));
		wrmsr_amd(BU_CFG_MSR, msr);

		/* Erratum 81... */
		msr = rdmsr_amd(DC_CFG_MSR);
		msr.lo |=  (1 << 10);
		wrmsr_amd(DC_CFG_MSR, msr);
	}

	/* I can't touch this msr on early buggy cpus */
	if (!is_cpu_pre_b3()) {
		/* Erratum 89 ... */
		msr = rdmsr(NB_CFG_MSR);
		msr.lo |= 1 << 3;
		if (!is_cpu_pre_c0()) {
			/* Erratum 86 Disable data masking on C0 and 
			 * later processor revs.
			 * FIXME this is only needed if ECC is enabled.
			 */
			msr.hi |= 1 << (36 - 32);
		}
		wrmsr(NB_CFG_MSR, msr);
	}

	/* Erratum 97 ... */
	if (!is_cpu_pre_c0()) {
		msr = rdmsr_amd(DC_CFG_MSR);
		msr.lo |= 1 << 3;
		wrmsr_amd(DC_CFG_MSR, msr);
	}

	/* Erratum 94 ... */
	msr = rdmsr_amd(IC_CFG_MSR);
	msr.lo |= 1 << 11;
	wrmsr_amd(IC_CFG_MSR, msr);

	/* Erratum 91 prefetch miss is handled in the kernel */
}

static void setup_toms(struct mem_range *mem)
{
	unsigned long i;
	msr_t msr;
	unsigned long mmio_basek, tomk;

	printk_spew("%s\n", __FUNCTION__);

	/* Except for the PCI MMIO hold just before 4GB there are no
	 * significant holes in the address space, so just account
	 * for those two and move on.
	 */
	mmio_basek = tomk = 0;
	for(i = 0; mem[i].sizek; i++) {
		unsigned long topk;
		topk = mem[i].basek + mem[i].sizek;
		if (tomk < topk) {
			tomk = topk;
		}
		if ((topk < 4*1024*1024) && (mmio_basek < topk)) {
			mmio_basek = topk;
		}
	}
	if (mmio_basek > tomk) {
		mmio_basek = tomk;
	}
	/* Round mmio_basek down to the nearst size that will fit in TOP_MEM */
	mmio_basek = mmio_basek & ~TOP_MEM_MASK_KB;
	/* Round tomk up to the next greater size that will fit in TOP_MEM */
	tomk = (tomk + TOP_MEM_MASK_KB) & ~TOP_MEM_MASK_KB;
		
	/* Setup TOP_MEM */
	msr.hi = mmio_basek >> 22;
	msr.lo = mmio_basek << 10;
	wrmsr(TOP_MEM, msr);

	/* Setup TOP_MEM2 */
	msr.hi = tomk >> 22;
	msr.lo = tomk << 10;
	wrmsr(TOP_MEM2, msr);	
}

static void setup_iorrs(void)
{
	unsigned long i;
	msr_t msr;
	device_t f3_dev;
	uint32_t base, size;

	/* zero the IORR's before we enable to prevent
	 * undefined side effects. */
	msr.lo = msr.hi = 0;
	for (i = IORR_FIRST; i <= IORR_LAST; i++) {
		wrmsr(i, msr);
	}

	/* enable IORR1 for AGP Aperture */ 
	f3_dev = dev_find_slot(0, PCI_DEVFN(0x18, 3));
	if (!f3_dev) {
		die("Cannot find cpu function 3\n");
	}

	size = (pci_read_config32(f3_dev, 0x90) & 0x0E) >> 1;
	size = (32*1024*1024) << size;
	base = pci_read_config32(f3_dev, 0x94) << 25;
	printk_debug("%s: setting IORR1 for AGP aperture base 0x%x, size 0x%x\n",
		     __FUNCTION__, base, size);

	msr.lo = base;
	msr.hi = 0xff;
	wrmsr(IORR1_BASE, msr);

	msr.lo = ~(size - 1);
	msr.hi = 0xff;
	wrmsr(IORR1_MASK, msr);
}

void k8_cpufixup(struct mem_range *mem)
{
	unsigned long i;
	msr_t msr;

	disable_cache();
	
	setup_toms(mem);

	setup_iorrs();

	/* Enable TOMs and IORRs */
	msr = rdmsr(SYSCFG_MSR);
	msr.lo |= SYSCFG_MSR_MtrrVarDramEn | SYSCFG_MSR_TOM2En;
	wrmsr(SYSCFG_MSR, msr);

	/* zero the machine check error status registers */
	msr.lo = 0;
	msr.hi = 0;
	for (i = 0; i < 5; i++) {
		wrmsr(MCI_STATUS + (i*4),msr);
	}

	k8_errata();

	enable_cache();

	/* Is this a bad location?  In particular can another node prefecth
	 * data from this node before we have initialized it?
	 */
	init_ecc_memory();
}

static
void k8_enable(struct chip *chip, enum chip_pass pass)
{

        struct cpu_k8_config *conf = (struct cpu_k8_config *)chip->chip_info;

        switch (pass) {
        case CONF_PASS_PRE_CONSOLE:
                break;
	case CONF_PASS_PRE_PCI:
		init_timer();
		break;
        default:
                /* nothing yet */
                break;
        }
}

struct chip_control cpu_k8_control = {
        .enable = k8_enable,
        .name   = "AMD K8 CPU",
};
