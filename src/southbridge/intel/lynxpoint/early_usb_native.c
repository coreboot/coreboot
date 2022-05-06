/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>
#include <device/mmio.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/iobp.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <timer.h>
#include <types.h>

static unsigned int is_usbr_enabled(void)
{
	return !!(pci_read_config32(PCH_XHCI_DEV, XHCI_USB3FUS) & BIT(5));
}

static char *const xhci_bar = (char *)PCH_XHCI_TEMP_BAR0;

static void ehci_hcs_init(const pci_devfn_t dev, const uintptr_t ehci_bar)
{
	pci_write_config32(dev, PCI_BASE_ADDRESS_0, ehci_bar);

	/** FIXME: Determine whether Bus Master is required (or clean it up afterwards) **/
	pci_or_config16(dev, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	char *const mem_bar = (char *)ehci_bar;

	/**
	 * Shared EHCI/XHCI ports w/a.
	 * This step is required when some of the ports are routed to EHCI
	 * and other ports are routed XHCI at the same time.
	 *
	 * FIXME: Under which conditions should this be done?
	 */
	pci_and_config16(dev, 0x78, ~0x03);

	/* Skip reset if usbdebug is enabled */
	if (!CONFIG(USBDEBUG_IN_PRE_RAM))
		setbits32(mem_bar + EHCI_USB_CMD, EHCI_USB_CMD_HCRESET);

	/* 2: Configure number of controllers and ports */
	pci_or_config16(dev, EHCI_ACCESS_CNTL, ACCESS_CNTL_ENABLE);
	clrsetbits32(mem_bar + EHCI_HCS_PARAMS, 0xf << 12, 0);
	clrsetbits32(mem_bar + EHCI_HCS_PARAMS, 0xf <<  0, 2 + is_usbr_enabled());
	pci_and_config16(dev, EHCI_ACCESS_CNTL, ~ACCESS_CNTL_ENABLE);

	pci_or_config16(dev, 0x78, BIT(2));
	pci_or_config16(dev, 0x7c, BIT(14) | BIT(7));
	pci_update_config32(dev, 0x8c, ~(0xf << 8), (4 << 8));
	pci_update_config32(dev, 0x8c, ~BIT(26), BIT(17));
}

static inline unsigned int physical_port_count(void)
{
	return MAX_USB2_PORTS;
}

static unsigned int hs_port_count(void)
{
	/** TODO: Apparently, WPT-LP has 10 USB2 ports **/
	if (CONFIG(INTEL_LYNXPOINT_LP))
		return 8;

	switch ((pci_read_config32(PCH_XHCI_DEV, XHCI_USB3FUS) >> 1) & 3) {
	case 3:
		return 8;
	case 2:
		return 10;
	case 1:
		return 12;
	case 0:
	default:
		return 14;
	}
}

static unsigned int ss_port_count(void)
{
	if (CONFIG(INTEL_LYNXPOINT_LP))
		return 4;

	switch ((pci_read_config32(PCH_XHCI_DEV, XHCI_USB3FUS) >> 3) & 3) {
	case 3:
		return 0;
	case 2:
		return 2;
	case 1:
		return 4;
	case 0:
	default:
		return 6;
	}
}

static void common_ehci_hcs_init(void)
{
	const bool is_lp = CONFIG(INTEL_LYNXPOINT_LP);

	ehci_hcs_init(PCH_EHCI1_DEV, PCH_EHCI1_TEMP_BAR0);
	if (!is_lp)
		ehci_hcs_init(PCH_EHCI2_DEV, PCH_EHCI2_TEMP_BAR0);

	pch_iobp_update(0xe5007f04, 0, 0x00004481);

	for (unsigned int port = 0; port < physical_port_count(); port++)
		pch_iobp_update(0xe500400f + port * 0x100, ~(1 << 0), 0 << 0);

	pch_iobp_update(0xe5007f14, ~(3 << 19), (3 << 19));

	if (is_lp)
		pch_iobp_update(0xe5007f02, ~(3 << 22), (0 << 22));
}

static void xhci_open_memory_space(void)
{
	/** FIXME: Determine whether Bus Master is required (or clean it up afterwards) **/
	pci_write_config32(PCH_XHCI_DEV, PCI_BASE_ADDRESS_0, (uintptr_t)xhci_bar);
	pci_or_config16(PCH_XHCI_DEV, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

static void xhci_close_memory_space(void)
{
	pci_and_config16(PCH_XHCI_DEV, PCI_COMMAND, ~(PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY));
	pci_write_config32(PCH_XHCI_DEV, PCI_BASE_ADDRESS_0, 0);
}

static void common_xhci_hc_init(void)
{
	const bool is_lp = CONFIG(INTEL_LYNXPOINT_LP);

	if (!is_lp) {
		const unsigned int max_ports = 15 + ss_port_count();
		clrsetbits32(xhci_bar + XHCI_HCS_PARAMS_1, 0xf << 28, max_ports << 28);
	}

	clrsetbits32(xhci_bar + XHCI_HCS_PARAMS_3, 0xffff << 16 | 0xff, 0x200 << 16 | 0x0a);
	clrsetbits32(xhci_bar + XHCI_HCC_PARAMS, BIT(5), BIT(10) | BIT(9));

	if (!is_lp)
		clrsetbits32(xhci_bar + 0x8008, BIT(19), 0);

	if (is_lp)
		clrsetbits32(xhci_bar + 0x8058, BIT(8), BIT(16));
	else
		clrsetbits32(xhci_bar + 0x8058, BIT(8), BIT(16) | BIT(20));

	clrsetbits32(xhci_bar + 0x8060, 0, BIT(25) | BIT(18));
	clrsetbits32(xhci_bar + 0x8090, 0, BIT(14) | BIT(8));
	clrsetbits32(xhci_bar + 0x8094, 0, BIT(23) | BIT(21) | BIT(14));
	clrsetbits32(xhci_bar + 0x80e0, BIT(16), BIT(6));
	clrsetbits32(xhci_bar + 0x80ec, (7 << 12) | (7 << 9), (0 << 12) | (6 << 9));
	clrsetbits32(xhci_bar + 0x80f0, BIT(20), 0);

	if (is_lp)
		clrsetbits32(xhci_bar + 0x80fc, 0, BIT(25));

	if (is_lp)
		clrsetbits32(xhci_bar + 0x8110, BIT(8) | BIT(2), BIT(20) | BIT(11));
	else
		clrsetbits32(xhci_bar + 0x8110, BIT(2), BIT(20) | BIT(11));

	if (is_lp)
		write32(xhci_bar + 0x8140, 0xff00f03c);
	else
		write32(xhci_bar + 0x8140, 0xff03c132);

	if (is_lp)
		clrsetbits32(xhci_bar + 0x8154, BIT(21), BIT(13));
	else
		clrsetbits32(xhci_bar + 0x8154, BIT(21) | BIT(13), 0);

	clrsetbits32(xhci_bar + 0x8154, BIT(3), 0);

	if (is_lp) {
		clrsetbits32(xhci_bar + 0x8164, 0, BIT(1) | BIT(0));
		write32(xhci_bar + 0x8174, 0x01400c0a);
		write32(xhci_bar + 0x817c, 0x033200a3);
		write32(xhci_bar + 0x8180, 0x00cb0028);
		write32(xhci_bar + 0x8184, 0x0064001e);
	}

	/*
	 * Note: Register at offset 0x44 is 32-bit, but bit 31 is write-once.
	 * We use these weird partial accesses here to avoid locking bit 31.
	 */
	pci_or_config16(PCH_XHCI_DEV, 0x44, BIT(15) | BIT(14) | BIT(10) | BIT(0));
	pci_or_config8(PCH_XHCI_DEV, 0x44 + 2, 0x0f);

	/* LPT-LP >= B0 */
	if (is_lp)
		clrsetbits32(xhci_bar + 0x8188, 0, BIT(26) | BIT(24));

	/* LPT-H >= C0 */
	if (!is_lp)
		clrsetbits32(xhci_bar + 0x8188, 0, BIT(24));
}

static inline bool is_mem_sr(void)
{
	return pci_read_config16(PCH_LPC_DEV, GEN_PMCON_2) & GEN_PMCON_2_MEM_SR;
}

static bool should_restore_xhci_smart_auto(void)
{
	if (!is_mem_sr())
		return false;

	return pci_read_config32(PCH_LPC_DEV, PMIR) & PMIR_XHCI_SMART_AUTO;
}

enum usb_port_route {
	ROUTE_TO_EHCI,
	ROUTE_TO_XHCI,
};

/* Returns whether port reset was successful */
static bool reset_usb2_ports(const unsigned int ehci_ports)
{
	for (unsigned int port = 0; port < ehci_ports; port++) {
		/* Initiate port reset for all USB2 ports */
		clrsetbits32(
			xhci_bar + XHCI_USB2_PORTSC(port),
			XHCI_USB2_PORTSC_PED,
			XHCI_USB2_PORTSC_PR);
	}
	/* Poll for port reset bit to be cleared or time out at 100ms */
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 100);
	uint32_t reg32;
	do {
		reg32 = 0;
		for (unsigned int port = 0; port < ehci_ports; port++)
			reg32 |= read32(xhci_bar + XHCI_USB2_PORTSC(port));

		reg32 &= XHCI_USB2_PORTSC_PR;
		if (!reg32) {
			const long elapsed_time = stopwatch_duration_usecs(&timer);
			printk(BIOS_DEBUG, "%s: took %lu usecs\n", __func__, elapsed_time);
			return true;
		}
		/* Reference code has a 10 ms delay here, but a smaller delay works too */
		udelay(100);
	} while (!stopwatch_expired(&timer));
	printk(BIOS_ERR, "%s: timed out\n", __func__);
	return !reg32;
}

/* Returns whether warm reset was successful */
static bool warm_reset_usb3_ports(const unsigned int xhci_ports)
{
	for (unsigned int port = 0; port < xhci_ports; port++) {
		/* Initiate warm reset for all USB3 ports */
		clrsetbits32(
			xhci_bar + XHCI_USB3_PORTSC(port),
			XHCI_USB3_PORTSC_PED,
			XHCI_USB3_PORTSC_WPR);
	}
	/* Poll for port reset bit to be cleared or time out at 100ms */
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 100);
	uint32_t reg32;
	do {
		reg32 = 0;
		for (unsigned int port = 0; port < xhci_ports; port++)
			reg32 |= read32(xhci_bar + XHCI_USB3_PORTSC(port));

		reg32 &= XHCI_USB3_PORTSC_PR;
		if (!reg32) {
			const long elapsed_time = stopwatch_duration_usecs(&timer);
			printk(BIOS_DEBUG, "%s: took %lu usecs\n", __func__, elapsed_time);
			return true;
		}
		/* Reference code has a 10 ms delay here, but a smaller delay works too */
		udelay(100);
	} while (!stopwatch_expired(&timer));
	printk(BIOS_ERR, "%s: timed out\n", __func__);
	return !reg32;
}

static void perform_xhci_ehci_switching_flow(const enum usb_port_route usb_route)
{
	const pci_devfn_t dev = PCH_XHCI_DEV;

	const unsigned int ehci_ports = hs_port_count() + is_usbr_enabled();
	const unsigned int xhci_ports = ss_port_count();

	const uint32_t ehci_mask = BIT(ehci_ports) - 1;
	const uint32_t xhci_mask = BIT(xhci_ports) - 1;

	/** TODO: Handle USBr port? How, though? **/
	pci_update_config32(dev, XHCI_USB2PRM, ~XHCI_USB2PR_HCSEL, ehci_mask);
	pci_update_config32(dev, XHCI_USB3PRM, ~XHCI_USB3PR_SSEN,  xhci_mask);

	/*
	 * Workaround for USB2PR / USB3PR value not surviving warm reset.
	 * Restore USB Port Routing registers if OS HC Switch driver has been executed.
	 */
	if (should_restore_xhci_smart_auto()) {
		/** FIXME: Derive values from mainboard code instead? **/
		pci_update_config32(dev, XHCI_USB2PR, ~XHCI_USB2PR_HCSEL, ehci_mask);
		pci_update_config32(dev, XHCI_USB3PR, ~XHCI_USB3PR_SSEN,  xhci_mask);
	}

	/* Later stages shouldn't need the value of this bit */
	pci_and_config32(PCH_LPC_DEV, PMIR, ~PMIR_XHCI_SMART_AUTO);

	/**
	 * FIXME: Things here depend on the chosen routing mode.
	 *        For now, implement both functions.
	 */

	/* Route to EHCI if xHCI disabled or auto mode */
	if (usb_route == ROUTE_TO_EHCI) {
		if (!reset_usb2_ports(ehci_ports))
			printk(BIOS_ERR, "USB2 port reset timed out\n");

		pci_and_config32(dev, XHCI_USB2PR, ~XHCI_USB2PR_HCSEL);

		for (unsigned int port = 0; port < ehci_ports; port++) {
			clrsetbits32(
				xhci_bar + XHCI_USB2_PORTSC(port),
				XHCI_USB2_PORTSC_PED,
				XHCI_USB2_PORTSC_CHST);
		}

		if (!warm_reset_usb3_ports(xhci_ports))
			printk(BIOS_ERR, "USB3 warm reset timed out\n");

		/* FIXME: BWG says this should be inside the warm reset function */
		pci_and_config32(dev, XHCI_USB3PR, ~XHCI_USB3PR_SSEN);

		for (unsigned int port = 0; port < ehci_ports; port++) {
			clrsetbits32(
				xhci_bar + XHCI_USB3_PORTSC(port),
				XHCI_USB3_PORTSC_PED,
				XHCI_USB3_PORTSC_CHST);
		}

		setbits32(xhci_bar + XHCI_USBCMD, BIT(0));
		clrbits32(xhci_bar + XHCI_USBCMD, BIT(0));
	}

	/* Route to xHCI if xHCI enabled */
	if (usb_route == ROUTE_TO_XHCI) {
		if (is_mem_sr()) {
			if (!warm_reset_usb3_ports(xhci_ports))
				printk(BIOS_ERR, "USB3 warm reset timed out\n");
		}

		const uint32_t xhci_port_mask = pci_read_config32(dev, XHCI_USB3PRM) & 0x3f;
		pci_update_config32(dev, XHCI_USB3PR, ~XHCI_USB3PR_SSEN, xhci_port_mask);

		const uint32_t ehci_port_mask = pci_read_config32(dev, XHCI_USB2PRM) & 0x7fff;
		pci_update_config32(dev, XHCI_USB2PR, ~XHCI_USB2PR_HCSEL, ehci_port_mask);
	}
}

/* Do not shift in this macro, as it can cause undefined behaviour for bad port/oc values */
#define PORT_TO_OC_SHIFT(port, oc)	((oc) * 8 + (port))

/* Avoid shifting into undefined behaviour */
static inline bool shift_ok(const int shift)
{
	return shift >= 0 && shift < 32;
}

static void usb_overcurrent_mapping(void)
{
	const bool is_lp = CONFIG(INTEL_LYNXPOINT_LP);

	uint32_t ehci_1_ocmap = 0;
	uint32_t ehci_2_ocmap = 0;
	uint32_t xhci_1_ocmap = 0;
	uint32_t xhci_2_ocmap = 0;

	/*
	 * EHCI
	 */
	for (unsigned int idx = 0; idx < physical_port_count(); idx++) {
		const struct usb2_port_config *const port = &mainboard_usb2_ports[idx];
		printk(BIOS_DEBUG, "USB2 port %u => ", idx);
		if (!port->enable) {
			printk(BIOS_DEBUG, "disabled\n");
			continue;
		}
		const unsigned short oc_pin = port->oc_pin;
		if (oc_pin == USB_OC_PIN_SKIP) {
			printk(BIOS_DEBUG, "not mapped to OC pin\n");
			continue;
		}
		/* Ports 0 .. 7 => OC 0 .. 3 */
		if (idx < 8 && oc_pin <= 3) {
			const int shift = PORT_TO_OC_SHIFT(idx, oc_pin);
			if (shift_ok(shift)) {
				printk(BIOS_DEBUG, "mapped to OC pin %u\n", oc_pin);
				ehci_1_ocmap |= 1 << shift;
				continue;
			}
		}
		/* Ports 8 .. 13 => OC 4 .. 7 (LPT-H only) */
		if (!is_lp && idx >= 8 && oc_pin >= 4) {
			const int shift = PORT_TO_OC_SHIFT(idx, oc_pin - 4);
			if (shift_ok(shift)) {
				printk(BIOS_DEBUG, "mapped to OC pin %u\n", oc_pin);
				ehci_2_ocmap |= 1 << shift;
				continue;
			}
		}
		printk(BIOS_ERR, "Invalid OC pin %u for USB2 port %u\n", oc_pin, idx);
	}
	printk(BIOS_DEBUG, "\n");
	pci_write_config32(PCH_EHCI1_DEV, EHCI_OCMAP, ehci_1_ocmap);
	if (!is_lp)
		pci_write_config32(PCH_EHCI2_DEV, EHCI_OCMAP, ehci_2_ocmap);

	/*
	 * xHCI
	 */
	for (unsigned int idx = 0; idx < ss_port_count(); idx++) {
		const struct usb3_port_config *const port = &mainboard_usb3_ports[idx];
		printk(BIOS_DEBUG, "USB3 port %u => ", idx);
		if (!port->enable) {
			printk(BIOS_DEBUG, "disabled\n");
			continue;
		}
		const unsigned short oc_pin = port->oc_pin;
		if (oc_pin == USB_OC_PIN_SKIP) {
			printk(BIOS_DEBUG, "not mapped to OC pin\n");
			continue;
		}
		/* Ports 0 .. 5 => OC 0 .. 3 */
		if (oc_pin <= 3) {
			const int shift = PORT_TO_OC_SHIFT(idx, oc_pin);
			if (shift_ok(shift)) {
				printk(BIOS_DEBUG, "mapped to OC pin %u\n", oc_pin);
				xhci_1_ocmap |= 1 << shift;
				continue;
			}
		}
		/* Ports 0 .. 5 => OC 4 .. 7 (LPT-H only) */
		if (!is_lp && oc_pin >= 4) {
			const int shift = PORT_TO_OC_SHIFT(idx, oc_pin - 4);
			if (shift_ok(shift)) {
				printk(BIOS_DEBUG, "mapped to OC pin %u\n", oc_pin);
				xhci_2_ocmap |= 1 << shift;
				continue;
			}
		}
		printk(BIOS_ERR, "Invalid OC pin %u for USB3 port %u\n", oc_pin, idx);
	}
	printk(BIOS_DEBUG, "\n");
	pci_write_config32(PCH_XHCI_DEV, XHCI_U2OCM1, ehci_1_ocmap);
	pci_write_config32(PCH_XHCI_DEV, XHCI_U3OCM1, xhci_1_ocmap);
	if (!is_lp) {
		pci_write_config32(PCH_XHCI_DEV, XHCI_U2OCM2, ehci_2_ocmap);
		pci_write_config32(PCH_XHCI_DEV, XHCI_U3OCM2, xhci_2_ocmap);
	}
}

static uint8_t get_ehci_tune_param_1(const struct usb2_port_config *const port)
{
	const bool is_lp = CONFIG(INTEL_LYNXPOINT_LP);

	const enum pch_platform_type plat_type = get_pch_platform_type();
	const enum usb2_port_location location = port->location;
	const uint16_t length = port->length;
	if (!is_lp) {
		if (plat_type == PCH_TYPE_DESKTOP) {
			if (location == USB_PORT_BACK_PANEL)
				return 4; /* Back Panel */
			else
				return 3; /* Front Panel */

		} else if (plat_type == PCH_TYPE_MOBILE) {
			if (location == USB_PORT_INTERNAL)
				return 5; /* Internal Topology */
			else if (location == USB_PORT_DOCK)
				return 4; /* Dock */
			else if (length < 0x70)
				return 5; /* Back Panel, less than 7" */
			else
				return 6; /* Back Panel, 7" or more */
		}
	} else {
		if (location == USB_PORT_BACK_PANEL || location == USB_PORT_MINI_PCIE) {
			if (length < 0x70)
				return 5; /* Back Panel, less than 7" */
			else
				return 6; /* Back Panel, 7" or more */
		} else if (location == USB_PORT_DOCK) {
			return 4; /* Dock */
		} else {
			return 5; /* Internal Topology */
		}
	}
	printk(BIOS_ERR, "%s: Unhandled case\n", __func__);
	return 0;
}

static uint8_t get_ehci_tune_param_2(const struct usb2_port_config *const port)
{
	const bool is_lp = CONFIG(INTEL_LYNXPOINT_LP);

	const enum pch_platform_type plat_type = get_pch_platform_type();
	const enum usb2_port_location location = port->location;
	const uint16_t length = port->length;
	if (!is_lp) {
		if (plat_type == PCH_TYPE_DESKTOP) {
			if (location == USB_PORT_BACK_PANEL) {
				if (length < 0x80)
					return 2; /* Back Panel, less than 8" */
				else if (length < 0x130)
					return 3; /* Back Panel, 8"-13" */
				else
					return 4; /* Back Panel, 13" or more */
			} else {
				return 2; /* Front Panel */
			}

		} else if (plat_type == PCH_TYPE_MOBILE) {
			if (location == USB_PORT_INTERNAL) {
				return 2; /* Internal Topology */
			} else if (location == USB_PORT_DOCK) {
				if (length < 0x50)
					return 1; /* Dock, less than 5" */
				else
					return 2; /* Dock, 5" or more */
			} else {
				if (length < 0x100)
					return 2; /* Back Panel, less than 10" */
				else
					return 3; /* Back Panel, 10" or more */
			}
		}
	} else {
		if (location == USB_PORT_BACK_PANEL || location == USB_PORT_MINI_PCIE) {
			if (length < 0x100)
				return 2; /* Back Panel, less than 10" */
			else
				return 3; /* Back Panel, 10" or more */
		} else if (location == USB_PORT_DOCK) {
			if (length < 0x50)
				return 1; /* Dock, less than 5" */
			else
				return 2; /* Dock, 5" or more */
		} else {
			return 2; /* Internal Topology */
		}
	}
	printk(BIOS_ERR, "%s: Unhandled case\n", __func__);
	return 0;
}

static void program_ehci_port_length(void)
{
	for (unsigned int port = 0; port < physical_port_count(); port++) {
		if (!mainboard_usb2_ports[port].enable)
			continue;
		const uint32_t addr = 0xe5004000 + (port + 1) * 0x100;
		const uint8_t param_1 = get_ehci_tune_param_1(&mainboard_usb2_ports[port]);
		const uint8_t param_2 = get_ehci_tune_param_2(&mainboard_usb2_ports[port]);
		pch_iobp_update(addr, ~0x7f00, param_2 << 11 | param_1 << 8);
	}
}

void early_usb_init(void)
{
	/** TODO: Make this configurable? How do the modes affect usbdebug? **/
	const enum usb_port_route usb_route = ROUTE_TO_XHCI;
	///(pd->boot_mode == 2 && pd->usb_xhci_on_resume) ? ROUTE_TO_XHCI : ROUTE_TO_EHCI;

	common_ehci_hcs_init();
	xhci_open_memory_space();
	common_xhci_hc_init();
	perform_xhci_ehci_switching_flow(usb_route);
	usb_overcurrent_mapping();
	program_ehci_port_length();
	/** FIXME: USB per port control is missing, is it needed? **/
	xhci_close_memory_space();
	/** TODO: Close EHCI memory space? **/
}
