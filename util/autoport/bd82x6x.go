package main

import (
	"fmt"
	"os"
)

type bd82x6x struct {
	variant string
	node    *DevTreeNode
}

func (b bd82x6x) writeGPIOSet(ctx Context, sb *os.File,
	val uint32, set uint, partno int, constraint uint32) {

	max := uint(32)
	if set == 3 {
		max = 12
	}

	bits := [6][2]string{
		{"GPIO_MODE_NATIVE", "GPIO_MODE_GPIO"},
		{"GPIO_DIR_OUTPUT", "GPIO_DIR_INPUT"},
		{"GPIO_LEVEL_LOW", "GPIO_LEVEL_HIGH"},
		{"GPIO_RESET_PWROK", "GPIO_RESET_RSMRST"},
		{"GPIO_NO_INVERT", "GPIO_INVERT"},
		{"GPIO_NO_BLINK", "GPIO_BLINK"},
	}

	for i := uint(0); i < max; i++ {
		if (constraint>>i)&1 == 1 {
			fmt.Fprintf(sb, "	.gpio%d = %s,\n",
				(set-1)*32+i,
				bits[partno][(val>>i)&1])
		}
	}
}

func (b bd82x6x) GPIO(ctx Context, inteltool InteltoolData) {
	var constraint uint32
	gpio := Create(ctx, "gpio.c")
	defer gpio.Close()

	AddBootBlockFile("gpio.c", "")
	AddROMStageFile("gpio.c", "")

	Add_gpl(gpio)
	gpio.WriteString("#include <southbridge/intel/common/gpio.h>\n\n")

	addresses := [3][6]int{
		{0x00, 0x04, 0x0c, 0x60, 0x2c, 0x18},
		{0x30, 0x34, 0x38, 0x64, -1, -1},
		{0x40, 0x44, 0x48, 0x68, -1, -1},
	}

	for set := 1; set <= 3; set++ {
		for partno, part := range []string{"mode", "direction", "level", "reset", "invert", "blink"} {
			addr := addresses[set-1][partno]
			if addr < 0 {
				continue
			}
			fmt.Fprintf(gpio, "static const struct pch_gpio_set%d pch_gpio_set%d_%s = {\n",
				set, set, part)

			constraint = 0xffffffff
			switch part {
			case "direction":
				/* Ignored on native mode */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
			case "level":
				/* Level doesn't matter for input */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
				constraint &^= inteltool.GPIO[uint16(addresses[set-1][1])]
			case "reset":
				/* Only show reset */
				constraint = inteltool.GPIO[uint16(addresses[set-1][3])]
			case "invert":
				/* Only on input and only show inverted GPIO */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
				constraint &= inteltool.GPIO[uint16(addresses[set-1][1])]
				constraint &= inteltool.GPIO[uint16(addresses[set-1][4])]
			case "blink":
				/* Only on output and only show blinking GPIO */
				constraint = inteltool.GPIO[uint16(addresses[set-1][0])]
				constraint &^= inteltool.GPIO[uint16(addresses[set-1][1])]
				constraint &= inteltool.GPIO[uint16(addresses[set-1][5])]
			}
			b.writeGPIOSet(ctx, gpio, inteltool.GPIO[uint16(addr)], uint(set), partno, constraint)
			gpio.WriteString("};\n\n")
		}
	}

	gpio.WriteString(`const struct pch_gpio_map mainboard_gpio_map = {
	.set1 = {
		.mode		= &pch_gpio_set1_mode,
		.direction	= &pch_gpio_set1_direction,
		.level		= &pch_gpio_set1_level,
		.blink		= &pch_gpio_set1_blink,
		.invert		= &pch_gpio_set1_invert,
		.reset		= &pch_gpio_set1_reset,
	},
	.set2 = {
		.mode		= &pch_gpio_set2_mode,
		.direction	= &pch_gpio_set2_direction,
		.level		= &pch_gpio_set2_level,
		.reset		= &pch_gpio_set2_reset,
	},
	.set3 = {
		.mode		= &pch_gpio_set3_mode,
		.direction	= &pch_gpio_set3_direction,
		.level		= &pch_gpio_set3_level,
		.reset		= &pch_gpio_set3_reset,
	},
};
`)
}

func (b bd82x6x) IsPCIeHotplug(ctx Context, port int) bool {
	portDev, ok := PCIMap[PCIAddr{Bus: 0, Dev: 0x1c, Func: port}]
	if !ok {
		return false
	}
	return (portDev.ConfigDump[0xdb] & (1 << 6)) != 0
}

