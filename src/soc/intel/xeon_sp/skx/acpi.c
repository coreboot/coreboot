/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/smp/mpspec.h>
#include <intelblocks/acpi.h>
#include <device/pci.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/pm.h>
#include <string.h>

#include "chip.h"

static void uncore_inject_dsdt(void)
{
	size_t hob_size;
	const uint8_t uds_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob = fsp_find_extension_hob_by_guid(uds_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	acpigen_write_scope("\\_SB");
	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		IIO_RESOURCE_INSTANCE iio_resource =
			hob->PlatformData.IIO_resource[socket];
		for (int stack = 0; stack <= PSTACK2; ++stack) {
			const STACK_RES *ri = &iio_resource.StackRes[stack];
			char rtname[16];
			snprintf(rtname, sizeof(rtname), "RT%02x",
				(socket*MAX_IIO_STACK)+stack);

			acpigen_write_name(rtname);
			printk(BIOS_DEBUG, "\tCreating ResourceTemplate %s for socket: %d, stack: %d\n",
				rtname, socket, stack);

			acpigen_write_resourcetemplate_header();

			/* bus resource */
			acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase, ri->BusLimit,
				0x0, (ri->BusLimit - ri->BusBase + 1));

			// additional io resources on socket 0 bus 0
			if (socket == 0 && stack == 0) {
				/* ACPI 6.4.2.5 I/O Port Descriptor */
				acpigen_write_io16(0xCF8, 0xCFF, 0x1, 0x8, 1);

				/* IO decode  CF8-CFF */
				acpigen_resource_word(1, 0xc, 0x3, 0, 0x0000, 0x03AF,
					0, 0x03B0);
				acpigen_resource_word(1, 0xc, 0x3, 0, 0x03E0, 0x0CF7,
					0, 0x0918);
				acpigen_resource_word(1, 0xc, 0x3, 0, 0x03B0, 0x03BB,
					0, 0x000C);
				acpigen_resource_word(1, 0xc, 0x3, 0, 0x03C0, 0x03DF,
					0, 0x0020);
			}

			/* IO resource */
			acpigen_resource_word(1, 0xc, 0x3, 0, ri->PciResourceIoBase,
				ri->PciResourceIoLimit, 0x0,
				(ri->PciResourceIoLimit - ri->PciResourceIoBase + 1));

			// additional mem32 resources on socket 0 bus 0
			if (socket == 0 && stack == 0) {
				acpigen_resource_dword(0, 0xc, 3, 0, VGA_BASE_ADDRESS,
					(VGA_BASE_ADDRESS + VGA_BASE_SIZE - 1), 0x0,
					VGA_BASE_SIZE);
				acpigen_resource_dword(0, 0xc, 1, 0, SPI_BASE_ADDRESS,
					(SPI_BASE_ADDRESS + SPI_BASE_SIZE - 1), 0x0,
					SPI_BASE_SIZE);
			}

			/* Mem32 resource */
			acpigen_resource_dword(0, 0xc, 1, 0, ri->PciResourceMem32Base,
				ri->PciResourceMem32Limit, 0x0,
				(ri->PciResourceMem32Limit - ri->PciResourceMem32Base + 1));

			/* Mem64 resource */
			acpigen_resource_qword(0, 0xc, 1, 0, ri->PciResourceMem64Base,
				ri->PciResourceMem64Limit, 0x0,
				(ri->PciResourceMem64Limit - ri->PciResourceMem64Base + 1));

			acpigen_write_resourcetemplate_footer();
		}
	}
	acpigen_pop_len();
}

static unsigned long xeonsp_acpi_create_madt_lapics(unsigned long current)
{
	struct device *cpu;
	int num_cpus = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
			(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current,
			num_cpus, cpu->path.apic.apic_id);
	}

	return current;
}

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	struct device *cpu;
	int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
		   (cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		printk(BIOS_DEBUG, "SRAT: lapic cpu_index=%02x, node_id=%02x, apic_id=%02x\n",
			cpu_index, cpu->path.apic.node_id, cpu->path.apic.apic_id);
		current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current,
			cpu->path.apic.node_id, cpu->path.apic.apic_id);
		cpu_index++;
	}
	return current;
}

