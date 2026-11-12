/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
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

	/* hook for platform specific policy configuration */
	if (qclib_mainboard_override(table)) {
		printk(BIOS_ERR, "qclib_mainboard_override failed\n");
		return -1;
	}
	return 0;
}
