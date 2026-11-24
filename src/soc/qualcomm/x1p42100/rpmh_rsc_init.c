/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/cmd_db.h>
#include <soc/rpmh_config.h>
#include <soc/rpmh_internal.h>
#include <soc/rpmh_regulator.h>
#include <soc/rpmh_rsc.h>
#include <string.h>
#include <timer.h>

static struct rsc_drv rsc_driver;
static struct cache_req non_batch_cache[CMD_DB_MAX_RESOURCES];

/**
 * rpmh_rsc_init - Initialize the RPMh RSC driver
 *
 * Initializes the RPMh Resource State Coordinator driver with platform-specific configuration.
 * Configuration is read from soc/rpmh_config.h for this chipset.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_rsc_init(void)
{
	struct rsc_drv *drv = &rsc_driver;
	u32 rsc_id, major_rsc_version, minor_rsc_version, config, max_tcs, ncpt;
	int offset;
	u32 solver_config;

	/* Check if already initialized */
	if (drv->initialized) {
		printk(BIOS_INFO, "RPMH RSC: Already initialized, skipping re-initialization\n");
		return 0;
	}

	memset(drv, 0, sizeof(*drv));

	/* Set configuration from chipset-specific header */
	drv->base = (void *)RPMH_BASE;
	drv->tcs_base = (void *)(RPMH_BASE + RPMH_TCS_OFFSET);
	drv->tcs_distance = RPMH_TCS_DISTANCE;
	drv->id = RPMH_RSC_DRV_ID;
	drv->num_channels = RPMH_NUM_CHANNELS;
	strncpy(drv->name, RPMH_RSC_NAME, sizeof(drv->name) - 1);

	/* Read hardware version */
	rsc_id = read32(drv->base);
	major_rsc_version = (rsc_id >> 16) & 0xFF;
	minor_rsc_version = (rsc_id >> 8) & 0xFF;

	/* Select register offsets based on version and HW channel mode */
	if (RPMH_HW_CHANNEL_MODE && major_rsc_version >= 3) {
		drv->regs = rpmh_rsc_reg_offset_ver_3_0_hw_channel;
		printk(BIOS_INFO, "RPMH RSC: Using v3.0 HW channel register offsets\n");
	} else if (major_rsc_version >= 3) {
		drv->regs = rpmh_rsc_reg_offset_ver_3_0;
		printk(BIOS_INFO, "RPMH RSC: Using v3.0 register offsets\n");
	} else {
		drv->regs = rpmh_rsc_reg_offset_ver_2_7;
		printk(BIOS_INFO, "RPMH RSC: Using v2.7 register offsets\n");
	}

	/* Read TCS configuration from hardware */
	config = read32(drv->base + drv->regs[DRV_PRNT_CHLD_CONFIG]);
	max_tcs = (config >> (DRV_NUM_TCS_SHIFT * drv->id)) & DRV_NUM_TCS_MASK;
	ncpt = (config >> DRV_NCPT_SHIFT) & DRV_NCPT_MASK;

	/* Configure TCS groups for Channel 0 - explicit approach */
	offset = 0;
	drv->num_tcs = 0;

	if (RPMH_NUM_ACTIVE_TCS > 0) {
		drv->ch[CH0].tcs[ACTIVE_TCS].drv = drv;
		drv->ch[CH0].tcs[ACTIVE_TCS].type = ACTIVE_TCS;
		drv->ch[CH0].tcs[ACTIVE_TCS].num_tcs = RPMH_NUM_ACTIVE_TCS;
		drv->ch[CH0].tcs[ACTIVE_TCS].ncpt = ncpt;
		drv->ch[CH0].tcs[ACTIVE_TCS].offset = offset;
		drv->ch[CH0].tcs[ACTIVE_TCS].mask = ((1 << RPMH_NUM_ACTIVE_TCS) - 1) << offset;
		offset += RPMH_NUM_ACTIVE_TCS;
		drv->num_tcs += RPMH_NUM_ACTIVE_TCS;
		printk(BIOS_DEBUG, "RPMH RSC: ACTIVE TCS: %d TCSes at offset %d (mask=0x%x)\n",
		       RPMH_NUM_ACTIVE_TCS, drv->ch[CH0].tcs[ACTIVE_TCS].offset,
		       drv->ch[CH0].tcs[ACTIVE_TCS].mask);
	}

	if (RPMH_NUM_SLEEP_TCS > 0) {
		drv->ch[CH0].tcs[SLEEP_TCS].drv = drv;
		drv->ch[CH0].tcs[SLEEP_TCS].type = SLEEP_TCS;
		drv->ch[CH0].tcs[SLEEP_TCS].num_tcs = RPMH_NUM_SLEEP_TCS;
		drv->ch[CH0].tcs[SLEEP_TCS].ncpt = ncpt;
		drv->ch[CH0].tcs[SLEEP_TCS].offset = offset;
		drv->ch[CH0].tcs[SLEEP_TCS].mask = ((1 << RPMH_NUM_SLEEP_TCS) - 1) << offset;
		offset += RPMH_NUM_SLEEP_TCS;
		drv->num_tcs += RPMH_NUM_SLEEP_TCS;
		printk(BIOS_DEBUG, "RPMH RSC: SLEEP TCS: %d TCSes at offset %d (mask=0x%x)\n",
		       RPMH_NUM_SLEEP_TCS, drv->ch[CH0].tcs[SLEEP_TCS].offset,
		       drv->ch[CH0].tcs[SLEEP_TCS].mask);
	}

	if (RPMH_NUM_WAKE_TCS > 0) {
		drv->ch[CH0].tcs[WAKE_TCS].drv = drv;
		drv->ch[CH0].tcs[WAKE_TCS].type = WAKE_TCS;
		drv->ch[CH0].tcs[WAKE_TCS].num_tcs = RPMH_NUM_WAKE_TCS;
		drv->ch[CH0].tcs[WAKE_TCS].ncpt = ncpt;
		drv->ch[CH0].tcs[WAKE_TCS].offset = offset;
		drv->ch[CH0].tcs[WAKE_TCS].mask = ((1 << RPMH_NUM_WAKE_TCS) - 1) << offset;
		offset += RPMH_NUM_WAKE_TCS;
		drv->num_tcs += RPMH_NUM_WAKE_TCS;
		printk(BIOS_DEBUG, "RPMH RSC: WAKE TCS: %d TCSes at offset %d (mask=0x%x)\n",
		       RPMH_NUM_WAKE_TCS, drv->ch[CH0].tcs[WAKE_TCS].offset,
		       drv->ch[CH0].tcs[WAKE_TCS].mask);
	}

	/* Check if total TCS count exceeds hardware limit */
	if (drv->num_tcs > max_tcs) {
		printk(BIOS_ERR, "RPMH RSC: TCS config exceeds hardware limit (configured: %d, max: %d)\n",
				drv->num_tcs, max_tcs);
		return -1;
	}

	drv->ch[CH0].drv = drv;
	drv->ch[CH0].initialized = true;
	drv->client.non_batch_cache = non_batch_cache;
	drv->client.non_batch_cache_idx = 0;

	for (int i = 0; i < CMD_DB_MAX_RESOURCES; i++) {
		non_batch_cache[i].addr = 0;
		non_batch_cache[i].sleep_val = UINT_MAX;
		non_batch_cache[i].wake_val = UINT_MAX;
	}

	/* Initialize TCS in-use bitmap */
	memset(drv->tcs_in_use, 0, sizeof(drv->tcs_in_use));

	/* Detect solver mode from hardware */
	solver_config = read32(drv->base + drv->regs[DRV_SOLVER_CONFIG]);
	solver_config = (solver_config >> 24) & 0x1;

	if (solver_config || RPMH_HW_SOLVER_SUPPORTED) {
		drv->client.flags |= SOLVER_PRESENT;
		printk(BIOS_INFO, "RPMH RSC: Hardware solver mode supported\n");

		if (RPMH_HW_CHANNEL_MODE) {
			drv->client.flags |= HW_CHANNEL_PRESENT;
			drv->in_solver_mode = true;
			drv->client.in_solver_mode = true;
			printk(BIOS_INFO, "RPMH RSC: HW channel mode enabled\n");
		} else {
			drv->in_solver_mode = false;
			drv->client.in_solver_mode = false;
			printk(BIOS_INFO, "RPMH RSC: Software control mode\n");
		}
	} else {
		printk(BIOS_INFO, "RPMH RSC: Hardware solver mode not supported\n");
	}

	drv->initialized = true;

	rpmh_set_rsc_drv(drv);

	printk(BIOS_INFO, "RPMH RSC: Initialized %s (drv-%d) at 0x%lx Version %d.%d\n",
	       drv->name, drv->id, (uintptr_t)drv->base, major_rsc_version, minor_rsc_version);

	return 0;
}