static unsigned long acpi_fill_srat(unsigned long current)
{
	acpi_srat_mem_t srat_mem[MAX_ACPI_MEMORY_AFFINITY_COUNT];
	unsigned int mem_count;

	/* create all subtables for processors */
	current = acpi_create_srat_lapics(current);

	mem_count = get_srat_memory_entries(srat_mem);
	for (int i = 0; i < mem_count; ++i) {
		printk(BIOS_DEBUG, "adding srat memory %d entry length: %d, addr: 0x%x%x, "
			"length: 0x%x%x, proximity_domain: %d, flags: %x\n",
			i, srat_mem[i].length,
			srat_mem[i].base_address_high, srat_mem[i].base_address_low,
			srat_mem[i].length_high, srat_mem[i].length_low,
			srat_mem[i].proximity_domain, srat_mem[i].flags);
		memcpy((acpi_srat_mem_t *)current, &srat_mem[i], sizeof(srat_mem[i]));
		current += srat_mem[i].length;
	}

	return current;
}

static unsigned long acpi_fill_slit(unsigned long current)
{
	int nodes = get_cpu_count();

	uint8_t *p = (uint8_t *)current;
	memset(p, 0, 8 + nodes * nodes);
	*p = (uint8_t)nodes;
	p += 8;

	/* this assumes fully connected socket topology */
	for (int i = 0; i < nodes; i++) {
		for (int j = 0; j < nodes; j++) {
			if (i == j)
				p[i*nodes+j] = 10;
			else
				p[i*nodes+j] = 16;
		}
	}

	current += 8+nodes*nodes;
	return current;
}

static int get_stack_for_port(int p)
{
	if (p == 0)
		return CSTACK;
	else if (p >= PORT_1A && p <= PORT_1D)
		return PSTACK0;
	else if (p >= PORT_2A && p <= PORT_2D)
		return PSTACK1;
	else if (p >= PORT_3A && p <= PORT_3D)
		return PSTACK2;
	else if (p >= PORT_4A && p <= PORT_4D)
		return PSTACK3; // MCP0
	else
		return PSTACK4; // MCP1
}

