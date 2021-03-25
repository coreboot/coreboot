/* SPDX-License-Identifier: GPL-2.0-only */

// See https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports
Scope(\_SB.PCI0.RP05) {
	Method(_DSD, 0, NotSerialized) {
		Return (Package(4) {
			// https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports#identifying-pcie-root-ports-supporting-hot-plug-in-d3
			ToUUID("6211e2c0-58a3-4af3-90e1-927a4e0c55a4"),
			Package(1) {
				Package(2) {
					"HotPlugSupportInD3",
					1
				}
			},
			// https://docs.microsoft.com/en-us/windows-hardware/drivers/pci/dsd-for-pcie-root-ports#identifying-externally-exposed-pcie-root-ports
			ToUUID("efcc06cc-73ac-4bc3-bff0-76143807c389"),
			Package(2) {
				Package(2) {
					"ExternalFacingPort",
					1
				},
				Package(2) {
					"UID",
					0
				}
			}
		})
	}
}
