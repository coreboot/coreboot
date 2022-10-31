/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_pm.h>
#include <acpi/acpigen.h>
#include <arch/ioapic.h>
#include <arch/smp/mpspec.h>
#include <console/console.h>
#include <cpu/cpu.h>
#include <cpu/intel/common/common.h>
#include <cpu/intel/msr.h>
#include <cpu/intel/turbo.h>
#include <cpu/x86/lapic.h>
#include <cpu/x86/smm.h>
#include <intelblocks/acpi_wake_source.h>
#include <intelblocks/acpi.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/sgx.h>
#include <intelblocks/uart.h>
#include <soc/gpio.h>
#include <soc/iomap.h>
#include <soc/pm.h>

#define  CPUID_6_EAX_ISST	(1 << 7)

static int acpi_sci_irq(void)
{
	int sci_irq = 9;
	uint32_t scis;

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
	current +=
	    acpi_create_madt_irqoverride((void *)current, 0, sci, sci, flags);

	/* NMI */
	current += acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *)current, 0xff, 5, 1);

	if (is_x2apic_mode())
		current += acpi_create_madt_lx2apic_nmi((acpi_madt_lx2apic_nmi_t *)current,
				0xffffffff, 0x5, 1);

	return current;
}

__weak const struct madt_ioapic_info *soc_get_ioapic_info(size_t *entries)
{
	*entries = 0;
	return NULL;
}

unsigned long acpi_fill_madt(unsigned long current)
{
	const struct madt_ioapic_info *ioapic_table;
	size_t ioapic_entries;

	/* Local APICs */
	current = acpi_create_madt_lapics(current);

	/* IOAPIC */
	ioapic_table = soc_get_ioapic_info(&ioapic_entries);
	if (ioapic_entries) {
		for (int i = 0; i < ioapic_entries; i++) {
			current += acpi_create_madt_ioapic(
					(void *)current,
					ioapic_table[i].id,
					ioapic_table[i].addr,
					ioapic_table[i].gsi_base);
		}
	} else {
		/* Default SOC IOAPIC entry */
		current += acpi_create_madt_ioapic((void *)current, 2, IO_APIC_ADDR, 0);
	}

	return acpi_madt_irq_overrides(current);
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->sci_int = acpi_sci_irq();

	if (permanent_smi_handler()) {
		fadt->smi_cmd = APM_CNT;
		fadt->acpi_enable = APM_CNT_ACPI_ENABLE;
		fadt->acpi_disable = APM_CNT_ACPI_DISABLE;
	}

	fadt->pm1a_evt_blk = pmbase + PM1_STS;
	fadt->pm1a_cnt_blk = pmbase + PM1_CNT;

	fadt->gpe0_blk = pmbase + GPE0_STS(0);

	fadt->pm1_evt_len = 4;
	fadt->pm1_cnt_len = 2;

	/* GPE0 STS/EN pairs each 32 bits wide. */
	fadt->gpe0_blk_len = 2 * GPE0_REG_MAX * sizeof(uint32_t);

	fadt->day_alrm = 0xd;

	fadt->flags |= ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_SEALED_CASE | ACPI_FADT_S4_RTC_WAKE;

	if (CONFIG(USE_PM_ACPI_TIMER))
		fadt->flags |= ACPI_FADT_PLATFORM_CLOCK;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.addrl = fadt->pm1a_evt_blk;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.addrl = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;

	/*
	 * Windows 10 requires x_gpe0_blk to be set starting with FADT revision 5.
	 * The bit_width field intentionally overflows here.
	 * The OSPM can instead use the values in `fadt->gpe0_blk{,_len}`, which
	 * seems to work fine on Linux 5.0 and Windows 10.
	 */
	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0;
}

