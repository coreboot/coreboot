package main

import "fmt"

func FIXMEEC(ctx Context) {
	ap := Create(ctx, "acpi/platform.asl")
	defer ap.Close()

	hasKeyboard := ctx.InfoSource.HasPS2()

	sbGPE := GuessECGPE(ctx)
	var GPEUnsure bool
	if sbGPE < 0 {
		sbGPE = SouthBridge.EncodeGPE(1)
		GPEUnsure = true
		SouthBridge.NeedRouteGPIOManually()
	} else {
		GPEUnsure = false
		SouthBridge.EnableGPE(SouthBridge.DecodeGPE(sbGPE))
	}

	Add_SPDX(ap, ASL, GPL2_only)
	ap.WriteString(
		`Method(_WAK, 1)
{
	/* FIXME: EC support  */
	Return(Package() {0, 0})
}

Method(_PTS,1)
{
	/* FIXME: EC support  */
}
`)

	ecs := ctx.InfoSource.GetEC()
	MainboardIncludes = append(MainboardIncludes, "ec/acpi/ec.h")
	MainboardIncludes = append(MainboardIncludes, "console/console.h")

	MainboardInit +=
		`	/* FIXME: trim this down or remove if necessary */
	{
		int i;
		const u8 dmp[256] = {`
	for i := 0; i < 0x100; i++ {
		if (i & 0xf) == 0 {
			MainboardInit += fmt.Sprintf("\n\t\t\t/* %02x */ ", i)
		}
		MainboardInit += fmt.Sprintf("0x%02x,", ecs[i])
		if (i & 0xf) != 0xf {
			MainboardInit += " "
		}
	}
	MainboardInit += "\n\t\t};\n"
	MainboardInit += `
		printk(BIOS_DEBUG, "Replaying EC dump ...");
		for (i = 0; i < 256; i++)
			ec_write (i, dmp[i]);
		printk(BIOS_DEBUG, "done\n");
	}
`

	KconfigBool["EC_ACPI"] = true
	si := Create(ctx, "acpi/superio.asl")
	defer si.Close()

	if hasKeyboard {
		Add_SPDX(si, ASL, GPL2_only)
		si.WriteString("#include <drivers/pc80/pc/ps2_controller.asl>\n")
		MainboardInit += fmt.Sprintf("\tpc_keyboard_init(NO_AUX_DEVICE);\n")
		MainboardIncludes = append(MainboardIncludes, "pc80/keyboard.h")
	}

	ec := Create(ctx, "acpi/ec.asl")
	defer ec.Close()

	Add_SPDX(ec, ASL, GPL2_only)
	ec.WriteString(`Device(EC)
{
	Name (_HID, EISAID("PNP0C09"))
	Name (_UID, 0)
`)
	if GPEUnsure {
		ec.WriteString("\t/* FIXME: Set GPE  */\n")
		ec.WriteString("\t/* Name (_GPE, ?)  */\n")
	} else {
		fmt.Fprintf(ec, "\tName (_GPE, %d)\n", sbGPE)
	}
	ec.WriteString("/* FIXME: EC support */\n")
	ec.WriteString("}\n")
}
