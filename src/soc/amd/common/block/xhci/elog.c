/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <amdblocks/xhci.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <device/pci_def.h>
#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <device/pci_type.h>
#include <device/xhci.h>
#include <elog.h>

#include <inttypes.h>

#define PORTSC_OFFSET 0x400
#define PORTSC_STRIDE 0x10
#define XHCI_PROG_ID 0x30

static void xhci_port_wake_check(uintptr_t base, uint8_t controller, uint8_t num, uint8_t event)
{
	for (uint8_t i = 0; i < num; i++) {
		uint32_t portsc = read32p(base + i * PORTSC_STRIDE);

		/* Encode the controller number and port number. */
		uint32_t payload = controller << 8 | i;

		/* Ensure that we've read a valid value. */
		if (portsc == 0xffffffff)
			continue;

		/* Check for connect/disconnect wake. */
		if (xhci_portsc_csc(portsc) && xhci_portsc_wake_capable(portsc)) {
			elog_add_event_wake(event, payload);
			continue;
		}

		if (xhci_portsc_plc(portsc) && xhci_portsc_resume(portsc))
			elog_add_event_wake(event, payload);
	}
}

struct xhci_context {
	uintptr_t bar;
	uint8_t controller;
};

static void xhci_cap_callback(void *data, const struct xhci_supported_protocol *protocol)
{
	const struct xhci_context *context = (const struct xhci_context *)data;
	uint8_t count = protocol->port_count;
	const struct xhci_capability_regs *cap_regs =
		(const struct xhci_capability_regs *)context->bar;
	uint8_t controller = context->controller;
	/* PORTSC registers start at operational base + 0x400 + 0x10 * (n - 1). */
	uintptr_t op_base = context->bar + cap_regs->caplength;
	uintptr_t addr = op_base + PORTSC_OFFSET + PORTSC_STRIDE * (protocol->port_offset - 1);

	switch (protocol->major_rev) {
	case 2:
		xhci_port_wake_check(addr, controller, count, ELOG_WAKE_SOURCE_PME_XHCI_USB_2);
		break;

	case 3:
		xhci_port_wake_check(addr, controller, count, ELOG_WAKE_SOURCE_PME_XHCI_USB_3);
		break;

	default:
		printk(BIOS_WARNING, "Skipping logging XHCI events for controller %u, unsupported protocol",
		       controller);
		break;
	}
}

void soc_xhci_log_wake_events(void)
{
	const volatile struct smm_pci_resource_info *res_store;
	size_t res_count;
	uint8_t i_xhci = 0;

	smm_pci_get_stored_resources(&res_store, &res_count);
	for (size_t i_slot = 0; i_slot < res_count; i_slot++) {
		/* Skip any non-XHCI controller devices. */
		if (res_store[i_slot].class_device != PCI_CLASS_SERIAL_USB ||
		    res_store[i_slot].class_prog != XHCI_PROG_ID) {
			continue;
		}

		/* Validate our BAR. */
		uintptr_t stored_bar = res_store[i_slot].resources[0].base;
		uintptr_t bar = pci_s_read_config32(res_store[i_slot].pci_addr,
						    PCI_BASE_ADDRESS_0);
		bar &= ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;

		if (!stored_bar || !bar || bar != stored_bar) {
			printk(BIOS_WARNING, "Skipping logging XHCI events for controller %u, resource error, stored %" PRIxPTR ", found %" PRIxPTR "\n",
			       i_xhci, stored_bar, bar);
			i_xhci++;
			continue;
		}

		struct xhci_context context = {
			.bar = bar,
			.controller = i_xhci,
		};

		const struct resource *res = (const struct resource *)(&res_store[i_slot].resources[0]);
		enum cb_err err
			= xhci_resource_for_each_supported_usb_cap(res, &context,
								   &xhci_cap_callback);
		if (err)
			printk(BIOS_ERR, "Failed to iterate over capabilities for XHCI controller %u (%d)\n",
			       i_xhci, err);

		i_xhci++;
	}
}