unsigned long southbridge_write_acpi_tables(const struct device *device,
					    unsigned long current,
					    struct acpi_rsdp *rsdp)
{
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_UART)) {
		current = acpi_write_dbg2_pci_uart(rsdp, current,
						uart_get_device(),
						ACPI_ACCESS_SIZE_DWORD_ACCESS);
	}

	return acpi_write_hpet(device, current, rsdp);
}

__weak
void acpi_fill_soc_wake(uint32_t *pm1_en, uint32_t *gpe0_en,
			const struct chipset_power_state *ps)
{
}

/*
 * Save wake source information for calculating ACPI _SWS values
 *
 * @pm1:  PM1_STS register with only enabled events set
 * @gpe0: GPE0_STS registers with only enabled events set
 *
 * return the number of registers in the gpe0 array
 */

int soc_fill_acpi_wake(const struct chipset_power_state *ps, uint32_t *pm1, uint32_t **gpe0)
{
	static uint32_t gpe0_sts[GPE0_REG_MAX];
	uint32_t gpe0_en[GPE0_REG_MAX];
	uint32_t pm1_en;
	int i;

	/*
	 * PM1_EN to check the basic wake events which can happen through
	 * powerbtn or any other wake source like lidopen, key board press etc.
	 */
	pm1_en = ps->pm1_en;
	pm1_en |= WAK_STS | PWRBTN_EN;

	memcpy(gpe0_en, ps->gpe0_en, sizeof(gpe0_en));

	acpi_fill_soc_wake(&pm1_en, gpe0_en, ps);

	*pm1 = ps->pm1_sts & pm1_en;

	/* Mask off GPE0 status bits that are not enabled */
	*gpe0 = &gpe0_sts[0];
	for (i = 0; i < GPE0_REG_MAX; i++)
		gpe0_sts[i] = ps->gpe0_sts[i] & gpe0_en[i];

	return GPE0_REG_MAX;
}

int common_calculate_power_ratio(int tdp, int p1_ratio, int ratio)
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

	return power;
}

static void generate_c_state_entries(void)
{
	const acpi_cstate_t *c_state_map;
	size_t entries;

	c_state_map = soc_get_cstate_map(&entries);

	/* Generate C-state tables */
	acpigen_write_CST_package(c_state_map, entries);
}

void generate_p_state_entries(int core, int cores_per_package)
{
	int ratio_min, ratio_max, ratio_turbo, ratio_step;
	int coord_type, power_max, num_entries;
	int ratio, power, clock, clock_max;
	bool turbo;

	coord_type = cpu_get_coord_type();
	ratio_min = cpu_get_min_ratio();
	ratio_max = cpu_get_max_ratio();
	clock_max = (ratio_max * cpu_get_bus_clock()) / KHz;
	turbo = (get_turbo_state() == TURBO_ENABLED);

	/* Calculate CPU TDP in mW */
	power_max = cpu_get_power_max();

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
	do {
		num_entries = ((ratio_max - ratio_min) / ratio_step) + 1;
		if (((ratio_max - ratio_min) % ratio_step) > 0)
			num_entries += 1;
		if (turbo)
			num_entries += 1;
		if (num_entries > PSS_MAX_ENTRIES)
			ratio_step += 1;
	} while (num_entries > PSS_MAX_ENTRIES);

	/* _PSS package count depends on Turbo */
	acpigen_write_package(num_entries);

	/* P[T] is Turbo state if enabled */
	if (turbo) {
		ratio_turbo = cpu_get_max_turbo_ratio();

		/* Add entry for Turbo ratio */
		acpigen_write_PSS_package(clock_max + 1,	/* MHz */
					  power_max,		/* mW */
					  PSS_LATENCY_TRANSITION,/* lat1 */
					  PSS_LATENCY_BUSMASTER,/* lat2 */
					  ratio_turbo << 8,	/* control */
					  ratio_turbo << 8);	/* status */
		num_entries -= 1;
	}

	/* First regular entry is max non-turbo ratio */
	acpigen_write_PSS_package(clock_max,		/* MHz */
				  power_max,		/* mW */
				  PSS_LATENCY_TRANSITION,/* lat1 */
				  PSS_LATENCY_BUSMASTER,/* lat2 */
				  ratio_max << 8,	/* control */
				  ratio_max << 8);	/* status */
	num_entries -= 1;

	/* Generate the remaining entries */
	for (ratio = ratio_min + ((num_entries - 1) * ratio_step);
	     ratio >= ratio_min; ratio -= ratio_step) {

		/* Calculate power at this ratio */
		power = common_calculate_power_ratio(power_max, ratio_max, ratio);
		clock = (ratio * cpu_get_bus_clock()) / KHz;

		acpigen_write_PSS_package(clock,		/* MHz */
					  power,		/* mW */
					  PSS_LATENCY_TRANSITION,/* lat1 */
					  PSS_LATENCY_BUSMASTER,/* lat2 */
					  ratio << 8,		/* control */
					  ratio << 8);		/* status */
	}
	/* Fix package length */
	acpigen_pop_len();
}

