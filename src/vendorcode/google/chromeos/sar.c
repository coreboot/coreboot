/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <drivers/vpd/vpd.h>
#include <fw_config.h>
#include <lib.h>
#include <sar.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <types.h>

#define LEGACY_BYTES_PER_GEO_OFFSET	6
#define LEGACY_BYTES_PER_SAR_LIMIT	10
#define LEGACY_NUM_SAR_LIMITS		4
#define LEGACY_SAR_BIN_SIZE		81
#define LEGACY_SAR_WGDS_BIN_SIZE	119
#define LEGACY_SAR_NUM_WGDS_GROUPS	3

static uint8_t *wifi_hextostr(const char *sar_str, size_t str_len, size_t *sar_bin_len,
			      bool legacy_hex_format)
{
	uint8_t *sar_bin = NULL;
	size_t bin_len;

	if (!legacy_hex_format) {
		sar_bin = malloc(str_len);
		if (!sar_bin) {
			printk(BIOS_ERR, "Failed to allocate space for SAR binary!\n");
			return NULL;
		}

		memcpy(sar_bin, sar_str, str_len);
		*sar_bin_len = str_len;
	} else {
		bin_len = ((str_len - 1) / 2);
		sar_bin = malloc(bin_len);
		if (!sar_bin) {
			printk(BIOS_ERR, "Failed to allocate space for SAR binary!\n");
			return NULL;
		}

		if (hexstrtobin(sar_str, (uint8_t *)sar_bin, bin_len) != bin_len) {
			printk(BIOS_ERR, "sar_limits contains non-hex value!\n");
			free(sar_bin);
			return NULL;
		}

		*sar_bin_len = bin_len;
	}

	return sar_bin;
}

static int sar_table_size(const struct sar_profile *sar)
{
	if (sar == NULL)
		return 0;

	return (sizeof(struct sar_profile) + ((1 + sar->dsar_set_count) * sar->chains_count *
					      sar->subbands_count));
}

static int wgds_table_size(const struct geo_profile *geo)
{
	if (geo == NULL)
		return 0;

	return sizeof(struct geo_profile) + (geo->chains_count * geo->bands_count);
}

static int gain_table_size(const struct gain_profile *gain)
{
	if (gain == NULL)
		return 0;

	return sizeof(struct gain_profile) + (gain->chains_count * gain->bands_count);
}

static int sar_avg_table_size(const struct avg_profile *sar_avg)
{
	if (sar_avg == NULL)
		return 0;

	return sizeof(struct avg_profile);
}

static int dsm_table_size(const struct dsm_profile *dsm)
{
	if (dsm == NULL)
		return 0;

	return sizeof(struct dsm_profile);
}

static bool valid_legacy_length(size_t bin_len)
{
	if (bin_len == LEGACY_SAR_WGDS_BIN_SIZE)
		return true;

	if (bin_len == LEGACY_SAR_BIN_SIZE && !CONFIG(GEO_SAR_ENABLE))
		return true;

	return false;
}

static int sar_header_size(void)
{
	return (MAX_PROFILE_COUNT * sizeof(uint16_t)) + sizeof(struct sar_header);
}

static int fill_wifi_sar_limits(union wifi_sar_limits *sar_limits, const uint8_t *sar_bin,
				size_t sar_bin_size)
{
	struct sar_header *header;
	size_t i = 0, expected_sar_bin_size;
	size_t header_size = sar_header_size();

	if (sar_bin_size < header_size) {
		printk(BIOS_ERR, "Invalid SAR format!\n");
		return -1;
	}

	header = (struct sar_header *)sar_bin;

	if (header->version != SAR_FILE_REVISION) {
		printk(BIOS_ERR, "Invalid SAR file version: %d!\n", header->version);
		return -1;
	}

	for (i = 0; i < MAX_PROFILE_COUNT; i++) {
		if (header->offsets[i] > sar_bin_size) {
			printk(BIOS_ERR, "Offset is outside the file size!\n");
			return -1;
		}

		if (header->offsets[i])
			sar_limits->profile[i] = (void *) (sar_bin + header->offsets[i]);
	}

	expected_sar_bin_size = header_size;
	expected_sar_bin_size += sar_table_size(sar_limits->sar);
	expected_sar_bin_size += wgds_table_size(sar_limits->wgds);
	expected_sar_bin_size += gain_table_size(sar_limits->ppag);
	expected_sar_bin_size += sar_avg_table_size(sar_limits->wtas);
	expected_sar_bin_size += dsm_table_size(sar_limits->dsm);

	if (sar_bin_size != expected_sar_bin_size) {
		printk(BIOS_ERR, "Invalid SAR size, expected: %zu, obtained: %zu\n",
		       expected_sar_bin_size, sar_bin_size);
		return -1;
	}

	return 0;
}

