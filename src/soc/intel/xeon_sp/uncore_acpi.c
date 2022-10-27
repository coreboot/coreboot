/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <arch/hpet.h>
#include <arch/ioapic.h>
#include <assert.h>
#include <cbmem.h>
#include <cpu/x86/lapic.h>
#include <commonlib/sort.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <soc/acpi.h>
#include <soc/hest.h>
#include <soc/iomap.h>
#include <soc/numa.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <intelblocks/p2sb.h>

#include "chip.h"

/* NUMA related ACPI table generation. SRAT, SLIT, etc */

/* Increase if necessary. Currently all x86 CPUs only have 2 SMP threads */
#define MAX_THREAD 2

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	struct device *cpu;
	unsigned int num_cpus = 0;
	int apic_ids[CONFIG_MAX_CPUS] = {};

	unsigned int sort_start = 0;
	for (unsigned int thread_id = 0; thread_id < MAX_THREAD; thread_id++) {
		for (cpu = all_devices; cpu; cpu = cpu->next) {
			if (!is_enabled_cpu(cpu))
				continue;
			if (num_cpus >= ARRAY_SIZE(apic_ids))
				break;
			if (cpu->path.apic.thread_id != thread_id)
				continue;
			apic_ids[num_cpus++] = cpu->path.apic.apic_id;
		}
		bubblesort(&apic_ids[sort_start], num_cpus - sort_start, NUM_ASCENDING);
		sort_start = num_cpus;
	}

	for (unsigned int i = 0; i < num_cpus; i++) {
		/* Match the sorted apic_ids to a struct device */
		for (cpu = all_devices; cpu; cpu = cpu->next) {
			if (!is_enabled_cpu(cpu))
				continue;
			if (cpu->path.apic.apic_id == apic_ids[i])
				break;
		}
		if (!cpu)
			continue;

		if (is_x2apic_mode()) {
			printk(BIOS_DEBUG, "SRAT: x2apic cpu_index=%04x, node_id=%02x, apic_id=%08x\n",
			       i, cpu->path.apic.node_id, cpu->path.apic.apic_id);

			current += acpi_create_srat_x2apic((acpi_srat_x2apic_t *)current,
				cpu->path.apic.node_id, cpu->path.apic.apic_id);
		} else {
			printk(BIOS_DEBUG, "SRAT: lapic cpu_index=%02x, node_id=%02x, apic_id=%02x\n",
			       i, cpu->path.apic.node_id, cpu->path.apic.apic_id);

			current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current,
				cpu->path.apic.node_id, cpu->path.apic.apic_id);
		}
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
			(uint64_t)((uint64_t)mem_element->BaseAddress <<
				MEM_ADDR_64MB_SHIFT_BITS);
		uint64_t size =
			(uint64_t)((uint64_t)mem_element->ElementSize <<
				MEM_ADDR_64MB_SHIFT_BITS);

		printk(BIOS_DEBUG, "memory_map %d addr: 0x%llx, BaseAddress: 0x%x, size: 0x%llx, "
			"ElementSize: 0x%x, type: %d, reserved: %d\n",
			e, addr, mem_element->BaseAddress, size,
			mem_element->ElementSize, mem_element->Type,
			(mem_element->Type & MEM_TYPE_RESERVED));

		assert(mmap_index < MAX_ACPI_MEMORY_AFFINITY_COUNT);

		/* skip reserved memory region */
		if (mem_element->Type & MEM_TYPE_RESERVED)
			continue;
#if CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP)
		/* Skip all non processor attached memory regions */
		/* In other words, skip all the types >= MemTypeCxlAccVolatileMem */
		if (mem_element->Type >= MemTypeCxlAccVolatileMem)
			continue;
#endif

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
		srat_mem[mmap_index].base_address_low = (uint32_t)(addr & 0xffffffff);
		srat_mem[mmap_index].base_address_high = (uint32_t)(addr >> 32);
		srat_mem[mmap_index].length_low = (uint32_t)(size & 0xffffffff);
		srat_mem[mmap_index].length_high = (uint32_t)(size >> 32);
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

	if (CONFIG(SOC_INTEL_HAS_CXL))
		current = cxl_fill_srat(current);

	return current;
}

