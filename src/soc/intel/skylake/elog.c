/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <bootstate.h>
#include <commonlib/helpers.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <elog.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/xhci.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/smbus.h>

static void pch_log_gpio_gpe(u32 gpe0_sts, u32 gpe0_en, int start)
{
	int i;

	gpe0_sts &= gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPE, i + start);
	}
}

struct pme_status_info {
	unsigned int devfn;
	uint8_t reg_offset;
	uint32_t elog_event;
};

#define PME_STS_BIT		(1 << 15)

static void pch_log_pme_internal_wake_source(void)
{
	size_t i;
	uint16_t val;
	bool dev_found = false;

	const struct pme_status_info pme_status_info[] = {
		{ PCH_DEVFN_HDA, 0x54, ELOG_WAKE_SOURCE_PME_HDA },
		{ PCH_DEVFN_GBE, 0xcc, ELOG_WAKE_SOURCE_PME_GBE },
		{ PCH_DEVFN_SATA, 0x74, ELOG_WAKE_SOURCE_PME_SATA },
		{ PCH_DEVFN_CSE, 0x54, ELOG_WAKE_SOURCE_PME_CSE },
		{ PCH_DEVFN_USBOTG, 0x84, ELOG_WAKE_SOURCE_PME_XDCI },
	};
	const struct xhci_wake_info xhci_wake_info[] = {
		{ PCH_DEVFN_XHCI, ELOG_WAKE_SOURCE_PME_XHCI },
	};

	for (i = 0; i < ARRAY_SIZE(pme_status_info); i++) {
		pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(pme_status_info[i].devfn),
					  PCI_FUNC(pme_status_info[i].devfn));

		val = pci_s_read_config16(dev, pme_status_info[i].reg_offset);

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

#define RP_PME_STS_BIT		(1 << 16)
static void pch_log_rp_wake_source(void)
{
	size_t i, maxports;
	uint32_t val;

	struct pme_status_info pme_status_info[] = {
		{ PCH_DEVFN_PCIE1, 0x60, ELOG_WAKE_SOURCE_PME_PCIE1 },
		{ PCH_DEVFN_PCIE2, 0x60, ELOG_WAKE_SOURCE_PME_PCIE2 },
		{ PCH_DEVFN_PCIE3, 0x60, ELOG_WAKE_SOURCE_PME_PCIE3 },
		{ PCH_DEVFN_PCIE4, 0x60, ELOG_WAKE_SOURCE_PME_PCIE4 },
		{ PCH_DEVFN_PCIE5, 0x60, ELOG_WAKE_SOURCE_PME_PCIE5 },
		{ PCH_DEVFN_PCIE6, 0x60, ELOG_WAKE_SOURCE_PME_PCIE6 },
		{ PCH_DEVFN_PCIE7, 0x60, ELOG_WAKE_SOURCE_PME_PCIE7 },
		{ PCH_DEVFN_PCIE8, 0x60, ELOG_WAKE_SOURCE_PME_PCIE8 },
		{ PCH_DEVFN_PCIE9, 0x60, ELOG_WAKE_SOURCE_PME_PCIE9 },
		{ PCH_DEVFN_PCIE10, 0x60, ELOG_WAKE_SOURCE_PME_PCIE10 },
		{ PCH_DEVFN_PCIE11, 0x60, ELOG_WAKE_SOURCE_PME_PCIE11 },
		{ PCH_DEVFN_PCIE12, 0x60, ELOG_WAKE_SOURCE_PME_PCIE12 },
		{ PCH_DEVFN_PCIE13, 0x60, ELOG_WAKE_SOURCE_PME_PCIE13 },
		{ PCH_DEVFN_PCIE14, 0x60, ELOG_WAKE_SOURCE_PME_PCIE14 },
		{ PCH_DEVFN_PCIE15, 0x60, ELOG_WAKE_SOURCE_PME_PCIE15 },
		{ PCH_DEVFN_PCIE16, 0x60, ELOG_WAKE_SOURCE_PME_PCIE16 },
		{ PCH_DEVFN_PCIE17, 0x60, ELOG_WAKE_SOURCE_PME_PCIE17 },
		{ PCH_DEVFN_PCIE18, 0x60, ELOG_WAKE_SOURCE_PME_PCIE18 },
		{ PCH_DEVFN_PCIE19, 0x60, ELOG_WAKE_SOURCE_PME_PCIE19 },
		{ PCH_DEVFN_PCIE20, 0x60, ELOG_WAKE_SOURCE_PME_PCIE20 },
		{ PCH_DEVFN_PCIE21, 0x60, ELOG_WAKE_SOURCE_PME_PCIE21 },
		{ PCH_DEVFN_PCIE22, 0x60, ELOG_WAKE_SOURCE_PME_PCIE22 },
		{ PCH_DEVFN_PCIE23, 0x60, ELOG_WAKE_SOURCE_PME_PCIE23 },
		{ PCH_DEVFN_PCIE24, 0x60, ELOG_WAKE_SOURCE_PME_PCIE24 },
	};

	maxports = MIN(CONFIG_MAX_ROOT_PORTS, ARRAY_SIZE(pme_status_info));

	for (i = 0; i < maxports; i++) {
		pci_devfn_t dev = PCI_DEV(0, PCI_SLOT(pme_status_info[i].devfn),
					  PCI_FUNC(pme_status_info[i].devfn));

		val = pci_s_read_config32(dev, pme_status_info[i].reg_offset);

		if ((val == 0xFFFFFFFF) || !(val & RP_PME_STS_BIT))
			continue;

		/*
		 * Linux kernel uses PME STS bit information. So do not clear
		 * this bit.
		 */
		elog_add_event_wake(pme_status_info[i].elog_event, 0);
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

	/*
	 * PCIE Root Port .
	 * This should be done when PCIEXPWAK_STS bit is set.
	 * In SPT, this bit isn't getting set due to known bug.
	 * So scan all PCIe RP for PME status bit.
	 */
	pch_log_rp_wake_source();

	/* PME (TODO: determine wake device) */
	if (ps->gpe0_sts[GPE_STD] & PME_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME, 0);

	/* Internal PME (TODO: determine wake device) */
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
	bool deep_sx;

	/*
	 * Platform entered deep Sx if:
	 * 1. Prev sleep state was Sx and deep_sx_enabled() is true
	 * 2. SUS well power was lost
	 */
	deep_sx = ((((ps->prev_sleep_state == ACPI_S3) && deep_s3_enabled()) ||
		   ((ps->prev_sleep_state == ACPI_S5) && deep_s5_enabled())) &&
		   (ps->gen_pmcon_b & SUS_PWR_FLR));

	/* Thermal Trip */
	if (ps->gblrst_cause[0] & GBLRST_CAUSE0_THERMTRIP)
		elog_add_event(ELOG_TYPE_THERM_TRIP);

	/* PWR_FLR Power Failure */
	if (ps->gen_pmcon_b & PWR_FLR)
		elog_add_event(ELOG_TYPE_POWER_FAIL);

	/* SUS Well Power Failure */
	if (ps->gen_pmcon_b & SUS_PWR_FLR) {
		/* Do not log SUS_PWR_FLR if waking from deep Sx */
		if (!deep_sx)
			elog_add_event(ELOG_TYPE_SUS_POWER_FAIL);
	}

	/* TCO Timeout */
	if (ps->prev_sleep_state != ACPI_S3 &&
	    ps->tco2_sts & TCO_STS_SECOND_TO)
		elog_add_event(ELOG_TYPE_TCO_RESET);

	/* Power Button Override */
	if (ps->pm1_sts & PRBTNOR_STS)
		elog_add_event(ELOG_TYPE_POWER_BUTTON_OVERRIDE);

	/* RTC reset */
	if (ps->gen_pmcon_b & RTC_BATTERY_DEAD)
		elog_add_event(ELOG_TYPE_RTC_RESET);

	/* Host Reset Status */
	if (ps->gen_pmcon_b & HOST_RST_STS)
		elog_add_event(ELOG_TYPE_SYSTEM_RESET);

	/* ACPI Wake Event */
	if (ps->prev_sleep_state != ACPI_S0) {
		if (deep_sx)
			elog_add_event_byte(ELOG_TYPE_ACPI_DEEP_WAKE,
					    ps->prev_sleep_state);
		else
			elog_add_event_byte(ELOG_TYPE_ACPI_WAKE,
					    ps->prev_sleep_state);
	}
}

static void pch_log_state(void *unused)
{
	const struct chipset_power_state *ps;

	if (acpi_fetch_pm_state(&ps, PS_CLAIMER_ELOG) < 0)
		return;

	/* Power and Reset */
	pch_log_power_and_resets(ps);

	/* Wake Sources */
	if (ps->prev_sleep_state > 0)
		pch_log_wake_source(ps);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, pch_log_state, NULL);

void elog_gsmi_cb_platform_log_wake_source(void)
{
	struct chipset_power_state ps;
	pmc_fill_pm_reg_info(&ps);
	pch_log_wake_source(&ps);
}