static int fill_wifi_sar_limits_legacy(union wifi_sar_limits *sar_limits,
				       const uint8_t *sar_bin, size_t sar_bin_size)
{
	uint8_t *new_sar_bin;
	size_t size = sar_bin_size + sizeof(struct sar_profile);

	if (CONFIG(GEO_SAR_ENABLE))
		size += sizeof(struct geo_profile);

	new_sar_bin = malloc(size);
	if (!new_sar_bin) {
		printk(BIOS_ERR, "Failed to allocate space for SAR binary!\n");
		return -1;
	}

	sar_limits->sar = (struct sar_profile *) new_sar_bin;
	sar_limits->sar->revision = 0;
	sar_limits->sar->dsar_set_count = CONFIG_DSAR_SET_NUM;
	sar_limits->sar->chains_count = SAR_REV0_CHAINS_COUNT;
	sar_limits->sar->subbands_count = SAR_REV0_SUBBANDS_COUNT;
	memcpy(&sar_limits->sar->sar_table, sar_bin,
	       LEGACY_BYTES_PER_SAR_LIMIT * LEGACY_NUM_SAR_LIMITS);

	if (!CONFIG(GEO_SAR_ENABLE))
		return 0;

	sar_limits->wgds = (struct geo_profile *)(new_sar_bin +
						  sar_table_size(sar_limits->sar));
	sar_limits->wgds->revision = 0;
	sar_limits->wgds->chains_count = LEGACY_SAR_NUM_WGDS_GROUPS;
	sar_limits->wgds->bands_count = LEGACY_BYTES_PER_GEO_OFFSET;
	memcpy(&sar_limits->wgds->wgds_table,
	       sar_bin + LEGACY_BYTES_PER_SAR_LIMIT * LEGACY_NUM_SAR_LIMITS + REVISION_SIZE,
	       LEGACY_BYTES_PER_GEO_OFFSET * LEGACY_SAR_NUM_WGDS_GROUPS);

	return 0;
}