static unsigned long acpi_create_drhd(unsigned long current, int socket, int stack)
{
	int IoApicID[] = {
		// socket 0
		PC00_IOAPIC_ID, PC01_IOAPIC_ID, PC02_IOAPIC_ID, PC03_IOAPIC_ID,
		PC04_IOAPIC_ID, PC05_IOAPIC_ID,
		// socket 1
		PC06_IOAPIC_ID, PC07_IOAPIC_ID, PC08_IOAPIC_ID, PC09_IOAPIC_ID,
		PC10_IOAPIC_ID, PC11_IOAPIC_ID,
	};

	uint32_t enum_id;
	unsigned long tmp = current;

	size_t hob_size;
	const uint8_t fsp_hob_iio_universal_data_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob = fsp_find_extension_hob_by_guid(
		fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	uint32_t bus = hob->PlatformData.CpuQpiInfo[socket].StackBus[stack];
	uint32_t pcie_seg = hob->PlatformData.CpuQpiInfo[socket].PcieSegment;
	uint32_t reg_base =
		hob->PlatformData.IIO_resource[socket].StackRes[stack].VtdBarAddress;
	printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, pcie_seg: 0x%x, reg_base: 0x%x\n",
		__func__, socket, stack, bus, pcie_seg, reg_base);

	// Add DRHD Hardware Unit
	if (socket == 0 && stack == CSTACK) {
		printk(BIOS_DEBUG, "[Hardware Unit Definition] Flags: 0x%x, PCI Segment Number: 0x%x, "
			"Register Base Address: 0x%x\n",
			DRHD_INCLUDE_PCI_ALL, pcie_seg, reg_base);
		current += acpi_create_dmar_drhd(current, DRHD_INCLUDE_PCI_ALL,
			pcie_seg, reg_base);
	} else {
		printk(BIOS_DEBUG, "[Hardware Unit Definition] Flags: 0x%x, PCI Segment Number: 0x%x, "
			"Register Base Address: 0x%x\n", 0, pcie_seg, reg_base);
		current += acpi_create_dmar_drhd(current, 0, pcie_seg, reg_base);
	}

	// Add PCH IOAPIC
	if (socket == 0 && stack == CSTACK) {
		printk(BIOS_DEBUG, "    [IOAPIC Device] Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
			"PCI Path: 0x%x, 0x%x\n",
			PCH_IOAPIC_ID, PCH_IOAPIC_BUS_NUMBER,
			PCH_IOAPIC_DEV_NUM, PCH_IOAPIC_FUNC_NUM);
		current += acpi_create_dmar_ds_ioapic(current, PCH_IOAPIC_ID,
			PCH_IOAPIC_BUS_NUMBER, PCH_IOAPIC_DEV_NUM, PCH_IOAPIC_FUNC_NUM);
	}

	// Add IOAPIC entry
	enum_id = IoApicID[(socket*MAX_IIO_STACK)+stack];
	printk(BIOS_DEBUG, "    [IOAPIC Device] Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
		"PCI Path: 0x%x, 0x%x\n", enum_id, bus, APIC_DEV_NUM, APIC_FUNC_NUM);
	current += acpi_create_dmar_ds_ioapic(current, enum_id, bus,
		APIC_DEV_NUM, APIC_FUNC_NUM);

	// Add CBDMA devices for CSTACK
	if (socket != 0 && stack == CSTACK) {
		for (int cbdma_func_id = 0; cbdma_func_id < 8; ++cbdma_func_id) {
			printk(BIOS_DEBUG, "    [PCI Endpoint Device] Enumeration ID: 0x%x, "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				0, bus, CBDMA_DEV_NUM, cbdma_func_id);
			current += acpi_create_dmar_ds_pci(current,
				bus, CBDMA_DEV_NUM, cbdma_func_id);
		}
	}

	// Add PCIe Ports
	if (socket != 0 || stack != CSTACK) {
		IIO_RESOURCE_INSTANCE iio_resource =
			hob->PlatformData.IIO_resource[socket];
		for (int p = 0; p < NUMBER_PORTS_PER_SOCKET; ++p) {
			if (get_stack_for_port(p) != stack)
				continue;

			uint32_t dev = iio_resource.PcieInfo.PortInfo[p].Device;
			uint32_t func = iio_resource.PcieInfo.PortInfo[p].Function;

			uint32_t id = pci_mmio_read_config32(PCI_DEV(bus, dev, func),
				PCI_VENDOR_ID);
			if (id == 0xffffffff)
				continue;

			printk(BIOS_DEBUG, "    [PCI Bridge Device] Enumeration ID: 0x%x, "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				0, bus, dev, func);
			current += acpi_create_dmar_ds_pci_br(current,
				bus, dev, func);
		}

		// Add VMD
		if (hob->PlatformData.VMDStackEnable[socket][stack] &&
			stack >= PSTACK0 && stack <= PSTACK2) {
			printk(BIOS_DEBUG, "    [PCI Endpoint Device] Enumeration ID: 0x%x, "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				 0, bus, VMD_DEV_NUM, VMD_FUNC_NUM);
			current += acpi_create_dmar_ds_pci(current,
				bus, VMD_DEV_NUM, VMD_FUNC_NUM);
		}
	}

	// Add HPET
	if (socket == 0 && stack == CSTACK) {
		uint16_t hpet_capid = read16((void *)HPET_BASE_ADDRESS);
		uint16_t num_hpets = (hpet_capid >> 0x08) & 0x1F;  // Bits [8:12] has hpet count
		printk(BIOS_SPEW, "%s hpet_capid: 0x%x, num_hpets: 0x%x\n",
			__func__, hpet_capid, num_hpets);
		//BIT 15
		if (num_hpets && (num_hpets != 0x1f) &&
			(read32((void *)(HPET_BASE_ADDRESS + 0x100)) & (0x00008000))) {
			printk(BIOS_DEBUG, "    [Message-capable HPET Device] Enumeration ID: 0x%x, "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				0, HPET_BUS_NUM, HPET_DEV_NUM, HPET0_FUNC_NUM);
			current += acpi_create_dmar_ds_msi_hpet(current, 0, HPET_BUS_NUM,
				HPET_DEV_NUM, HPET0_FUNC_NUM);
		}
	}

	acpi_dmar_drhd_fixup(tmp, current);

	return current;
}

