/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __DRIVERS_INTEL_USB4_PCIE_H__
#define __DRIVERS_INTEL_USB4_PCIE_H__

/*
 * This virtual generic driver provides the ACPI properties for an
 * Intel USB4/TBT PCIe Root Port which is already declared in the DSDT,
 *
 * The associated USB4 port number is obtained from the generic ID and
 * the related host interface (DMA) device is provided by the devicetree.
 *
 * The "ExternalFacingPort", and "HotPlugSupportInD3" properties are defined at
 * https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports
 *
 * Example: PCIe Root Port 2 via USB4 host interface 1 port 0:
 *
 *   device pci 0d.3 alias tbt_dma1 on end  # \_SB.PCI0.TDM1
 *   device pci 07.2 alias tbt_pcie_rp2 on  # \_SB.PCI0.TRP2
 *       chip soc/intel/common/block/usb4
 *           use tbt_dma1 as usb4_port      # USB4 host interface for this root port
 *           device generic 0 on end        # USB4 port number on this host interface
 *       end
 *   end
 *
 * The host interface and PCIe Root Port are declared in the DSDT:
 *
 *   Scope (\_SB.PCI0) {
 *       Device (TDM1) {
 *           Name (_ADR, 0x000d0003)
 *       }
 *       Device (TRP2) {
 *           Name (_ADR, 0x00070002)
 *       }
 *   }
 *
 * This driver will provide the following properties in the SSDT:
 *
 *   Scope (\_SB.PCI0.TRP2) {
 *       Name (_DSD, Package () {
 *           ToUUID ("daffd814-6eba-4d8c-8a91-bc9bbf4aa301"),
 *           Package () {
 *               Package () { "usb4-host-interface", \_SB.PCI0.TDM1 },
 *               Package () { "usb4-port-number", 0 },
 *           },
 *           ToUUID ("6211e2c0-58a3-4af3-90e1-927a4e0c55a4"),
 *           Package () {
 *               Package () { "HotPlugSupportInD3", 1 },
 *           },
 *           ToUUID ("efcc06cc-73ac-4bc3-bff0-76143807c389"),
 *           Package () {
 *               Package () { "ExternalFacingPort", 1 },
 *           },
 *       }
 *   }
 */

struct soc_intel_common_block_usb4_config {
	const char *desc;

	/* USB4 host interface (DMA device) that this PCIe root port is routed to. */
	DEVTREE_CONST struct device *usb4_port;
};

#endif /* __DRIVERS_INTEL_USB4_PCIE_H__ */
