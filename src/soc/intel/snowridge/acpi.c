/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_device.h>
#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <arch/hpet.h>
#include <arch/ioapic.h>
#include <assert.h>
#include <cbmem.h>
#include <commonlib/bsd/helpers.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <intelblocks/acpi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/p2sb.h>
#include <intelblocks/systemagent_server.h>
#include <soc/acpi.h>
#include <soc/iomap.h>
#include <soc/irq.h>
#include <soc/itss.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/systemagent.h>
#include <southbridge/intel/common/acpi_pirq_gen.h>
#include <static.h>
#include <string.h>

uint32_t soc_read_sci_irq_select(void)
{
	return read32p(PCH_PWRM_BASE_ADDRESS + ACTL);
}

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;

	return MP_IRQ_POLARITY_HIGH;
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	/**
	 * The default field value is 0 if it's not set when calling this function.
	 */

	fadt->pm2_cnt_blk = ACPI_BASE_ADDRESS + PM2_CNT;
	fadt->pm_tmr_blk = ACPI_BASE_ADDRESS + PM1_TMR;
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;

	fadt->duty_offset = 1;
	fadt->duty_width = 0;

	fill_fadt_extended_pm_io(fadt);
}

void soc_power_states_generation(int core_id, int cores_per_package)
{
	generate_p_state_entries(core_id, cores_per_package);
}

static void acpigen_write_pci_prt(const struct device *dev)
{
	unsigned int map_count = 0;
	struct slot_pin_irq_map *pin_irq_map =
		calloc(MAX_SLOTS * PCI_INT_MAX, sizeof(struct slot_pin_irq_map));
	if (!pin_irq_map)
		return;

	struct device *child = NULL;
	while ((child = dev_bus_each_child(dev->downstream, child))) {
		if (!is_enabled_pci(child))
			continue;

		enum pci_pin pin = pci_read_config8(child, PCI_INTERRUPT_PIN);
		if (pin < PCI_INT_A || pin > PCI_INT_D)
			continue;

		pin_irq_map[map_count].slot = PCI_SLOT(child->path.pci.devfn);
		pin_irq_map[map_count].pin = pin;

		/* `pic_pirq` is actually `enum pirq` type. */
		pin_irq_map[map_count].pic_pirq = itss_soc_get_dev_pirq(child);
		if (pin_irq_map[map_count].pic_pirq == PIRQ_INVALID)
			continue;

		/* PIRQA-H is hardwired to IRQ16-23. */
		pin_irq_map[map_count].apic_gsi =
			PCH_IRQ16 + pirq_idx(pin_irq_map[map_count].pic_pirq);

		printk(BIOS_SPEW, "%s, slot_pin_irq_map[%2d]: {0x%2x, %s, PIRQ%c, IRQ%d}\n",
		       dev_path(child), map_count, pin_irq_map[map_count].slot,
		       pin_to_str(pin_irq_map[map_count].pin),
		       (unsigned int)pirq_idx(pin_irq_map[map_count].pic_pirq) + 'A',
		       pin_irq_map[map_count].apic_gsi);

		map_count++;
	}

	size_t pirq_routes;
	const uint8_t *legacy_pirq_routing = lpc_get_pic_pirq_routing(&pirq_routes);
	struct pic_pirq_map pirq_map = {.type = PIRQ_GSI};
	for (size_t i = 0; i < PIRQ_COUNT && i < pirq_routes; i++)
		pirq_map.gsi[i] = legacy_pirq_routing[i];

	intel_write_pci_PRT(acpi_device_path(dev), pin_irq_map, map_count, &pirq_map);
	free(pin_irq_map);
}

void domain_fill_ssdt(const struct device *dev)
{
	const char *acpi_scope = acpi_device_scope(dev);
	const char *acpi_name = acpi_device_name(dev);
	printk(BIOS_DEBUG, "%s ACPI scope: '%s', name: '%s'\n", __func__, acpi_scope,
	       acpi_name);

	/**
	 * PCH domain is defined in uncore.asl.
	 *
	 * Generating accelerator domains dynamically since they are SKU-dependent.
	 */
	if (!is_domain0(dev)) {
		/**
		 * Device (PCIx)
		 * {
		 *     Method (_STA) { Return (status) }
		 *     Name (_HID, EISAID ("PNP0A08")) // PCI Express Bus
		 *     Name (_CID, EISAID ("PNP0A03")) // PCI Bus
		 *     Name (_UID, "PCIx")
		 *     Name (_PXM, 0)
		 *     Method (_OSC, 4) { Return (\_SB.DOSC (Arg0, Arg1, Arg2, Arg3)) }
		 * }
		 */
		acpigen_write_scope(acpi_scope);
		acpigen_write_device(acpi_name);

		acpigen_write_STA(dev->enabled ? ACPI_STATUS_DEVICE_ALL_ON :
						 ACPI_STATUS_DEVICE_ALL_OFF);

		acpigen_write_name("_HID");
		acpigen_emit_eisaid("PNP0A08");

		acpigen_write_name("_CID");
		acpigen_emit_eisaid("PNP0A03");

		acpigen_write_name("_UID");
		acpigen_write_string(acpi_name);

		acpigen_write_name("_PXM");
		acpigen_write_integer(0);

		acpigen_write_method("_OSC", 4);
		acpigen_write_return_namestr("\\_SB.DOSC");
		acpigen_emit_byte(ARG0_OP);
		acpigen_emit_byte(ARG1_OP);
		acpigen_emit_byte(ARG2_OP);
		acpigen_emit_byte(ARG3_OP);
		acpigen_write_method_end();

		acpigen_write_device_end();
		acpigen_write_scope_end();
	}

	pci_domain_fill_ssdt(dev);

	acpigen_write_pci_prt(dev);
}

