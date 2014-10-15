package main

import "fmt"

type rce823 struct {
	variant string
}

func (r rce823) Scan(ctx Context, addr PCIDevData) {
	if addr.Dev == 0 && addr.Func == 0 {
		cur := DevTreeNode{
			Chip:    "drivers/ricoh/rce822",
			Comment: "Ricoh cardreader",
			Registers: map[string]string{

				"sdwppol":      fmt.Sprintf("%d", (addr.ConfigDump[0xfb]&2)>>1),
				"disable_mask": fmt.Sprintf("0x%x", addr.ConfigDump[0xcb]),
			},
			PCISlots: []PCISlot{
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 0}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 1}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 2}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 3}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 4}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 5}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 6}, writeEmpty: false},
				PCISlot{PCIAddr: PCIAddr{Bus: addr.Bus, Dev: 0x0, Func: 7}, writeEmpty: false},
			},
		}
		PutPCIChip(addr, cur)
	}
	PutPCIDev(addr, "Ricoh SD card reader")
	KconfigBool["DRIVERS_RICOH_RCE822"] = true
}

func init() {
	RegisterPCI(0x1180, 0xe822, rce823{})
	RegisterPCI(0x1180, 0xe823, rce823{})
}
