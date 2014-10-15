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

	az.WriteString(
		`#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
`)

	for _, codec := range ctx.InfoSource.GetAzaliaCodecs() {
		fmt.Fprintf(az, "\t0x%08x, /* Codec Vendor / Device ID: %s */\n",
			codec.VendorID, codec.Name)
		fmt.Fprintf(az, "\t0x%08x, /* Subsystem ID */\n",
			codec.SubsystemID)
		fmt.Fprintf(az, "\n\t0x%08x, /* Number of 4 dword sets */\n",
			len(codec.PinConfig)+1)
		fmt.Fprintf(az, "\t/* NID 0x01: Subsystem ID.  */\n")
		fmt.Fprintf(az, "\tAZALIA_SUBVENDOR(0x%x, 0x%08x),\n",
			codec.CodecNo, codec.SubsystemID)

		keys := []int{}
		for nid, _ := range codec.PinConfig {
			keys = append(keys, nid)
		}

		sort.Ints(keys)

		for _, nid := range keys {
			fmt.Fprintf(az, "\n\t/* NID 0x%02x.  */\n", nid)
			fmt.Fprintf(az, "\tAZALIA_PIN_CFG(0x%x, 0x%02x, 0x%08x),\n",
				codec.CodecNo, nid, codec.PinConfig[nid])
		}
	}

	az.WriteString(
		`};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
`)

	PutPCIDev(addr, "Audio controller")
}

func init() {
	/* I82801GX/I945 */
	RegisterPCI(0x8086, 0x27d8, azalia{})
	/* BD82X6X/sandybridge */
	RegisterPCI(0x8086, 0x1c20, azalia{})
	/* C216/ivybridge */
	RegisterPCI(0x8086, 0x1e20, azalia{})
}