func ich9GetFlashSize(ctx Context) {
	inteltool := ctx.InfoSource.GetInteltool()
	switch (inteltool.RCBA[0x3410] >> 10) & 3 {
	/* SPI. All boards I've seen with sandy/ivy use SPI.  */
	case 3:
		ROMProtocol = "SPI"
		highflkb := uint32(0)
		for reg := uint16(0); reg < 5; reg++ {
			fl := (inteltool.RCBA[0x3854+4*reg] >> 16) & 0x1fff
			flkb := (fl + 1) << 2
			if flkb > highflkb {
				highflkb = flkb
			}
		}
		ROMSizeKB = int(highflkb)
		/* Shared with ME. Flashrom is unable to handle it.  */
		FlashROMSupport = "n"
	}
}

func (b bd82x6x) GetGPIOHeader() string {
	return "southbridge/intel/bd82x6x/pch.h"
}

func (b bd82x6x) EnableGPE(in int) {
	b.node.Registers[fmt.Sprintf("gpi%d_routing", in)] = "2"
}

func (b bd82x6x) EncodeGPE(in int) int {
	return in + 0x10
}

func (b bd82x6x) DecodeGPE(in int) int {
	return in - 0x10
}

func (b bd82x6x) NeedRouteGPIOManually() {
	b.node.Comment += ", FIXME: set gpiX_routing for EC support"
}

