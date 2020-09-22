/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <assert.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/msr.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/soc_util.h>

#include "chip.h"

#define SCI_INT_NUM		9

unsigned long southbridge_write_acpi_tables(const struct device *device,
	unsigned long current, struct acpi_rsdp *rsdp)
{
	current = acpi_write_hpet(device, current, rsdp);
	current = (ALIGN(current, 16));
	printk(BIOS_DEBUG, "current = %lx\n", current);
	return current;
}

unsigned long acpi_fill_mcfg(unsigned long current)
{
	current += acpi_create_mcfg_mmconfig((acpi_mcfg_mmconfig_t *)current,
		CONFIG_MMCONF_BASE_ADDRESS, 0, 0, 255);
	return current;
}

static void uncore_inject_dsdt(void)
{
	struct iiostack_resource stack_info = {0};

	get_iiostack_info(&stack_info);

	acpigen_write_scope("\\_SB");

	for (uint8_t stack = 0; stack < stack_info.no_of_stacks; ++stack) {
		const STACK_RES *ri = &stack_info.res[stack];
		char rtname[16];

		snprintf(rtname, sizeof(rtname), "RT%02x", stack);

		acpigen_write_name(rtname);
		printk(BIOS_DEBUG, "\tCreating ResourceTemplate %s for stack: %d\n",
			rtname, stack);

		acpigen_write_resourcetemplate_header();

		/* bus resource */
		acpigen_resource_word(2, 0xc, 0, 0, ri->BusBase, ri->BusLimit,
			0x0, (ri->BusLimit - ri->BusBase + 1));

		/* additional io resources on socket 0 bus 0 */
		if (stack == 0) {
			/* ACPI 6.4.2.5 I/O Port Descriptor */
			acpigen_write_io16(0xCF8, 0xCFF, 0x1, 0x8, 1);

			/* IO decode  CF8-CFF */
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x0000, 0x03AF, 0, 0x03B0);
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x03E0, 0x0CF7, 0, 0x0918);
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x03B0, 0x03BB, 0, 0x000C);
			acpigen_resource_word(1, 0xc, 0x3, 0, 0x03C0, 0x03DF, 0, 0x0020);
		}

		/* IO resource */
		acpigen_resource_word(1, 0xc, 0x3, 0, ri->PciResourceIoBase,
			ri->PciResourceIoLimit, 0x0,
			(ri->PciResourceIoLimit - ri->PciResourceIoBase + 1));

		/* additional mem32 resources on socket 0 bus 0 */
		if (stack == 0) {
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
	acpigen_pop_len();
}

void southbridge_inject_dsdt(const struct device *device)
{
	struct global_nvs *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);
	if (!gnvs) {
		gnvs = cbmem_add(CBMEM_ID_ACPI_GNVS, 0x2000);
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

	/* Add IIOStack ACPI Resource Templates */
	uncore_inject_dsdt();
}

void acpi_create_gnvs(struct global_nvs *gnvs)
{
	/* CPU core count */
	gnvs->pcnt = dev_count_cpu();
	printk(BIOS_DEBUG, "%s gnvs->pcnt: %d\n", __func__, gnvs->pcnt);
}

