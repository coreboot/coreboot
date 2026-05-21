/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <soc/qclib_common.h>
#include <device/mmio.h>
#include <soc/symbols_common.h>
#include <soc/addressmap.h>
#include <soc/platform_info.h>

__weak int qclib_mainboard_override(struct qclib_cb_if_table *table) { return 0; }

bool qclib_check_dload_mode(void)
{
	if (!CONFIG(QC_RAMDUMP_ENABLE))
		return false;

	uint32_t boot_misc_detect = read32((void *)TCSR_BOOT_MISC_DETECT);

	if (boot_misc_detect & DLOAD_BOTH) {
		printk(BIOS_DEBUG, "Download mode detected: 0x%x\n", boot_misc_detect);
		return true;
	}

	printk(BIOS_DEBUG, "Download mode not enabled (TCSR value: 0x%x)\n",
	       boot_misc_detect);
	return false;
}

struct qclib_file_map {
	const char *dcb;
	const char *dtb;
	const char *cpr;
};

static const struct qclib_file_map hamoa_files = {
	.dcb = CONFIG_CBFS_PREFIX "/dcb_hamoa",
	.dtb = CONFIG_CBFS_PREFIX "/dtb_hamoa",
	.cpr = CONFIG_CBFS_PREFIX "/cpr_hamoa",
};

static const struct qclib_file_map x1p_files = {
	.dcb = CONFIG_CBFS_PREFIX "/dcb_x1p42100",
	.dtb = CONFIG_CBFS_PREFIX "/dtb_x1p42100",
	.cpr = CONFIG_CBFS_PREFIX "/cpr_x1p42100",
};

static const struct qclib_file_map *get_soc_file_map(void)
{
	enum qclib_soc_id soc = platform_get_soc_id();

	if (soc == SOC_ID_HAMOA)
		return &hamoa_files;

	/* Fall back to x1p42100 configuration by default */
	return &x1p_files;
}

const char *qclib_override_soc_file(enum qclib_cbfs_file file)
{
	const struct qclib_file_map *map = get_soc_file_map();

	switch (file) {
	case QCLIB_CBFS_DCB:
		return map->dcb;
	case QCLIB_CBFS_DTB:
		return map->dtb;
	case QCLIB_CBFS_CPR:
		return map->cpr;
	default:
		return NULL;
	}
}

int qclib_soc_override(struct qclib_cb_if_table *table)
{
	ssize_t data_size;

	/* Attempt to load DCB Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_DCB), _dcb, REGION_SIZE(dcb));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /dcb failed\n", __func__);
		return -1;
	}
	qclib_add_if_table_entry(QCLIB_TE_DCB_SETTINGS, _dcb, data_size, 0);

	/* Attempt to load DTB Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_DTB), _dtb, REGION_SIZE(dtb));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /dtb failed\n", __func__);
		return -1;
	}
	qclib_add_if_table_entry(QCLIB_TE_DTB_SETTINGS, _dtb, data_size, 0);

	/* Attempt to load CPR Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_CPR), _cpr_settings, REGION_SIZE(cpr_settings));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /cpr failed\n", __func__);
		return -1;
	}
	qclib_add_if_table_entry(QCLIB_TE_CPR_SETTINGS, _cpr_settings, data_size, 0);

	if (!qclib_check_dload_mode())	{
		/* Attempt to load shrm_meta Blob */
		data_size = cbfs_load(qclib_file(QCLIB_CBFS_SHRM_META),
				_qc_blob_meta, REGION_SIZE(qc_blob_meta));
		if (!data_size) {
			printk(BIOS_ERR, "[%s] /shrm_meta failed\n", __func__);
			return -1;
		}

		qclib_add_if_table_entry(QCLIB_TE_SHRM_META_SETTINGS, _qc_blob_meta, data_size, 0);
	}

	/* hook for platform specific policy configuration */
	if (qclib_mainboard_override(table)) {
		printk(BIOS_ERR, "qclib_mainboard_override failed\n");
		return -1;
	}

	return 0;
}
