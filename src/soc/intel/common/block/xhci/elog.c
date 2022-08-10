/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/xhci.h>
#include <elog.h>
#include <intelblocks/xhci.h>
#include <soc/pci_devs.h>
#include <stdint.h>

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
static bool xhci_port_wake_check(uintptr_t base, uint8_t num, uint8_t host_event, uint8_t event)
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
		if (xhci_portsc_csc(port_status) &&
		    xhci_portsc_wake_capable(port_status)) {
			elog_add_event_wake(host_event, 0);
			elog_add_event_wake(event, i + 1);
			found = true;
			continue;
		}

		/*
		 * Check if PLC is set and PLS indicates resume to identify if
		 * the port caused wake event for USB activity.
		 */
		if (xhci_portsc_plc(port_status) &&
		    xhci_portsc_resume(port_status)) {
			elog_add_event_wake(host_event, 0);
			elog_add_event_wake(event, i + 1);
			found = true;
		}
	}
	return found;
}

bool xhci_update_wake_event(const struct xhci_wake_info *wake_info,
			    size_t wake_info_count)
{
	const struct xhci_usb_info *usb_info;
	uintptr_t mmio_base;
	bool event_found = false;
	size_t i;

	for (i = 0; i < wake_info_count; ++i) {
		/* Assumes BAR0 is MBAR */
		pci_devfn_t devfn = PCI_DEV(0, PCI_SLOT(wake_info[i].xhci_dev),
					    PCI_FUNC(wake_info[i].xhci_dev));
		mmio_base = pci_s_read_config32(devfn, PCI_BASE_ADDRESS_0);
		mmio_base &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
		usb_info = soc_get_xhci_usb_info(wake_info[i].xhci_dev);

		/* Check USB2 port status & control registers */
		if (xhci_port_wake_check(mmio_base + usb_info->usb2_port_status_reg,
					 usb_info->num_usb2_ports,
					 wake_info[i].elog_wake_type_host,
					 ELOG_WAKE_SOURCE_PME_XHCI_USB_2))
			event_found = true;

		/* Check USB3 port status & control registers */
		if (xhci_port_wake_check(mmio_base + usb_info->usb3_port_status_reg,
					 usb_info->num_usb3_ports,
					 wake_info[i].elog_wake_type_host,
					 ELOG_WAKE_SOURCE_PME_XHCI_USB_3))
			event_found = true;
	}

	return event_found;
}