func (b bd82x6x) Scan(ctx Context, addr PCIDevData) {

	SouthBridge = &b

	inteltool := ctx.InfoSource.GetInteltool()
	b.GPIO(ctx, inteltool)

	KconfigBool["SOUTHBRIDGE_INTEL_"+b.variant] = true
	KconfigBool["SERIRQ_CONTINUOUS_MODE"] = true
	KconfigInt["USBDEBUG_HCD_INDEX"] = 2
	KconfigComment["USBDEBUG_HCD_INDEX"] = "FIXME: check this"
	dmi := ctx.InfoSource.GetDMI()
	if dmi.Vendor == "LENOVO" {
		KconfigInt["DRAM_RESET_GATE_GPIO"] = 10
	} else {
		KconfigInt["DRAM_RESET_GATE_GPIO"] = 60
	}
	KconfigComment["DRAM_RESET_GATE_GPIO"] = "FIXME: check this"

	ich9GetFlashSize(ctx)

	DSDTDefines = append(DSDTDefines,
		DSDTDefine{
			Key:   "BRIGHTNESS_UP",
			Value: "\\_SB.PCI0.GFX0.INCB",
		},
		DSDTDefine{
			Key:   "BRIGHTNESS_DOWN",
			Value: "\\_SB.PCI0.GFX0.DECB",
		})

	/* SPI init */
	MainboardIncludes = append(MainboardIncludes, "southbridge/intel/bd82x6x/pch.h")

	FADT := ctx.InfoSource.GetACPI()["FACP"]

	pcieHotplugMap := "{ "

	for port := 0; port < 7; port++ {
		if b.IsPCIeHotplug(ctx, port) {
			pcieHotplugMap += "1, "
		} else {
			pcieHotplugMap += "0, "
		}
	}

	if b.IsPCIeHotplug(ctx, 7) {
		pcieHotplugMap += "1 }"
	} else {
		pcieHotplugMap += "0 }"
	}

	cur := DevTreeNode{
		Chip:    "southbridge/intel/bd82x6x",
		Comment: "Intel Series 6 Cougar Point PCH",

		Registers: map[string]string{
			"sata_interface_speed_support": "0x3",
			"gen1_dec":                     FormatHexLE32(PCIMap[PCIAddr{Bus: 0, Dev: 0x1f, Func: 0}].ConfigDump[0x84:0x88]),
			"gen2_dec":                     FormatHexLE32(PCIMap[PCIAddr{Bus: 0, Dev: 0x1f, Func: 0}].ConfigDump[0x88:0x8c]),
			"gen3_dec":                     FormatHexLE32(PCIMap[PCIAddr{Bus: 0, Dev: 0x1f, Func: 0}].ConfigDump[0x8c:0x90]),
			"gen4_dec":                     FormatHexLE32(PCIMap[PCIAddr{Bus: 0, Dev: 0x1f, Func: 0}].ConfigDump[0x90:0x94]),
			"pcie_port_coalesce":           "1",
			"pcie_hotplug_map":             pcieHotplugMap,

			"sata_port_map": fmt.Sprintf("0x%x", PCIMap[PCIAddr{Bus: 0, Dev: 0x1f, Func: 2}].ConfigDump[0x92]&0x3f),

			"docking_supported": (FormatBool((FADT[113] & (1 << 1)) != 0)),
			"spi_uvscc":         fmt.Sprintf("0x%x", inteltool.RCBA[0x38c8]),
			"spi_lvscc":         fmt.Sprintf("0x%x", inteltool.RCBA[0x38c4]&^(1<<23)),
		},
		PCISlots: []PCISlot{
			PCISlot{PCIAddr: PCIAddr{Dev: 0x14, Func: 0}, writeEmpty: false, alias: "xhci", additionalComment: "USB 3.0 Controller"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x16, Func: 0}, writeEmpty: true, alias: "mei1", additionalComment: "Management Engine Interface 1"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x16, Func: 1}, writeEmpty: true, alias: "mei2", additionalComment: "Management Engine Interface 2"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x16, Func: 2}, writeEmpty: true, alias: "me_ide_r", additionalComment: "Management Engine IDE-R"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x16, Func: 3}, writeEmpty: true, alias: "me_kt", additionalComment: "Management Engine KT"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x19, Func: 0}, writeEmpty: true, alias: "gbe", additionalComment: "Intel Gigabit Ethernet"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1a, Func: 0}, writeEmpty: true, alias: "echi2", additionalComment: "USB2 EHCI #2"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1b, Func: 0}, writeEmpty: true, alias: "hda", additionalComment: "High Definition Audio"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 0}, writeEmpty: true, alias: "pcie_rp1", additionalComment: "PCIe Port #1"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 1}, writeEmpty: true, alias: "pcie_rp2", additionalComment: "PCIe Port #2"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 2}, writeEmpty: true, alias: "pcie_rp3", additionalComment: "PCIe Port #3"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 3}, writeEmpty: true, alias: "pcie_rp4", additionalComment: "PCIe Port #4"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 4}, writeEmpty: true, alias: "pcie_rp5", additionalComment: "PCIe Port #5"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 5}, writeEmpty: true, alias: "pcie_rp6", additionalComment: "PCIe Port #6"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 6}, writeEmpty: true, alias: "pcie_rp7", additionalComment: "PCIe Port #7"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1c, Func: 7}, writeEmpty: true, alias: "pcie_rp8", additionalComment: "PCIe Port #8"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1d, Func: 0}, writeEmpty: true, alias: "ehci1", additionalComment: "USB2 EHCI #1"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1e, Func: 0}, writeEmpty: true, alias: "pci_bridge", additionalComment: "PCI bridge"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1f, Func: 0}, writeEmpty: true, alias: "lpc", additionalComment: "LPC bridge"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1f, Func: 2}, writeEmpty: true, alias: "sata1", additionalComment: "SATA Controller 1"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1f, Func: 3}, writeEmpty: true, alias: "smbus", additionalComment: "SMBus"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1f, Func: 5}, writeEmpty: true, alias: "sata2", additionalComment: "SATA Controller 2"},
			PCISlot{PCIAddr: PCIAddr{Dev: 0x1f, Func: 6}, writeEmpty: true, alias: "thermal", additionalComment: "Thermal"},
		},
	}

	b.node = &cur

	xhciDev, ok := PCIMap[PCIAddr{Bus: 0, Dev: 0x14, Func: 0}]

	if ok {
		cur.Registers["xhci_switchable_ports"] = FormatHexLE32(xhciDev.ConfigDump[0xd4:0xd8])
		cur.Registers["superspeed_capable_ports"] = FormatHexLE32(xhciDev.ConfigDump[0xdc:0xe0])
		cur.Registers["xhci_overcurrent_mapping"] = FormatHexLE32(xhciDev.ConfigDump[0xc0:0xc4])
	}

	PutPCIChip(addr, cur)
	PutPCIDevParent(addr, "", "lpc")

	DSDTIncludes = append(DSDTIncludes, DSDTInclude{
		File: "southbridge/intel/common/acpi/platform.asl",
	})
	DSDTIncludes = append(DSDTIncludes, DSDTInclude{
		File: "southbridge/intel/bd82x6x/acpi/globalnvs.asl",
	})
	DSDTIncludes = append(DSDTIncludes, DSDTInclude{
		File: "southbridge/intel/common/acpi/sleepstates.asl",
	})
	DSDTPCI0Includes = append(DSDTPCI0Includes, DSDTInclude{
		File: "southbridge/intel/bd82x6x/acpi/pch.asl",
	})

	AddBootBlockFile("early_init.c", "")
	AddROMStageFile("early_init.c", "")

	sb := Create(ctx, "early_init.c")
	defer sb.Close()
	Add_gpl(sb)

	sb.WriteString(`
#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <northbridge/intel/sandybridge/raminit_native.h>
#include <southbridge/intel/bd82x6x/pch.h>

`)
	sb.WriteString("const struct southbridge_usb_port mainboard_usb_ports[] = {\n")

	currentMap := map[uint32]int{
		0x20000153: 0,
		0x20000f57: 1,
		0x2000055b: 2,
		0x20000f51: 3,
		0x2000094a: 4,
	}

	for port := uint(0); port < 14; port++ {
		var pinmask uint32
		OCPin := -1
		if port < 8 {
			pinmask = inteltool.RCBA[0x35a0]
		} else {
			pinmask = inteltool.RCBA[0x35a4]
		}
		for pin := uint(0); pin < 4; pin++ {
			if ((pinmask >> ((port % 8) + 8*pin)) & 1) != 0 {
				OCPin = int(pin)
				if port >= 8 {
					OCPin += 4
				}
			}
		}
		fmt.Fprintf(sb, "\t{ %d, %d, %d },\n",
			((inteltool.RCBA[0x359c]>>port)&1)^1,
			currentMap[inteltool.RCBA[uint16(0x3500+4*port)]],
			OCPin)
	}
	sb.WriteString("};\n")

	guessedMap := GuessSPDMap(ctx)

	sb.WriteString(`
void bootblock_mainboard_early_init(void)
{
`)
	RestorePCI16Simple(sb, addr, 0x82)

	RestorePCI16Simple(sb, addr, 0x80)

	sb.WriteString(`}

/* FIXME: Put proper SPD map here. */
void mainboard_get_spd(spd_raw_data *spd, bool id_only)
{
`)
	for i, spd := range guessedMap {
		fmt.Fprintf(sb, "\tread_spd(&spd[%d], 0x%02x, id_only);\n", i, spd)
	}
	sb.WriteString("}\n")

	gnvs := Create(ctx, "acpi_tables.c")
	defer gnvs.Close()

	Add_gpl(gnvs)
	gnvs.WriteString(`#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>

/* FIXME: check this function.  */
void mainboard_fill_gnvs(struct global_nvs *gnvs)
{
	/* The lid is open by default. */
	gnvs->lids = 1;

	/* Temperature at which OS will shutdown */
	gnvs->tcrt = 100;
	/* Temperature at which OS will throttle CPU */
	gnvs->tpsv = 90;
}
`)
}