static enum cb_err rpmh_vrm_init(struct rpmh_vreg *vreg, const char *name)
{
	int rc;

	if (!vreg || !name)
		return CB_ERR;

	if (cmd_db_ready() != CB_SUCCESS) {
		volatile u32 *boot_cookie = (volatile u32 *)AOP_BOOT_COOKIE_ADDR;

		if (!wait_us(AOP_BOOT_TIMEOUT_US, *boot_cookie == AOP_BOOT_COOKIE)) {
			printk(BIOS_ERR, "RPMH_VRM: AOP not booted after %dus (cookie: 0x%x)\n",
			       AOP_BOOT_TIMEOUT_US, *boot_cookie);
			return CB_ERR;
		}
		if (cmd_db_init(CMD_DB_BASE_ADDR, CMD_DB_SIZE) != CB_SUCCESS) {
			printk(BIOS_ERR, "RPMH_VRM: cmd_db init failed for %s\n", name);
			return CB_ERR;
		}
	}

	rc = rpmh_rsc_init();
	if (rc) {
		printk(BIOS_ERR, "RPMH_VRM: rpmh_rsc_init failed rc=%d for %s\n", rc, name);
		return CB_ERR;
	}

	rc = rpmh_regulator_init(vreg, name, RPMH_REGULATOR_TYPE_VRM);
	if (rc) {
		printk(BIOS_ERR, "RPMH_VRM: Failed to init %s rc=%d\n", name, rc);
		return CB_ERR;
	}

