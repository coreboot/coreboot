/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpigen.h>
#include <arch/hpet.h>
#include <arch/ioapic.h>
#include <assert.h>
#include <cpu/x86/lapic.h>
#include <commonlib/sort.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pciexp.h>
#include <device/pci_ids.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/hest.h>
#include <soc/iomap.h>
#include <soc/numa.h>
#include <soc/pci_devs.h>
#include <soc/soc_util.h>
#include <soc/util.h>
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
			       i, device_to_pd(cpu), cpu->path.apic.apic_id);

			current += acpi_create_srat_x2apic((acpi_srat_x2apic_t *)current,
				device_to_pd(cpu), cpu->path.apic.apic_id);
		} else {
			printk(BIOS_DEBUG, "SRAT: lapic cpu_index=%02x, node_id=%02x, apic_id=%02x\n",
			       i, device_to_pd(cpu), cpu->path.apic.apic_id);

			current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current,
				device_to_pd(cpu), cpu->path.apic.apic_id);
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
			is_memtype_reserved(mem_element->Type));

		assert(mmap_index < MAX_ACPI_MEMORY_AFFINITY_COUNT);

		/* skip reserved memory region */
		if (is_memtype_reserved(mem_element->Type))
			continue;
		/* skip all non processor attached memory regions */
		if (CONFIG(SOC_INTEL_HAS_CXL) &&
			(!is_memtype_processor_attached(mem_element->Type)))
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
		srat_mem[mmap_index].base_address_low = (uint32_t)(addr & 0xffffffff);
		srat_mem[mmap_index].base_address_high = (uint32_t)(addr >> 32);
		srat_mem[mmap_index].length_low = (uint32_t)(size & 0xffffffff);
		srat_mem[mmap_index].length_high = (uint32_t)(size >> 32);
		srat_mem[mmap_index].proximity_domain = memory_to_pd(mem_element);
		srat_mem[mmap_index].flags = ACPI_SRAT_MEMORY_ENABLED;
		if (is_memtype_non_volatile(mem_element->Type))
			srat_mem[mmap_index].flags |= ACPI_SRAT_MEMORY_NONVOLATILE;
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

	memset(srat_mem, 0, sizeof(srat_mem));
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
	const uint32_t bus = bridge_dev->upstream->secondary;
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

	printk(BIOS_DEBUG, "    [PCI Bridge Device] %s\n", dev_path(bridge_dev));
	pci_br_size = acpi_create_dmar_ds_pci_br(current + atsr_size, bus, dev, func);

	return (atsr_size + pci_br_size);
}

