/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/qclib_common.h>
#include <device/mmio.h>

#define LONG_SYS_DCB_REG 0x7801C0
#define FUSE_BIT 15

static int dcb_fuse_longsys1p8(void)
{
	unsigned int fuse_value, bit_value;
	fuse_value = read32((unsigned int *)LONG_SYS_DCB_REG);
	bit_value = (fuse_value >> FUSE_BIT) & 0x1;
	return bit_value;
}

const char *qclib_file(enum qclib_cbfs_file file)
{
	if ((file == QCLIB_CBFS_DCB) && dcb_fuse_longsys1p8()) {
		printk(BIOS_INFO, "Using DCB for Longsys 1.8V memory based on fuse setting\n");
		return CONFIG_CBFS_PREFIX "/dcb_longsys1p8";
	} else {
		return qclib_file_default(file);
	}
}