func init() {
	/* BD82X6X LPC */
	for id := 0x1c40; id <= 0x1c5f; id++ {
		RegisterPCI(0x8086, uint16(id), bd82x6x{variant: "BD82X6X"})
	}

	/* C216 LPC */
	for id := 0x1e41; id <= 0x1e5f; id++ {
		RegisterPCI(0x8086, uint16(id), bd82x6x{variant: "C216"})
	}

	/* PCIe bridge */
	for _, id := range []uint16{
		0x1c10, 0x1c12, 0x1c14, 0x1c16,
		0x1c18, 0x1c1a, 0x1c1c, 0x1c1e,
		0x1e10, 0x1e12, 0x1e14, 0x1e16,
		0x1e18, 0x1e1a, 0x1e1c, 0x1e1e,
		0x1e25, 0x244e, 0x2448,
	} {
		RegisterPCI(0x8086, id, GenericPCI{})
	}

	/* SMBus controller  */
	RegisterPCI(0x8086, 0x1c22, GenericPCI{MissingParent: "smbus"})
	RegisterPCI(0x8086, 0x1e22, GenericPCI{MissingParent: "smbus"})

	/* SATA */
	for _, id := range []uint16{
		0x1c00, 0x1c01, 0x1c02, 0x1c03,
		0x1e00, 0x1e01, 0x1e02, 0x1e03,
	} {
		RegisterPCI(0x8086, id, GenericPCI{})
	}

	/* EHCI */
	for _, id := range []uint16{
		0x1c26, 0x1c2d, 0x1e26, 0x1e2d,
	} {
		RegisterPCI(0x8086, id, GenericPCI{})
	}

	/* XHCI */
	RegisterPCI(0x8086, 0x1e31, GenericPCI{})

	/* ME and children */
	for _, id := range []uint16{
		0x1c3a, 0x1c3b, 0x1c3c, 0x1c3d,
		0x1e3a, 0x1e3b, 0x1e3c, 0x1e3d,
	} {
		RegisterPCI(0x8086, id, GenericPCI{})
	}

	/* Ethernet */
	RegisterPCI(0x8086, 0x1502, GenericPCI{})
	RegisterPCI(0x8086, 0x1503, GenericPCI{})

}
