package main

import (
	"fmt"
	"sort"
)

type azalia struct {
}

func (i azalia) Scan(ctx Context, addr PCIDevData) {
	az := Create(ctx, "hda_verb.c")
	defer az.Close()

	Add_SPDX(az, C, GPL2_only)
	az.WriteString(
		`#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
`)

	for _, codec := range ctx.InfoSource.GetAzaliaCodecs() {
		fmt.Fprintf(az, "\t0x%08x,\t/* Codec Vendor / Device ID: %s */\n",
			codec.VendorID, codec.Name)
		fmt.Fprintf(az, "\t0x%08x,\t/* Subsystem ID */\n",
			codec.SubsystemID)
		fmt.Fprintf(az, "\t%d,\t\t/* Number of 4 dword sets */\n",
			len(codec.PinConfig)+1)
		fmt.Fprintf(az, "\tAZALIA_SUBVENDOR(%d, 0x%08x),\n",
			codec.CodecNo, codec.SubsystemID)

		keys := []int{}
		for nid, _ := range codec.PinConfig {
			keys = append(keys, nid)
		}

		sort.Ints(keys)

		for _, nid := range keys {
			fmt.Fprintf(az, "\tAZALIA_PIN_CFG(%d, 0x%02x, 0x%08x),\n",
				codec.CodecNo, nid, codec.PinConfig[nid])
		}
		az.WriteString("\n");
	}

	az.WriteString(
		`};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
`)

	PutPCIDev(addr, "")
}

func init() {
	/* I82801GX/I945 */
	RegisterPCI(0x8086, 0x27d8, azalia{})
	/* BD82X6X/sandybridge */
	RegisterPCI(0x8086, 0x1c20, azalia{})
	/* C216/ivybridge */
	RegisterPCI(0x8086, 0x1e20, azalia{})
	/* Lynx Point */
	RegisterPCI(0x8086, 0x8c20, azalia{})
	RegisterPCI(0x8086, 0x9c20, azalia{})
}
