/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <intelblocks/acpi.h>
#include <intelblocks/pcr.h>
#include <intelblocks/itss.h>
#include <soc/acpi.h>
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