static unsigned long acpi_madt_irq_overrides(unsigned long current)
{
	int sci = SCI_INT_NUM;
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

static unsigned long xeonsp_acpi_create_madt_lapics(unsigned long current)
{
	struct device *cpu;
	uint8_t num_cpus = 0;

	for (cpu = all_devices; cpu; cpu = cpu->next) {
		if ((cpu->path.type != DEVICE_PATH_APIC) ||
			(cpu->bus->dev->path.type != DEVICE_PATH_CPU_CLUSTER)) {
			continue;
		}
		if (!cpu->enabled)
			continue;
		current += acpi_create_madt_lapic((acpi_madt_lapic_t *)current,
			num_cpus, cpu->path.apic.apic_id);
		num_cpus++;
	}

	return current;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	int cur_index;
	struct iiostack_resource stack_info = {0};

	int gsi_bases[] = { 0, 0x18, 0x20, 0x28, 0x30, 0x48, 0x50, 0x58, 0x60 };
	int ioapic_ids[] = { 0x8, 0x9, 0xa, 0xb, 0xc, 0xf, 0x10, 0x11, 0x12 };

	/* Local APICs */
	current = xeonsp_acpi_create_madt_lapics(current);

	cur_index = 0;
	get_iiostack_info(&stack_info);

	for (int stack = 0; stack < stack_info.no_of_stacks; ++stack) {
		const STACK_RES *ri = &stack_info.res[stack];
		assert(cur_index < ARRAY_SIZE(ioapic_ids));
		assert(cur_index < ARRAY_SIZE(gsi_bases));
		int ioapic_id = ioapic_ids[cur_index];
		int gsi_base = gsi_bases[cur_index];
		printk(BIOS_DEBUG, "Adding MADT IOAPIC for stack: %d, ioapic_id: 0x%x, "
			"ioapic_base: 0x%x, gsi_base: 0x%x\n",
			stack,  ioapic_id, ri->IoApicBase, gsi_base);
		current += acpi_create_madt_ioapic(
			(acpi_madt_ioapic_t *)current,
			ioapic_id, ri->IoApicBase, gsi_base);
		++cur_index;

		/*
		 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
		 * Add entry for PCH IOAPIC.
		 */
		if (stack == 0) { /* PCH IOAPIC */
			assert(cur_index < ARRAY_SIZE(ioapic_ids));
			assert(cur_index < ARRAY_SIZE(gsi_bases));
			ioapic_id = ioapic_ids[cur_index];
			gsi_base = gsi_bases[cur_index];
			printk(BIOS_DEBUG, "Adding MADT IOAPIC for stack: %d, ioapic_id: 0x%x, "
				"ioapic_base: 0x%x, gsi_base: 0x%x\n",
				stack,  ioapic_id,
				ri->IoApicBase + 0x1000, gsi_base);
			current += acpi_create_madt_ioapic(
				(acpi_madt_ioapic_t *)current,
				ioapic_id, ri->IoApicBase + 0x1000, gsi_base);
			++cur_index;
		}
	}

	return acpi_madt_irq_overrides(current);
}

static int calculate_power(int tdp, int p1_ratio, int ratio)
{
	u32 m;
	u32 power;

	/*
	 * M = ((1.1 - ((p1_ratio - ratio) * 0.00625)) / 1.1) ^ 2
	 *
	 * Power = (ratio / p1_ratio) * m * tdp
	 */

	m = (110000 - ((p1_ratio - ratio) * 625)) / 11;
	m = (m * m) / 1000;

	power = ((ratio * 100000 / p1_ratio) / 100);
	power *= (m / 100) * (tdp / 1000);
	power /= 1000;

	return (int)power;
}

static void cpx_generate_p_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, power_unit, num_entries;
	int ratio, power, clock, clock_max;
	msr_t msr;

	/* Determine P-state coordination type from MISC_PWR_MGMT[0] */
	msr = rdmsr(MSR_MISC_PWR_MGMT);
	if (msr.lo & MISC_PWR_MGMT_EIST_HW_DIS)
		coord_type = SW_ANY;
	else
		coord_type = HW_ALL;

	/* Get bus ratio limits and calculate clock speeds */
	msr = rdmsr(MSR_PLATFORM_INFO);
	ratio_min = (msr.hi >> (40-32)) & 0xff; /* Max Efficiency Ratio */

	/* Determine if this CPU has configurable TDP */
	if (cpu_config_tdp_levels()) {
		/* Set max ratio to nominal TDP ratio */
		msr = rdmsr(MSR_CONFIG_TDP_NOMINAL);
		ratio_max = msr.lo & 0xff;
	} else {
		/* Max Non-Turbo Ratio */
		ratio_max = (msr.lo >> 8) & 0xff;
	}
	clock_max = ratio_max * CONFIG_CPU_BCLK_MHZ;

	/* Calculate CPU TDP in mW */
	msr = rdmsr(MSR_PKG_POWER_SKU_UNIT);
	power_unit = 2 << ((msr.lo & 0xf) - 1);
	msr = rdmsr(MSR_PKG_POWER_SKU);
	power_max = ((msr.lo & 0x7fff) / power_unit) * 1000;

	/* Write _PCT indicating use of FFixedHW */
	acpigen_write_empty_PCT();

	/* Write _PPC with no limit on supported P-state */
	acpigen_write_PPC_NVS();

	/* Write PSD indicating configured coordination type */
	acpigen_write_PSD_package(core, 1, coord_type);

	/* Add P-state entries in _PSS table */
	acpigen_write_name("_PSS");

	/* Determine ratio points */
	ratio_step = PSS_RATIO_STEP;
	num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	if (num_entries > PSS_MAX_ENTRIES) {
		ratio_step += 1;
		num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
	}

	/* P[T] is Turbo state if enabled */
	if (get_turbo_state() == TURBO_ENABLED) {
		/* _PSS package count including Turbo */
		acpigen_write_package(num_entries + 2);

		msr = rdmsr(MSR_TURBO_RATIO_LIMIT);
		ratio_turbo = msr.lo & 0xff;

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(
			clock_max + 1,		/* MHz */
			power_max,		/* mW */
			PSS_LATENCY_TRANSITION,	/* lat1 */
			PSS_LATENCY_BUSMASTER,	/* lat2 */
			ratio_turbo << 8,	/* control */
			ratio_turbo << 8);	/* status */
	} else {
		/* _PSS package count without Turbo */
		acpigen_write_package(num_entries + 1);
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(
		clock_max,		/* MHz */
		power_max,		/* mW */
		PSS_LATENCY_TRANSITION,	/* lat1 */
		PSS_LATENCY_BUSMASTER,	/* lat2 */
		ratio_max << 8,		/* control */
		ratio_max << 8);	/* status */

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = calculate_power(power_max, ratio_max, ratio);
		clock = ratio * CONFIG_CPU_BCLK_MHZ;
		//clock = 1;
		acpigen_write_PSS_package(
			clock,			/* MHz */
			power,			/* mW */
			PSS_LATENCY_TRANSITION,	/* lat1 */
			PSS_LATENCY_BUSMASTER,	/* lat2 */
			ratio << 8,		/* control */
			ratio << 8);		/* status */
	}

	/* Fix package length */
	acpigen_pop_len();
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

			/* Generate P-state tables */
			cpx_generate_p_state_entries(core_id, threads_per_package);
			acpigen_pop_len();
		}
	}
	/* PPKG is usually used for thermal management of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, threads_per_package);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(threads_per_package);
}

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;
	else
		return MP_IRQ_POLARITY_HIGH;
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{

	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->header.revision = get_acpi_table_revision(FADT);
	fadt->sci_int = SCI_INT_NUM;

	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;

	fadt->gpe0_blk = pmbase + GPE0_STS(0);

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;

	/* GPE0 STS/EN pairs each 32 bits wide. */
	fadt->gpe0_blk_len = 2 * GPE0_REG_MAX * sizeof(uint32_t);

	fadt->duty_offset = 1;
	fadt->day_alrm = 0xd;

	fadt->flags |= ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED | ACPI_FADT_C2_MP_SUPPORTED |
			ACPI_FADT_PLATFORM_CLOCK;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.addrl = pmbase + PM1_STS;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + PM1_CNT;

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}

	/*  General-Purpose Event Registers */
	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = 64; /* EventStatus + EventEnable */
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0;
}