static unsigned long acpi_create_atsr(unsigned long current)
{
	size_t hob_size;
	const uint8_t uds_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob = fsp_find_extension_hob_by_guid(uds_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		uint32_t pcie_seg = hob->PlatformData.CpuQpiInfo[socket].PcieSegment;
		unsigned long tmp = current;
		bool first = true;
		IIO_RESOURCE_INSTANCE iio_resource =
			hob->PlatformData.IIO_resource[socket];

		for (int stack = 0; stack <= PSTACK2; ++stack) {
			uint32_t bus = hob->PlatformData.CpuQpiInfo[socket].StackBus[stack];
			uint32_t vtd_base = iio_resource.StackRes[stack].VtdBarAddress;
			if (!vtd_base)
				continue;
			uint64_t vtd_mmio_cap = read64((void *)(vtd_base + VTD_EXT_CAP_LOW));
			printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, vtd_base: 0x%x, "
				"vtd_mmio_cap: 0x%llx\n",
				__func__, socket, stack, bus, vtd_base, vtd_mmio_cap);

			// ATSR is applicable only for platform supporting device IOTLBs
			// through the VT-d extended capability register
			assert(vtd_mmio_cap != 0xffffffffffffffff);
			if ((vtd_mmio_cap & 0x4) == 0) // BIT 2
				continue;

			for (int p = 0; p < NUMBER_PORTS_PER_SOCKET; ++p) {
				if (socket == 0 && p == 0)
					continue;
				if (get_stack_for_port(p) != stack)
					continue;

				uint32_t dev = iio_resource.PcieInfo.PortInfo[p].Device;
				uint32_t func = iio_resource.PcieInfo.PortInfo[p].Function;

				u32 id = pci_mmio_read_config32(PCI_DEV(bus, dev, func),
					PCI_VENDOR_ID);
				if (id == 0xffffffff)
					continue;

				if (first) {
					printk(BIOS_DEBUG, "[Root Port ATS Capability] Flags: 0x%x, "
						"PCI Segment Number: 0x%x\n",
						0, pcie_seg);
					current += acpi_create_dmar_atsr(current, 0, pcie_seg);
					first = 0;
				}

				printk(BIOS_DEBUG, "    [PCI Bridge Device] Enumeration ID: 0x%x, "
					"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
					0, bus, dev, func);
				current += acpi_create_dmar_ds_pci_br(current, bus, dev, func);
			}
		}
		if (tmp != current)
			acpi_dmar_atsr_fixup(tmp, current);
	}

	return current;
}

static unsigned long acpi_create_rmrr(unsigned long current)
{
	uint32_t size = ALIGN_UP(MEM_BLK_COUNT * sizeof(MEM_BLK), 0x1000);

	uint32_t *ptr;

	// reserve memory
	ptr = cbmem_find(CBMEM_ID_STORAGE_DATA);
	if (!ptr) {
		ptr = cbmem_add(CBMEM_ID_STORAGE_DATA, size);
		assert(ptr != NULL);
		memset(ptr, 0, size);
	}

	unsigned long tmp = current;
	printk(BIOS_DEBUG, "[Reserved Memory Region] PCI Segment Number: 0x%x, Base Address: 0x%x, "
		"End Address (limit): 0x%x\n",
		0, (uint32_t) ptr, (uint32_t) ((uint32_t) ptr + size - 1));
	current += acpi_create_dmar_rmrr(current, 0, (uint32_t) ptr,
		(uint32_t) ((uint32_t) ptr + size - 1));

	printk(BIOS_DEBUG, "    [PCI Endpoint Device] Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
		"PCI Path: 0x%x, 0x%x\n",
		 0, XHCI_BUS_NUMBER, PCH_DEV_SLOT_XHCI, XHCI_FUNC_NUM);
	current += acpi_create_dmar_ds_pci(current, XHCI_BUS_NUMBER,
		PCH_DEV_SLOT_XHCI, XHCI_FUNC_NUM);

	acpi_dmar_rmrr_fixup(tmp, current);

	return current;
}

