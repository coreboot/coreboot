/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <fmap.h>
#include <console/console.h>
#include <soc/symbols.h>
#include <soc/qclib_common.h>

int qclib_soc_blob_load(void)
{
	size_t size;
	ssize_t ssize;

	/* Attempt to load PMICCFG Blob */
	size = cbfs_boot_load_file(CONFIG_CBFS_PREFIX "/pmiccfg",
			_pmic, REGION_SIZE(pmic), CBFS_TYPE_RAW);
	if (!size)
		return -1;
	qclib_add_if_table_entry(QCLIB_TE_PMIC_SETTINGS, _pmic, size, 0);

	/* Attempt to load DCB Blob */
	size = cbfs_boot_load_file(CONFIG_CBFS_PREFIX "/dcb",
			_dcb, REGION_SIZE(dcb), CBFS_TYPE_RAW);
	if (!size)
		return -1;
	qclib_add_if_table_entry(QCLIB_TE_DCB_SETTINGS, _dcb, size, 0);

	/* Attempt to load Limits Config Blob */
	ssize = fmap_read_area(QCLIB_FR_LIMITS_CFG_DATA, _limits_cfg,
		REGION_SIZE(limits_cfg));
	if (ssize < 0)
		return -1;
	qclib_add_if_table_entry(QCLIB_TE_LIMITS_CFG_DATA,
				 _limits_cfg, ssize, 0);

	return 0;
}
