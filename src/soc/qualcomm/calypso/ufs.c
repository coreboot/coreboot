/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/cmd_db.h>
#include <soc/rpmh.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>
#include <types.h>

static enum cb_err ufs_vrm_disable(const char *name)
{
	struct rpmh_vreg vreg;
	int rc;

	rc = rpmh_regulator_init(&vreg, name, RPMH_REGULATOR_TYPE_VRM);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to init regulator %s rc=%d\n", name, rc);
		return CB_ERR;
	}

	rc = rpmh_regulator_disable(&vreg, true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to disable regulator %s rc=%d\n", name, rc);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "UFS: %s disabled\n", name);
	return CB_SUCCESS;
}

enum cb_err ufs_rpmh_disable(void)
{
	int rc;

	if (cmd_db_ready() != CB_SUCCESS) {
		rc = cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE);
		if (rc != CB_SUCCESS) {
			printk(BIOS_ERR, "UFS: cmd_db init failed\n");
			return CB_ERR;
		}
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "UFS: rpmh_rsc_init failed rc=%d, UFS rails may leak power\n", rc);
		return CB_ERR;
	}

	if (ufs_vrm_disable(UFS_VCC_RAIL) != CB_SUCCESS)
		return CB_ERR;

	if (ufs_vrm_disable(UFS_VCCQ_RAIL) != CB_SUCCESS)
		return CB_ERR;

	printk(BIOS_INFO, "UFS: %s and %s disabled\n", UFS_VCC_RAIL, UFS_VCCQ_RAIL);
	return CB_SUCCESS;
}
