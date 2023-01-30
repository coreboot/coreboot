package main

import "fmt"

type sandybridgemc struct {
}

func (i sandybridgemc) Scan(ctx Context, addr PCIDevData) {
	inteltool := ctx.InfoSource.GetInteltool()

	/* FIXME:XX Move this somewhere else.  */
	MainboardIncludes = append(MainboardIncludes, "drivers/intel/gma/int15.h")
	MainboardEnable += (`	/* FIXME: fix these values. */
	install_intel_vga_int15_handler(GMA_INT15_ACTIVE_LFP_INT_LVDS,
					GMA_INT15_PANEL_FIT_DEFAULT,
					GMA_INT15_BOOT_DISPLAY_DEFAULT, 0);
`)

	DevTree = DevTreeNode{
		Chip:          "northbridge/intel/sandybridge",
		MissingParent: "northbridge",
		Comment:       "FIXME: GPU registers may not always apply.",
		Registers: map[string]string{
			"gpu_dp_b_hotplug":                    FormatInt32((inteltool.IGD[0xc4030] >> 2) & 7),
			"gpu_dp_c_hotplug":                    FormatInt32((inteltool.IGD[0xc4030] >> 10) & 7),
			"gpu_dp_d_hotplug":                    FormatInt32((inteltool.IGD[0xc4030] >> 18) & 7),
			"gpu_panel_port_select":               FormatInt32((inteltool.IGD[0xc7208] >> 30) & 3),
			"gpu_panel_power_up_delay":            FormatInt32((inteltool.IGD[0xc7208] >> 16) & 0x1fff),
			"gpu_panel_power_backlight_on_delay":  FormatInt32(inteltool.IGD[0xc7208] & 0x1fff),
			"gpu_panel_power_down_delay":          FormatInt32((inteltool.IGD[0xc720c] >> 16) & 0x1fff),
			"gpu_panel_power_backlight_off_delay": FormatInt32(inteltool.IGD[0xc720c] & 0x1fff),
			"gpu_panel_power_cycle_delay":         FormatInt32(inteltool.IGD[0xc7210] & 0xff),
			"gpu_cpu_backlight":                   FormatHex32(inteltool.IGD[0x48254]),
			"gpu_pch_backlight":                   FormatHex32((inteltool.IGD[0xc8254] >> 16) * 0x10001),
			"gfx": fmt.Sprintf("GMA_STATIC_DISPLAYS(%d)", (inteltool.IGD[0xc6200] >> 12) & 1),
		},
		Children: []DevTreeNode{
			{
				Chip:          "domain",
				Dev:           0,
				PCIController: true,
				ChildPCIBus:   0,
				PCISlots: []PCISlot{
					PCISlot{PCIAddr: PCIAddr{Dev: 0x0, Func: 0}, writeEmpty: true, alias: "host_bridge", additionalComment: "Host bridge"},
					PCISlot{PCIAddr: PCIAddr{Dev: 0x1, Func: 0}, writeEmpty: true, alias: "peg10", additionalComment: "PEG"},
					PCISlot{PCIAddr: PCIAddr{Dev: 0x2, Func: 0}, writeEmpty: true, alias: "igd", additionalComment: "iGPU"},
				},
			},
		},
	}

	PutPCIDev(addr, "Host bridge")

	/* FIXME:XX some configs are unsupported.  */
	KconfigBool["NORTHBRIDGE_INTEL_SANDYBRIDGE"] = true
	KconfigBool["USE_NATIVE_RAMINIT"] = true
	KconfigBool["INTEL_INT15"] = true
	KconfigBool["HAVE_ACPI_TABLES"] = true
	KconfigBool["HAVE_ACPI_RESUME"] = true

	DSDTIncludes = append(DSDTIncludes, DSDTInclude{
		File: "cpu/intel/common/acpi/cpu.asl",
	})

	DSDTPCI0Includes = append(DSDTPCI0Includes, DSDTInclude{
		File: "northbridge/intel/sandybridge/acpi/sandybridge.asl",
	}, DSDTInclude{
		File: "drivers/intel/gma/acpi/default_brightness_levels.asl",
	})
}

func init() {
	RegisterPCI(0x8086, 0x0100, sandybridgemc{})
	RegisterPCI(0x8086, 0x0104, sandybridgemc{})
	RegisterPCI(0x8086, 0x0150, sandybridgemc{})
	RegisterPCI(0x8086, 0x0154, sandybridgemc{})
	RegisterPCI(0x8086, 0x0158, sandybridgemc{})
	for _, id := range []uint16{
		0x0102, 0x0106, 0x010a,
		0x0112, 0x0116, 0x0122, 0x0126,
		0x0152, 0x0156, 0x0162, 0x0166,
	} {
		RegisterPCI(0x8086, id, GenericVGA{GenericPCI{}})
	}

	/* PCIe bridge */
	for _, id := range []uint16{
		0x0101, 0x0105, 0x0109, 0x010d,
		0x0151, 0x0155, 0x0159, 0x015d,
	} {
		RegisterPCI(0x8086, id, GenericPCI{})
	}
}
