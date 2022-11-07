/* SPDX-License-Identifier: GPL-2.0-only */

#include <cpu/cpu.h>
#include <console/console.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <commonlib/helpers.h>
#include <device/pci_ops.h>
#include <delay.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <types.h>
#include "chip.h"
#include "sandybridge.h"
#include <cpu/intel/smm_reloc.h>
#include <security/intel/txt/txt_platform.h>

/* IGD UMA memory */
static uint64_t uma_memory_base = 0;
static uint64_t uma_memory_size = 0;

bool is_sandybridge(void)
{
	const uint16_t bridge_id = pci_read_config16(pcidev_on_root(0, 0), PCI_DEVICE_ID);

	return (bridge_id & BASE_REV_MASK) == BASE_REV_SNB;
}

/* Reserve everything between A segment and 1MB:
 *
 * 0xa0000 - 0xbffff: legacy VGA
 * 0xc0000 - 0xcffff: VGA OPROM (needed by kernel)
 * 0xe0000 - 0xfffff: SeaBIOS, if used, otherwise DMI
 */

static const char *northbridge_acpi_name(const struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_DOMAIN)
		return "PCI0";

	if (dev->path.type != DEVICE_PATH_PCI)
		return NULL;

	switch (dev->path.pci.devfn) {
	case PCI_DEVFN(0, 0):
		return "MCHC";
	}

	return NULL;
}

struct device_operations sandybridge_pci_domain_ops = {
	.read_resources    = pci_domain_read_resources,
	.set_resources     = pci_domain_set_resources,
	.scan_bus          = pci_domain_scan_bus,
	.write_acpi_tables = northbridge_write_acpi_tables,
	.acpi_name         = northbridge_acpi_name,
};

static void add_fixed_resources(struct device *dev, int index)
{
	mmio_resource_kb(dev, index++, uma_memory_base >> 10, uma_memory_size >> 10);

	mmio_from_to(dev, index++, 0xa0000, 0xc0000);
	reserved_ram_from_to(dev, index++, 0xc0000, 1 * MiB);

	if (is_sandybridge()) {
		/* Required for SandyBridge sighting 3715511 */
		bad_ram_resource_kb(dev, index++, 0x20000000 >> 10, 0x00200000 >> 10);
		bad_ram_resource_kb(dev, index++, 0x40000000 >> 10, 0x00200000 >> 10);
	}

	/* Reserve IOMMU BARs */
	const u32 capid0_a = pci_read_config32(dev, CAPID0_A);
	if (!(capid0_a & (1 << 23))) {
		mmio_resource_kb(dev, index++, GFXVT_BASE >> 10, 4);
		mmio_resource_kb(dev, index++, VTVC0_BASE >> 10, 4);
	}
}

static uint64_t get_touud(const struct device *dev)
{
	uint64_t touud = pci_read_config32(dev, TOUUD + 4);
	touud <<= 32;
	touud |= pci_read_config32(dev, TOUUD) & 0xfff00000;
	return touud;
}

