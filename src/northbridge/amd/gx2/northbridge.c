#include <console/console.h>
#include <arch/io.h>
#include <stdint.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <stdlib.h>
#include <string.h>
#include <bitops.h>
#include "chip.h"
#include "northbridge.h"
#include <cpu/amd/gx2def.h>
#include <cpu/x86/msr.h>
#include <cpu/x86/cache.h>
#include <cpu/amd/vr.h>
#include <cpu/cpu.h>
#include "../../../southbridge/amd/cs5536/cs5536.h"
#define VIDEO_MB 8

#define NORTHBRIDGE_FILE "northbridge.c"

/* todo: add a resource record. We don't do this here because this may be called when
 * very little of the platform is actually working.
 */
int sizeram(void)
{
	msr_t msr;
	int sizem = 0;
	unsigned short dimm;

	/* Get the RAM size from the memory controller as calculated and set by auto_size_dimm() */
	msr = rdmsr(MC_CF07_DATA);
	printk(BIOS_DEBUG, "sizeram: _MSR MC_CF07_DATA: %08x:%08x\n", msr.hi, msr.lo);

	/* dimm 0 */
	dimm = msr.hi;
	/* installed? */
	if ((dimm & 7) != 7)
		sizem = (1 << ((dimm >> 12)-1)) * 8;

	/* dimm 1 */
	dimm = msr.hi >> 16;
	/* installed? */
	if ((dimm & 7) != 7)
		sizem += (1 << ((dimm >> 12)-1)) * 8;

	printk(BIOS_DEBUG, "sizeram: sizem 0x%x\n", sizem);
	return sizem;
}

/* here is programming for the various MSRs. */
#define IM_QWAIT 0x100000

#define DMCF_WRITE_SERIALIZE_REQUEST (2<<12) /* 2 outstanding */ /* in high */
#define DMCF_SERIAL_LOAD_MISSES  (2) /* enabled */

/* these are the 8-bit attributes for controlling RCONF registers */
#define CACHE_DISABLE (1<<0)
#define WRITE_ALLOCATE (1<<1)
#define WRITE_PROTECT (1<<2)
#define WRITE_THROUGH (1<<3)
#define WRITE_COMBINE (1<<4)
#define WRITE_SERIALIZE (1<<5)

/* ram has none of this stuff */
#define RAM_PROPERTIES (0)
#define DEVICE_PROPERTIES (WRITE_SERIALIZE|CACHE_DISABLE)
#define ROM_PROPERTIES (WRITE_SERIALIZE|WRITE_PROTECT|CACHE_DISABLE)
/* build initializer for P2D MSR */
#define P2D_BM(msr, pdid1, bizarro, pbase, pmask) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(pbase>>24), .lo=(pbase<<8)|pmask}}
#define P2D_SC(msr, pdid1, bizarro, wen, ren,pscbase) {msr, {.hi=(pdid1<<29)|(bizarro<<28)|(wen), .lo=(ren<<16)|(pscbase>>18)}}

struct msr_defaults
{
	int msr_no;
	msr_t msr;
} msr_defaults [] = {
	{0x1700, {.hi = 0, .lo = IM_QWAIT}},
	{0x1800, {.hi = DMCF_WRITE_SERIALIZE_REQUEST, .lo = DMCF_SERIAL_LOAD_MISSES}},
	/* now for GLPCI routing */
	/* GLIU0 */
	P2D_BM(0x10000020, 0x1, 0x0, 0x0, 0xfff80),
	P2D_BM(0x10000021, 0x1, 0x0, 0x80000, 0xfffe0),
	P2D_SC(0x1000002c, 0x1, 0x0, 0x0,  0xff03, 0xC0000),
	/* GLIU1 */
	P2D_BM(0x40000020, 0x1, 0x0, 0x0, 0xfff80),
	P2D_BM(0x40000021, 0x1, 0x0, 0x80000, 0xfffe0),
	P2D_SC(0x4000002d, 0x1, 0x0, 0x0,  0xff03, 0xC0000),
	{0}
};

