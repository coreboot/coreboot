/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <delay.h>
#include <device/device.h>
#include <string.h>
#include <types.h>
#include <soc/rpmh.h>
#include <soc/cmd_db.h>
#include <soc/rpmh_regulator.h>

/**
 * rpmh_regulator_load_arc_level_mapping() - load the RPMh ARC resource's
 *		voltage level mapping from command db
 * @vreg:	Pointer to the rpmh regulator
 *
 * Return: 0 on success, -1 on failure
 */
static int rpmh_regulator_load_arc_level_mapping(struct rpmh_vreg *vreg)
{
	size_t len = 0;
	int i, j;
	const u8 *buf;

	buf = cmd_db_read_aux_data(vreg->resource_name, &len);
	if (!buf) {
		printk(BIOS_ERR, "RPMH_REG: Could not retrieve ARC aux data for %s\n",
		       vreg->resource_name);
		return -1;
	}

	if (len == 0) {
		printk(BIOS_ERR, "RPMH_REG: ARC level mapping data missing for %s\n",
		       vreg->resource_name);
		return -1;
	}

	if (len > RPMH_ARC_MAX_LEVELS * RPMH_ARC_LEVEL_SIZE) {
		printk(BIOS_ERR, "RPMH_REG: Too many ARC levels defined for %s: %zu > %d\n",
		       vreg->resource_name, len, RPMH_ARC_MAX_LEVELS * RPMH_ARC_LEVEL_SIZE);
		return -1;
	}

	if (len % RPMH_ARC_LEVEL_SIZE) {
		printk(BIOS_ERR, "RPMH_REG: Invalid ARC aux data size for %s: %zu\n",
		       vreg->resource_name, len);
		return -1;
	}

	vreg->level_count = len / RPMH_ARC_LEVEL_SIZE;

	for (i = 0; i < vreg->level_count; i++) {
		vreg->level[i] = 0;
		for (j = 0; j < RPMH_ARC_LEVEL_SIZE; j++)
			vreg->level[i] |= buf[i * RPMH_ARC_LEVEL_SIZE + j] << (8 * j);

		/* The AUX data may be zero padded - ignore 0 valued entries at the end */
		if (i > 0 && vreg->level[i] == 0) {
			vreg->level_count = i;
			break;
		}

		printk(BIOS_DEBUG, "RPMH_REG: %s ARC hlvl=%2d --> vlvl=%4u\n",
		       vreg->resource_name, i, vreg->level[i]);
	}

	return 0;
}

/**
 * rpmh_regulator_send_request() - send the RPMh request
 * @vreg:	Pointer to the rpmh regulator
 * @req:	Request to send
 * @state:	RPMh state (active/sleep)
 * @wait:	Whether to wait for completion
 *
 * Return: 0 on success, -1 on failure
 */
static int rpmh_regulator_send_request(struct rpmh_vreg *vreg,
				       struct rpmh_regulator_request *req,
				       enum rpmh_state state, bool wait)
{
	struct tcs_cmd cmd[RPMH_REGULATOR_REG_MAX];
	int i, j = 0;
	int rc;

	/* Build command array from valid registers */
	for (i = 0; i < RPMH_REGULATOR_REG_MAX; i++) {
		if (req->valid & BIT(i)) {
			cmd[j].addr = vreg->addr + i * 4;
			cmd[j].data = req->reg[i];
			cmd[j].wait = true;
			j++;
		}
	}
	if (j == 0)
		return 0;

	/* Send the request */
	if (wait)
		rc = rpmh_write(state, cmd, j);
	else
		rc = rpmh_write_async(state, cmd, j);

	if (rc) {
		printk(BIOS_ERR, "RPMH_REG: Failed to send %s request for %s, rc=%d\n",
		       state == RPMH_ACTIVE_ONLY_STATE ? "active" : "sleep",
		       vreg->resource_name, rc);
		return rc;
	}

