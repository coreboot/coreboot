/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <console/console.h>
#include <soc/qclib_common.h>
#include <device/mmio.h>
#include <soc/symbols_common.h>

#define LONG_SYS_DCB_REG 0x7801C0
#define FUSE_BIT 15

__weak int qclib_mainboard_override(struct qclib_cb_if_table *table) { return 0; }

static int dcb_fuse_longsys1p8(void)
{
	unsigned int fuse_value, bit_value;
	fuse_value = read32((unsigned int *)LONG_SYS_DCB_REG);
	bit_value = (fuse_value >> FUSE_BIT) & 0x1;
	return bit_value;
}

const char *qclib_override_soc_file(enum qclib_cbfs_file file)
{
	if ((file == QCLIB_CBFS_DCB) && dcb_fuse_longsys1p8()) {
		printk(BIOS_INFO, "Using DCB for Longsys 1.8V memory based on fuse setting\n");
		return CONFIG_CBFS_PREFIX "/dcb_longsys1p8";
	} else {
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

	/* hook for platform specific policy configuration */
	if (qclib_mainboard_override(table)) {
		printk(BIOS_ERR, "qclib_mainboard_override failed\n");
		return -1;
	}

	return 0;
}