/* note that dev is NOT used -- yet */
static void irq_init_steering(struct device *dev, u16 irq_map)
{
	/* Set up IRQ steering */
	u32 pciAddr = 0x80000000 | (CHIPSET_DEV_NUM << 11) | 0x5C;

	printk(BIOS_DEBUG, "%s(%p [%08X], %04X)\n", __func__, dev, pciAddr, irq_map);

	/* The IRQ steering values (in hex) are effectively dcba, where:
	 *    <a> represents the IRQ for INTA,
	 *    <b> represents the IRQ for INTB,
	 *    <c> represents the IRQ for INTC, and
	 *    <d> represents the IRQ for INTD.
	 * Thus, a value of irq_map = 0xAA5B translates to:
	 *    INTA = IRQB (IRQ 11)
	 *    INTB = IRQ5 (IRQ 5)
	 *    INTC = IRQA (IRQ 10)
	 *    INTD = IRQA (IRQ 10)
	 */
	outl(pciAddr & ~3, 0xCF8);
	outl(irq_map,      0xCFC);
}

/* setup_gx2_cache
 *
 * Returns the amount of memory (in KB) available to the system.  This is the
 * total amount of memory less the amount of memory reserved for SMM use.
 */
static int setup_gx2_cache(void)
{
	msr_t msr;
	unsigned long long val;
	int sizekbytes, sizereg;

	sizekbytes = sizeram() * 1024;
	printk(BIOS_DEBUG, "setup_gx2_cache: enable for %d KB\n", sizekbytes);
	/* build up the rconf word. */
	/* the SYSTOP bits 27:8 are actually the top bits from 31:12. Book fails to say that */
	/* set romrp */
	val = ((unsigned long long) ROM_PROPERTIES) << 56;
	/* make rom base useful for 1M roms */
	/* Flash base address -- sized for 1M for now */
	val |= ((unsigned long long) 0xfff00)<<36;
	/* set the devrp properties */
	val |= ((unsigned long long) DEVICE_PROPERTIES) << 28;
	/* Take our TOM, RIGHT shift 12, since it page-aligned, then LEFT-shift 8 for reg. */
	/* yank off memory for the SMM handler */
	sizekbytes -= SMM_SIZE;
	sizereg = sizekbytes;
	sizereg *= 1024;	/* convert to bytes */
	sizereg >>= 12;
	sizereg <<= 8;
	val |= sizereg;
	val |= RAM_PROPERTIES;
	msr.lo = val;
	msr.hi = (val >> 32);
	printk(BIOS_DEBUG, "msr 0x%08X will be set to %08x:%08x\n", CPU_RCONF_DEFAULT, msr.hi, msr.lo);
	wrmsr(CPU_RCONF_DEFAULT, msr);

	enable_cache();
	wbinvd();
	return sizekbytes;
}

/* we have to do this here. We have not found a nicer way to do it */
static void setup_gx2(void)
{
	unsigned long tmp, tmp2;
	msr_t msr;
	unsigned long size_kb, membytes;

	size_kb = setup_gx2_cache();

	membytes = size_kb * 1024;
	/* NOTE! setup_gx2_cache returns the SIZE OF RAM - RAMADJUST!
	 * so it is safe to use. You should NOT at this point call
	 * sizeram() directly.
	 */

	/* fixme: SMM MSR 0x10000026 and 0x400000023 */
	/* calculate the OFFSET field */
	tmp = membytes - SMM_OFFSET;
	tmp >>= 12;
	tmp <<= 8;
	tmp |= 0x20000000;
	tmp |= (SMM_OFFSET >> 24);

	/* calculate the PBASE and PMASK fields */
	tmp2 = (SMM_OFFSET << 8) & 0xFFF00000; /* shift right 12 then left 20  == left 8 */
	tmp2 |= (((~(SMM_SIZE * 1024) + 1) >> 12) & 0xfffff);
	printk(BIOS_DEBUG, "MSR 0x%x is now 0x%lx:0x%lx\n", 0x10000026, tmp, tmp2);
	msr.hi = tmp;
	msr.lo = tmp2;
	wrmsr(0x10000026, msr);
}

static void enable_shadow(device_t dev)
{

}

static void northbridge_init(device_t dev)
{
	struct northbridge_amd_gx2_config *nb = (struct northbridge_amd_gx2_config *)dev->chip_info;
	printk(BIOS_DEBUG, "northbridge: %s()\n", __func__);

	enable_shadow(dev);
	irq_init_steering(dev, nb->irqmap);
}