static unsigned long acpi_create_rhsa(unsigned long current)
{
	size_t hob_size;
	const uint8_t uds_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob = fsp_find_extension_hob_by_guid(uds_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		IIO_RESOURCE_INSTANCE iio_resource =
			hob->PlatformData.IIO_resource[socket];
		for (int stack = 0; stack <= PSTACK2; ++stack) {
			uint32_t vtd_base = iio_resource.StackRes[stack].VtdBarAddress;
			if (!vtd_base)
				continue;

			printk(BIOS_DEBUG, "[Remapping Hardware Static Affinity] Base Address: 0x%x, "
				"Proximity Domain: 0x%x\n", vtd_base, socket);
			current += acpi_create_dmar_rhsa(current, vtd_base, socket);
		}
	}

	return current;
}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	size_t hob_size;
	const uint8_t uds_guid[16] = FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob = fsp_find_extension_hob_by_guid(uds_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	// DRHD
	for (int iio = 1; iio <= hob->PlatformData.numofIIO; ++iio) {
		int socket = iio;
		if (socket == hob->PlatformData.numofIIO) // socket 0 should be last DRHD entry
			socket = 0;

		if (socket == 0) {
			for (int stack = 1; stack <= PSTACK2; ++stack)
				current = acpi_create_drhd(current, socket, stack);
			current = acpi_create_drhd(current, socket, CSTACK);
		} else {
			for (int stack = 0; stack <= PSTACK2; ++stack)
				current = acpi_create_drhd(current, socket, stack);
		}
	}

	// RMRR
	current = acpi_create_rmrr(current);

	// ATSR - causes hang
	current = acpi_create_atsr(current);

	// RHSA
	current = acpi_create_rhsa(current);

	return current;
}

unsigned long northbridge_write_acpi_tables(const struct device *device,
					    unsigned long current,
					    struct acpi_rsdp *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;
	acpi_dmar_t *dmar;

	const struct soc_intel_xeon_sp_skx_config *const config = config_of(device);

	/* SRAT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *) current;
	acpi_create_srat(srat, acpi_fill_srat);
	current += srat->header.length;
	acpi_add_table(rsdp, srat);

	/* SLIT */
	current = ALIGN(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *) current;
	acpi_create_slit(slit, acpi_fill_slit);
	current += slit->header.length;
	acpi_add_table(rsdp, slit);

	/* DMAR */
	if (config->vtd_support) {
		current = ALIGN(current, 8);
		dmar = (acpi_dmar_t *)current;
		printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
		printk(BIOS_DEBUG, "[DMA Remapping table] Flags: 0x%x\n",
			(DMAR_INTR_REMAP | DMAR_X2APIC_OPT_OUT));
		acpi_create_dmar(dmar, (DMAR_INTR_REMAP | DMAR_X2APIC_OPT_OUT), acpi_fill_dmar);
		current += dmar->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, dmar);
	}

	return current;
}

void acpi_init_gnvs(struct global_nvs *gnvs)
{
	/* CPU core count */
	gnvs->pcnt = dev_count_cpu();
	printk(BIOS_DEBUG, "%s gnvs->pcnt: %d\n", __func__, gnvs->pcnt);

	/* Update the mem console pointer. */
	if (CONFIG(CONSOLE_CBMEM))
		gnvs->cbmc = (uint32_t)cbmem_find(CBMEM_ID_CONSOLE);
}

uint32_t soc_read_sci_irq_select(void)
{
	struct device *dev = PCH_DEV_PMC;

	if (!dev)
		return 0;

	return pci_read_config32(dev, PMC_ACPI_CNT);
}

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;
	else
		return MP_IRQ_POLARITY_HIGH;
}

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = 0;
	return NULL;
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
		CONFIG_MMCONF_BASE_ADDRESS, 0, 0, 255);
	return current;
}

static int acpi_sci_irq(void)
{
	int sci_irq = 9;
	int32_t scis;

	scis = soc_read_sci_irq_select();
	scis &= SCI_IRQ_SEL;
	scis >>= SCI_IRQ_ADJUST;

	/* Determine how SCI is routed. */
	switch (scis) {
	case SCIS_IRQ9:
	case SCIS_IRQ10:
	case SCIS_IRQ11:
		sci_irq = scis - SCIS_IRQ9 + 9;
		break;
	case SCIS_IRQ20:
	case SCIS_IRQ21:
	case SCIS_IRQ22:
	case SCIS_IRQ23:
		sci_irq = scis - SCIS_IRQ20 + 20;
		break;
	default:
		printk(BIOS_DEBUG, "Invalid SCI route! Defaulting to IRQ9.\n");
		sci_irq = 9;
		break;
	}

	printk(BIOS_DEBUG, "SCI is IRQ%d\n", sci_irq);
	return sci_irq;
}

