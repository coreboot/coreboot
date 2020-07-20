/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <assert.h>
#include <cbmem.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci.h>
#include <intelblocks/acpi.h>
#include <hob_iiouds.h>
#include <hob_memmap.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
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

unsigned long northbridge_write_acpi_tables(const struct device *device,
					    unsigned long current,
					    struct acpi_rsdp *rsdp)
{
	acpi_srat_t *srat;
	acpi_slit_t *slit;

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

	return current;
}
