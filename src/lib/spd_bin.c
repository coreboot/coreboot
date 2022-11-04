/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <memory_info.h>
#include <spd_bin.h>
#include <string.h>
#include <device/dram/ddr3.h>

void dump_spd_info(struct spd_block *blk)
{
	u8 i;

	for (i = 0; i < CONFIG_DIMM_MAX; i++)
		if (blk->spd_array[i] != NULL && blk->spd_array[i][0] != 0) {
			printk(BIOS_DEBUG, "SPD @ 0x%02X\n", blk->addr_map[i]);
			print_spd_info(blk->spd_array[i]);
		}
}

const char * __weak mainboard_get_dram_part_num(void)
{
	/* Default weak implementation, no need to override part number. */
	return NULL;
}

static bool use_ddr4_params(int dram_type)
{
	switch (dram_type) {
	case SPD_DRAM_DDR3:
	case SPD_DRAM_LPDDR3_INTEL:
		return false;
	/* Below DDR type share the same attributes */
	case SPD_DRAM_LPDDR3_JEDEC:
	case SPD_DRAM_DDR4:
	case SPD_DRAM_DDR5:
	case SPD_DRAM_LPDDR5:
	case SPD_DRAM_LPDDR4:
	case SPD_DRAM_LPDDR4X:
		return true;
	default:
		printk(BIOS_NOTICE, "Defaulting to using DDR4 params. Please add dram_type check for %d to %s\n",
			dram_type, __func__);
		return true;
	}
}

static const char *spd_get_module_type_string(int dram_type)
{
	switch (dram_type) {
	case SPD_DRAM_DDR3:
		return "DDR3";
	case SPD_DRAM_LPDDR3_INTEL:
	case SPD_DRAM_LPDDR3_JEDEC:
		return "LPDDR3";
	case SPD_DRAM_DDR4:
		return "DDR4";
	case SPD_DRAM_LPDDR4:
		return "LPDDR4";
	case SPD_DRAM_LPDDR4X:
		return "LPDDR4X";
	case SPD_DRAM_DDR5:
		return "DDR5";
	case SPD_DRAM_LPDDR5:
		return "LPDDR5";
	case SPD_DRAM_LPDDR5X:
		return "LPDDR5X";
	}
	return "UNKNOWN";
}

static int spd_get_banks(const uint8_t spd[], int dram_type)
{
	static const int ddr3_banks[4] = { 8, 16, 32, 64 };
	static const int ddr4_banks[10] = { 4, 8, -1, -1, 8, 16, -1, -1, 16, 32 };
	int index = (spd[SPD_DENSITY_BANKS] >> 4) & 0xf;

	if (use_ddr4_params(dram_type)) {
		if (index >= ARRAY_SIZE(ddr4_banks))
			return -1;
		return ddr4_banks[index];
	} else {
		if (index >= ARRAY_SIZE(ddr3_banks))
			return -1;
		return ddr3_banks[index];
	}
}

static int spd_get_capmb(const uint8_t spd[])
{
	static const int spd_capmb[13] = { 1, 2, 4, 8, 16, 32, 64,
					   128, 48, 96, 12, 24, 72 };
	int index = spd[SPD_DENSITY_BANKS] & 0xf;
	if (index >= ARRAY_SIZE(spd_capmb))
		return -1;
	return spd_capmb[index] * 256;
}

static int spd_get_rows(const uint8_t spd[])
{
	static const int spd_rows[7]  = { 12, 13, 14, 15, 16, 17, 18 };
	int index = (spd[SPD_ADDRESSING] >> 3) & 7;
	if (index >= ARRAY_SIZE(spd_rows))
		return -1;
	return spd_rows[index];
}

static int spd_get_cols(const uint8_t spd[])
{
	static const int spd_cols[4]  = { 9, 10, 11, 12 };
	int index = spd[SPD_ADDRESSING] & 7;
	if (index >= ARRAY_SIZE(spd_cols))
		return -1;
	return spd_cols[index];
}

static int spd_get_ranks(const uint8_t spd[], int dram_type)
{
	static const int spd_ranks[8] = { 1, 2, 3, 4, 5, 6, 7, 8 };
	int organ_offset = use_ddr4_params(dram_type) ? DDR4_ORGANIZATION
						      : DDR3_ORGANIZATION;
	int index = (spd[organ_offset] >> 3) & 7;
	if (index >= ARRAY_SIZE(spd_ranks))
		return -1;
	return spd_ranks[index];
}

static int spd_get_devw(const uint8_t spd[], int dram_type)
{
	static const int spd_devw[4]  = { 4, 8, 16, 32 };
	int organ_offset = use_ddr4_params(dram_type) ? DDR4_ORGANIZATION
						      : DDR3_ORGANIZATION;
	int index = spd[organ_offset] & 7;
	if (index >= ARRAY_SIZE(spd_devw))
		return -1;
	return spd_devw[index];
}

static int spd_get_busw(const uint8_t spd[], int dram_type)
{
	static const int spd_busw[4]  = { 8, 16, 32, 64 };
	int busw_offset = use_ddr4_params(dram_type) ? DDR4_BUS_DEV_WIDTH
						     : DDR3_BUS_DEV_WIDTH;
	int index = spd[busw_offset] & 7;
	if (index >= ARRAY_SIZE(spd_busw))
		return -1;
	return spd_busw[index];
}