#if CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP)
/*
Because pds.num_pds comes from spr/numa.c function fill_pds().
pds.num_pds = soc_get_num_cpus() + get_cxl_node_count().
*/
/* SPR-SP platform has Generic Initiator domain in addition to processor domain */
static unsigned long acpi_fill_slit(unsigned long current)
{
	uint8_t *p = (uint8_t *)current;
	/* According to table 5.60 of ACPI 6.4 spec, "Number of System Localities" field takes
	   up 8 bytes. Following that, each matrix entry takes up 1 byte. */
	memset(p, 0, 8 + pds.num_pds * pds.num_pds);
	*p = (uint8_t)pds.num_pds;
	p += 8;

	for (int i = 0; i < pds.num_pds; i++) {
		for (int j = 0; j < pds.num_pds; j++)
			p[i * pds.num_pds + j] = pds.pds[i].distances[j];
	}

	current += 8 + pds.num_pds * pds.num_pds;
	return current;
}
#else
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
#endif

/*
 * This function adds PCIe bridge device entry in DMAR table. If it is called
 * in the context of ATSR subtable, it adds ATSR subtable when it is first called.
 */
static unsigned long acpi_create_dmar_ds_pci_br_for_port(unsigned long current,
							 const struct device *bridge_dev,
							 uint32_t pcie_seg,
							 bool is_atsr, bool *first)
{
	const uint32_t bus = bridge_dev->bus->secondary;
	const uint32_t dev = PCI_SLOT(bridge_dev->path.pci.devfn);
	const uint32_t func = PCI_FUNC(bridge_dev->path.pci.devfn);

	if (bus == 0)
		return current;

	unsigned long atsr_size = 0;
	unsigned long pci_br_size = 0;
	if (is_atsr == true && first && *first == true) {
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
	unsigned long tmp = current;
	const STACK_RES *ri = &hob->PlatformData.IIO_resource[socket].StackRes[stack];
	const uint32_t bus = ri->BusBase;
	const uint32_t pcie_seg = hob->PlatformData.CpuQpiInfo[socket].PcieSegment;
	const uint32_t reg_base = ri->VtdBarAddress;
	printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, pcie_seg: 0x%x, reg_base: 0x%x\n",
		__func__, socket, stack, bus, pcie_seg, reg_base);

	/* Do not generate DRHD for non-PCIe stack */
	if (!reg_base)
		return current;

	// Add DRHD Hardware Unit

	if (socket == 0 && stack == IioStack0) {
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
	if (socket == 0 && stack == IioStack0) {
		union p2sb_bdf ioapic_bdf = p2sb_get_ioapic_bdf();
		printk(BIOS_DEBUG, "    [IOAPIC Device] Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
		       "PCI Path: 0x%x, 0x%x\n", get_ioapic_id(VIO_APIC_VADDR), ioapic_bdf.bus,
		       ioapic_bdf.dev, ioapic_bdf.fn);
		current += acpi_create_dmar_ds_ioapic_from_hw(current,
				IO_APIC_ADDR, ioapic_bdf.bus, ioapic_bdf.dev, ioapic_bdf.fn);
	}

/* SPR has no per stack IOAPIC or CBDMA devices */
#if CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)
	uint32_t enum_id;
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
#endif

	// Add PCIe Ports
	if (socket != 0 || stack != IioStack0) {
		struct device *dev = pcidev_path_on_bus(bus, PCI_DEVFN(0, 0));
		while (dev) {
			if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE)
				current +=
				acpi_create_dmar_ds_pci_br_for_port(
				current, dev, pcie_seg, false, NULL);

			dev = dev->sibling;
		}

#if CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)
		// Add VMD
		if (hob->PlatformData.VMDStackEnable[socket][stack] &&
			stack >= PSTACK0 && stack <= PSTACK2) {
			printk(BIOS_DEBUG, "    [PCI Endpoint Device] Enumeration ID: 0x%x, "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				 0, bus, VMD_DEV_NUM, VMD_FUNC_NUM);
			current += acpi_create_dmar_ds_pci(current,
				bus, VMD_DEV_NUM, VMD_FUNC_NUM);
		}
#endif
	}

#if CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)
	// Add DINO End Points (with memory resources. We don't report every End Point device.)
	if (ri->Personality == TYPE_DINO) {
		for (int b = ri->BusBase; b <= ri->BusLimit; ++b) {
			struct device *dev = pcidev_path_on_bus(b, PCI_DEVFN(0, 0));
			while (dev) {
				/* This may also require a check for IORESOURCE_PREFETCH,
				 * but that would not include the FPU (4942/0) */
				if ((dev->resource_list->flags &
				 (IORESOURCE_MEM | IORESOURCE_PCI64 | IORESOURCE_ASSIGNED)) ==
				 (IORESOURCE_MEM | IORESOURCE_PCI64 | IORESOURCE_ASSIGNED)) {
					const uint32_t d = PCI_SLOT(dev->path.pci.devfn);
					const uint32_t f = PCI_FUNC(dev->path.pci.devfn);
					printk(BIOS_DEBUG, "    [PCIE Endpoint Device] "
						"Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
						" PCI Path: 0x%x, 0x%x\n", 0, b, d, f);
					current += acpi_create_dmar_ds_pci(current, b, d, f);
				}
				dev = dev->sibling;
			}
		}
	}
#endif

	// Add HPET
	if (socket == 0 && stack == IioStack0) {
		uint16_t hpet_capid = read16p(HPET_BASE_ADDRESS);
		uint16_t num_hpets = (hpet_capid >> 0x08) & 0x1F;  // Bits [8:12] has hpet count
		printk(BIOS_SPEW, "%s hpet_capid: 0x%x, num_hpets: 0x%x\n",
			__func__, hpet_capid, num_hpets);
		//BIT 15
		if (num_hpets && (num_hpets != 0x1f) &&
			(read32p(HPET_BASE_ADDRESS + 0x100) & (0x00008000))) {
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

		for (int stack = 0; stack < MAX_LOGIC_IIO_STACK; ++stack) {
			uint32_t bus = iio_resource.StackRes[stack].BusBase;
			uint32_t vtd_base = iio_resource.StackRes[stack].VtdBarAddress;
			if (!vtd_base)
				continue;
			uint64_t vtd_mmio_cap = read64p(vtd_base + VTD_EXT_CAP_LOW);
			printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, vtd_base: 0x%x, "
				"vtd_mmio_cap: 0x%llx\n",
				__func__, socket, stack, bus, vtd_base, vtd_mmio_cap);

			// ATSR is applicable only for platform supporting device IOTLBs
			// through the VT-d extended capability register
			assert(vtd_mmio_cap != 0xffffffffffffffff);
			if ((vtd_mmio_cap & 0x4) == 0) // BIT 2
				continue;

			if (bus == 0)
				continue;

			struct device *dev = pcidev_path_on_bus(bus, PCI_DEVFN(0, 0));
			while (dev) {
				if ((dev->hdr_type & 0x7f) == PCI_HEADER_TYPE_BRIDGE)
					current +=
					acpi_create_dmar_ds_pci_br_for_port(
					current, dev, pcie_seg, true, &first);

				dev = dev->sibling;
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
		0, (uint32_t)ptr, (uint32_t)((uint32_t)ptr + size - 1));
	current += acpi_create_dmar_rmrr(current, 0, (uint32_t)ptr,
		(uint32_t)((uint32_t)ptr + size - 1));

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
		for (int stack = 0; stack < MAX_LOGIC_IIO_STACK; ++stack) {
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

/* Skylake-SP doesn't have DINO but not sure how to verify this on CPX */
#if CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)
static unsigned long xeonsp_create_satc_dino(unsigned long current, const STACK_RES *ri)
{
	for (int b = ri->BusBase; b <= ri->BusLimit; ++b) {
		struct device *dev = pcidev_path_on_bus(b, PCI_DEVFN(0, 0));
		while (dev) {
			if (pciexp_find_extended_cap(dev, PCIE_EXT_CAP_ID_ATS, 0)) {
				const uint32_t d = PCI_SLOT(dev->path.pci.devfn);
				const uint32_t f = PCI_FUNC(dev->path.pci.devfn);
				printk(BIOS_DEBUG, "    [SATC Endpoint Device] "
					"Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
					" PCI Path: 0x%x, 0x%x\n", 0, b, d, f);
					current += acpi_create_dmar_ds_pci(current, b, d, f);
			}
			dev = dev->sibling;
		}
	}
	return current;
}

/* SoC Integrated Address Translation Cache */
static unsigned long acpi_create_satc(unsigned long current, const IIO_UDS *hob)
{

	const unsigned long tmp = current;

	// Add the SATC header
	current += acpi_create_dmar_satc(current, 0, 0);

	// Find the DINO devices on each socket
	for (int socket = (hob->PlatformData.numofIIO - 1); socket >= 0; --socket) {
		for (int stack = (MAX_LOGIC_IIO_STACK - 1); stack >= 0; --stack) {
			const STACK_RES *ri = &hob->PlatformData.IIO_resource[socket].StackRes[stack];
			// Add the DINO ATS devices to the SATC
			if (ri->Personality == TYPE_DINO)
				current = xeonsp_create_satc_dino(current, ri);
		}
	}

	acpi_dmar_satc_fixup(tmp, current);
	return current;
}
#endif

static unsigned long acpi_fill_dmar(unsigned long current)
{
	const IIO_UDS *hob = get_iio_uds();

	// DRHD - socket 0 stack 0 must be the last DRHD entry.
	for (int socket = (hob->PlatformData.numofIIO - 1); socket >= 0; --socket) {
		for (int stack = (MAX_LOGIC_IIO_STACK - 1); stack >= 0; --stack)
			current = acpi_create_drhd(current, socket, stack, hob);
	}

	// RMRR
	current = acpi_create_rmrr(current);

	// Root Port ATS Capability
	current = acpi_create_atsr(current, hob);

	// RHSA
	current = acpi_create_rhsa(current);

#if CONFIG(SOC_INTEL_SAPPHIRERAPIDS_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)
	// SATC
	current = acpi_create_satc(current, hob);
#endif

	return current;
}

unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long current,
					    struct acpi_rsdp *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;
	acpi_dmar_t *dmar;
	acpi_hmat_t *hmat;
	acpi_cedt_t *cedt;

	const config_t *const config = config_of(device);

	/* SRAT */
	current = ALIGN_UP(current, 8);
	printk(BIOS_DEBUG, "ACPI:    * SRAT at %lx\n", current);
	srat = (acpi_srat_t *)current;
	acpi_create_srat(srat, acpi_fill_srat);
	current += srat->header.length;
	acpi_add_table(rsdp, srat);

	/* SLIT */
	current = ALIGN_UP(current, 8);
	printk(BIOS_DEBUG, "ACPI:   * SLIT at %lx\n", current);
	slit = (acpi_slit_t *)current;
	acpi_create_slit(slit, acpi_fill_slit);
	current += slit->header.length;
	acpi_add_table(rsdp, slit);

	if (CONFIG(SOC_INTEL_HAS_CXL)) {
		/* HMAT*/
		current = ALIGN_UP(current, 8);
		printk(BIOS_DEBUG, "ACPI:    * HMAT at %lx\n", current);
		hmat = (acpi_hmat_t *)current;
		acpi_create_hmat(hmat, acpi_fill_hmat);
		current += hmat->header.length;
		acpi_add_table(rsdp, hmat);
	}

	/* DMAR */
	if (config->vtd_support) {
		current = ALIGN_UP(current, 8);
		dmar = (acpi_dmar_t *)current;
		enum dmar_flags flags = DMAR_INTR_REMAP;

		/* SKX FSP doesn't support X2APIC, but CPX FSP does */
		if (CONFIG(SOC_INTEL_SKYLAKE_SP))
			flags |= DMAR_X2APIC_OPT_OUT;

		printk(BIOS_DEBUG, "ACPI:    * DMAR at %lx\n", current);
		printk(BIOS_DEBUG, "[DMA Remapping table] Flags: 0x%x\n", flags);
		acpi_create_dmar(dmar, flags, acpi_fill_dmar);
		current += dmar->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, dmar);
	}

	if (CONFIG(SOC_INTEL_HAS_CXL)) {
		/* CEDT: CXL Early Discovery Table */
		if (get_cxl_node_count() > 0) {
			current = ALIGN_UP(current, 8);
			printk(BIOS_DEBUG, "ACPI:    * CEDT at %lx\n", current);
			cedt = (acpi_cedt_t *)current;
			acpi_create_cedt(cedt, acpi_fill_cedt);
			current += cedt->header.length;
			acpi_add_table(rsdp, cedt);
		}
	}

	if (CONFIG(SOC_ACPI_HEST)) {
		printk(BIOS_DEBUG, "ACPI:    * HEST at %lx\n", current);
		current = hest_create(current, rsdp);
	}

	return current;
}
