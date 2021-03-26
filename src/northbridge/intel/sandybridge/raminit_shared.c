/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/mmio.h>
#include <types.h>

#include "sandybridge.h"

static const char *const ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active",
};

#define ON_OFF(val) (((val) & 1) ? "on" : "off")

/* Print the memory controller configuration as read from the memory controller registers. */
void report_memory_config(void)
{
	u32 addr_decoder_common, addr_decode_ch[2];
	int i;

	addr_decoder_common = mchbar_read32(MAD_CHNL);
	addr_decode_ch[0]   = mchbar_read32(MAD_DIMM_CH0);
	addr_decode_ch[1]   = mchbar_read32(MAD_DIMM_CH1);

	const int refclk = mchbar_read32(MC_BIOS_REQ) & 0x100 ? 100 : 133;

	printk(BIOS_DEBUG, "memcfg DDR3 ref clock %d MHz\n", refclk);
	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (mchbar_read32(MC_BIOS_DATA) * refclk * 100 * 2 + 50) / 100);

	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       (addr_decoder_common >> 0) & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < ARRAY_SIZE(addr_decode_ch); i++) {
		u32 ch_conf = addr_decode_ch[i];
		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n", i, ch_conf);
		printk(BIOS_DEBUG, "   ECC %s\n", ecc_decoder[(ch_conf >> 24) & 3]);
		printk(BIOS_DEBUG, "   enhanced interleave mode %s\n", ON_OFF(ch_conf >> 22));
		printk(BIOS_DEBUG, "   rank interleave %s\n", ON_OFF(ch_conf >> 21));
		printk(BIOS_DEBUG, "   DIMMA %d MB width x%d %s rank%s\n",
		       ((ch_conf >>  0) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? 16 : 8,
		       ((ch_conf >> 17) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? "" : ", selected");
		printk(BIOS_DEBUG, "   DIMMB %d MB width x%d %s rank%s\n",
		       ((ch_conf >>  8) & 0xff) * 256,
		       ((ch_conf >> 20) & 1) ? 16 : 8,
		       ((ch_conf >> 18) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? ", selected" : "");
	}
}
#undef ON_OFF
