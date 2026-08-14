/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <cbfs.h>
#include <console/console.h>
#include <device/mmio.h>
#include <program_loading.h>
#include <soc/addressmap.h>
#include <soc/qclib_common.h>
#include <soc/symbols_common.h>

__weak int qclib_mainboard_override(struct qclib_cb_if_table *table) { return 0; }

void qupv3_fw_load(void)
{
	struct prog qupv3_fw_prog =
		PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/qupv3_fw");

	printk(BIOS_INFO, "Loading QUPv3 firmware\n");
	if (!selfload(&qupv3_fw_prog))
		die("SOC image: QUPv3 FW load failed");

	printk(BIOS_INFO, "QUPv3 firmware loaded successfully\n");
}

bool qclib_check_dload_mode(void)
{
	uint32_t boot_misc_detect = read32((void *)TCSR_BOOT_MISC_DETECT);

	if (boot_misc_detect & DLOAD_BOTH) {
		printk(BIOS_DEBUG, "Download mode detected: 0x%x\n", boot_misc_detect);
		return true;
	}

	printk(BIOS_DEBUG, "Download mode not enabled (TCSR value: 0x%x)\n",
			boot_misc_detect);
	return false;
}

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
