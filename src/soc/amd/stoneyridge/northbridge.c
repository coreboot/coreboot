/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <amdblocks/acpi.h>
#include <amdblocks/biosram.h>
#include <device/pci_ops.h>
#include <arch/hpet.h>
#include <arch/ioapic.h>
#include <arch/vga.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <cbmem.h>
#include <console/console.h>
#include <cpu/amd/mtrr.h>
#include <cpu/x86/lapic_def.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <amdblocks/agesawrapper.h>
#include <amdblocks/agesawrapper_call.h>
#include <amdblocks/ioapic.h>
#include <agesa_headers.h>
#include <soc/cpu.h>
#include <soc/northbridge.h>
#include <soc/pci_devs.h>
#include <soc/iomap.h>
#include <static.h>
#include <stdint.h>
#include <string.h>

#include "chip.h"

static void read_resources(struct device *dev)
{
	unsigned int idx = 0;

	/* The northbridge has no PCI BARs implemented, so there's no need to call
	   pci_dev_read_resources for it */

	/*
	 * This MMCONF resource must be reserved in the PCI domain.
	 * It is not honored by the coreboot resource allocator if it is in
	 * the CPU_CLUSTER.
	 */
	mmconf_resource(dev, idx++);
}

/**
 * I tried to reuse the resource allocation code in set_resource()
 * but it is too difficult to deal with the resource allocation magic.
 */

static void create_vga_resource(struct device *dev)
{
	if (!dev->downstream)
		return;
	if (!(dev->downstream->bridge_ctrl & PCI_BRIDGE_CTL_VGA))
		return;

	printk(BIOS_DEBUG, "VGA: %s has VGA device\n",	dev_path(dev));
	/* Route A0000-BFFFF, IO 3B0-3BB 3C0-3DF */
	pci_write_config32(SOC_ADDR_DEV, D18F1_VGAEN, VGA_ADDR_ENABLE);
}

static void set_resources(struct device *dev)
{
	/* do we need this? */
	create_vga_resource(dev);

	if (dev->downstream && dev->downstream->children)
		assign_resources(dev->downstream);
}

static void northbridge_init(struct device *dev)
{
	register_new_ioapic(IO_APIC2_ADDR);
}

/* Used by \_SB.PCI0._CRS */
static void acpi_fill_root_complex_tom(const struct device *device)
{
	const char *scope;

	assert(device);

	scope = acpi_device_scope(device);
	assert(scope);
	acpigen_write_scope(scope);

	acpigen_write_name_dword("TOM1", get_top_of_mem_below_4gb());

	/*
	 * Since XP only implements parts of ACPI 2.0, we can't use a qword
	 * here.
	 * See http://www.acpi.info/presentations/S01USMOBS169_OS%2520new.ppt
	 * slide 22ff.
	 * Shift value right by 20 bit to make it fit into 32bit,
	 * giving us 1MB granularity and a limit of almost 4Exabyte of memory.
	 */
	acpigen_write_name_dword("TOM2", get_top_of_mem_above_4gb() >> 20);
	acpigen_pop_len();
}

static unsigned long acpi_fill_hest(acpi_hest_t *hest)
{
	void *addr, *current;

	/* Skip the HEST header. */
	current = (void *)(hest + 1);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_MCE);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 0,
				(void *)((u32)addr + 2), *(uint16_t *)addr - 2);

	addr = agesawrapper_getlateinitptr(PICK_WHEA_CMC);
	if (addr != NULL)
		current += acpi_create_hest_error_source(hest, current, 1,
				(void *)((u32)addr + 2), *(uint16_t *)addr - 2);

	return (unsigned long)current;
}

