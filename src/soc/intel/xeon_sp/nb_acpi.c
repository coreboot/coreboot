/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <arch/hpet.h>
#include <assert.h>
#include <cbmem.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/hest.h>
#include <soc/iomap.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <intelblocks/p2sb.h>

#include "chip.h"

/* Northbridge(NUMA) ACPI table generation. SRAT, SLIT, etc */

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	struct device *cpu;
	unsigned int cpu_index = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if (!is_enabled_cpu(cpu))
			continue;
		printk(BIOS_DEBUG, "SRAT: lapic cpu_index=%02x, node_id=%02x, apic_id=%02x\n",
			cpu_index, cpu->path.apic.node_id, cpu->path.apic.apic_id);
		current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current,
			cpu->path.apic.node_id, cpu->path.apic.apic_id);
		cpu_index++;
	}
	return current;
}

static unsigned int get_srat_memory_entries(acpi_srat_mem_t *srat_mem)
{
	const struct SystemMemoryMapHob *memory_map;
	unsigned int mmap_index;

	memory_map = get_system_memory_map();
	assert(memory_map);
	printk(BIOS_DEBUG, "memory_map: %p\n", memory_map);

	mmap_index = 0;
	for (int e = 0; e < memory_map->numberEntries; ++e) {
		const struct SystemMemoryMapElement *mem_element = &memory_map->Element[e];
		uint64_t addr =
			(uint64_t) ((uint64_t)mem_element->BaseAddress <<
				MEM_ADDR_64MB_SHIFT_BITS);
		uint64_t size =
			(uint64_t) ((uint64_t)mem_element->ElementSize <<
				MEM_ADDR_64MB_SHIFT_BITS);

		printk(BIOS_DEBUG, "memory_map %d addr: 0x%llx, BaseAddress: 0x%x, size: 0x%llx, "
			"ElementSize: 0x%x, reserved: %d\n",
			e, addr, mem_element->BaseAddress, size,
			mem_element->ElementSize, (mem_element->Type & MEM_TYPE_RESERVED));

		assert(mmap_index < MAX_ACPI_MEMORY_AFFINITY_COUNT);

		/* skip reserved memory region */
		if (mem_element->Type & MEM_TYPE_RESERVED)
			continue;

		/* skip if this address is already added */
		bool skip = false;
		for (int idx = 0; idx < mmap_index; ++idx) {
			uint64_t base_addr = ((uint64_t)srat_mem[idx].base_address_high << 32) +
				srat_mem[idx].base_address_low;
			if (addr == base_addr) {
				skip = true;
				break;
			}
		}
		if (skip)
			continue;

		srat_mem[mmap_index].type = 1; /* Memory affinity structure */
		srat_mem[mmap_index].length = sizeof(acpi_srat_mem_t);
		srat_mem[mmap_index].base_address_low = (uint32_t) (addr & 0xffffffff);
		srat_mem[mmap_index].base_address_high = (uint32_t) (addr >> 32);
		srat_mem[mmap_index].length_low = (uint32_t) (size & 0xffffffff);
		srat_mem[mmap_index].length_high = (uint32_t) (size >> 32);
		srat_mem[mmap_index].proximity_domain = mem_element->SocketId;
		srat_mem[mmap_index].flags = SRAT_ACPI_MEMORY_ENABLED;
		if ((mem_element->Type & MEMTYPE_VOLATILE_MASK) == 0)
			srat_mem[mmap_index].flags |= SRAT_ACPI_MEMORY_NONVOLATILE;
		++mmap_index;
	}

	return mmap_index;
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
	unsigned int nodes = soc_get_num_cpus();

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

	current += 8 + nodes * nodes;
	return current;
}

/*
 * This function adds PCIe bridge device entry in DMAR table. If it is called
 * in the context of ATSR subtable, it adds ATSR subtable when it is first called.
 */
static unsigned long acpi_create_dmar_ds_pci_br_for_port(unsigned long current,
	int port, int stack, const IIO_RESOURCE_INSTANCE *iio_resource, uint32_t pcie_seg,
	bool is_atsr, bool *first)
{

	if (soc_get_stack_for_port(port) != stack)
		return 0;

	const uint32_t bus = iio_resource->StackRes[stack].BusBase;
	const uint32_t dev = iio_resource->PcieInfo.PortInfo[port].Device;
	const uint32_t func = iio_resource->PcieInfo.PortInfo[port].Function;

	const uint32_t id = pci_s_read_config32(PCI_DEV(bus, dev, func),
		PCI_VENDOR_ID);
	if (id == 0xffffffff)
		return 0;

	unsigned long atsr_size = 0;
	unsigned long pci_br_size = 0;
	if (is_atsr && first && *first) {
		printk(BIOS_DEBUG, "[Root Port ATS Capability] Flags: 0x%x, "
			"PCI Segment Number: 0x%x\n", 0, pcie_seg);
		atsr_size = acpi_create_dmar_atsr(current, 0, pcie_seg);
		*first = false;
	}

	printk(BIOS_DEBUG, "    [PCI Bridge Device] Enumeration ID: 0x%x, "
		"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
		0, bus, dev, func);
	pci_br_size = acpi_create_dmar_ds_pci_br(current + atsr_size, bus, dev, func);

	return (atsr_size + pci_br_size);
}