	printk(BIOS_DEBUG, "RPMH_REG: Sent %s request for %s\n",
	       state == RPMH_ACTIVE_ONLY_STATE ? "active" : "sleep",
	       vreg->resource_name);

	return 0;
}

/**
 * rpmh_regulator_vrm_set_voltage() - set the voltage of a VRM regulator
 * @vreg:	Pointer to the rpmh regulator
 * @min_uv:	Minimum voltage in microvolts
 * @max_uv:	Maximum voltage in microvolts
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_vrm_set_voltage(struct rpmh_vreg *vreg, int min_uv, int max_uv,
				   bool set_active, bool set_sleep)
{
	int mv;
	int rc = 0;

	if (!vreg || vreg->type != RPMH_REGULATOR_TYPE_VRM)
		return -1;

	if (min_uv < RPMH_VRM_MIN_UV || max_uv > RPMH_VRM_MAX_UV) {
		printk(BIOS_ERR, "RPMH_REG: Voltage range %d-%d uV out of bounds for %s\n",
		       min_uv, max_uv, vreg->resource_name);
		return -1;
	}

	mv = DIV_ROUND_UP(min_uv, 1000);
	if (mv * 1000 > max_uv) {
		printk(BIOS_ERR, "RPMH_REG: No set points available in range %d-%d uV for %s\n",
		       min_uv, max_uv, vreg->resource_name);
		return -1;
	}

	if (set_active) {
		vreg->req_active.reg[RPMH_REGULATOR_REG_VRM_VOLTAGE] = mv;
		vreg->req_active.valid |= BIT(RPMH_REGULATOR_REG_VRM_VOLTAGE);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_active,
						 RPMH_ACTIVE_ONLY_STATE, true);
		if (rc)
			return rc;
	}

	if (set_sleep) {
		vreg->req_sleep.reg[RPMH_REGULATOR_REG_VRM_VOLTAGE] = mv;
		vreg->req_sleep.valid |= BIT(RPMH_REGULATOR_REG_VRM_VOLTAGE);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_sleep,
						 RPMH_SLEEP_STATE, false);
	}

	return rc;
}

/**
 * rpmh_regulator_vrm_set_mode() - set the mode of a VRM regulator
 * @vreg:	Pointer to the rpmh regulator
 * @mode:	PMIC mode value
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_vrm_set_mode(struct rpmh_vreg *vreg, u32 mode,
				bool set_active, bool set_sleep)
{
	int rc = 0;

	if (!vreg || vreg->type != RPMH_REGULATOR_TYPE_VRM)
		return -1;

	if (mode < RPMH_VRM_MODE_MIN || mode > RPMH_VRM_MODE_MAX) {
		printk(BIOS_ERR, "RPMH_REG: Mode %u out of range for %s\n",
		       mode, vreg->resource_name);
		return -1;
	}

	if (set_active) {
		vreg->req_active.reg[RPMH_REGULATOR_REG_VRM_MODE] = mode;
		vreg->req_active.valid |= BIT(RPMH_REGULATOR_REG_VRM_MODE);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_active,
						 RPMH_ACTIVE_ONLY_STATE, true);
		if (rc)
			return rc;
	}

	if (set_sleep) {
		vreg->req_sleep.reg[RPMH_REGULATOR_REG_VRM_MODE] = mode;
		vreg->req_sleep.valid |= BIT(RPMH_REGULATOR_REG_VRM_MODE);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_sleep,
						 RPMH_SLEEP_STATE, false);
	}

	return rc;
}

/**
 * rpmh_regulator_enable() - enable a regulator
 * @vreg:	Pointer to the rpmh regulator
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_enable(struct rpmh_vreg *vreg, bool set_active, bool set_sleep)
{
	int rc = 0;
	int reg_index;

	if (!vreg)
		return -1;

	/* Determine the correct enable register based on type */
	switch (vreg->type) {
	case RPMH_REGULATOR_TYPE_VRM:
	case RPMH_REGULATOR_TYPE_ARC:
	case RPMH_REGULATOR_TYPE_XOB:
		reg_index = RPMH_REGULATOR_REG_ENABLE;
		break;
	case RPMH_REGULATOR_TYPE_PBS:
		reg_index = RPMH_REGULATOR_REG_PBS_ENABLE;
		break;
	default:
		return -1;
	}

	if (set_active) {
		vreg->req_active.reg[reg_index] = 1;
		vreg->req_active.valid |= BIT(reg_index);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_active,
						 RPMH_ACTIVE_ONLY_STATE, true);
		if (rc)
			return rc;
	}

	if (set_sleep) {
		vreg->req_sleep.reg[reg_index] = 1;
		vreg->req_sleep.valid |= BIT(reg_index);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_sleep,
						 RPMH_SLEEP_STATE, false);
	}

	return rc;
}