static unsigned long acpi_madt_irq_overrides(unsigned long current)
{
	int sci = acpi_sci_irq();
	uint16_t flags = MP_IRQ_TRIGGER_LEVEL;

	/* INT_SRC_OVR */
	current += acpi_create_madt_irqoverride((void *)current, 0, 0, 2, 0);

	flags |= soc_madt_sci_irq_polarity(sci);

	/* SCI */
	current += acpi_create_madt_irqoverride((void *)current, 0, sci, sci, flags);

	current +=
		acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *) current, 0xff, 0x0d, 1);

	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	size_t hob_size = 0;
	const uint8_t fsp_hob_iio_universal_data_guid[16] =
		FSP_HOB_IIO_UNIVERSAL_DATA_GUID;
	const IIO_UDS *hob;
	int cur_stack;

	int gsi_bases[] = { 0, 0x18, 0x20, 0x28, 0x30, 0x48, 0x50, 0x58, 0x60 };
	int ioapic_ids[] = { 0x8, 0x9, 0xa, 0xb, 0xc, 0xf, 0x10, 0x11, 0x12 };

	/* Local APICs */
	current = xeonsp_acpi_create_madt_lapics(current);

	hob = fsp_find_extension_hob_by_guid(fsp_hob_iio_universal_data_guid, &hob_size);
	assert(hob != NULL && hob_size != 0);

	cur_stack = 0;
	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			// TODO: do we have situation with only bus 0 and one stack?
			if (ri->BusBase != ri->BusLimit) {
				assert(cur_stack < ARRAY_SIZE(ioapic_ids));
				assert(cur_stack < ARRAY_SIZE(gsi_bases));
				int ioapic_id = ioapic_ids[cur_stack];
				int gsi_base = gsi_bases[cur_stack];
				printk(BIOS_DEBUG, "Adding MADT IOAPIC for socket: %d, stack: %d, ioapic_id: 0x%x, "
					"ioapic_base: 0x%x, gsi_base: 0x%x\n",
					socket, stack,  ioapic_id, ri->IoApicBase, gsi_base);
				current += acpi_create_madt_ioapic(
					(acpi_madt_ioapic_t *)current,
					ioapic_id, ri->IoApicBase, gsi_base);
				++cur_stack;

				if (socket == 0 && stack == 0) {
					assert(cur_stack < ARRAY_SIZE(ioapic_ids));
					assert(cur_stack < ARRAY_SIZE(gsi_bases));
					ioapic_id = ioapic_ids[cur_stack];
					gsi_base = gsi_bases[cur_stack];
					printk(BIOS_DEBUG, "Adding MADT IOAPIC for socket: %d, stack: %d, ioapic_id: 0x%x, "
						"ioapic_base: 0x%x, gsi_base: 0x%x\n",
						socket, stack,  ioapic_id,
						ri->IoApicBase + 0x1000, gsi_base);
					current += acpi_create_madt_ioapic(
						(acpi_madt_ioapic_t *)current,
						ioapic_id, ri->IoApicBase + 0x1000, gsi_base);
					++cur_stack;
				}
			}
		}
	}

	return acpi_madt_irq_overrides(current);
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->header.revision = get_acpi_table_revision(FADT);

	fadt->sci_int = acpi_sci_irq();

	/* TODO: enabled SMM mode switch when SMM handlers are set up. */
	if (0 && permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}

	/* Power Control */
	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;
	fadt->pm2_cnt_blk = pmbase + PM2_CNT;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->gpe0_blk = pmbase + GPE0_STS(0);

	/* Control Registers - Length */
	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;
	/* There are 4 GPE0 STS/EN pairs each 32 bits wide. */
	fadt->gpe0_blk_len = 2 * GPE0_REG_MAX * sizeof(uint32_t);
	fadt->p_lvl2_lat = ACPI_FADT_C2_NOT_SUPPORTED;
	fadt->p_lvl3_lat = ACPI_FADT_C3_NOT_SUPPORTED;
	fadt->duty_offset = 1;
	fadt->duty_width = 0;

	/* RTC Registers */
	fadt->day_alrm = 0x0d;
	fadt->mon_alrm = 0x00;
	fadt->century = 0x00;
	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	fadt->flags |= ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
		       ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
		       ACPI_FADT_SLEEP_TYPE | ACPI_FADT_S4_RTC_WAKE |
		       ACPI_FADT_PLATFORM_CLOCK;

	/* PM1 Status & PM1 Enable */
	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = 32;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = fadt->pm1a_evt_blk;
	fadt->x_pm1a_evt_blk.addrh = 0x00;

	/* PM1 Control Registers */
	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = 16;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.addrh = 0x00;

	/* PM2 Control Registers */
	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = fadt->pm2_cnt_blk;
	fadt->x_pm2_cnt_blk.addrh = 0x00;

	/* PM1 Timer Register */
	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = 32;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x00;

	/*  General-Purpose Event Registers */
	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = 64; /* EventStatus + EventEnable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0x00;
}

unsigned long southbridge_write_acpi_tables(const struct device *device,
					     unsigned long current,
					     struct acpi_rsdp *rsdp)
{
	current = acpi_write_hpet(device, current, rsdp);
	current = (ALIGN(current, 16));
	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}

void southbridge_inject_dsdt(const struct device *device)
{
	struct global_nvs *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, sizeof(*gnvs));
		if (gnvs)
			memset(gnvs, 0, sizeof(*gnvs));
	}

	if (gnvs) {
		acpi_create_gnvs(gnvs);
		/* TODO: tell SMI about it, if HAVE_SMI_HANDLER */
		// apm_control(APM_CNT_GNVS_UPDATE);

		/* Add it to DSDT.  */
		printk(BIOS_SPEW, "%s injecting NVSA with 0x%x\n", __FILE__, (uint32_t)gnvs);
		acpigen_write_scope("\\");
		acpigen_write_name_dword("NVSA", (uint32_t)gnvs);
		acpigen_pop_len();
	}

	// Add IIOStack ACPI Resource Templates
	uncore_inject_dsdt();
}

