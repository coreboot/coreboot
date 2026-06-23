/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <soc/addressmap.h>
#include <soc/cmd_db.h>
#include <soc/platform_info.h>
#include <soc/rpmh.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>
#include <types.h>

static enum cb_err ufs_vrm_enable(const char *name, int uv)
{
	struct rpmh_vreg vreg;
	int rc;

	rc = rpmh_regulator_init(&vreg, name, RPMH_REGULATOR_TYPE_VRM);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to init regulator %s\n", name);
		return CB_ERR;
	}

	rc = rpmh_regulator_vrm_set_voltage(&vreg, uv, uv, true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to set voltage for %s to %d uV\n",
		       name, uv);
		return CB_ERR;
	}

	rc = rpmh_regulator_vrm_set_mode(&vreg, UFS_VRM_HPM_MODE, true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to set HPM mode for %s\n", name);
		return CB_ERR;
	}

	rc = rpmh_regulator_enable(&vreg, true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to enable regulator %s\n", name);
		return CB_ERR;
	}

	printk(BIOS_DEBUG, "UFS: %s enabled at %d mV HPM\n", name, uv / 1000);
	return CB_SUCCESS;
}

enum cb_err ufs_rpmh_init(void)
{
	struct rpmh_vreg clk;
	int rc;

	if (platform_get_soc_id() != SOC_ID_X1P42100) {
		printk(BIOS_DEBUG, "UFS: Skipping RPMh init for non-X1P42100 SoC\n");
		return CB_SUCCESS;
	}

	if (cmd_db_ready() != CB_SUCCESS) {
		rc = cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE);
		if (rc != CB_SUCCESS) {
			printk(BIOS_ERR, "UFS: cmd_db init failed\n");
			return CB_ERR;
		}
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "UFS: rpmh_rsc init failed\n");
		return CB_ERR;
	}

	if (ufs_vrm_enable(UFS_VCCQ2_RAIL, UFS_VCCQ2_UV) != CB_SUCCESS)
		return CB_ERR;

	if (ufs_vrm_enable(UFS_VCC_RAIL, UFS_VCC_UV) != CB_SUCCESS)
		return CB_ERR;

	if (ufs_vrm_enable(UFS_VCCQ_RAIL_1, UFS_VCCQ_UV) != CB_SUCCESS)
		return CB_ERR;

	if (ufs_vrm_enable(UFS_VCC_AUX_RAIL, UFS_VCC_UV) != CB_SUCCESS)
		return CB_ERR;

	if (ufs_vrm_enable(UFS_VCCQ_RAIL_2, UFS_VCCQ_UV) != CB_SUCCESS)
		return CB_ERR;

	printk(BIOS_INFO, "UFS: All power rails enabled\n");

	rc = rpmh_regulator_init(&clk, UFS_CLK_RAIL, RPMH_REGULATOR_TYPE_XOB);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to init %s\n", UFS_CLK_RAIL);
		return CB_ERR;
	}
	rc = rpmh_regulator_enable(&clk, true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to enable %s\n", UFS_CLK_RAIL);
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "UFS: %s (lbnnclk8) enabled\n", UFS_CLK_RAIL);

	rc = rpmh_regulator_init(&clk, UFS_XO_CLK_RAIL, RPMH_REGULATOR_TYPE_ARC);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to init %s\n", UFS_XO_CLK_RAIL);
		return CB_ERR;
	}
	rc = rpmh_regulator_arc_set_level(&clk, UFS_XO_CLK_LEVEL_ON, true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to set %s to ON\n", UFS_XO_CLK_RAIL);
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "UFS: %s (xo2clk) enabled\n", UFS_XO_CLK_RAIL);

	rc = rpmh_regulator_init(&clk, UFS_CX_RAIL, RPMH_REGULATOR_TYPE_ARC);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to init %s\n", UFS_CX_RAIL);
		return CB_ERR;
	}
	rc = rpmh_regulator_arc_set_level(&clk, RPMH_REGULATOR_LEVEL_NOM_L1,
					  true, false);
	if (rc) {
		printk(BIOS_ERR, "UFS: Failed to set %s to NOM_L1\n", UFS_CX_RAIL);
		return CB_ERR;
	}
	printk(BIOS_DEBUG, "UFS: %s (CX rail) set to NOM_L1 (level %d)\n",
	       UFS_CX_RAIL, RPMH_REGULATOR_LEVEL_NOM_L1);

	printk(BIOS_INFO, "UFS: All clocks enabled\n");
	return CB_SUCCESS;
}
