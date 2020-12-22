/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <acpi/acpi.h>
#include <acpi/acpi_gnvs.h>
#include <bootmode.h>
#include <console/console.h>
#include <elog.h>
#include <halt.h>
#include <security/vboot/vboot_common.h>
#include <soc/southbridge.h>
#include <soc/nvs.h>

void poweroff(void)
{
	acpi_write32(MMIO_ACPI_PM1_CNT_BLK,
			  (SLP_TYP_S5 << SLP_TYP_SHIFT) | SLP_EN);

	/*
	 * Setting SLP_TYP_S5 in PM1 triggers SLP_SMI, which is handled by SMM
	 * to transition to S5 state. If halt is called in SMM, then it prevents
	 * the SMI handler from being triggered and system never enters S5.
	 */
	if (!ENV_SMM)
		halt();
}

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char *const pm1_sts_bits[16] = {
		[0] = "TMROF",
		[4] = "BMSTATUS",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_DEBUG, "PM1_STS: ");
	print_num_status_bits(ARRAY_SIZE(pm1_sts_bits), pm1_sts, pm1_sts_bits);
	printk(BIOS_DEBUG, "\n");

	return pm1_sts;
}

static void log_pm1_status(uint16_t pm1_sts)
{
	if (!CONFIG(ELOG))
		return;

	if (pm1_sts & WAK_STS)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE,
				    acpi_is_wakeup_s3() ? ACPI_S3 : ACPI_S5);

	if (pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	if (pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	if (pm1_sts & PCIEXPWAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);
}

static void log_gpe_events(const struct acpi_pm_gpe_state *state)
{
	int i;
	uint32_t valid_gpe = state->gpe0_sts & state->gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (valid_gpe & (1U << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, i);
	}
}

void acpi_fill_pm_gpe_state(struct acpi_pm_gpe_state *state)
{
	state->pm1_sts = acpi_read16(MMIO_ACPI_PM1_STS);
	state->pm1_en = acpi_read16(MMIO_ACPI_PM1_EN);
	state->gpe0_sts = acpi_read32(MMIO_ACPI_GPE0_STS);
	state->gpe0_en = acpi_read32(MMIO_ACPI_GPE0_EN);
	state->previous_sx_state = acpi_get_sleep_type();
	state->aligning_field = 0;
}

void acpi_pm_gpe_add_events_print_events(const struct acpi_pm_gpe_state *state)
{
	log_pm1_status(state->pm1_sts);
	print_pm1_status(state->pm1_sts);
	log_gpe_events(state);
}

void acpi_clear_pm_gpe_status(void)
{
	acpi_write16(MMIO_ACPI_PM1_STS, acpi_read16(MMIO_ACPI_PM1_STS));
	acpi_write32(MMIO_ACPI_GPE0_STS, acpi_read32(MMIO_ACPI_GPE0_STS));
}

int acpi_get_sleep_type(void)
{
	return acpi_sleep_from_pm1(acpi_read16(MMIO_ACPI_PM1_CNT_BLK));
}

int platform_is_resuming(void)
{
	if (!(acpi_read16(MMIO_ACPI_PM1_STS) & WAK_STS))
		return 0;

	return acpi_get_sleep_type() == ACPI_S3;
}

/* If a system reset is about to be requested, modify the PM1 register so it
 * will never be misinterpreted as an S3 resume. */
void set_pm1cnt_s5(void)
{
	uint16_t pm1;

	pm1 = acpi_read16(MMIO_ACPI_PM1_CNT_BLK);
	pm1 &= ~SLP_TYP;
	pm1 |= SLP_TYP_S5 << SLP_TYP_SHIFT;
	acpi_write16(MMIO_ACPI_PM1_CNT_BLK, pm1);
}

void vboot_platform_prepare_reboot(void)
{
	set_pm1cnt_s5();
}

void acpi_enable_sci(void)
{
	uint32_t pm1;

	pm1 = acpi_read32(MMIO_ACPI_PM1_CNT_BLK);
	pm1 |= ACPI_PM1_CNT_SCIEN;
	acpi_write32(MMIO_ACPI_PM1_CNT_BLK, pm1);
}

void acpi_disable_sci(void)
{
	uint32_t pm1;

	pm1 = acpi_read32(MMIO_ACPI_PM1_CNT_BLK);
	pm1 &= ~ACPI_PM1_CNT_SCIEN;
	acpi_write32(MMIO_ACPI_PM1_CNT_BLK, pm1);
}