/*
 * Retrieve WiFi SAR limits data from CBFS and decode it
 * Legacy WiFi SAR data is expected in the format: [<WRDD><EWRD>][WGDS]
 *
 * [<WRDD><EWRD>] = NUM_SAR_LIMITS * BYTES_PER_SAR_LIMIT bytes.
 * [WGDS]=[WGDS_REVISION][WGDS_DATA]
 *
 * Current SAR configuration data is expected in the format:
 * "$SAR" Marker
 * Version
 * Offset count
 * Offsets
 * [SAR_REVISION,DSAR_SET_COUNT,CHAINS_COUNT,SUBBANDS_COUNT <WRDD>[EWRD]]
 * [WGDS_REVISION,CHAINS_COUNT,SUBBANDS_COUNT<WGDS_DATA>]
 * [PPAG_REVISION,MODE,CHAINS_COUNT,SUBBANDS_COUNT<PPAG_DATA>]
 * [WTAS_REVISION, WTAS_DATA]
 * [DSM_RETURN_VALUES]
 *
 * The configuration data will always have the revision added in the file for each of the
 * block, based on the revision number and validity, size of the specific block will be
 * calculated.
 *
 * [WGDS_DATA] = [GROUP#0][GROUP#1][GROUP#2]
 *
 * [GROUP#<i>] =
 *	Supported by Revision 0, 1 and 2
 *              [2.4Ghz - Max Allowed][2.4Ghz - Chain A Offset][2.4Ghz - Chain B Offset]
 *              [5Ghz - Max Allowed][5Ghz - Chain A Offset][5Ghz - Chain B Offset]
 *	Supported by Revision 1 and 2
 *              [6Ghz - Max Allowed][6Ghz - Chain A Offset][6Ghz - Chain B Offset]
 *
 * [GROUP#0] is for FCC
 * [GROUP#1] is for Europe/Japan
 * [GROUP#2] is for ROW
 *
 * [PPAG_DATA] = [ANT_gain Table Chain A] [ANT_gain Table Chain A]
 *
 * [ANT_gain Table] =
 *	Supported by Revision 0, 1 and 2
 *              [Antenna gain used for 2400MHz frequency]
 *              [Antenna gain used for 5150-5350MHz frequency]
 *              [Antenna gain used for 5350-5470MHz frequency]
 *              [Antenna gain used for 5470-5725MHz frequency]
 *              [Antenna gain used for 5725-5945MHz frequency]
 *	Supported by Revision 1 and 2
 *              [Antenna gain used for 5945-6165MHz frequency]
 *              [Antenna gain used for 6165-6405MHz frequency]
 *              [Antenna gain used for 6405-6525MHz frequency]
 *              [Antenna gain used for 6525-6705MHz frequency]
 *              [Antenna gain used for 6705-6865MHz frequency]
 *              [Antenna gain used for 6865-7105MHz frequency]
 *
 * [WTAS_DATA] =
 *      [Enable/disable the TAS feature]
 *      [Number of blocked countries that are not approved by the OEM to support this feature]
 *      [deny_list_entry_<1-16>: ISO country code to block]
 */
int get_wifi_sar_limits(union wifi_sar_limits *sar_limits)
{
	const char *filename;
	size_t sar_bin_len, sar_str_len;
	uint8_t *sar_bin;
	char *sar_str;
	int ret = -1;
	bool legacy_hex_format = false;

	filename = get_wifi_sar_cbfs_filename();
	if (filename == NULL) {
		printk(BIOS_ERR, "Filename missing for CBFS SAR file!\n");
		return ret;
	}

	sar_str = cbfs_map(filename, &sar_str_len);
	if (!sar_str) {
		printk(BIOS_ERR, "Failed to get the %s file size!\n", filename);
		return ret;
	}

	if (strncmp(sar_str, SAR_STR_PREFIX, SAR_STR_PREFIX_SIZE) == 0) {
		legacy_hex_format = false;
	} else if (valid_legacy_length(sar_str_len)) {
		legacy_hex_format = true;
	} else {
		printk(BIOS_ERR, "Invalid SAR format!\n");
		goto error;
	}

	sar_bin = wifi_hextostr(sar_str, sar_str_len, &sar_bin_len, legacy_hex_format);
	if (sar_bin == NULL) {
		printk(BIOS_ERR, "Failed to parse SAR file %s\n", filename);
		goto error;
	}

	memset(sar_limits, 0, sizeof(*sar_limits));
	if (legacy_hex_format) {
		ret = fill_wifi_sar_limits_legacy(sar_limits, sar_bin, sar_bin_len);
		free(sar_bin);
	} else {
		ret = fill_wifi_sar_limits(sar_limits, sar_bin, sar_bin_len);
		if (ret < 0)
			free(sar_bin);
	}

error:
	cbfs_unmap(sar_str);
	return ret;
}

__weak
const char *get_wifi_sar_cbfs_filename(void)
{
	return WIFI_SAR_CBFS_DEFAULT_FILENAME;
}

char *get_wifi_sar_fw_config_filename(const struct fw_config_field *field)
{
	uint64_t sar_id = fw_config_get_field(field);
	if (sar_id == UNDEFINED_FW_CONFIG) {
		printk(BIOS_WARNING, "fw_config unprovisioned, set sar filename to NULL\n");
		return NULL;
	}
	static char filename[20];
	printk(BIOS_INFO, "Use wifi_sar_%lld.hex.\n", sar_id);
	if (snprintf(filename, sizeof(filename), "wifi_sar_%lld.hex", sar_id) < 0) {
		printk(BIOS_ERR, "Error occurred with snprintf, set sar filename to NULL\n");
		return NULL;
	}
	return filename;
}
