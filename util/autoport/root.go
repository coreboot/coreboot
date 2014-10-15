package main

import "fmt"

var supportedPCIDevices map[uint32]PCIDevice = map[uint32]PCIDevice{}
var PCIMap map[PCIAddr]PCIDevData = map[PCIAddr]PCIDevData{}

func ScanRoot(ctx Context) {
	for _, pciDev := range ctx.InfoSource.GetPCIList() {
		PCIMap[pciDev.PCIAddr] = pciDev
	}
	for _, pciDev := range ctx.InfoSource.GetPCIList() {
		vendevid := (uint32(pciDev.PCIDevID) << 16) | uint32(pciDev.PCIVenID)

		dev, ok := supportedPCIDevices[vendevid]
		if !ok {
			if pciDev.PCIAddr.Bus != 0 {
				fmt.Printf("Unknown PCI device %04x:%04x, assuming removable\n",
					pciDev.PCIVenID, pciDev.PCIDevID)
				continue
			}
			fmt.Printf("Unsupported PCI device %04x:%04x\n",
				pciDev.PCIVenID, pciDev.PCIDevID)
			dev = GenericPCI{Comment: fmt.Sprintf("Unsupported PCI device %04x:%04x",
				pciDev.PCIVenID, pciDev.PCIDevID)}
		}
		dev.Scan(ctx, pciDev)
	}
	dmi := ctx.InfoSource.GetDMI()
	if !dmi.IsLaptop {
		NoEC(ctx)
	} else if dmi.Vendor == "LENOVO" {
		LenovoEC(ctx)
	} else {
		FIXMEEC(ctx)
	}
}

func RegisterPCI(VenID uint16, DevID uint16, dev PCIDevice) {
	vendevid := (uint32(DevID) << 16) | uint32(VenID)
	supportedPCIDevices[vendevid] = dev
}
