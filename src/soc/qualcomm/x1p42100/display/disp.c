/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/device.h>
#include <console/console.h>
#include <timer.h>
#include <soc/addressmap.h>
#include <soc/cmd_db.h>
#include <soc/rpmh.h>
#include <soc/rpmh_bcm.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_regulator.h>

/**
 * display_rpmh_init() - Initialize RPMh for display power management
 *
 * Initializes the RPMh (Resource Power Manager-Hardened) subsystem for display:
 * - Waits for AOP boot
 * - Initializes Command DB and RPMh RSC (Resource State Coordinator)
 * - Configures MMCX ARC regulator for display power rail
 * - Votes for MM0 BCM (Bus Clock Manager) for display bus clocks
 *
 * Return: CB_SUCCESS on success, CB_ERR on failure
 */
enum cb_err display_rpmh_init(void)
{
	enum cb_err ret;
	struct rpmh_vreg arc_reg;
	int rc;
	volatile u32 *boot_cookie = (volatile u32 *)AOP_BOOT_COOKIE_ADDR;

	if (!wait_us(AOP_BOOT_TIMEOUT_US, *boot_cookie == AOP_BOOT_COOKIE)) {
		printk(BIOS_ERR, "AOP not booted after %dus (cookie: 0x%x, expected: 0x%x)\n",
			AOP_BOOT_TIMEOUT_US, *boot_cookie, AOP_BOOT_COOKIE);
		return CB_ERR;
	}

	printk(BIOS_INFO, "AOP boot detected (cookie: 0x%x)\n", *boot_cookie);

	ret = cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE);
	if (ret != CB_SUCCESS) {
		printk(BIOS_ERR, "Failed to initialize Command DB\n");
		return CB_ERR;
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "Failed to initialize RPMh RSC\n");
		return CB_ERR;
	}

	rc = rpmh_regulator_init(&arc_reg, "mmcx.lvl", RPMH_REGULATOR_TYPE_ARC);
	if (rc) {
		printk(BIOS_ERR, "Failed to initialize display power rail mmcx ARC regulator\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "ARC regulator initialized successfully\n");

	rc = rpmh_regulator_arc_set_level(&arc_reg, RPMH_REGULATOR_LEVEL_MIN_MM0, true, false);
	if (rc) {
		printk(BIOS_ERR, "Failed to set ARC level\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "ARC level was set successfully\n");

	rc = rpmh_bcm_vote("MM0", BCM_MM0_VOTE_VALUE);
	if (rc) {
		printk(BIOS_ERR, "Failed to send BCM vote for display bus clock manager MM0\n");
		return CB_ERR;
	}

	printk(BIOS_INFO, "BCM vote for MM0 sent successfully\n");

	return CB_SUCCESS;
}
