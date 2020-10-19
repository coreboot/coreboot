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

acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = 0;
	return NULL;
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
