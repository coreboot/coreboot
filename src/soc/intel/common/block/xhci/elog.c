/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <elog.h>
#include <intelblocks/xhci.h>
#include <soc/pci_devs.h>
#include <stdint.h>

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
static bool pch_xhci_port_wake_check(uintptr_t base, uint8_t num, uint8_t event)
{
	uint32_t i, port_status;
	bool found = false;

	for (i = 0; i < num; i++, base += 0x10) {
		/* Read port status and control register for the port. */
		port_status = read32((void *)base);

		/* Ensure that the status is not all 1s. */
		if (port_status == 0xffffffff)
			continue;

		/*
		 * Check if CSC bit is set and port is capable of wake on
		 * connect/disconnect to identify if the port caused wake
		 * event for USB attach/detach.
		 */
		if (pch_xhci_csc_set(port_status) &&
		    pch_xhci_wake_capable(port_status)) {
			elog_add_event_wake(event, i + 1);
			found = true;
			continue;
		}

		/*
		 * Check if PLC is set and PLS indicates resume to identify if
		 * the port caused wake event for USB activity.
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
static inline bool pch_xhci_usb2_update_wake_event(uintptr_t mmio_base,
					const struct xhci_usb_info *info)
{
	return pch_xhci_port_wake_check(mmio_base + info->usb2_port_status_reg,
					info->num_usb2_ports,
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
static inline bool pch_xhci_usb3_update_wake_event(uintptr_t mmio_base,
					const struct xhci_usb_info *info)
{
	return pch_xhci_port_wake_check(mmio_base + info->usb3_port_status_reg,
					info->num_usb3_ports,
					ELOG_WAKE_SOURCE_PME_XHCI_USB_3);
}

bool pch_xhci_update_wake_event(const struct xhci_usb_info *info)
{
	uintptr_t mmio_base;
	bool event_found = false;
	mmio_base = ALIGN_DOWN(pci_read_config32(PCH_DEV_XHCI,
					PCI_BASE_ADDRESS_0), 16);

	if (pch_xhci_usb2_update_wake_event(mmio_base, info))
		event_found = true;

	if (pch_xhci_usb3_update_wake_event(mmio_base, info))
		event_found = true;

	return event_found;
}