/**
 * rpmh_regulator_disable() - disable a regulator
 * @vreg:	Pointer to the rpmh regulator
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_disable(struct rpmh_vreg *vreg, bool set_active, bool set_sleep)
{
	int rc = 0;
	int reg_index;

	if (!vreg)
		return -1;

	/* Determine the correct enable register based on type */
	switch (vreg->type) {
	case RPMH_REGULATOR_TYPE_VRM:
	case RPMH_REGULATOR_TYPE_ARC:
	case RPMH_REGULATOR_TYPE_XOB:
		reg_index = RPMH_REGULATOR_REG_ENABLE;
		break;
	case RPMH_REGULATOR_TYPE_PBS:
		reg_index = RPMH_REGULATOR_REG_PBS_ENABLE;
		break;
	default:
		return -1;
	}

	if (set_active) {
		vreg->req_active.reg[reg_index] = 0;
		vreg->req_active.valid |= BIT(reg_index);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_active,
						 RPMH_ACTIVE_ONLY_STATE, true);
		if (rc)
			return rc;
	}

	if (set_sleep) {
		vreg->req_sleep.reg[reg_index] = 0;
		vreg->req_sleep.valid |= BIT(reg_index);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_sleep,
						 RPMH_SLEEP_STATE, false);
	}

	return rc;
}

/**
 * rpmh_regulator_arc_set_level() - set the voltage level of an ARC regulator
 * @vreg:	Pointer to the rpmh regulator
 * @level:	ARC voltage level (0 to level_count-1)
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_arc_set_level(struct rpmh_vreg *vreg, u32 level,
				  bool set_active, bool set_sleep)
{
	int rc = 0;

	if (!vreg || vreg->type != RPMH_REGULATOR_TYPE_ARC)
		return -1;

	if (level >= vreg->level_count) {
		printk(BIOS_ERR, "RPMH_REG: Level %u out of range for %s (max %d)\n",
		       level, vreg->resource_name, vreg->level_count - 1);
		return -1;
	}

	if (set_active) {
		vreg->req_active.reg[RPMH_REGULATOR_REG_ARC_LEVEL] = level;
		vreg->req_active.valid |= BIT(RPMH_REGULATOR_REG_ARC_LEVEL);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_active,
						 RPMH_ACTIVE_ONLY_STATE, true);
		if (rc)
			return rc;
	}

	if (set_sleep) {
		vreg->req_sleep.reg[RPMH_REGULATOR_REG_ARC_LEVEL] = level;
		vreg->req_sleep.valid |= BIT(RPMH_REGULATOR_REG_ARC_LEVEL);
		rc = rpmh_regulator_send_request(vreg, &vreg->req_sleep,
						 RPMH_SLEEP_STATE, false);
	}

	return rc;
}

/**
 * rpmh_regulator_arc_get_level() - get the current voltage level of an ARC regulator
 * @vreg:	Pointer to the rpmh regulator
 *
 * This function returns the cached ARC voltage level that was last set.
 * Note: This returns the requested level, not necessarily the actual hardware level,
 * as there's no way to read back from RPMh hardware.
 *
 * Return: ARC voltage level (0 to level_count-1) on success, -1 on failure
 */