static void mc_read_resources(struct device *dev)
{
	uint64_t tom, me_base, touud;
	uint32_t tseg_base, uma_size, tolud;
	uint32_t dpr_base_k, dpr_size_k;
	uint16_t ggc;
	unsigned long long tomk;
	unsigned long index = 3;
	const union dpr_register dpr = txt_get_chipset_dpr();

	pci_dev_read_resources(dev);

	mmconf_resource(dev, PCIEXBAR);

	/* Total Memory 2GB example:
	 *
	 *  00000000  0000MB-1992MB  1992MB  RAM     (writeback)
	 *  7c800000  1992MB-2000MB     8MB  TSEG    (SMRR)
	 *  7d000000  2000MB-2002MB     2MB  GFX GTT (uncached)
	 *  7d200000  2002MB-2034MB    32MB  GFX UMA (uncached)
	 *  7f200000   2034MB TOLUD
	 *  7f800000   2040MB MEBASE
	 *  7f800000  2040MB-2048MB     8MB  ME UMA  (uncached)
	 *  80000000   2048MB TOM
	 * 100000000  4096MB-4102MB     6MB  RAM     (writeback)
	 *
	 * Total Memory 4GB example:
	 *
	 *  00000000  0000MB-2768MB  2768MB  RAM     (writeback)
	 *  ad000000  2768MB-2776MB     8MB  TSEG    (SMRR)
	 *  ad800000  2776MB-2778MB     2MB  GFX GTT (uncached)
	 *  ada00000  2778MB-2810MB    32MB  GFX UMA (uncached)
	 *  afa00000   2810MB TOLUD
	 *  ff800000   4088MB MEBASE
	 *  ff800000  4088MB-4096MB     8MB  ME UMA  (uncached)
	 * 100000000   4096MB TOM
	 * 100000000  4096MB-5374MB  1278MB  RAM     (writeback)
	 * 14fe00000   5368MB TOUUD
	 */

	/* Top of Upper Usable DRAM, including remap */
	touud  = get_touud(dev);

	/* Top of Lower Usable DRAM */
	tolud = pci_read_config32(dev, TOLUD);

	/* Top of Memory - does not account for any UMA */
	tom  = pci_read_config32(dev, TOM + 4);
	tom <<= 32;
	tom |= pci_read_config32(dev, TOM);

	printk(BIOS_DEBUG, "TOUUD 0x%llx TOLUD 0x%08x TOM 0x%llx\n",
	       touud, tolud, tom);

	/* ME UMA needs excluding if total memory < 4GB */
	me_base  = pci_read_config32(dev, MESEG_BASE + 4);
	me_base <<= 32;
	me_base |= pci_read_config32(dev, MESEG_BASE);

	printk(BIOS_DEBUG, "MEBASE 0x%llx\n", me_base);

	uma_memory_base = tolud;
	tomk = tolud >> 10;
	if (me_base == tolud) {
		/* ME is from MEBASE-TOM */
		uma_size = (tom - me_base) >> 10;
		/* Increment TOLUD to account for ME as RAM */
		tolud += uma_size << 10;
		/* UMA starts at old TOLUD */
		uma_memory_base = tomk * 1024ULL;
		uma_memory_size = uma_size * 1024ULL;
		printk(BIOS_DEBUG, "ME UMA base 0x%llx size %uM\n",
		       me_base, uma_size >> 10);
	}

	/* Graphics memory comes next */
	ggc = pci_read_config16(dev, GGC);
	if (!(ggc & 2)) {
		printk(BIOS_DEBUG, "IGD decoded, subtracting ");

		/* Graphics memory */
		uma_size = ((ggc >> 3) & 0x1f) * 32 * 1024ULL;
		printk(BIOS_DEBUG, "%uM UMA", uma_size >> 10);
		tomk -= uma_size;
		uma_memory_base = tomk * 1024ULL;
		uma_memory_size += uma_size * 1024ULL;

		/* GTT Graphics Stolen Memory Size (GGMS) */
		uma_size = ((ggc >> 8) & 0x3) * 1024ULL;
		tomk -= uma_size;
		uma_memory_base = tomk * 1024ULL;
		uma_memory_size += uma_size * 1024ULL;
		printk(BIOS_DEBUG, " and %uM GTT\n", uma_size >> 10);
	}

	/* Calculate TSEG size from its base which must be below GTT */
	tseg_base = pci_read_config32(dev, TSEGMB);
	uma_size = (uma_memory_base - tseg_base) >> 10;
	tomk -= uma_size;
	uma_memory_base = tomk * 1024ULL;
	uma_memory_size += uma_size * 1024ULL;
	printk(BIOS_DEBUG, "TSEG base 0x%08x size %uM\n", tseg_base, uma_size >> 10);

	/* Calculate DMA Protected Region if enabled */
	if (dpr.epm && dpr.size) {
		dpr_size_k = dpr.size * MiB / KiB;
		tomk -= dpr_size_k;
		dpr_base_k = (tseg_base - dpr.size * MiB) / KiB;
		reserved_ram_resource_kb(dev, index++, dpr_base_k, dpr_size_k);
		printk(BIOS_DEBUG, "DPR base 0x%08x size %uM\n", dpr_base_k * KiB, dpr.size);
	}

	printk(BIOS_INFO, "Available memory below 4GB: %lluM\n", tomk >> 10);

	/* Report the memory regions */
	ram_from_to(dev, index++, 0, 0xa0000);
	ram_from_to(dev, index++, 1 * MiB, tomk * KiB);

	/*
	 * If >= 4GB installed, then memory from TOLUD to 4GB is remapped above TOM.
	 * TOUUD will account for both memory chunks.
	 */
	upper_ram_end(dev, index++, touud);

	add_fixed_resources(dev, index++);
}