void generate_p_state_entries(int core, int cores_per_package)
{
}

static acpi_tstate_t xeon_sp_tss_table[] = {
	{ 100, 1000, 0, 0x00, 0 },
	{ 88, 875, 0, 0x1e, 0 },
	{ 75, 750, 0, 0x1c, 0 },
	{ 63, 625, 0, 0x1a, 0 },
	{ 50, 500, 0, 0x18, 0 },
	{ 38, 375, 0, 0x16, 0 },
	{ 25, 250, 0, 0x14, 0 },
	{ 13, 125, 0, 0x12, 0 },
};

acpi_tstate_t *soc_get_tss_table(int *entries)
{
	*entries = ARRAY_SIZE(xeon_sp_tss_table);
	return xeon_sp_tss_table;
}

void generate_t_state_entries(int core, int cores_per_package)
{
}

void generate_cpu_entries(const struct device *device)
{
	int core_id, cpu_id, pcontrol_blk = ACPI_BASE_ADDRESS;
	int plen = 6;
	int total_threads = dev_count_cpu();
	int threads_per_package = get_threads_per_package();
	int numcpus = total_threads / threads_per_package;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d core(s) each, totalcores: %d.\n",
	       numcpus, threads_per_package, total_threads);

	for (cpu_id = 0; cpu_id < numcpus; cpu_id++) {
		for (core_id = 0; core_id < threads_per_package; core_id++) {
			if (core_id > 0) {
				pcontrol_blk = 0;
				plen = 0;
			}

			/* Generate processor \_PR.CPUx */
			acpigen_write_processor((cpu_id) * threads_per_package +
						core_id, pcontrol_blk, plen);

			/* NOTE: Intel idle driver doesn't use ACPI C-state tables */

			/* TODO: Soc specific power states generation */
			acpigen_pop_len();
		}
	}
	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, threads_per_package);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(threads_per_package);
}