/* due to vsa interactions, we need not not touch the nb settings ... */
/* this is a test -- we are not sure it will work -- but it ought to */
static void set_resources(struct device *dev)
{
	struct bus *bus;

	for(bus = dev->link_list; bus; bus = bus->next) {
		if (bus->children) {
			assign_resources(bus);
		}
	}

#if 0
	/* set a default latency timer */
	pci_write_config8(dev, PCI_LATENCY_TIMER, 0x40);

	/* set a default secondary latency timer */
	if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE) {
		pci_write_config8(dev, PCI_SEC_LATENCY_TIMER, 0x40);
	}

	/* zero the irq settings */
	u8 line = pci_read_config8(dev, PCI_INTERRUPT_PIN);
	if (line) {
		pci_write_config8(dev, PCI_INTERRUPT_LINE, 0);
	}
	/* set the cache line size, so far 64 bytes is good for everyone */
	pci_write_config8(dev, PCI_CACHE_LINE_SIZE, 64 >> 2);
#endif
}

static struct device_operations northbridge_operations = {
	.read_resources = pci_dev_read_resources,
	.set_resources = set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init = northbridge_init,
	.enable = 0,
	.ops_pci = 0,
};

static const struct pci_driver northbridge_driver __pci_driver = {
	.ops = &northbridge_operations,
	.vendor = PCI_VENDOR_ID_NS,
	.device = PCI_DEVICE_ID_NS_GX2,
};

/* FIXME handle UMA correctly. */
#define FRAMEBUFFERK 4096

static void pci_domain_set_resources(device_t dev)
{
	assign_resources(dev->link_list);
}

static struct device_operations pci_domain_ops = {
	.read_resources = pci_domain_read_resources,
	.set_resources = pci_domain_set_resources,
	.enable_resources = NULL,
	.init = NULL,
	.scan_bus = pci_domain_scan_bus,
};

static void cpu_bus_init(device_t dev)
{
	initialize_cpus(dev->link_list);
}

static void cpu_bus_noop(device_t dev)
{
}

static struct device_operations cpu_bus_ops = {
	.read_resources = cpu_bus_noop,
	.set_resources = cpu_bus_noop,
	.enable_resources = cpu_bus_noop,
	.init = cpu_bus_init,
	.scan_bus = 0,
};

void chipsetInit (void);

#if CONFIG_WRITE_HIGH_TABLES==1
#include <cbmem.h>
#endif

static void enable_dev(struct device *dev)
{
	printk(BIOS_DEBUG, "gx2 north: enable_dev\n");
	void do_vsmbios(void);

	/* Set the operations if it is a special bus type */
	if (dev->path.type == DEVICE_PATH_PCI_DOMAIN) {
		u32 tomk;
		printk(BIOS_DEBUG, "DEVICE_PATH_PCI_DOMAIN\n");
		/* cpubug MUST be called before setup_gx2(), so we force the issue here */
		northbridgeinit();
		cpubug();
		chipsetinit();
		setup_gx2();
		do_vsmbios();
		graphics_init();
		dev->ops = &pci_domain_ops;
		pci_set_method(dev);
		tomk = ((sizeram() - VIDEO_MB) * 1024) - SMM_SIZE;
#if CONFIG_WRITE_HIGH_TABLES==1
		/* Leave some space for ACPI, PIRQ and MP tables */
		high_tables_base = (tomk * 1024) - HIGH_MEMORY_SIZE;
		high_tables_size = HIGH_MEMORY_SIZE;
#endif
		ram_resource(dev, 0, 0, tomk);
	} else if (dev->path.type == DEVICE_PATH_APIC_CLUSTER) {
		printk(BIOS_DEBUG, "DEVICE_PATH_APIC_CLUSTER\n");
		dev->ops = &cpu_bus_ops;
	}
	printk(BIOS_DEBUG, "gx2 north: end enable_dev\n");
}

struct chip_operations northbridge_amd_gx2_ops = {
	CHIP_NAME("AMD GX (previously GX2) Northbridge")
	.enable_dev = enable_dev,
};