void pcie_rp_fill_ssdt(const struct device *dev)
{
	const char *acpi_scope = acpi_device_scope(dev);
	const char *acpi_name = acpi_device_name(dev);
	printk(BIOS_DEBUG, "%s ACPI scope: '%s', name: '%s'\n", __func__, acpi_scope,
	       acpi_name);

	acpigen_write_scope(acpi_scope);
	acpigen_write_device(acpi_name);
	acpigen_write_STA(dev->enabled ? ACPI_STATUS_DEVICE_ALL_ON :
					 ACPI_STATUS_DEVICE_ALL_OFF);
	acpigen_write_ADR_pci_device(dev);
	acpigen_write_device_end();
	acpigen_write_scope_end();

	acpigen_write_pci_prt(dev);
}

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	for (struct device *cpu = DEV_PTR(cpu_bus)->downstream->children; cpu != NULL;
	     cpu = cpu->next) {
		if (!is_enabled_cpu(cpu))
			continue;

		printk(BIOS_DEBUG,
		       "SRAT: APIC ID = 0x%02x, package ID = 0x%02x, node ID = 0x%02x, core ID = 0x%02x, thread ID = 0x%02x\n",
		       cpu->path.apic.apic_id, cpu->path.apic.package_id,
		       cpu->path.apic.node_id, cpu->path.apic.core_id,
		       cpu->path.apic.thread_id);
		current += acpi_create_srat_lapic((acpi_srat_lapic_t *)current,
						  cpu->path.apic.node_id,
						  cpu->path.apic.apic_id);
	}

	return current;
}

static unsigned long acpi_fill_srat(unsigned long current)
{
	const uint32_t low_mem_end = sa_server_get_tolud();
	const uint64_t hi_mem_end = sa_server_get_touud(), mem_4G = 4ull * GiB;

	current = acpi_create_srat_lapics(current);

	current += acpi_create_srat_mem((acpi_srat_mem_t *)current, 0, 0, low_mem_end >> 10,
					ACPI_SRAT_MEMORY_ENABLED);

	if (hi_mem_end > mem_4G)
		current += acpi_create_srat_mem((acpi_srat_mem_t *)current, 0, mem_4G >> 10,
						(hi_mem_end - mem_4G) >> 10,
						ACPI_SRAT_MEMORY_ENABLED);

	return current;
}

