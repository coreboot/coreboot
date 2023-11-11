package main

import "fmt"

type haswellmc struct {
	variant string
}

func divceil(a uint32, b uint32) uint32 {
	return (a + b - 1) / b
}

func getPanelCfg(inteltool InteltoolData, isULT bool) string {
	var refclk uint32
	var pwm_hz uint32

	if isULT {
		refclk = 24000000
	} else {
		refclk = 135000000
	}
	if (inteltool.IGD[0xc8254] >> 16) != 0 {
		pwm_hz = refclk / 128 / (inteltool.IGD[0xc8254] >> 16)
	} else {
		pwm_hz = 0
	}

	gpu_panel_power_up_delay := (inteltool.IGD[0xc7208] >> 16) & 0x1fff
	gpu_panel_power_backlight_on_delay := inteltool.IGD[0xc7208] & 0x1fff
	gpu_panel_power_down_delay := (inteltool.IGD[0xc720c] >> 16) & 0x1fff
	gpu_panel_power_backlight_off_delay := inteltool.IGD[0xc720c] & 0x1fff
	gpu_panel_power_cycle_delay := inteltool.IGD[0xc7210] & 0x1f

	return fmt.Sprintf(`{
		.up_delay_ms		= %3d,
		.down_delay_ms		= %3d,
		.cycle_delay_ms		= %3d,
		.backlight_on_delay_ms	= %3d,
		.backlight_off_delay_ms	= %3d,
		.backlight_pwm_hz	= %3d,
	}`,
		divceil(gpu_panel_power_up_delay, 10),
		divceil(gpu_panel_power_down_delay, 10),
		(gpu_panel_power_cycle_delay-1)*100,
		divceil(gpu_panel_power_backlight_on_delay, 10),
		divceil(gpu_panel_power_backlight_off_delay, 10),
		pwm_hz)
}

func (i haswellmc) Scan(ctx Context, addr PCIDevData) {
	inteltool := ctx.InfoSource.GetInteltool()

	isULT := (i.variant == "ULT")
	DevTree = DevTreeNode{
		Chip:          "northbridge/intel/haswell",
		MissingParent: "northbridge",
		Comment:       "FIXME: check ec_present, usb_xhci_on_resume, gfx",
		Registers: map[string]string{
			"gpu_dp_b_hotplug":    FormatInt32((inteltool.IGD[0xc4030] >> 2) & 4),
			"gpu_dp_c_hotplug":    FormatInt32((inteltool.IGD[0xc4030] >> 10) & 4),
			"gpu_dp_d_hotplug":    FormatInt32((inteltool.IGD[0xc4030] >> 18) & 4),
			"panel_cfg":           getPanelCfg(inteltool, isULT),
			"gpu_ddi_e_connected": FormatBool(((inteltool.IGD[0x64000] >> 4) & 1) == 0),
			"ec_present":          "false",
			"usb_xhci_on_resume":  "false",
			/* FIXME:XX hardcoded.  */
			"gfx": "GMA_STATIC_DISPLAYS(0)",
			"spd_addresses": "{0x50, 0x51, 0x52, 0x53}\" # FIXME: Put proper SPD map here",
		},
		Children: []DevTreeNode{
			{
				Chip: "cpu/intel/haswell",
				Children: []DevTreeNode{
					{
						Chip: "cpu_cluster",
						Dev:  0,
						Ops:  "haswell_cpu_bus_ops",
					},
				},
			},

			{
				Chip:          "domain",
				Dev:           0,
				Ops:           "haswell_pci_domain_ops",
				PCIController: true,
				ChildPCIBus:   0,
				PCISlots: []PCISlot{
					PCISlot{PCIAddr: PCIAddr{Dev: 0x0, Func: 0}, writeEmpty: true, additionalComment: i.variant},
					PCISlot{PCIAddr: PCIAddr{Dev: 0x1, Func: 0}, writeEmpty: !isULT, additionalComment: "PCIe Bridge for discrete graphics"},
					PCISlot{PCIAddr: PCIAddr{Dev: 0x2, Func: 0}, writeEmpty: true, additionalComment: "Internal graphics"},
					PCISlot{PCIAddr: PCIAddr{Dev: 0x3, Func: 0}, writeEmpty: true, additionalComment: "Mini-HD audio"},
				},
			},
		},
	}

	if isULT {
		DevTree.Registers["dq_pins_interleaved"] = FormatBool(((inteltool.MCHBAR[0x2008] >> 10) & 1) == 0)
	}

	PutPCIDev(addr, "Host bridge")

	KconfigBool["NORTHBRIDGE_INTEL_HASWELL"] = true
	KconfigBool["HAVE_ACPI_TABLES"] = true
	KconfigBool["HAVE_ACPI_RESUME"] = true

	DSDTIncludes = append(DSDTIncludes, DSDTInclude{
		File: "cpu/intel/common/acpi/cpu.asl",
	})

	DSDTPCI0Includes = append(DSDTPCI0Includes, DSDTInclude{
		File: "northbridge/intel/haswell/acpi/hostbridge.asl",
	}, DSDTInclude{
		File:    "drivers/intel/gma/acpi/default_brightness_levels.asl",
		Comment: "FIXME: remove this if the board doesn't have backlight",
	})
}

func init() {
	RegisterPCI(0x8086, 0x0c00, haswellmc{variant: "Desktop"})
	RegisterPCI(0x8086, 0x0c04, haswellmc{variant: "Mobile"})
	RegisterPCI(0x8086, 0x0a04, haswellmc{variant: "ULT"})
	RegisterPCI(0x8086, 0x0c08, haswellmc{variant: "Server"})
	RegisterPCI(0x8086, 0x0d00, haswellmc{variant: "Crystal Well Desktop"})
	RegisterPCI(0x8086, 0x0d04, haswellmc{variant: "Crystal Well Mobile"})
	RegisterPCI(0x8086, 0x0d08, haswellmc{variant: "Crystal Well Server"})
	for _, id := range []uint16{
		0x0402, 0x0412, 0x041e, 0x0422, 0x0d22, /* Desktop */
		0x0406, 0x0416, 0x0426, /* Mobile */
		0x040a, 0x041a, 0x042a, /* Server */
		0x0a06, 0x0a16, 0x0a26, /* ULT */
		0x0a0e, 0x0a1e, /* ULX */
		0x0d16, 0x0d26, 0x0d36, /* Mobile 4+3, GT3e */
	} {
		RegisterPCI(0x8086, id, GenericVGA{GenericPCI{Comment: "VGA controller"}})
	}
	/* CPU HD Audio */
	RegisterPCI(0x8086, 0x0a0c, GenericPCI{})
	RegisterPCI(0x8086, 0x0c0c, GenericPCI{})
}