unsigned long soc_acpi_write_tables(const struct device *device, unsigned long current,
				    acpi_rsdp_t *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;
	acpi_header_t *alib;
	acpi_header_t *ivrs;
	acpi_hest_t *hest;

	/* HEST */
	current = acpi_align_current(current);
	hest = (acpi_hest_t *)current;
	acpi_write_hest(hest, acpi_fill_hest);
	acpi_add_table(rsdp, (void *)current);
	current += hest->header.length;

	current = acpi_align_current(current);
	printk(BIOS_DEBUG, "ACPI:    * IVRS at %lx\n", current);
	ivrs = agesawrapper_getlateinitptr(PICK_IVRS);
	if (ivrs != NULL) {
		memcpy((void *)current, ivrs, ivrs->length);
		ivrs = (acpi_header_t *)current;
		current += ivrs->length;
		acpi_add_table(rsdp, ivrs);
	} else {
		printk(BIOS_DEBUG, "  AGESA IVRS table NULL. Skipping.\n");
	}

	/* SRAT */
	current = acpi_align_current(current);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *)agesawrapper_getlateinitptr(PICK_SRAT);
	if (srat != NULL) {
		memcpy((void *)current, srat, srat->header.length);
		srat = (acpi_srat_t *)current;
		current += srat->header.length;
		acpi_add_table(rsdp, srat);
	} else {
		printk(BIOS_DEBUG, "  AGESA SRAT table NULL. Skipping.\n");
	}

	/* SLIT */
	current = acpi_align_current(current);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *)agesawrapper_getlateinitptr(PICK_SLIT);
	if (slit != NULL) {
		memcpy((void *)current, slit, slit->header.length);
		slit = (acpi_slit_t *)current;
		current += slit->header.length;
		acpi_add_table(rsdp, slit);
	} else {
		printk(BIOS_DEBUG, "  AGESA SLIT table NULL. Skipping.\n");
	}

	/* ALIB */
	current = acpi_align_current(current);
	printk(BIOS_DEBUG, "ACPI:  * AGESA ALIB SSDT at %lx\n", current);
	alib = (acpi_header_t *)agesawrapper_getlateinitptr(PICK_ALIB);
	if (alib != NULL) {
		memcpy((void *)current, alib, alib->length);
		alib = (acpi_header_t *)current;
		current += alib->length;
		acpi_add_table(rsdp, (void *)alib);
	} else {
		printk(BIOS_DEBUG, "	AGESA ALIB SSDT table NULL."
							" Skipping.\n");
	}

	printk(BIOS_DEBUG, "ACPI:    * SSDT for PState at %lx\n", current);
	return current;
}

struct device_operations stoneyridge_northbridge_operations = {
	.read_resources	  = read_resources,
	.set_resources	  = set_resources,
	.enable_resources = pci_dev_enable_resources,
	.init		  = northbridge_init,
	.acpi_fill_ssdt   = acpi_fill_root_complex_tom,
	.write_acpi_tables = soc_acpi_write_tables,
};

/*
 * Enable VGA cycles.  Set memory ranges of the FCH legacy devices (TPM, HPET,
 * BIOS RAM, Watchdog Timer, IOAPIC and ACPI) as non-posted.  Set remaining
 * MMIO to posted.  Route all I/O to the southbridge.
 */
void amd_initcpuio(void)
{
	uintptr_t topmem = get_top_of_mem_below_4gb();
	uintptr_t base, limit;

	/* Enable legacy video routing: D18F1xF4 VGA Enable */
	pci_write_config32(SOC_ADDR_DEV, D18F1_VGAEN, VGA_ADDR_ENABLE);

	/* Non-posted: range(HPET-LAPIC) or 0xfed00000 through 0xfee00000-1 */
	base = (HPET_BASE_ADDRESS >> 8) | MMIO_WE | MMIO_RE;
	limit = (ALIGN_DOWN(LAPIC_DEFAULT_BASE - 1, 64 * KiB) >> 8) | MMIO_NP;
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_LIMIT_LO(0), limit);
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_BASE_LO(0), base);

	/* Remaining PCI hole posted MMIO: TOM-HPET (TOM through 0xfed00000-1 */
	base = (topmem >> 8) | MMIO_WE | MMIO_RE;
	limit = ALIGN_DOWN(HPET_BASE_ADDRESS - 1, 64 * KiB) >> 8;
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_LIMIT_LO(1), limit);
	pci_write_config32(SOC_ADDR_DEV, NB_MMIO_BASE_LO(1), base);

	/* Route all I/O downstream */
	base = 0 | IO_WE | IO_RE;
	limit = ALIGN_DOWN(0xffff, 4 * KiB);
	pci_write_config32(SOC_ADDR_DEV, NB_IO_LIMIT(0), limit);
	pci_write_config32(SOC_ADDR_DEV, NB_IO_BASE(0), base);
}

