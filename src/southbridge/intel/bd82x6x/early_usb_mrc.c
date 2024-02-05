/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/pci_ops.h>
#include <device/pci_def.h>
#include "pch.h"
#include "chip.h"
#include <northbridge/intel/sandybridge/pei_data.h>

#define PCH_EHCI1_TEMP_BAR0 0xe8000000
#define PCH_EHCI2_TEMP_BAR0 0xe8000400

/*
 * Setup USB controller MMIO BAR to prevent the
 * reference code from resetting the controller.
 *
 * The BAR will be re-assigned during device
 * enumeration so these are only temporary.
 */
void enable_usb_bar(void)
{
	pci_devfn_t usb0 = PCH_EHCI1_DEV;
	pci_devfn_t usb1 = PCH_EHCI2_DEV;

	/* USB Controller 1 */
	pci_write_config32(usb0, PCI_BASE_ADDRESS_0,
			   PCH_EHCI1_TEMP_BAR0);
	pci_or_config16(usb0, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);

	/* USB Controller 2 */
	pci_write_config32(usb1, PCI_BASE_ADDRESS_0,
			   PCH_EHCI2_TEMP_BAR0);
	pci_or_config16(usb1, PCI_COMMAND, PCI_COMMAND_MASTER | PCI_COMMAND_MEMORY);
}

/*
 * Translate coreboot native USB port configuration in devicetree
 * into a format reference code expects:
 *
 * [MRC index] = .native_field // what for
 *         [0] = .enabled // enable
 *         [1] = .oc_pin // overcurrent pin
 *         [2] = .current // length
 *
 * For .current, use these native values for MRC settings 1-3, corresponding
 * to values of 0x40/0x80/0x130, which should produce the correct values
 * across all supported PCHs.
 *
 * PCH type    | 1 | 2 | 3
 * ------------+---+---+---
 * Mobile      | 0 | 1 | 2
 * Desktop x6x | 6 | 1 | 7
 * Desktop x7x | 8 | 9 | 2
 *
 * See also:
 * northbridge/intel/sandybridge/pei_data.h
 * pch.h
 * early_usb.c
 */
void southbridge_fill_pei_data(struct pei_data *pei_data)
{
	const struct device *dev = pcidev_on_root(0x1d, 0);
	const struct southbridge_intel_bd82x6x_config *config = dev->chip_info;
	/* Native current -> MRC length map to get the same USBIRx register value */
	const uint16_t currents[] = { 0x40, 0x80, 0x130,
				      0, 0, 0, /* 3-5 not seen in MRC */
				      0x40, 0x130, 0x40, 0x80};
	for (unsigned int port = 0; port < ARRAY_SIZE(config->usb_port_config); port++) {
		uint16_t current = 0;
		int ocp = config->usb_port_config[port].oc_pin;
		if (ocp == -1)
			ocp = (port < 8) ? 0 : 4;

		if (config->usb_port_config[port].current < ARRAY_SIZE(currents))
			current = currents[config->usb_port_config[port].current];

		/*
		 * Note for developers: If this message shows, your board uses a
		 * current setting MRC.bin cannot produce. Choose a value as close
		 * as possible and test all USB ports, or consider using native raminit.
		 */
		if (current == 0) {
			printk(BIOS_NOTICE,
			       "%s: USB%02d: %d is an invalid setting for MRC.bin!\n",
			       __func__, port, config->usb_port_config[port].current);
		}

		pei_data->usb_port_config[port][0] = config->usb_port_config[port].enabled;
		pei_data->usb_port_config[port][1] = ocp;
		pei_data->usb_port_config[port][2] = current;
	}

	pei_data->usb3.hs_port_switch_mask = config->xhci_switchable_ports;
}