__attribute__ ((weak)) acpi_tstate_t *soc_get_tss_table(int *entries)
{
	*entries = 0;
	return NULL;
}

void generate_t_state_entries(int core, int cores_per_package)
{
	acpi_tstate_t *soc_tss_table;
	int entries;

	soc_tss_table = soc_get_tss_table(&entries);
	if (entries == 0)
		return;

	/* Indicate SW_ALL coordination for T-states */
	acpigen_write_TSD_package(core, cores_per_package, SW_ALL);

	/* Indicate FixedHW so OS will use MSR */
	acpigen_write_empty_PTC();

	/* Set NVS controlled T-state limit */
	acpigen_write_TPC("\\TLVL");

	/* Write TSS table for MSR access */
	acpigen_write_TSS_package(entries, soc_tss_table);
}

static void generate_cppc_entries(int core_id)
{
	u32 version = CPPC_VERSION_2;

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_CPU_HYBRID))
		version = CPPC_VERSION_3;

	if (!(CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_CPPC) &&
	      cpuid_eax(6) & CPUID_6_EAX_ISST))
		return;

	/* Generate GCPC package in first logical core */
	if (core_id == 0) {
		struct cppc_config cppc_config;
		cpu_init_cppc_config(&cppc_config, version);
		acpigen_write_CPPC_package(&cppc_config);
	}

	/* Write _CPC entry for each logical core */
	if (CONFIG(SOC_INTEL_COMMON_BLOCK_ACPI_CPU_HYBRID))
		acpigen_write_CPPC_hybrid_method(core_id);
	else
		acpigen_write_CPPC_method();
}

__weak void soc_power_states_generation(int core_id,
						int cores_per_package)
{
}

void generate_cpu_entries(const struct device *device)
{
	int core_id, cpu_id;
	int totalcores = dev_count_cpu();
	unsigned int num_virt;
	unsigned int num_phys;

	cpu_read_topology(&num_phys, &num_virt);

	int numcpus = totalcores / num_virt;

	printk(BIOS_DEBUG, "Found %d CPU(s) with %d/%d physical/logical core(s) each.\n",
	       numcpus, num_phys, num_virt);

	for (cpu_id = 0; cpu_id < numcpus; cpu_id++) {
		for (core_id = 0; core_id < num_virt; core_id++) {
			/* Generate processor \_SB.CPUx */
			acpigen_write_processor((cpu_id) * num_virt + core_id, 0, 0);

			/* Generate C-state tables */
			generate_c_state_entries();

			generate_cppc_entries(core_id);

			/* Soc specific power states generation */
			soc_power_states_generation(core_id, num_virt);

			acpigen_pop_len();
		}
	}
	/* PPKG is usually used for thermal management
	   of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, num_virt);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(num_virt);

	if (CONFIG(SOC_INTEL_COMMON_BLOCK_SGX_ENABLE))
		sgx_fill_ssdt();
}
