/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/qclib_common.h>
#include <soc/symbols_common.h>

__weak int qclib_mainboard_override(struct qclib_cb_if_table *table) { return 0; }

int qclib_soc_override(struct qclib_cb_if_table *table)
{
	size_t data_size;

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

	if (!qclib_check_dload_mode()) {
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