static unsigned long acpi_create_drhd(unsigned long current, struct device *iommu,
	const IIO_UDS *hob)
{
	unsigned long tmp = current;

	struct resource *resource;
	resource = probe_resource(iommu, VTD_BAR_CSR);
	if (!resource)
		return current;

	uint32_t reg_base = resource->base;
	if (!reg_base)
		return current;

	const uint32_t bus = iommu->upstream->secondary;
	uint32_t pcie_seg = iommu->upstream->segment_group;
	int socket = iio_pci_domain_socket_from_dev(iommu);
	int stack = iio_pci_domain_stack_from_dev(iommu);

	printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, pcie_seg: 0x%x, reg_base: 0x%x\n",
		__func__, socket, stack, bus, pcie_seg, reg_base);

	// Add DRHD Hardware Unit

	if (is_dev_on_domain0(iommu)) {
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
	if (is_dev_on_domain0(iommu)) {
		union p2sb_bdf ioapic_bdf = soc_get_ioapic_bdf();
		printk(BIOS_DEBUG, "    [IOAPIC Device] Enumeration ID: 0x%x, PCI Bus Number: 0x%x, "
		       "PCI Path: 0x%x, 0x%x\n", get_ioapic_id(IO_APIC_ADDR), ioapic_bdf.bus,
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
			printk(BIOS_DEBUG, "    [PCI Endpoint Device] "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				bus, CBDMA_DEV_NUM, cbdma_func_id);
			current += acpi_create_dmar_ds_pci(current,
				bus, CBDMA_DEV_NUM, cbdma_func_id);
		}
	}
#endif

	// Add PCIe Ports
	if (!is_dev_on_domain0(iommu)) {
		const struct device *domain = dev_get_domain(iommu);
		struct device *dev = NULL;
		while ((dev = dev_bus_each_child(domain->downstream, dev)))
			if (is_pci_bridge(dev))
				current +=
				acpi_create_dmar_ds_pci_br_for_port(
				current, dev, pcie_seg, false, NULL);

#if CONFIG(SOC_INTEL_SKYLAKE_SP) || CONFIG(SOC_INTEL_COOPERLAKE_SP)
		// Add VMD
		if (hob->PlatformData.VMDStackEnable[socket][stack] &&
			stack >= PSTACK0 && stack <= PSTACK2) {
			printk(BIOS_DEBUG, "    [PCI Endpoint Device] "
				"PCI Bus Number: 0x%x, PCI Path: 0x%x, 0x%x\n",
				 bus, VMD_DEV_NUM, VMD_FUNC_NUM);
			current += acpi_create_dmar_ds_pci(current,
				bus, VMD_DEV_NUM, VMD_FUNC_NUM);
		}
#endif
	}

	// Add IOAT End Points (with memory resources. We don't report every End Point device.)
	if (CONFIG(HAVE_IOAT_DOMAINS) && is_dev_on_ioat_domain(iommu)) {
		struct device *dev = NULL;
		while ((dev = dev_find_all_devices_on_stack(socket, stack,
			XEONSP_VENDOR_MAX, XEONSP_DEVICE_MAX, dev)))
			/* This may also require a check for IORESOURCE_PREFETCH,
			 * but that would not include the FPU (4942/0) */
			if ((dev->resource_list->flags &
				(IORESOURCE_MEM | IORESOURCE_PCI64 | IORESOURCE_ASSIGNED)) ==
				(IORESOURCE_MEM | IORESOURCE_PCI64 | IORESOURCE_ASSIGNED)) {
				const uint32_t b = dev->upstream->secondary;
				const uint32_t d = PCI_SLOT(dev->path.pci.devfn);
				const uint32_t f = PCI_FUNC(dev->path.pci.devfn);
				printk(BIOS_DEBUG, "    [PCIE Endpoint Device] %s\n", dev_path(dev));
				current += acpi_create_dmar_ds_pci(current, b, d, f);
			}
	}

	// Add HPET
	if (is_dev_on_domain0(iommu)) {
		uint16_t hpet_capid = read16p(HPET_BASE_ADDRESS);
		uint16_t num_hpets = (hpet_capid >> 0x08) & 0x1F;  // Bits [8:12] has hpet count
		printk(BIOS_SPEW, "%s hpet_capid: 0x%x, num_hpets: 0x%x\n",
			__func__, hpet_capid, num_hpets);
		//BIT 15
		if (num_hpets && (num_hpets != 0x1f) &&
			(read32p(HPET_BASE_ADDRESS + 0x100) & (0x00008000))) {
			union p2sb_bdf hpet_bdf = soc_get_hpet_bdf();
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

static unsigned long acpi_create_atsr(unsigned long current)
{
	struct device *child, *dev;
	struct resource *resource;

	/*
	 * The assumption made here is that the host bridges on a socket share the
	 * PCI segment group and thus only one ATSR header needs to be emitted for
	 * a single socket.
	 * This is easier than to sort the host bridges by PCI segment group first
	 * and then generate one ATSR header for every new segment.
	 */
	for (int socket = 0; socket < CONFIG_MAX_SOCKET; ++socket) {
		if (!soc_cpu_is_enabled(socket))
			continue;
		unsigned long tmp = current;
		bool first = true;

		dev = NULL;
		while ((dev = dev_find_device(PCI_VID_INTEL, MMAP_VTD_CFG_REG_DEVID, dev))) {
			/* Only add devices for the current socket */
			if (iio_pci_domain_socket_from_dev(dev) != socket)
				continue;
			/* See if there is a resource with the appropriate index. */
			resource = probe_resource(dev, VTD_BAR_CSR);
			if (!resource)
				continue;
			int stack = iio_pci_domain_stack_from_dev(dev);

			uint64_t vtd_mmio_cap = read64(res2mmio(resource, VTD_EXT_CAP_LOW, 0));
			printk(BIOS_SPEW, "%s socket: %d, stack: %d, bus: 0x%x, vtd_base: %p, "
				"vtd_mmio_cap: 0x%llx\n",
				__func__, socket, stack, dev->upstream->secondary,
				res2mmio(resource, 0, 0), vtd_mmio_cap);

			// ATSR is applicable only for platform supporting device IOTLBs
			// through the VT-d extended capability register
			assert(vtd_mmio_cap != 0xffffffffffffffff);
			if ((vtd_mmio_cap & 0x4) == 0) // BIT 2
				continue;

			if (dev->upstream->secondary == 0 && dev->upstream->segment_group == 0)
				continue;

			for (child = dev->upstream->children; child; child = child->sibling) {
				if (!is_pci_bridge(child))
					continue;
				current +=
					acpi_create_dmar_ds_pci_br_for_port(
					current, child, child->upstream->segment_group, true, &first);
			}
		}
		if (tmp != current)
			acpi_dmar_atsr_fixup(tmp, current);
	}

	return current;
}

static unsigned long acpi_create_rmrr(unsigned long current)
{
	return current;
}

static unsigned long acpi_create_rhsa(unsigned long current)
{
	struct device *dev = NULL;
	struct resource *resource;

	while ((dev = dev_find_device(PCI_VID_INTEL, MMAP_VTD_CFG_REG_DEVID, dev))) {
		/* See if there is a resource with the appropriate index. */
		resource = probe_resource(dev, VTD_BAR_CSR);
		if (!resource)
			continue;

		printk(BIOS_DEBUG, "[Remapping Hardware Static Affinity] Base Address: %p, "
			"Proximity Domain: 0x%x\n", res2mmio(resource, 0, 0), device_to_pd(dev));
		current += acpi_create_dmar_rhsa(current, (uintptr_t)res2mmio(resource, 0, 0), device_to_pd(dev));
	}

	return current;
}

static unsigned long xeonsp_create_satc(unsigned long current, struct device *domain)
{
	struct device *dev = NULL;
	while ((dev = dev_bus_each_child(domain->downstream, dev))) {
		if (pciexp_find_extended_cap(dev, PCIE_EXT_CAP_ID_ATS, 0)) {
			const uint32_t b = domain->downstream->secondary;
			const uint32_t d = PCI_SLOT(dev->path.pci.devfn);
			const uint32_t f = PCI_FUNC(dev->path.pci.devfn);
			printk(BIOS_DEBUG, "    [SATC Endpoint Device] %s\n", dev_path(dev));
			current += acpi_create_dmar_ds_pci(current, b, d, f);
		}
	}
	return current;
}

/* SoC Integrated Address Translation Cache */
static unsigned long acpi_create_satc(unsigned long current)
{
	unsigned long tmp = current, seg = ~0;
	struct device *dev;

	/*
	 * Best case only PCI segment group count SATC headers are emitted, worst
	 * case for every SATC entry a new SATC header is being generated.
	 *
	 * The assumption made here is that the host bridges on a socket share the
	 * PCI segment group and thus only one SATC header needs to be emitted for
	 * a single socket.
	 * This is easier than to sort the host bridges by PCI segment group first
	 * and then generate one SATC header for every new segment.
	 *
	 * With this assumption the best case scenario should always be used.
	 */
	for (int socket = 0; socket < CONFIG_MAX_SOCKET; ++socket) {
		if (!soc_cpu_is_enabled(socket))
			continue;

		dev = NULL;
		while ((dev = dev_find_path(dev, DEVICE_PATH_DOMAIN))) {
			/* Only add devices for the current socket */
			if (iio_pci_domain_socket_from_dev(dev) != socket)
				continue;

			if (seg != dev->downstream->segment_group) {
				// Close previous header
				if (tmp != current)
					acpi_dmar_satc_fixup(tmp, current);

				seg = dev->downstream->segment_group;
				tmp = current;
				printk(BIOS_DEBUG, "[SATC Segment Header] "
				       "Flags: 0x%x, PCI segment group: %lx\n", 0, seg);
				// Add the SATC header
				current += acpi_create_dmar_satc(current, 0, seg);
			}
			current = xeonsp_create_satc(current, dev);
		}
	}
	if (tmp != current)
		acpi_dmar_satc_fixup(tmp, current);

	return current;
}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	const IIO_UDS *hob = get_iio_uds();

	// DRHD - iommu0 must be the last DRHD entry.
	struct device *dev = NULL;
	struct device *iommu0 = NULL;
	while ((dev = dev_find_device(PCI_VID_INTEL, MMAP_VTD_CFG_REG_DEVID, dev))) {
		if (is_domain0(dev_get_domain(dev))) {
			iommu0 = dev;
			continue;
		}
		current = acpi_create_drhd(current, dev, hob);
	}
	assert(iommu0);
	current = acpi_create_drhd(current, iommu0, hob);

	// RMRR
	current = acpi_create_rmrr(current);

	// Root Port ATS Capability
	current = acpi_create_atsr(current);

	// RHSA
	current = acpi_create_rhsa(current);

	// SATC
	current = acpi_create_satc(current);

	return current;
}

unsigned long northbridge_write_acpi_tables(const struct device *device, unsigned long current,
					    struct acpi_rsdp *rsdp)
{
	/* Only write uncore ACPI tables for domain0 */
	if (device->path.domain.domain != 0)
		return current;

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
