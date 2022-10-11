/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <acpi/acpigen.h>
#include <arch/smp/mpspec.h>
#include <cpu/cpu.h>
#include <cpu/x86/smm.h>
#include <string.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <cbmem.h>
#include <console/console.h>
#include <intelblocks/acpi.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/nvs.h>
#include <soc/soc_util.h>
#include <soc/pmc.h>
#include <soc/systemagent.h>
#include <soc/pci_devs.h>

#define CSTATE_RES(address_space, width, offset, address)		\
	{								\
	.space_id = address_space,					\
	.bit_width = width,						\
	.bit_offset = offset,						\
	.addrl = address,						\
	}

static const acpi_cstate_t cstate_map[] = {
	{
		/* C1 */
		.ctype = 1,		/* ACPI C1 */
		.latency = 2,
		.power = 1000,
		.resource = MWAIT_RES(0, 0),
	},
	{
		.ctype = 2,		/* ACPI C2 */
		.latency = 10,
		.power = 10,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_IO, 8, 0,
				       ACPI_BASE_ADDRESS + 0x14),
	},
	{
		.ctype = 3,		/* ACPI C3 */
		.latency = 50,
		.power = 10,
		.resource = CSTATE_RES(ACPI_ADDRESS_SPACE_IO, 8, 0,
				       ACPI_BASE_ADDRESS + 0x15),
	}
};

void soc_fill_gnvs(struct global_nvs *gnvs)
{
	/* Top of Low Memory (start of resource allocation) */
	gnvs->tolm = (uintptr_t)cbmem_top();

	/* MMIO Low/High & TSEG base and length */
	gnvs->mmiob = (u32)get_top_of_low_memory();
	gnvs->mmiol = (u32)(get_pciebase() - 1);
	gnvs->mmiohb = (u64)get_top_of_upper_memory();
	gnvs->mmiohl = (u64)(((u64)1 << cpu_phys_address_size()) - 1);
	gnvs->tsegb = (u32)get_tseg_memory();
	gnvs->tsegl = (u32)(get_top_of_low_memory() - get_tseg_memory());
}

uint32_t soc_read_sci_irq_select(void)
{
	struct device *dev = get_pmc_dev();

	if (!dev)
		return 0;

	return pci_read_config32(dev, PMC_ACPI_CNT);
}

const acpi_cstate_t *soc_get_cstate_map(size_t *entries)
{
	*entries = ARRAY_SIZE(cstate_map);
	return cstate_map;
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	u16 pmbase = get_pmbase();

	/* Power Control */
	fadt->pm2_cnt_blk = pmbase + PM2_CNT;
	fadt->pm_tmr_blk = pmbase + PM1_TMR;

	/* Control Registers - Length */
	fadt->pm2_cnt_len = 1;
	fadt->pm_tmr_len = 4;

	fadt->iapc_boot_arch = ACPI_FADT_LEGACY_DEVICES | ACPI_FADT_8042;

	/* PM2 Control Registers */
	fadt->x_pm2_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm2_cnt_blk.bit_width = fadt->pm2_cnt_len * 8;
	fadt->x_pm2_cnt_blk.bit_offset = 0;
	fadt->x_pm2_cnt_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_pm2_cnt_blk.addrl = fadt->pm2_cnt_blk;
	fadt->x_pm2_cnt_blk.addrh = 0x00;

	/* PM1 Timer Register */
	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x00;
}

static acpi_tstate_t denverton_tss_table[] = {
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
	*entries = ARRAY_SIZE(denverton_tss_table);
	return denverton_tss_table;
}

void soc_power_states_generation(int core_id, int cores_per_package)
{
	generate_p_state_entries(core_id, cores_per_package);

	generate_t_state_entries(core_id, cores_per_package);
}

int soc_madt_sci_irq_polarity(int sci)
{
	if (sci >= 20)
		return MP_IRQ_POLARITY_LOW;
	else
		return MP_IRQ_POLARITY_HIGH;
}

unsigned long southcluster_write_acpi_tables(const struct device *device,
					     unsigned long current,
					     struct acpi_rsdp *rsdp)
{
	acpi_header_t *ssdt2;

	current = acpi_write_hpet(device, current, rsdp);
	current = (ALIGN(current, 16));

	ssdt2 = (acpi_header_t *)current;
	memset(ssdt2, 0, sizeof(acpi_header_t));
	acpi_create_serialio_ssdt(ssdt2);
	if (ssdt2->length) {
		current += ssdt2->length;
		acpi_add_table(rsdp, ssdt2);
		printk(BIOS_DEBUG, "ACPI:     * SSDT2 @ %p Length %x\n", ssdt2,
		       ssdt2->length);
		current = (ALIGN(current, 16));
	} else {
		ssdt2 = NULL;
		printk(BIOS_DEBUG, "ACPI:     * SSDT2 not generated.\n");
	}

	printk(BIOS_DEBUG, "current = %lx\n", current);

	return current;
}

__weak void acpi_create_serialio_ssdt(acpi_header_t *ssdt) {}

static unsigned long acpi_fill_dmar(unsigned long current)
{
	uint64_t vtbar;
	unsigned long tmp = current;

	vtbar = read64((void *)(DEFAULT_MCHBAR + MCH_VTBAR_OFFSET)) & MCH_VTBAR_MASK;
	printk(BIOS_DEBUG, "DEFVTBAR:0x%llx\n", vtbar);
	if (!vtbar)
		return current;

	current += acpi_create_dmar_drhd(current,
			DRHD_INCLUDE_PCI_ALL, 0, vtbar);

	current += acpi_create_dmar_ds_ioapic(current,
			2, PCH_IOAPIC_PCI_BUS, PCH_IOAPIC_PCI_SLOT, 0);
	current += acpi_create_dmar_ds_msi_hpet(current,
			0, PCH_HPET_PCI_BUS, PCH_HPET_PCI_SLOT, 0);

	acpi_dmar_drhd_fixup(tmp, current);

	/* Create RMRR; see "VTD PLATFORM CONFIGURATION" in FSP log */
	tmp = current;
	current += acpi_create_dmar_rmrr(current, 0,
					 RMRR_USB_BASE_ADDRESS,
					 RMRR_USB_LIMIT_ADDRESS);
	current += acpi_create_dmar_ds_pci(current,
			0, XHCI_DEV, XHCI_FUNC);
	acpi_dmar_rmrr_fixup(tmp, current);

	return current;
}

unsigned long systemagent_write_acpi_tables(const struct device *dev,
					    unsigned long current,
					    struct acpi_rsdp *const rsdp)
{
	/* Create DMAR table only if we have VT-d capability. */
	const u32 capid0_a = pci_read_config32(dev, CAPID0_A);
	if (capid0_a & VTD_DISABLE)
		return current;

	acpi_dmar_t *const dmar = (acpi_dmar_t *)current;
	printk(BIOS_DEBUG, "ACPI:    * DMAR\n");
	acpi_create_dmar(dmar, DMAR_INTR_REMAP, acpi_fill_dmar);
	current += dmar->header.length;
	current = acpi_align_current(current);
	acpi_add_table(rsdp, dmar);

	return current;
}
