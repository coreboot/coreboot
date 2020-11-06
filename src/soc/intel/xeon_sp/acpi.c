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
#include <soc/pm.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <string.h>

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = 0;
	return NULL;
}

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
	current += acpi_create_madt_irqoverride((void *)current, 0, sci, sci, flags);

	current +=
		acpi_create_madt_lapic_nmi((acpi_madt_lapic_nmi_t *) current, 0xff, 0x0d, 1);

	return current;
}

static unsigned long add_madt_ioapic(unsigned long current, int socket, int stack,
				     int ioapic_id, uint32_t ioapic_base, int gsi_base)
{
	printk(BIOS_DEBUG, "Adding MADT IOAPIC for socket: %d, stack: %d, ioapic_id: 0x%x, "
	       "ioapic_base: 0x%x, gsi_base: 0x%x\n",
	       socket, stack,  ioapic_id, ioapic_base, gsi_base);
	return acpi_create_madt_ioapic((acpi_madt_ioapic_t *)current, ioapic_id,
				       ioapic_base, gsi_base);
}

unsigned long acpi_fill_madt(unsigned long current)
{
	int cur_index;
	int ioapic_id;
	int gsi_base;
	const IIO_UDS *hob = get_iio_uds();

	/* With XEON-SP FSP, PCH IOAPIC is allocated with first 120 GSIs. */
#if (CONFIG(SOC_INTEL_COOPERLAKE_SP))
	const int gsi_bases[] = { 0, 0x78, 0x80, 0x88, 0x90, 0x98, 0xA0, 0xA8, 0xB0 };
	const int ioapic_ids[] = { 0x8, 0x9, 0xa, 0xb, 0xc, 0xf, 0x10, 0x11, 0x12 };
#endif

#if (CONFIG(SOC_INTEL_SKYLAKE_SP))
	const int gsi_bases[] = { 0, 0x18, 0x20, 0x28, 0x30, 0x48, 0x50, 0x58, 0x60 };
	const int ioapic_ids[] = { 0x8, 0x9, 0xa, 0xb, 0xc, 0xf, 0x10, 0x11, 0x12 };
#endif
	/* Local APICs */
	current = xeonsp_acpi_create_madt_lapics(current);

	cur_index = 0;
	ioapic_id = ioapic_ids[cur_index];
	gsi_base = gsi_bases[cur_index];
	current += add_madt_ioapic(current, 0, 0, ioapic_id,
				   hob->PlatformData.IIO_resource[0].StackRes[0].IoApicBase,
				   gsi_base);
		++cur_index;

	for (int socket = 0; socket < hob->PlatformData.numofIIO; ++socket) {
		for (int stack = 0; stack < MAX_IIO_STACK; ++stack) {
			const STACK_RES *ri =
				&hob->PlatformData.IIO_resource[socket].StackRes[stack];
			if (!is_iio_stack_res(ri))
				continue;
			assert(cur_index < ARRAY_SIZE(ioapic_ids));
			assert(cur_index < ARRAY_SIZE(gsi_bases));
			ioapic_id = ioapic_ids[cur_index];
			gsi_base = gsi_bases[cur_index];
			uint32_t ioapic_base = ri->IoApicBase;

			/*
			 * Stack 0 has non-PCH IOAPIC and PCH IOAPIC.
			 * The IIO IOAPIC is placed at 0x1000 from the reported base.
			 */
			if (stack == 0 && socket == 0)
				ioapic_base += 0x1000;

			current += add_madt_ioapic(current, socket, stack, ioapic_id,
						   ioapic_base, gsi_base);
			++cur_index;
		}
	}

	return acpi_madt_irq_overrides(current);
}

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->header.revision = get_acpi_table_revision(FADT);

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

	fadt->duty_offset = 1;
	fadt->day_alrm = 0xd;

	fadt->flags |= ACPI_FADT_WBINVD | ACPI_FADT_C1_SUPPORTED |
			ACPI_FADT_C2_MP_SUPPORTED | ACPI_FADT_SLEEP_BUTTON |
			ACPI_FADT_SEALED_CASE | ACPI_FADT_S4_RTC_WAKE |
			ACPI_FADT_PLATFORM_CLOCK;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.addrl = pmbase + PM1_STS;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.addrl = pmbase + PM1_CNT;
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
}

int calculate_power(int tdp, int p1_ratio, int ratio)
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

acpi_tstate_t *soc_get_tss_table(int *entries)
{
	*entries = 0;
	return NULL;
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

			/* Soc specific power states generation */
			soc_power_states_generation(core_id, threads_per_package);

			acpigen_pop_len();
		}
	}
	/* PPKG is usually used for thermal management of the first and only package. */
	acpigen_write_processor_package("PPKG", 0, threads_per_package);

	/* Add a method to notify processor nodes */
	acpigen_write_processor_cnot(threads_per_package);
}