static unsigned long acpi_create_drhd(unsigned long current, struct device *dev)
{
	unsigned long tmp = current;
	const struct device *dev_domain = dev_get_domain(dev);
	if (!dev_domain->enabled)
		return current;

	/**
	 * Bit 0 of VTBAR is read-write and it indicates whether VT-d base address is enabled.
	 */
	uint32_t vtd_base = pci_read_config32(dev, VTBAR);
	if (!(vtd_base & VTD_CHIPSET_BASE_ADDRESS_ENABLE))
		return current;

	vtd_base = ALIGN_DOWN(vtd_base, 4 * KiB);

	const uint32_t pcie_seg = dev->upstream->segment_group;
	if (is_dev_on_domain0(dev)) {
		printk(BIOS_DEBUG,
		       "[DMA Remapping Hardware Unit Definition] Flags: 0x%x, PCI Segment: 0x%x, Register Base Address: 0x%x\n",
		       DRHD_INCLUDE_PCI_ALL, pcie_seg, vtd_base);
		current += acpi_create_dmar_drhd_4k(current, DRHD_INCLUDE_PCI_ALL, pcie_seg,
						    vtd_base);

		union p2sb_bdf ioapic_bdf = p2sb_get_ioapic_bdf();
		printk(BIOS_DEBUG, "[IOAPIC] PCI Bus: 0x%x, PCI Path: 0x%x, 0x%x\n",
		       ioapic_bdf.bus, ioapic_bdf.dev, ioapic_bdf.fn);
		current += acpi_create_dmar_ds_ioapic_from_hw(
			current, IO_APIC_ADDR, ioapic_bdf.bus, ioapic_bdf.dev, ioapic_bdf.fn);

		uint16_t num_hpets = (read32p(HPET_BASE_ADDRESS) >> HPET_NUM_TIM_CAP_SHIFT) &
				     HPET_NUM_TIM_CAP_MASK;
		if (num_hpets && num_hpets != HPET_NUM_TIM_CAP_MASK) {
			if (read32p(HPET_BASE_ADDRESS + HPET_TMR0_CNF_CAP) &
			    HPET_TIMER_FSB_EN_CNF_MASK) {
				union p2sb_bdf hpet_bdf = p2sb_get_hpet_bdf();
				printk(BIOS_DEBUG,
				       "[MSI_CAPABLE_HPET] Enumeration ID: 0x%x, PCI Bus: 0x%x, PCI Path: 0x%x, 0x%x\n",
				       0, hpet_bdf.bus, hpet_bdf.dev, hpet_bdf.fn);
				current += acpi_create_dmar_ds_msi_hpet(
					current, 0, hpet_bdf.bus, hpet_bdf.dev, hpet_bdf.fn);
			}
		}
	} else {
		printk(BIOS_DEBUG,
		       "[DMA Remapping Hardware Unit Definition] Flags: 0x%x, PCI Segment: 0x%x, Register Base Address: 0x%x\n",
		       0, pcie_seg, vtd_base);
		current += acpi_create_dmar_drhd_4k(current, 0, pcie_seg, vtd_base);

		if (dev == DEV_PTR(dlb_sa)) {
			if (DEV_PTR(dlb)) {
				printk(BIOS_DEBUG, "[PCI Endpoint Device] %s\n",
				       dev_path(DEV_PTR(dlb)));
				current += acpi_create_dmar_ds_pci(
					current, PCI_DEV2BUS(PCI_BDF(DEV_PTR(dlb))),
					PCI_SLOT(DEV_PTR(dlb)->path.pci.devfn),
					PCI_FUNC(DEV_PTR(dlb)->path.pci.devfn));
			}
		} else {
			struct device *pci_bridge = NULL;
			while ((pci_bridge = dev_bus_each_child(dev_get_domain(dev)->downstream,
								pci_bridge))) {
				if (pci_bridge->vendor != PCI_VID_INTEL)
					continue;

				switch (pci_bridge->device) {
				case PCI_DID_INTEL_SNR_CPU_PCIE_RPA:
				case PCI_DID_INTEL_SNR_CPU_PCIE_RPB:
				case PCI_DID_INTEL_SNR_CPU_PCIE_RPC:
				case PCI_DID_INTEL_SNR_CPU_PCIE_RPD:
				case PCI_DID_INTEL_SNR_VRP4_NIS:
				case PCI_DID_INTEL_SNR_VRP5_QAT_1_8:
					printk(BIOS_DEBUG, "[PCI Sub-hierarchy] %s\n",
					       dev_path(pci_bridge));
					current += acpi_create_dmar_ds_pci_br(
						current, dev->upstream->secondary,
						PCI_SLOT(pci_bridge->path.pci.devfn),
						PCI_FUNC(pci_bridge->path.pci.devfn));
					break;
				default:
					continue;
				}
			}
		}
	}

	if (current != tmp)
		acpi_dmar_drhd_fixup(tmp, current);

	return current;
}

static unsigned long acpi_create_rmrr(unsigned long current)
{
	const uint32_t MEM_BLK_COUNT = 0x140, MEM_BLK_SIZE = 32;
	uint32_t size = ALIGN_UP(MEM_BLK_COUNT * MEM_BLK_SIZE, 0x1000);
	const struct cbmem_entry *entry;
	void *ptr;
	unsigned long tmp = current;
	struct device *dev = PCH_DEV_XHCI;

	entry = cbmem_entry_find(CBMEM_ID_STORAGE_DATA);
	if (!entry) {
		ptr = cbmem_add(CBMEM_ID_STORAGE_DATA, size);
		if (!ptr) {
			printk(BIOS_ERR, "Failed to allocate reserved memory in cbmem!\n");
			return current;
		}

		memset(ptr, 0, size);
	} else {
		ptr = cbmem_entry_start(entry);
		size = cbmem_entry_size(entry);
	}

	printk(BIOS_DEBUG,
	       "[Reserved Memory Region Reporting] PCI Segment: 0x%x, Base: %p, Limit: %p\n",
	       dev->upstream->segment_group, ptr, ptr + size - 1);
	current += acpi_create_dmar_rmrr(current, dev->upstream->segment_group, (uintptr_t)ptr,
					 (uintptr_t)(ptr + size - 1));

	printk(BIOS_DEBUG, "[PCI Endpoint Device] %s\n", dev_path(dev));
	current += acpi_create_dmar_ds_pci(current, PCI_DEV2BUS(PCI_BDF(dev)),
					   PCI_SLOT(dev->path.pci.devfn),
					   PCI_FUNC(dev->path.pci.devfn));

	if (current != tmp)
		acpi_dmar_rmrr_fixup(tmp, current);

	return current;
}

