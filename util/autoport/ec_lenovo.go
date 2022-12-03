package main

import "fmt"

func LenovoEC(ctx Context) {
	ap := Create(ctx, "acpi/platform.asl")
	defer ap.Close()

	wakeGPE := 13

	sbGPE := GuessECGPE(ctx)
	var GPE int
	var GPEUnsure bool
	if sbGPE < 0 {
		sbGPE = SouthBridge.EncodeGPE(1)
		GPE = 1
		GPEUnsure = true
		SouthBridge.NeedRouteGPIOManually()
	} else {
		GPE = SouthBridge.DecodeGPE(sbGPE)
		GPEUnsure = false
	}

	SouthBridge.EnableGPE(wakeGPE)
	SouthBridge.EnableGPE(GPE)

	GPEDefine := DSDTDefine{
		Key: "THINKPAD_EC_GPE",
	}

	GPEDefine.Value = fmt.Sprintf("%d", sbGPE)
	if GPEUnsure {
		GPEDefine.Comment = "FIXME: Check this"
	}

	DSDTDefines = append(DSDTDefines,
		DSDTDefine{
			Key:   "EC_LENOVO_H8_ME_WORKAROUND",
			Value: "1",
		}, GPEDefine)

	Add_gpl(ap)
	ap.WriteString(
		`Method(_WAK, 1)
{
	/* ME may not be up yet. */
	Store(0, \_TZ.MEB1)
	Store(0, \_TZ.MEB2)
	Return(Package() {0, 0})
}

Method(_PTS,1)
{
	\_SB.PCI0.LPCB.EC.RADI(0)
}
`)

	si := Create(ctx, "acpi/superio.asl")
	defer si.Close()

	Add_gpl(si)
	si.WriteString("#include <drivers/pc80/pc/ps2_controller.asl>\n")

	/* FIXME:XX Move this to ec/lenovo.  */
	smi := Create(ctx, "smihandler.c")
	defer smi.Close()

	AddSMMFile("smihandler.c", "")

	Add_gpl(smi)
	smi.WriteString(
		`#include <arch/io.h>
#include <console/console.h>
#include <cpu/x86/smm.h>
#include <ec/acpi/ec.h>
#include <ec/lenovo/h8/h8.h>
#include <delay.h>
#include <` + SouthBridge.GetGPIOHeader() + ">\n\n")

	if GPEUnsure {
		smi.WriteString("/* FIXME: check this */\n")
	}
	fmt.Fprintf(smi, "#define GPE_EC_SCI	%d\n", GPE)

	smi.WriteString("/* FIXME: check this */\n")
	fmt.Fprintf(smi, "#define GPE_EC_WAKE	%d\n", wakeGPE)

	smi.WriteString(`
static void mainboard_smi_handle_ec_sci(void)
{
	u8 status = inb(EC_SC);
	u8 event;

	if (!(status & EC_SCI_EVT))
		return;

	event = ec_query();
	printk(BIOS_DEBUG, "EC event %#02x\n", event);
}

void mainboard_smi_gpi(u32 gpi_sts)
{
	if (gpi_sts & (1 << GPE_EC_SCI))
		mainboard_smi_handle_ec_sci();
}

int mainboard_smi_apmc(u8 data)
{
	switch (data) {
	case APM_CNT_ACPI_ENABLE:
		/* use 0x1600/0x1604 to prevent races with userspace */
		ec_set_ports(0x1604, 0x1600);
		/* route EC_SCI to SCI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SCI);
		/* discard all events, and enable attention */
		ec_write(0x80, 0x01);
		break;
	case APM_CNT_ACPI_DISABLE:
		/* we have to use port 0x62/0x66, as 0x1600/0x1604 doesn't
		   provide a EC query function */
		ec_set_ports(0x66, 0x62);
		/* route EC_SCI to SMI */
		gpi_route_interrupt(GPE_EC_SCI, GPI_IS_SMI);
		/* discard all events, and enable attention */
		ec_write(0x80, 0x01);
		break;
	default:
		break;
	}
	return 0;
}

void mainboard_smi_sleep(u8 slp_typ)
{
	if (slp_typ == 3) {
		u8 ec_wake = ec_read(0x32);
		/* If EC wake events are enabled, enable wake on EC WAKE GPE. */
		if (ec_wake & 0x14) {
			/* Redirect EC WAKE GPE to SCI. */
			gpi_route_interrupt(GPE_EC_WAKE, GPI_IS_SCI);
		}
	}
}
`)

	ec := Create(ctx, "acpi/ec.asl")
	defer ec.Close()

	Add_gpl(ec)
	ec.WriteString("#include <ec/lenovo/h8/acpi/ec.asl>\n")

	KconfigBool["EC_LENOVO_PMH7"] = true
	KconfigBool["EC_LENOVO_H8"] = true

	pmh := DevTreeNode{
		Chip: "ec/lenovo/pmh7",
		Registers: map[string]string{
			"backlight_enable":  "0x01",
			"dock_event_enable": "0x01",
		},
		Children: []DevTreeNode{
			DevTreeNode{
				Chip:    "pnp",
				Comment: "dummy",
				Dev:     0xff,
				Func:    1,
			},
		},
	}
	PutChip("lpc", pmh)

	ecs := ctx.InfoSource.GetEC()
	h8 := DevTreeNode{
		Chip: "ec/lenovo/h8",
		Children: []DevTreeNode{
			DevTreeNode{
				Chip:    "pnp",
				Comment: "dummy",
				Dev:     0xff,
				Func:    2,
				IOs: map[uint16]uint16{
					0x60: 0x62,
					0x62: 0x66,
					0x64: 0x1600,
					0x66: 0x1604,
				},
			},
		},
		Comment: "FIXME: has_keyboard_backlight, has_power_management_beeps, has_uwb",
		Registers: map[string]string{
			"config0":   FormatHex8(ecs[0]),
			"config1":   FormatHex8(ecs[1]),
			"config2":   FormatHex8(ecs[2]),
			"config3":   FormatHex8(ecs[3]),
			"beepmask0": FormatHex8(ecs[4]),
			"beepmask1": FormatHex8(ecs[5]),
		},
	}
	for i := 0; i < 0x10; i++ {
		if ecs[0x10+i] != 0 {
			h8.Registers[fmt.Sprintf("event%x_enable", i)] = FormatHex8(ecs[0x10+i])
		}
	}
	PutChip("lpc", h8)

	eeprom := DevTreeNode{
		Chip:    "drivers/i2c/at24rf08c",
		Comment: "eeprom, 8 virtual devices, same chip",
		Children: []DevTreeNode{
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x54,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x55,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x56,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x57,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x5c,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x5d,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x5e,
			},
			DevTreeNode{
				Chip: "i2c",
				Dev:  0x5f,
			},
		},
	}
	PutChip("smbus", eeprom)
}
