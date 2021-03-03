/* SPDX-License-Identifier: GPL-2.0-only */

#include <boardid.h>
#include <cbfs.h>
#include <console/console.h>
#include <soc/dramc_param.h>

/* Reference: go/asurada-id */
static const char *const sdram_configs[] = {
	/* Group 0x0: eMCP 4GB dual rank (with exceptions). */
	[0x00] = "sdram-lpddr4x-MT29VZZZBD9DQKPR-046-6GB",  /* 6GB */
	[0x01] = "sdram-lpddr4x-MT29VZZZAD8GQFSL-046-4GB",  /* single rank */
	[0x02] = "sdram-lpddr4x-KMDP6001DA-B425-4GB",
	[0x03] = "sdram-lpddr4x-KMDV6001DA-B620-4GB",
	[0x04] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x05] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x06] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x07] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x08] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x09] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x0a] = "sdram-lpddr4x-EMCP-2RANK-4GB",
	[0x0b] = "sdram-lpddr4x-EMCP-2RANK-4GB",

	/* Group 0x1, 0x2, 0x3 are reserved for eMCP. */

	/* Group 0x4: discrete 4GB single rank. */
	[0x40] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x41] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x42] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x43] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x44] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x45] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x46] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x47] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x48] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x49] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x4a] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",
	[0x4b] = "sdram-lpddr4x-DISCRETE-1RANK-4GB",

	/* Group 0x5: discrete 4GB dual rank. */
	[0x50] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x51] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x52] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x53] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x54] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x55] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x56] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x57] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x58] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x59] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x5a] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",
	[0x5b] = "sdram-lpddr4x-DISCRETE-2RANK-4GB",

	/* Group 0x6: discrete 8GB dual rank, normal mode */
	[0x60] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x61] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x62] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x63] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x64] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x65] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x66] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x67] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x68] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x69] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x6a] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",
	[0x6b] = "sdram-lpddr4x-DISCRETE-2RANK-8GB",

	/* Group 0x7: discrete 8GB dual rank, byte mode. */
	[0x70] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x71] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x72] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x73] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x74] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x75] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x76] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x77] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x78] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x79] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x7a] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
	[0x7b] = "sdram-lpddr4x-DISCRETE-2RANK-8GB-BYTE-MODE",
};

static struct sdram_info params;

const struct sdram_info *get_sdram_config(void)
{
	uint32_t ramcode = ram_code();

	if (ramcode >= ARRAY_SIZE(sdram_configs) ||
	    cbfs_load(sdram_configs[ramcode], &params, sizeof(params)) != sizeof(params))
		die("Cannot load SDRAM parameter file for RAM code: %#x", ramcode);

	return &params;
}