static unsigned long acpi_create_atsr(unsigned long current, struct device *dev)
{
	unsigned long tmp = current;

	/**
	 * Bit 0 of VTBAR is read-write and it indicates whether VT-d base address is enabled.
	 */
	uint32_t vtd_base = pci_read_config32(dev, VTBAR);
	if (!(vtd_base & VTD_CHIPSET_BASE_ADDRESS_ENABLE))
		return current;

	vtd_base = ALIGN_DOWN(vtd_base, 4 * KiB);

	uint64_t vtd_ext_cap = read64p(vtd_base + VTD_ECAP);
	if (!(vtd_ext_cap & DEVICE_TLB))
		return current;

	printk(BIOS_DEBUG, "Domain 1 VT-d BAR: 0x%x, Extended Capability: 0x%llx\n", vtd_base,
	       vtd_ext_cap);

	bool first = true;
	struct device *cpu_pcie_rp = NULL;
	while ((cpu_pcie_rp =
			dev_bus_each_child(dev_get_domain(dev)->downstream, cpu_pcie_rp))) {
		if (cpu_pcie_rp->vendor != PCI_VID_INTEL)
			continue;

		switch (cpu_pcie_rp->device) {
		case PCI_DID_INTEL_SNR_CPU_PCIE_RPA:
		case PCI_DID_INTEL_SNR_CPU_PCIE_RPB:
		case PCI_DID_INTEL_SNR_CPU_PCIE_RPC:
		case PCI_DID_INTEL_SNR_CPU_PCIE_RPD:
			break;
		default:
			continue;
		}

		if (first) {
			const uint32_t pcie_seg = dev->upstream->segment_group;
			printk(BIOS_DEBUG,
			       "[Root Port ATS Capability] Flags: 0x%x, PCI Segment: 0x%x\n", 0,
			       pcie_seg);
			current += acpi_create_dmar_atsr(current, 0, pcie_seg);
			first = false;
		}

		printk(BIOS_DEBUG, "[PCI Sub-hierarchy] %s\n", dev_path(cpu_pcie_rp));
		current += acpi_create_dmar_ds_pci_br(current,
						      PCI_DEV2BUS(PCI_BDF(cpu_pcie_rp)),
						      PCI_SLOT(cpu_pcie_rp->path.pci.devfn),
						      PCI_FUNC(cpu_pcie_rp->path.pci.devfn));
	}

	if (tmp != current)
		acpi_dmar_atsr_fixup(tmp, current);

	return current;
}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	assert(DEV_PTR(pch_sa));

	/**
	 * Domain 0 hosts all PCH peripherals, and DRHD entry for this domain should be at last,
	 * thus we search from `DEV_PTR(pch_sa)` here.
	 */
	struct device *dev = DEV_PTR(pch_sa);
	while ((dev = dev_find_device(PCI_VID_INTEL, PCI_DID_INTEL_SNR_ID, dev)) != NULL)
		current = acpi_create_drhd(current, dev);

	current = acpi_create_drhd(current, DEV_PTR(pch_sa));

	current = acpi_create_rmrr(current);

	/* Only CPU PCIe root ports support address translation services (ATS). */
	assert(DEV_PTR(cpu_sa));
	current = acpi_create_atsr(current, DEV_PTR(cpu_sa));

	return current;
}

unsigned long sa_write_acpi_tables(const struct device *dev, unsigned long current,
				   struct acpi_rsdp *rsdp)
{
	acpi_srat_t *srat;
	acpi_dmar_t *dmar;

	/**
	 * Write only when calling from system agent in domain 0.
	 */
	if (!sa_server_is_on_pch_domain(dev))
		return current;

	/* SRAT */
	printk(BIOS_DEBUG, "ACPI:    * SRAT at 0x%08lx\n", current);
	srat = (acpi_srat_t *)current;
	acpi_create_srat(srat, acpi_fill_srat);
	acpi_add_table(rsdp, srat);
	current += srat->header.length;
	current = acpi_align_current(current);

	printk(BIOS_DEBUG, "ACPI:    * DMAR at 0x%08lx\n", current);
	dmar = (acpi_dmar_t *)current;
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
	acpi_add_table(rsdp, dmar);
	current += dmar->header.length;

	return current;
}

/**
 * To use ACPI in common block, this function should be defined.
 */
const acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = 0;
	return NULL;
}
