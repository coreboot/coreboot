/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <acpi/acpigen_pci.h>
#include <assert.h>
#include <intelblocks/acpi.h>
#include <intelblocks/pcr.h>
#include <intelblocks/itss.h>
#include <soc/acpi.h>
#include <soc/chip_common.h>
#include <soc/numa.h>
#include <soc/soc_util.h>
#include <soc/util.h>
#include <soc/itss.h>
#include <soc/pcr_ids.h>

int soc_madt_sci_irq_polarity(int sci)
{
	int reg = sci / IRQS_PER_IPC;
	int offset = sci % IRQS_PER_IPC;
	uint32_t val = pcr_read32(PID_ITSS, PCR_ITSS_IPC0_CONF + reg * sizeof(uint32_t));

	return (val & (1 << offset)) ? MP_IRQ_POLARITY_LOW : MP_IRQ_POLARITY_HIGH;
}

uint32_t soc_read_sci_irq_select(void)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;
	return inl(pmbase + PMC_ACPI_CNT);
}

void soc_fill_fadt(acpi_fadt_t *fadt)
{
	const uint16_t pmbase = ACPI_BASE_ADDRESS;

	fadt->pm_tmr_blk = pmbase + PM1_TMR;
	fadt->pm_tmr_len = 4;
	fadt->flags &=  ~(ACPI_FADT_SEALED_CASE | ACPI_FADT_S4_RTC_WAKE);
	fadt->preferred_pm_profile = PM_ENTERPRISE_SERVER;
}

void soc_power_states_generation(int core, int cores_per_package)
{
	generate_p_state_entries(core, cores_per_package);
}

static uint32_t get_granted_pcie_features(void)
{
	return PCIE_NATIVE_HOTPLUG_CONTROL | PCIE_PME_CONTROL |
		PCIE_CAP_STRUCTURE_CONTROL | PCIE_LTR_CONTROL |
		PCIE_AER_CONTROL;
}

static uint32_t get_granted_cxl_features(void)
{
	return CXL_ERROR_REPORTING_CONTROL;
}

void soc_pci_domain_fill_ssdt(const struct device *domain)
{
	const char *name = acpi_device_name(domain);
	if (!name)
		return;

	acpigen_write_scope(acpi_device_scope(domain));
	acpigen_write_device(name);

	if (is_cxl_domain(domain)) {
		acpigen_write_name("_HID");
		acpigen_emit_eisaid("ACPI0016");
		acpigen_write_name("_CID");
		acpigen_write_package(2);
		acpigen_emit_eisaid("PNP0A08");
		acpigen_emit_eisaid("PNP0A03");
		acpigen_pop_len();
	} else {
		acpigen_write_name("_HID");
		acpigen_emit_eisaid("PNP0A08");
		acpigen_write_name("_CID");
		acpigen_emit_eisaid("PNP0A03");
	}
	acpigen_write_name("_UID");
	acpigen_write_string(name);

	acpigen_write_name("_PXM");
	acpigen_write_integer(device_to_pd(domain));

	/* _OSC */
	acpigen_write_OSC_pci_domain_fixed_caps(domain,
		get_granted_pcie_features(),
		is_cxl_domain(domain),
		get_granted_cxl_features()
		);

	acpigen_pop_len();
	acpigen_pop_len();
}