static void northbridge_dmi_init(struct device *dev)
{
	const bool is_sandy = is_sandybridge();

	const u8 stepping = cpu_stepping();

	/* Steps prior to DMI ASPM */
	if (is_sandy) {
		dmibar_clrsetbits32(0x250, 7 << 20, 2 << 20);
	}

	dmibar_setbits32(DMILLTC, 1 << 29);

	if (is_sandy && stepping == SNB_STEP_C0) {
		dmibar_clrsetbits32(0xbc8, 0xfff << 7, 0x7d3 << 7);
	}

	if (!is_sandy || stepping >= SNB_STEP_D1) {
		dmibar_clrsetbits32(0x1f8, 1 << 26, 1 << 16);

		dmibar_setbits32(0x1fc, 1 << 12 | 1 << 23);

	} else if (stepping >= SNB_STEP_D0) {
		dmibar_setbits32(0x1f8, 1 << 16);
	}

	/* Clear error status bits */
	dmibar_write32(DMIUESTS, 0xffffffff);
	dmibar_write32(DMICESTS, 0xffffffff);

	if (!is_sandy)
		dmibar_write32(0xc34, 0xffffffff);

	/* Enable ASPM on SNB link, should happen before PCH link */
	if (is_sandy) {
		dmibar_setbits32(0xd04, 1 << 4);
	}

	dmibar_setbits32(DMILCTL, 1 << 1 | 1 << 0);
}

/* Disable unused PEG devices based on devicetree */
static void disable_peg(void)
{
	struct device *dev;
	u32 reg;

	dev = pcidev_on_root(0, 0);
	reg = pci_read_config32(dev, DEVEN);

	dev = pcidev_on_root(1, 2);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG12.\n");
		reg &= ~DEVEN_PEG12;
	}
	dev = pcidev_on_root(1, 1);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG11.\n");
		reg &= ~DEVEN_PEG11;
	}
	dev = pcidev_on_root(1, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG10.\n");
		reg &= ~DEVEN_PEG10;
	}
	dev = pcidev_on_root(2, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling IGD.\n");
		reg &= ~DEVEN_IGD;
	}
	dev = pcidev_on_root(4, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling Device 4.\n");
		reg &= ~DEVEN_D4EN;
	}
	dev = pcidev_on_root(6, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling PEG60.\n");
		reg &= ~DEVEN_PEG60;
	}
	dev = pcidev_on_root(7, 0);
	if (!dev || !dev->enabled) {
		printk(BIOS_DEBUG, "Disabling Device 7.\n");
		reg &= ~DEVEN_D7EN;
	}

	dev = pcidev_on_root(0, 0);
	pci_write_config32(dev, DEVEN, reg);

	if (!(reg & (DEVEN_PEG60 | DEVEN_PEG10 | DEVEN_PEG11 | DEVEN_PEG12))) {
		/*
		 * Set the PEG clock gating bit. Disables the IO clock on all PEG devices.
		 *
		 * FIXME: Never clock gate on Ivy Bridge stepping A0!
		 */
		mchbar_setbits32(PEGCTL, 1);
		printk(BIOS_DEBUG, "Disabling PEG IO clock.\n");
	} else {
		mchbar_clrbits32(PEGCTL, 1);
	}
}