void fam15_finalize(void *chip_info)
{
	u32 value;

	/* TODO: move IOAPIC code to dsdt.asl */
	pci_write_config32(SOC_GNB_DEV, NB_IOAPIC_INDEX, 0);
	pci_write_config32(SOC_GNB_DEV, NB_IOAPIC_DATA, 5);

	/* disable No Snoop */
	value = pci_read_config32(SOC_HDA0_DEV, HDA_DEV_CTRL_STATUS);
	value &= ~HDA_NO_SNOOP_EN;
	pci_write_config32(SOC_HDA0_DEV, HDA_DEV_CTRL_STATUS, value);
}

void domain_enable_resources(struct device *dev)
{
	/* Must be called after PCI enumeration and resource allocation */
	if (!acpi_is_wakeup_s3())
		do_agesawrapper(AMD_INIT_MID, "amdinitmid");
}

void domain_read_resources(struct device *dev)
{
	uint64_t uma_base = get_uma_base();
	uint32_t uma_size = get_uma_size();
	uint32_t mem_useable = cbmem_top();
	uint32_t tom = get_top_of_mem_below_4gb();
	uint64_t high_tom = get_top_of_mem_above_4gb();
	uint64_t high_mem_useable;
	int idx = 0x10;

	pci_domain_read_resources(dev);

	fixed_io_range_reserved(dev, idx++, PCI_IO_CONFIG_INDEX, PCI_IO_CONFIG_PORT_COUNT);

	/* 0x0 -> 0x9ffff */
	ram_range(dev, idx++, 0, 0xa0000);

	/* 0xa0000 -> 0xbffff: legacy VGA */
	mmio_range(dev, idx++, VGA_MMIO_BASE, VGA_MMIO_SIZE);

	/* 0xc0000 -> 0xfffff: Option ROM */
	reserved_ram_from_to(dev, idx++, 0xc0000, 1 * MiB);

	/*
	 * 0x100000 (1MiB) -> low top usable RAM
	 * cbmem_top() accounts for low UMA and TSEG if they are used.
	 */
	ram_from_to(dev, idx++, 1 * MiB, mem_useable);

	/* Low top usable RAM -> Low top RAM (bottom pci mmio hole) */
	reserved_ram_from_to(dev, idx++, mem_useable, tom);

	/* NB IOAPIC2 resource. IOMMU_IOAPIC_IDX is used as index, so that the common AMD MADT
	   code can find this resource */
	mmio_range(dev, IOMMU_IOAPIC_IDX, IO_APIC2_ADDR, 0x1000);

	/* If there is memory above 4GiB */
	if (high_tom >> 32) {
		/* 4GiB -> high top usable */
		if (uma_base >= (4ull * GiB))
			high_mem_useable = uma_base;
		else
			high_mem_useable = high_tom;

		ram_from_to(dev, idx++, 4ull * GiB, high_mem_useable);

		/* High top usable RAM -> high top RAM */
		if (uma_base >= (4ull * GiB)) {
			reserved_ram_range(dev, idx++, uma_base, uma_size);
		}
	}
}

__weak void set_board_env_params(GNB_ENV_CONFIGURATION *params) { }

void SetNbEnvParams(GNB_ENV_CONFIGURATION *params)
{
	params->IommuSupport = is_dev_enabled(DEV_PTR(iommu));
	set_board_env_params(params);
}

void SetNbMidParams(GNB_MID_CONFIGURATION *params)
{
	/* 0=Primary and decode all VGA resources, 1=Secondary - decode none */
	params->iGpuVgaMode = 0;
	params->GnbIoapicAddress = IO_APIC2_ADDR;
}