static void spd_get_name(const uint8_t spd[], int type, const char **spd_name, size_t *len)
{
	*spd_name = mainboard_get_dram_part_num();
	if (*spd_name != NULL) {
		*len = strlen(*spd_name);
		return;
	}

	switch (type) {
	case SPD_DRAM_DDR3:
		*spd_name = (const char *) &spd[DDR3_SPD_PART_OFF];
		*len = DDR3_SPD_PART_LEN;
		break;
	case SPD_DRAM_LPDDR3_INTEL:
		*spd_name = (const char *) &spd[LPDDR3_SPD_PART_OFF];
		*len = LPDDR3_SPD_PART_LEN;
		break;
	/* LPDDR3, LPDDR4 and DDR4 have same part number offset and length */
	case SPD_DRAM_LPDDR3_JEDEC:
	case SPD_DRAM_DDR4:
	case SPD_DRAM_DDR5:
	case SPD_DRAM_LPDDR5:
	case SPD_DRAM_LPDDR4:
	case SPD_DRAM_LPDDR4X:
		if (spd[DDR4_SPD_PART_OFF]) {
			*spd_name = (const char *) &spd[DDR4_SPD_PART_OFF];
			*len = DDR4_SPD_PART_LEN;
		}
		break;
	default:
		*len = 0;
		break;
	}
}

void print_spd_info(uint8_t spd[])
{
	const char *nameptr = NULL;
	size_t len;
	int type  = spd[SPD_DRAM_TYPE];
	int banks = spd_get_banks(spd, type);
	int capmb = spd_get_capmb(spd);
	int rows  = spd_get_rows(spd);
	int cols  = spd_get_cols(spd);
	int ranks = spd_get_ranks(spd, type);
	int devw  = spd_get_devw(spd, type);
	int busw  = spd_get_busw(spd, type);

	/* Module type */
	printk(BIOS_INFO, "SPD: module type is %s\n",
		spd_get_module_type_string(type));
	/* Module Part Number */
	spd_get_name(spd, type, &nameptr, &len);
	if (nameptr)
		printk(BIOS_INFO, "SPD: module part number is %.*s\n", (int) len, nameptr);

	printk(BIOS_INFO,
		"SPD: banks %d, ranks %d, rows %d, columns %d, density %d Mb\n",
		banks, ranks, rows, cols, capmb);
	printk(BIOS_INFO, "SPD: device width %d bits, bus width %d bits\n",
		devw, busw);

	if (capmb > 0 && busw > 0 && devw > 0 && ranks > 0) {
		/* SIZE = DENSITY / 8 * BUS_WIDTH / SDRAM_WIDTH * RANKS */
		printk(BIOS_INFO, "SPD: module size is %u MB (per channel)\n",
			capmb / 8 * busw / devw * ranks);
	}
}

uintptr_t spd_cbfs_map(u8 spd_index)
{
	enum cbfs_type cbfs_type = CBFS_TYPE_SPD;
	size_t size;

	void *map = cbfs_type_map("spd.bin", &size, &cbfs_type);
	if (!map || size < (spd_index + 1) * CONFIG_DIMM_SPD_SIZE)
		return 0;

	return (uintptr_t)map + spd_index * CONFIG_DIMM_SPD_SIZE;
}

#if CONFIG_DIMM_SPD_SIZE == 128
int read_ddr3_spd_from_cbfs(u8 *buf, int idx)
{
	const int SPD_CRC_HI = 127;
	const int SPD_CRC_LO = 126;

	char *spd_file;
	size_t spd_file_len = 0;
	size_t min_len = (idx + 1) * CONFIG_DIMM_SPD_SIZE;

	spd_file = cbfs_map("spd.bin", &spd_file_len);
	if (!spd_file)
		printk(BIOS_EMERG, "file [spd.bin] not found in CBFS");
	if (spd_file_len < min_len)
		printk(BIOS_EMERG, "Missing SPD data.");
	if (!spd_file || spd_file_len < min_len)
		return -1;

	memcpy(buf, spd_file + (idx * CONFIG_DIMM_SPD_SIZE),
		CONFIG_DIMM_SPD_SIZE);
	cbfs_unmap(spd_file);

	u16 crc = spd_ddr3_calc_crc(buf, CONFIG_DIMM_SPD_SIZE);

	if (((buf[SPD_CRC_LO] == 0) && (buf[SPD_CRC_HI] == 0))
		|| (buf[SPD_CRC_LO] != (crc & 0xff))
		|| (buf[SPD_CRC_HI] != (crc >> 8))) {
		printk(BIOS_WARNING,
			"SPD CRC %02x%02x is invalid, should be %04x\n",
			buf[SPD_CRC_HI], buf[SPD_CRC_LO], crc);
		buf[SPD_CRC_LO] = crc & 0xff;
		buf[SPD_CRC_HI] = crc >> 8;
		u16 i;
		printk(BIOS_WARNING, "\nDisplay the SPD");
		for (i = 0; i < CONFIG_DIMM_SPD_SIZE; i++) {
			if ((i % 16) == 0x00)
				printk(BIOS_WARNING, "\n%02x:  ", i);
			printk(BIOS_WARNING, "%02x ", buf[i]);
		}
		printk(BIOS_WARNING, "\n");
	}
	return 0;
}
#endif