static void northbridge_init(struct device *dev)
{
	u32 bridge_type;

	northbridge_dmi_init(dev);

	bridge_type = mchbar_read32(SAPMTIMERS);
	bridge_type &= ~0xff;

	if (is_sandybridge()) {
		/* 20h for Sandybridge */
		bridge_type |= 0x20;
	} else {
		/* Enable Power Aware Interrupt Routing */
		mchbar_clrsetbits8(INTRDIRCTL, 0xf, 0x4); /* Clear 3:0, set Fixed Priority */

		/* 30h for IvyBridge */
		bridge_type |= 0x30;
	}
	mchbar_write32(SAPMTIMERS, bridge_type);

	/* Turn off unused devices. Has to be done before setting BIOS_RESET_CPL. */
	disable_peg();

	/*
	 * Set bit 0 of BIOS_RESET_CPL to indicate to the CPU
	 * that BIOS has initialized memory and power management
	 */
	mchbar_setbits8(BIOS_RESET_CPL, 1 << 0);
	printk(BIOS_DEBUG, "Set BIOS_RESET_CPL\n");

	/* Configure turbo power limits 1ms after reset complete bit */
	mdelay(1);
	set_power_limits(28);

	/*
	 * CPUs with configurable TDP also need power limits set in MCHBAR.
	 * Use the same values from MSR_PKG_POWER_LIMIT.
	 */
	if (cpu_config_tdp_levels()) {
		msr_t msr = rdmsr(MSR_PKG_POWER_LIMIT);
		mchbar_write32(MCH_PKG_POWER_LIMIT_LO, msr.lo);
		mchbar_write32(MCH_PKG_POWER_LIMIT_HI, msr.hi);
	}

	/* Set here before graphics PM init */
	mchbar_write32(PAVP_MSG, 0x00100001);
}

void northbridge_write_smram(u8 smram)
{
	pci_write_config8(pcidev_on_root(0, 0), SMRAM, smram);
}

static void set_above_4g_pci(const struct device *dev)
{
	const uint64_t touud = get_touud(dev);
	const uint64_t len = POWER_OF_2(cpu_phys_address_size()) - touud;

	acpigen_write_scope("\\");
	acpigen_write_name_qword("A4GB", touud);
	acpigen_write_name_qword("A4GS", len);
	acpigen_pop_len();

	printk(BIOS_DEBUG, "PCI space above 4GB MMIO is at 0x%llx, len = 0x%llx\n", touud, len);
}

static void mc_gen_ssdt(const struct device *dev)
{
	generate_cpu_entries(dev);
	set_above_4g_pci(dev);
}

static struct device_operations mc_ops = {
	.read_resources         = mc_read_resources,
	.set_resources          = pci_dev_set_resources,
	.enable_resources       = pci_dev_enable_resources,
	.init                   = northbridge_init,
	.ops_pci                = &pci_dev_ops_pci,
	.acpi_fill_ssdt		= mc_gen_ssdt,
};

static const unsigned short pci_device_ids[] = {
	0x0100, 0x0104, 0x0108,		/* Sandy Bridge */
	0x0150, 0x0154, 0x0158,		/* Ivy Bridge */
	0
};

static const struct pci_driver mc_driver __pci_driver = {
	.ops     = &mc_ops,
	.vendor  = PCI_VID_INTEL,
	.devices = pci_device_ids,
};

struct device_operations sandybridge_cpu_bus_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = noop_set_resources,
	.init             = mp_cpu_bus_init,
};

struct chip_operations northbridge_intel_sandybridge_ops = {
	CHIP_NAME("Intel SandyBridge/IvyBridge integrated Northbridge")
};
