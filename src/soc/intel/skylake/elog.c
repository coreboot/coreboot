/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <bootstate.h>
#include <cbmem.h>
#include <console/console.h>
#include <stdint.h>
#include <elog.h>
#include <soc/pm.h>
#include <soc/smbus.h>

static void pch_log_gpio_gpe(u32 gpe0_sts, u32 gpe0_en, int start)
{
	int i;

	gpe0_sts &= gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, i + start);
	}
}

static void pch_log_wake_source(struct chipset_power_state *ps)
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

	/* Internal PME (TODO: determine wake device) */
	if (ps->gpe0_sts[GPE_STD] & PME_B0_STS)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);

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

static void pch_log_power_and_resets(struct chipset_power_state *ps)
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
	    ps->tco2_sts & TCO2_STS_SECOND_TO)
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
	struct chipset_power_state *ps = cbmem_find(CBMEM_ID_POWER_STATE);

	if (ps == NULL) {
		printk(BIOS_ERR,
			"Not logging power state information. Power state not found in cbmem.\n");
		return;
	}

	/* Power and Reset */
	pch_log_power_and_resets(ps);

	/* Wake Sources */
	if (ps->prev_sleep_state > 0)
		pch_log_wake_source(ps);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_INIT, BS_ON_EXIT, pch_log_state, NULL);
