/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <soc/qclib_common.h>
#include <device/mmio.h>
#include <soc/symbols_common.h>

int qclib_soc_override(struct qclib_cb_if_table *table)
{
	ssize_t data_size;

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

	/* Attempt to load shrm_meta Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_SHRM_META),
			_qc_blob_meta, REGION_SIZE(qc_blob_meta));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /shrm_meta failed\n", __func__);
		return -1;
	}

	qclib_add_if_table_entry(QCLIB_TE_SHRM_META_SETTINGS, _qc_blob_meta, data_size, 0);

	return 0;
}