	return CB_SUCCESS;
}

enum cb_err rpmh_vrm_enable(const char *name, int uv, u32 mode)
{
	struct rpmh_vreg vreg;
	int rc;

	if (rpmh_vrm_init(&vreg, name) != CB_SUCCESS)
		return CB_ERR;

	rc = rpmh_regulator_vrm_set_voltage(&vreg, uv, uv, true, false);
	if (rc) {
		printk(BIOS_ERR, "RPMH_VRM: Failed to set voltage for %s to %d uV rc=%d\n",
		       name, uv, rc);
		return CB_ERR;
	}

	rc = rpmh_regulator_vrm_set_mode(&vreg, mode, true, false);
	if (rc) {
		printk(BIOS_ERR, "RPMH_VRM: Failed to set mode %u for %s rc=%d\n",
		       mode, name, rc);
		return CB_ERR;
	}

	rc = rpmh_regulator_enable(&vreg, true, false);
	if (rc) {
		printk(BIOS_ERR, "RPMH_VRM: Failed to enable %s rc=%d\n", name, rc);
		return CB_ERR;
	}

	printk(BIOS_INFO, "RPMH_VRM: %s enabled at %d mV (mode=%u)\n", name, uv / 1000, mode);
	return CB_SUCCESS;
}

enum cb_err rpmh_vrm_disable(const char *name)
{
	struct rpmh_vreg vreg;
	int rc;

	if (rpmh_vrm_init(&vreg, name) != CB_SUCCESS)
		return CB_ERR;

	rc = rpmh_regulator_disable(&vreg, true, false);
	if (rc) {
		printk(BIOS_ERR, "RPMH_VRM: Failed to disable %s rc=%d\n", name, rc);
		return CB_ERR;
	}

	printk(BIOS_INFO, "RPMH_VRM: %s disabled\n", name);
	return CB_SUCCESS;
}
