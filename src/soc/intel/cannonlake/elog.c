/* SPDX-License-Identifier: GPL-2.0-only */

#define __SIMPLE_DEVICE__

#include <bootstate.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <elog.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xhci.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>

struct pme_status_info {
	pci_devfn_t dev;
	uint8_t reg_offset;
	uint32_t elog_event;
};

#define PME_STS_BIT		(1 << 15)

static void pch_log_pme_internal_wake_source(void)
{
	size_t i;
	pci_devfn_t dev;
	uint16_t val;
	bool dev_found = false;

	const struct pme_status_info pme_status_info[] = {
		{ PCH_DEV_HDA, 0x54, ELOG_WAKE_SOURCE_PME_HDA },
		{ PCH_DEV_GBE, 0xcc, ELOG_WAKE_SOURCE_PME_GBE },
		{ PCH_DEV_SATA, 0x74, ELOG_WAKE_SOURCE_PME_SATA },
		{ PCH_DEV_CSE, 0x54, ELOG_WAKE_SOURCE_PME_CSE },
		{ PCH_DEV_USBOTG, 0x84, ELOG_WAKE_SOURCE_PME_XDCI },
		/*
		 * The power management control/status register is not
		 * listed in the cannonlake PCH EDS. We have been told
		 * that the PMCS register is at offset 0xCC.
		 */
		{ PCH_DEV_CNViWIFI, 0xcc, ELOG_WAKE_SOURCE_PME_WIFI },
	};
	const struct xhci_wake_info xhci_wake_info[] = {
		{ PCH_DEVFN_XHCI, ELOG_WAKE_SOURCE_PME_XHCI },
	};

	for (i = 0; i < ARRAY_SIZE(pme_status_info); i++) {
		dev = pme_status_info[i].dev;
		if (!dev)
			continue;

		val = pci_read_config16(dev, pme_status_info[i].reg_offset);

		if ((val == 0xFFFF) || !(val & PME_STS_BIT))
			continue;

		elog_add_event_wake(pme_status_info[i].elog_event, 0);
		dev_found = true;
	}

	/*
	 * Check the XHCI controllers' USB2 & USB3 ports for wake events. There
	 * are cases (GSMI logging for S0ix clears PME_STS_BIT) where the XHCI
	 * controller's PME_STS_BIT may have already been cleared, so the host
	 * controller wake wouldn't get logged here; therefore, the host
	 * controller wake event is logged before its corresponding port wake
	 * event is logged.
	 */
	dev_found |= xhci_update_wake_event(xhci_wake_info,
					    ARRAY_SIZE(xhci_wake_info));

	if (!dev_found)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);
}

static void pch_log_gpio_gpe(u32 gpe0_sts, u32 gpe0_en, int start)
{
	int i;

	gpe0_sts &= gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, i + start);
	}
}

static void pch_log_wake_source(const struct chipset_power_state *ps)
{
	/* Power Button */
	if (ps->pm1_sts & PWRBTN_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PWRBTN, 0);

	/* RTC */
	if (ps->pm1_sts & RTC_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_RTC, 0);

	/* PCI Express (TODO: determine wake device) */
	if (ps->pm1_sts & PCIEXPWAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PCIE, 0);

	/* PME (TODO: determine wake device) */
	if (ps->gpe0_sts[GPE_STD] & PME_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME, 0);

	/* XHCI - "Power Management Event Bus 0" events include XHCI */
	if (ps->gpe0_sts[GPE_STD] & PME_B0_STS)
		pch_log_pme_internal_wake_source();

	/* SMBUS Wake */
	if (ps->gpe0_sts[GPE_STD] & SMB_WAK_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_SMBUS, 0);

	/* Log GPIO events in set 1-3 */
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_31_0], ps->gpe0_en[GPE_31_0], 0);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_63_32], ps->gpe0_en[GPE_63_32], 32);
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_95_64], ps->gpe0_en[GPE_95_64], 64);
	/* Treat the STD as an extension of GPIO to obtain visibility. */
	pch_log_gpio_gpe(ps->gpe0_sts[GPE_STD], ps->gpe0_en[GPE_STD], 96);
}

static void pch_log_power_and_resets(const struct chipset_power_state *ps)
{
	/* Thermal Trip */
	if (ps->gblrst_cause[0] & GBLRST_CAUSE0_THERMTRIP)
		elog_add_event(ELOG_TYPE_THERM_TRIP);

	/* PWR_FLR Power Failure */
	if (ps->gen_pmcon_a & PWR_FLR)
		elog_add_event(ELOG_TYPE_POWER_FAIL);

	/* SUS Well Power Failure */
	if (ps->gen_pmcon_a & SUS_PWR_FLR)
		elog_add_event(ELOG_TYPE_SUS_POWER_FAIL);

	/* TCO Timeout */
	if (ps->prev_sleep_state != ACPI_S3 &&
	    ps->tco2_sts & TCO2_STS_SECOND_TO)
		elog_add_event(ELOG_TYPE_TCO_RESET);

	/* Power Button Override */
	if (ps->pm1_sts & PRBTNOR_STS)
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);

	/* RTC reset */
	if (ps->gen_pmcon_b & RTC_BATTERY_DEAD)
		elog_add_event(ELOG_TYPE_RTC_RESET);

	/* Host Reset Status */
	if (ps->gen_pmcon_a & HOST_RST_STS)
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);

	/* ACPI Wake Event */
	if (ps->prev_sleep_state != ACPI_S0)
		elog_add_event_byte(ELOG_TYPE_ACPI_WAKE, ps->prev_sleep_state);
}

static void pch_log_state(void *unused)
{
	struct chipset_power_state *ps = pmc_get_power_state();

	if (!ps) {
		printk(BIOS_ERR, "chipset_power_state not found!\n");
		return;
	}

	/* Power and Reset */
	pch_log_power_and_resets(ps);

	/* Wake Sources */
	if (ps->prev_sleep_state > ACPI_S0)
		pch_log_wake_source(ps);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, pch_log_state, NULL);

void elog_gsmi_cb_platform_log_wake_source(void)
{
	struct chipset_power_state ps;
	pmc_fill_pm_reg_info(&ps);
	pch_log_wake_source(&ps);
}