static unsigned long acpi_create_drhd(unsigned long current, int socket,
	int stack, const IIO_UDS *hob)
{
	uint32_t enum_id;
	unsigned long tmp = current;

	uint32_t bus = hob->PlatformData.IIO_resource[socket].StackRes[stack].BusBase;
	uint32_t pcie_seg = hob->PlatformData.CpuQpiInfo[socket].PcieSegment;
	uint32_t reg_base =
		hob->PlatformData.IIO_resource[socket].StackRes[stack].VtdBarAddress;
	printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, pcie_seg: 0x%x, reg_base: 0x%x\n",
		__func__, socket, stack, bus, pcie_seg, reg_base);

	/* Do not generate DRHD for non-PCIe stack */
	if (!reg_base)
		return current;

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
		union p2sb_bdf ioapic_bdf = p2sb_get_ioapic_bdf();
		printk(BIOS_DEBUG, "    [IOAPIC Device] Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
			"PCI Path: 0x%x, 0x%x\n",
		       PCH_IOAPIC_ID, ioapic_bdf.bus, ioapic_bdf.dev, ioapic_bdf.fn);
		current += acpi_create_dmar_ds_ioapic(current, PCH_IOAPIC_ID,
						      ioapic_bdf.bus, ioapic_bdf.dev, ioapic_bdf.fn);
	}

	// Add IOAPIC entry
	enum_id = soc_get_iio_ioapicid(socket, stack);
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
		for (int p = PORT_0; p < MAX_PORTS; ++p)
			current += acpi_create_dmar_ds_pci_br_for_port(current, p, stack,
				&iio_resource, pcie_seg, false, NULL);

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
			union p2sb_bdf hpet_bdf = p2sb_get_hpet_bdf();
			printk(BIOS_DEBUG, "    [Message-capable HPET Device] Enumeration ID: 0x%x, "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				0, hpet_bdf.bus, hpet_bdf.dev, hpet_bdf.fn);
			current += acpi_create_dmar_ds_msi_hpet(current, 0, hpet_bdf.bus,
				hpet_bdf.dev, hpet_bdf.fn);
		}
	}

	acpi_dmar_drhd_fixup(tmp, current);

	return current;
}

static unsigned long acpi_create_atsr(unsigned long current, const IIO_UDS *hob)
{
	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		uint32_t pcie_seg = hob->PlatformData.CpuQpiInfo[socket].PcieSegment;
		unsigned long tmp = current;
		bool first = true;
		IIO_RESOURCE_INSTANCE iio_resource =
			hob->PlatformData.IIO_resource[socket];

		for (int stack = 0; stack <= PSTACK2; ++stack) {
			uint32_t bus = iio_resource.StackRes[stack].BusBase;
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

			for (int p = PORT_0; p < MAX_PORTS; ++p) {
				if (socket == 0 && p == PORT_0)
					continue;
				current += acpi_create_dmar_ds_pci_br_for_port(current, p,
					stack, &iio_resource, pcie_seg, true, &first);
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
		assert(ptr);
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
	const IIO_UDS *hob = get_iio_uds();

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
	const IIO_UDS *hob = get_iio_uds();

	// DRHD
	for (int iio = 1; iio <= hob->PlatformData.numofIIO; ++iio) {
		int socket = iio;
		if (socket == hob->PlatformData.numofIIO) // socket 0 should be last DRHD entry
			socket = 0;

		if (socket == 0) {
			for (int stack = 1; stack <= PSTACK2; ++stack)
				current = acpi_create_drhd(current, socket, stack, hob);
			current = acpi_create_drhd(current, socket, CSTACK, hob);
		} else {
			for (int stack = 0; stack <= PSTACK2; ++stack)
				current = acpi_create_drhd(current, socket, stack, hob);
		}
	}

	// RMRR
	current = acpi_create_rmrr(current);

	// Root Port ATS Capability
	current = acpi_create_atsr(current, hob);

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

	const config_t *const config = config_of(device);

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
		enum dmar_flags flags = DMAR_INTR_REMAP;

		/* SKX FSP doesn't support X2APIC, but CPX FSP does */
		if (CONFIG(SOC_INTEL_SKYLAKE_SP))
			flags |= DMAR_X2APIC_OPT_OUT;

		printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
		printk(BIOS_DEBUG, "[DMA Remapping table] Flags: 0x%x\n", flags);
		acpi_create_dmar(dmar, flags, acpi_fill_dmar);
		current += dmar->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, dmar);
	}

	if (CONFIG(SOC_ACPI_HEST))
		current = hest_create(current, rsdp);

	return current;
}
