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
#include <device/pci_ops.h>
#include <stdint.h>
#include <elog.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/smbus.h>
#include <stdint.h>

static void pch_log_gpio_gpe(u32 gpe0_sts, u32 gpe0_en, int start)
{
	int i;

	gpe0_sts &= gpe0_en;

	for (i = 0; i <= 31; i++) {
		if (gpe0_sts & (1 << i))
			elog_add_event_wake(ELOG_WAKE_SOURCE_GPIO, i + start);
	}
}

#define XHCI_USB2_PORT_STATUS_REG	0x480
#define XHCI_USB3_PORT_STATUS_REG	0x540
#define XHCI_USB2_PORT_NUM		10
#define XHCI_USB3_PORT_NUM		6
/* Wake on disconnect enable */
#define XHCI_STATUS_WDE			(1 << 26)
/* Wake on connect enable */
#define XHCI_STATUS_WCE			(1 << 25)
/* Port link status change */
#define XHCI_STATUS_PLC			(1 << 22)
/* Connect status change */
#define XHCI_STATUS_CSC			(1 << 17)
/* Port link status */
#define XHCI_STATUS_PLS_SHIFT		(5)
#define XHCI_STATUS_PLS_MASK		(0xF << XHCI_STATUS_PLS_SHIFT)
#define XHCI_STATUS_PLS_RESUME		(15 << XHCI_STATUS_PLS_SHIFT)

static bool pch_xhci_csc_set(uint32_t port_status)
{
	return !!(port_status & XHCI_STATUS_CSC);
}

static bool pch_xhci_wake_capable(uint32_t port_status)
{
	return !!((port_status & XHCI_STATUS_WCE) |
		  (port_status & XHCI_STATUS_WDE));
}

static bool pch_xhci_plc_set(uint32_t port_status)
{
	return !!(port_status & XHCI_STATUS_PLC);
}

static bool pch_xhci_resume(uint32_t port_status)
{
	return (port_status & XHCI_STATUS_PLS_MASK) == XHCI_STATUS_PLS_RESUME;
}

/*
 * Check if a particular USB port caused wake by:
 * 1. Change in connect/disconnect status (if enabled)
 * 2. USB device activity
 *
 * Params:
 * base  : MMIO address of first port.
 * num   : Number of ports.
 * event : Event that needs to be added in case wake source is found.
 *
 * Return value:
 * true  : Wake source was found.
 * false : Wake source was not found.
 */
static bool pch_xhci_port_wake_check(uintptr_t base, uint8_t num,
					uint32_t event)
{
	uint8_t i;
	uint32_t port_status;
	bool found = false;

	for (i = 0; i < num; i++, base += 0x10) {
		/* Read port status and control register for the port. */
		port_status = read32((void *)base);

		/*
		 * Check if CSC bit is set and port is capable of wake on
		 * connect/disconnect to identify if the port caused wake
		 * event for usb attach/detach.
		 */
		if (pch_xhci_csc_set(port_status) &&
		    pch_xhci_wake_capable(port_status)) {
			elog_add_event_wake(event, i + 1);
			found = true;
		}

		/*
		 * Check if PLC is set and PLS indicates resume to identify if
		 * the port caused wake event for usb activity.
		 */
		if (pch_xhci_plc_set(port_status) &&
		    pch_xhci_resume(port_status)) {
			elog_add_event_wake(event, i + 1);
			found = true;
		}
	}
	return found;
}

/*
 * Update elog event and instance depending upon the USB2 port that caused
 * the wake event.
 *
 * Return value:
 * true = Indicates that USB2 wake event was found.
 * false = Indicates that USB2 wake event was not found.
 */
static inline bool pch_xhci_usb2_update_wake_event(uintptr_t mmio_base)
{
	return pch_xhci_port_wake_check(mmio_base + XHCI_USB2_PORT_STATUS_REG,
					XHCI_USB2_PORT_NUM,
					ELOG_WAKE_SOURCE_PME_XHCI_USB_2);
}

/*
 * Update elog event and instance depending upon the USB3 port that caused
 * the wake event.
 *
 * Return value:
 * true = Indicates that USB3 wake event was found.
 * false = Indicates that USB3 wake event was not found.
 */