unsigned long acpi_create_srat_lapics(unsigned long current)
{
	struct device *cpu;
	unsigned int cpu_index = 0;

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

static unsigned int get_srat_memory_entries(acpi_srat_mem_t *srat_mem)
{
	const struct SystemMemoryMapHob *memory_map;
	unsigned int mmap_index;

	memory_map = get_system_memory_map();
	assert(memory_map != NULL);
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
	unsigned int nodes = xeon_sp_get_socket_count();

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
 * Ports    Stack   Stack(HOB)  IioConfigIou
 * ==========================================
 * 0        CSTACK  stack 0     IOU0
 * 1A..1D   PSTACK0	stack 1     IOU1
 * 2A..2D   PSTACK1	stack 2     IOU2
 * 3A..3D   PSTACK2	stack 4     IOU3
 */
static int get_stack_for_port(int port)
{
	if (port == PORT_0)
		return CSTACK;
	else if (port >= PORT_1A && port <= PORT_1D)
		return PSTACK0;
	else if (port >= PORT_2A && port <= PORT_2D)
		return PSTACK1;
	else if (port >= PORT_3A && port <= PORT_3D)
		return PSTACK2;
	else
		return -1;
}

/*
 * This function adds PCIe bridge device entry in DMAR table. If it is called
 * in the context of ATSR subtable, it adds ATSR subtable when it is first called.
 */
static unsigned long acpi_create_dmar_ds_pci_br_for_port(unsigned long current,
	int port, int stack, const IIO_RESOURCE_INSTANCE *iio_resource, uint32_t pcie_seg,
	bool is_atsr, bool *first)
{

	if (get_stack_for_port(port) != stack)
		return 0;

	const uint32_t bus = iio_resource->StackRes[stack].BusBase;
	const uint32_t dev = iio_resource->PcieInfo.PortInfo[port].Device;
	const uint32_t func = iio_resource->PcieInfo.PortInfo[port].Function;

	const uint32_t id = pci_mmio_read_config32(PCI_DEV(bus, dev, func),
		PCI_VENDOR_ID);
	if (id == 0xffffffff)
		return 0;

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

	uint32_t bus = hob->PlatformData.IIO_resource[socket].StackRes[stack].BusBase;
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
		const IIO_RESOURCE_INSTANCE *iio_resource =
			&hob->PlatformData.IIO_resource[socket];
		for (int p = PORT_0; p < MAX_PORTS; ++p)
			current += acpi_create_dmar_ds_pci_br_for_port(current, p, stack,
				iio_resource, pcie_seg, false, NULL);

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

static unsigned long acpi_create_atsr(unsigned long current, const IIO_UDS *hob)
{
	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		uint32_t pcie_seg = hob->PlatformData.CpuQpiInfo[socket].PcieSegment;
		unsigned long tmp = current;
		bool first = true;
		const IIO_RESOURCE_INSTANCE *iio_resource =
			&hob->PlatformData.IIO_resource[socket];

		for (int stack = 0; stack <= PSTACK2; ++stack) {
			uint32_t bus = iio_resource->StackRes[stack].BusBase;
			uint32_t vtd_base = iio_resource->StackRes[stack].VtdBarAddress;
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
					stack, iio_resource, pcie_seg, true, &first);
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
		IIO_RESOURCE_INSTANCE *iio_resource =
			&hob->PlatformData.IIO_resource[socket];
		for (int stack = 0; stack <= PSTACK2; ++stack) {
			uint32_t vtd_base = iio_resource->StackRes[stack].VtdBarAddress;
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

	const struct soc_intel_xeon_sp_cpx_config *const config = config_of(device);

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
		printk(BIOS_DEBUG, "[DMA Remapping table] Flags: 0x%x\n", DMAR_INTR_REMAP);
		acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
		current += dmar->header.length;
		current = acpi_align_current(current);
		acpi_add_table(rsdp, dmar);
	}

	return current;
}