int rpmh_regulator_arc_get_level(struct rpmh_vreg *vreg)
{
	if (!vreg || vreg->type != RPMH_REGULATOR_TYPE_ARC)
		return -1;

	/* Return the active set level if valid, otherwise return 0 */
	if (vreg->req_active.valid & BIT(RPMH_REGULATOR_REG_ARC_LEVEL))
		return vreg->req_active.reg[RPMH_REGULATOR_REG_ARC_LEVEL];

	return 0;
}

/**
 * rpmh_regulator_arc_get_voltage_level() - get the voltage level value for an ARC level
 * @vreg:	Pointer to the rpmh regulator
 * @level:	ARC level index
 *
 * This function returns the actual voltage level value (vlvl) that corresponds
 * to the given ARC hardware level (hlvl).
 *
 * Return: voltage level value on success, 0 on failure
 */
u32 rpmh_regulator_arc_get_voltage_level(struct rpmh_vreg *vreg, u32 level)
{
	if (!vreg || vreg->type != RPMH_REGULATOR_TYPE_ARC)
		return 0;

	if (level >= vreg->level_count)
		return 0;

	return vreg->level[level];
}

/**
 * rpmh_regulator_init() - initialize an rpmh regulator
 * @vreg:		Pointer to the rpmh regulator structure to initialize
 * @dev:		Device pointer for RPMh communication
 * @resource_name:	Name of the RPMh resource
 * @type:		Type of regulator (VRM, ARC, XOB, PBS)
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_init(struct rpmh_vreg *vreg, const char *resource_name,
			enum rpmh_regulator_type type)
{
	enum cmd_db_hw_type hw_type;
	int rc;

	if (!vreg || !resource_name)
		return -1;

	memset(vreg, 0, sizeof(*vreg));

	vreg->resource_name = resource_name;
	vreg->type = type;

	/* Get the RPMh address for this resource */
	vreg->addr = cmd_db_read_addr(resource_name);
	if (!vreg->addr) {
		printk(BIOS_ERR, "RPMH_REG: Could not find RPMh address for %s\n",
		       resource_name);
		return -1;
	}

	/* Verify the slave ID matches the expected type */
	hw_type = cmd_db_read_slave_id(resource_name);
	if (hw_type == CMD_DB_HW_INVALID) {
		printk(BIOS_ERR, "RPMH_REG: Could not find slave ID for %s\n",
		       resource_name);
		return -1;
	}

	/* Validate hardware type matches regulator type */
	if ((type == RPMH_REGULATOR_TYPE_ARC && hw_type != CMD_DB_HW_ARC) ||
	    (type == RPMH_REGULATOR_TYPE_VRM && hw_type != CMD_DB_HW_VRM) ||
	    (type == RPMH_REGULATOR_TYPE_XOB && hw_type != CMD_DB_HW_XOB) ||
	    (type == RPMH_REGULATOR_TYPE_BCM && hw_type != CMD_DB_HW_BCM) ||
	    (type == RPMH_REGULATOR_TYPE_PBS && hw_type != CMD_DB_HW_PBS)) {
		printk(BIOS_ERR, "RPMH_REG: Hardware type mismatch for %s\n",
		       resource_name);
		return -1;
	}

	/* Load ARC level mapping if this is an ARC regulator */
	if (type == RPMH_REGULATOR_TYPE_ARC) {
		rc = rpmh_regulator_load_arc_level_mapping(vreg);
		if (rc) {
			printk(BIOS_ERR, "RPMH_REG: Failed to load ARC level mapping for %s\n",
			       resource_name);
			return rc;
		}
	}
	printk(BIOS_INFO, "RPMH_REG: Initialized %s at addr=0x%05X\n",
	       resource_name, vreg->addr);

	return 0;
}