static inline bool pch_xhci_usb3_update_wake_event(uintptr_t mmio_base)
{
	return pch_xhci_port_wake_check(mmio_base + XHCI_USB3_PORT_STATUS_REG,
					XHCI_USB3_PORT_NUM,
					ELOG_WAKE_SOURCE_PME_XHCI_USB_3);
}

static bool pch_xhci_update_wake_event(device_t dev)
{
	uintptr_t mmio_base;
	bool event_found = false;
	mmio_base = ALIGN_DOWN(pci_read_config32(dev, PCI_BASE_ADDRESS_0), 16);

	if (pch_xhci_usb2_update_wake_event(mmio_base))
		event_found = true;

	if (pch_xhci_usb3_update_wake_event(mmio_base))
		event_found = true;

	return event_found;
}

struct pme_status_info {
	int devfn;
	uint8_t reg_offset;
	uint32_t elog_event;
};

#define PME_STS_BIT		(1 << 15)

static void pch_log_add_elog_event(const struct pme_status_info *info,
					device_t dev)
{
	/*
	 * If wake source is XHCI, check for detailed wake source events on
	 * USB2/3 ports.
	 */
	if ((info->devfn == PCH_DEVFN_XHCI) && pch_xhci_update_wake_event(dev))
		return;

	elog_add_event_wake(info->elog_event, 0);
}

static void pch_log_pme_internal_wake_source(void)
{
	size_t i;
	device_t dev;
	uint16_t val;
	bool dev_found = false;

	static const struct pme_status_info pme_status_info[] = {
		{ PCH_DEVFN_HDA, 0x54, ELOG_WAKE_SOURCE_PME_HDA },
		{ PCH_DEVFN_GBE, 0xcc, ELOG_WAKE_SOURCE_PME_GBE },
		{ PCH_DEVFN_EMMC, 0x84, ELOG_WAKE_SOURCE_PME_EMMC },
		{ PCH_DEVFN_SDCARD, 0x84, ELOG_WAKE_SOURCE_PME_SDCARD },
		{ PCH_DEVFN_PCIE1, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE1 },
		{ PCH_DEVFN_PCIE2, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE2 },
		{ PCH_DEVFN_PCIE3, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE3 },
		{ PCH_DEVFN_PCIE4, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE4 },
		{ PCH_DEVFN_PCIE5, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE5 },
		{ PCH_DEVFN_PCIE6, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE6 },
		{ PCH_DEVFN_PCIE7, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE7 },
		{ PCH_DEVFN_PCIE8, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE8 },
		{ PCH_DEVFN_PCIE9, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE9 },
		{ PCH_DEVFN_PCIE10, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE10 },
		{ PCH_DEVFN_PCIE11, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE11 },
		{ PCH_DEVFN_PCIE12, 0xa4, ELOG_WAKE_SOURCE_PME_PCIE12 },
		{ PCH_DEVFN_SATA, 0x74, ELOG_WAKE_SOURCE_PME_SATA },
		{ PCH_DEVFN_CSE, 0x54, ELOG_WAKE_SOURCE_PME_CSE },
		{ PCH_DEVFN_CSE_2, 0x54, ELOG_WAKE_SOURCE_PME_CSE2 },
		{ PCH_DEVFN_CSE_3, 0x54, ELOG_WAKE_SOURCE_PME_CSE3 },
		{ PCH_DEVFN_XHCI, 0x74, ELOG_WAKE_SOURCE_PME_XHCI },
		{ PCH_DEVFN_USBOTG, 0x84, ELOG_WAKE_SOURCE_PME_XDCI },
	};

	for (i = 0; i < ARRAY_SIZE(pme_status_info); i++) {
		dev = dev_find_slot(0, pme_status_info[i].devfn);

		if (!dev)
			continue;

		val = pci_read_config16(dev, pme_status_info[i].reg_offset);

		if ((val == 0xFFFF) || !(val & PME_STS_BIT))
			continue;

		pch_log_add_elog_event(&pme_status_info[i], dev);
		dev_found = true;
	}

	if (!dev_found)
		elog_add_event_wake(ELOG_WAKE_SOURCE_PME_INTERNAL, 0);
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
