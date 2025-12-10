/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SOC_QUALCOMM_RPMH_REGULATOR_H__
#define __SOC_QUALCOMM_RPMH_REGULATOR_H__

#include <types.h>
#include <device/device.h>

#define RPMH_ARC_LEVEL_SIZE             2
#define RPMH_ARC_MAX_LEVELS             32
#define RPMH_REGULATOR_LEVEL_OFF        0

/* Min and max limits of VRM resource request parameters */
#define RPMH_VRM_MIN_UV                 0
#define RPMH_VRM_MAX_UV                 8191000
#define RPMH_VRM_STEP_UV                1000
#define RPMH_VRM_HEADROOM_MIN_UV        0
#define RPMH_VRM_HEADROOM_MAX_UV        511000

#define RPMH_VRM_MODE_MIN               0
#define RPMH_VRM_MODE_MAX               7

/* XOB and PBS voting registers are found in the VRM hardware module */
#define CMD_DB_HW_XOB                   CMD_DB_HW_VRM
#define CMD_DB_HW_PBS                   CMD_DB_HW_VRM

/* Voltage unknown placeholder */
#define VOLTAGE_UNKNOWN                 1

/**
 * enum rpmh_regulator_type - supported RPMh accelerator types
 * @RPMH_REGULATOR_TYPE_VRM:	RPMh VRM accelerator which supports voting on
 *				enable, voltage, mode, and headroom voltage of
 *				LDO, SMPS, VS, and BOB type PMIC regulators.
 * @RPMH_REGULATOR_TYPE_ARC:	RPMh ARC accelerator which supports voting on
 *				the CPR managed voltage level of LDO and SMPS
 *				type PMIC regulators.
 * @RPMH_REGULATOR_TYPE_XOB:	RPMh XOB accelerator which supports voting on
 *				the enable state of PMIC regulators.
 * @RPMH_REGULATOR_TYPE_PBS:	RPMh PBS accelerator which supports voting on
 *				the enable state of PBS resources.
 * @RPMH_REGULATOR_TYPE_BCM:	RPMh BCM (Bus Clock Manager) accelerator which
 *				supports voting on bandwidth and clock frequency
 *				requirements for interconnect and bus resources.
 */
enum rpmh_regulator_type {
	RPMH_REGULATOR_TYPE_VRM,
	RPMH_REGULATOR_TYPE_ARC,
	RPMH_REGULATOR_TYPE_XOB,
	RPMH_REGULATOR_TYPE_PBS,
	RPMH_REGULATOR_TYPE_BCM,
};

/**
 * enum rpmh_regulator_reg_index - RPMh accelerator register indices
 * @RPMH_REGULATOR_REG_VRM_VOLTAGE:	VRM voltage voting register index
 * @RPMH_REGULATOR_REG_ARC_LEVEL:	ARC voltage level voting register index
 * @RPMH_REGULATOR_REG_PBS_ENABLE:	PBS enable voting register index
 * @RPMH_REGULATOR_REG_VRM_ENABLE:	VRM enable voltage voting register index
 * @RPMH_REGULATOR_REG_ARC_PSEUDO_ENABLE: Place-holder for enable aggregation
 * @RPMH_REGULATOR_REG_XOB_ENABLE:	XOB enable voting register index
 * @RPMH_REGULATOR_REG_ENABLE:		Common enable index
 * @RPMH_REGULATOR_REG_VRM_MODE:	VRM regulator mode voting register index
 * @RPMH_REGULATOR_REG_VRM_HEADROOM:	VRM headroom voltage voting register index
 * @RPMH_REGULATOR_REG_MAX:		Maximum number of registers
 */
enum rpmh_regulator_reg_index {
	RPMH_REGULATOR_REG_VRM_VOLTAGE		= 0,
	RPMH_REGULATOR_REG_ARC_LEVEL		= 0,
	RPMH_REGULATOR_REG_PBS_ENABLE		= 0,
	RPMH_REGULATOR_REG_VRM_ENABLE		= 1,
	RPMH_REGULATOR_REG_ARC_PSEUDO_ENABLE	= 1,
	RPMH_REGULATOR_REG_XOB_ENABLE		= 1,
	RPMH_REGULATOR_REG_ENABLE		= 1,
	RPMH_REGULATOR_REG_VRM_MODE		= 2,
	RPMH_REGULATOR_REG_VRM_HEADROOM		= 3,
	RPMH_REGULATOR_REG_MAX			= 4,
};

/**
 * struct rpmh_regulator_request - internal request structure
 */
struct rpmh_regulator_request {
	u32 reg[RPMH_REGULATOR_REG_MAX];
	u32 valid;
};

/**
 * struct rpmh_vreg - individual rpmh regulator data structure
 * @resource_name:      Name of rpmh regulator resource
 * @addr:               Base address of the regulator resource within an RPMh accelerator
 * @type:               RPMh accelerator type for this regulator resource
 * @level:              Mapping from ARC resource specific voltage levels to consumer levels
 * @level_count:        The number of valid entries in the level array
 * @req_active:         Active set RPMh accelerator register request
 * @req_sleep:          Sleep set RPMh accelerator register request
 */

struct rpmh_vreg {
	const char *resource_name;
	u32 addr;
	enum rpmh_regulator_type type;
	u32 level[RPMH_ARC_MAX_LEVELS];
	int level_count;
	struct rpmh_regulator_request req_active;
	struct rpmh_regulator_request req_sleep;
};


/**
 * rpmh_regulator_init() - initialize an rpmh regulator
 * @vreg:		Pointer to the rpmh regulator structure to initialize
 * @dev:		Device pointer for RPMh communication
 * @resource_name:	Name of the RPMh resource (e.g., "ldoa1")
 * @type:		Type of regulator (VRM, ARC, XOB, PBS)
 *
 * This function initializes an RPMh regulator by looking up its address
 * in the command DB and loading any necessary configuration data.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_init(struct rpmh_vreg *vreg, const char *resource_name,
			enum rpmh_regulator_type type);

/**
 * rpmh_regulator_vrm_set_voltage() - set the voltage of a VRM regulator
 * @vreg:	Pointer to the rpmh regulator
 * @min_uv:	Minimum voltage in microvolts
 * @max_uv:	Maximum voltage in microvolts
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * This function sets the voltage for a VRM type regulator. The actual
 * voltage set will be the minimum value that satisfies min_uv.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_vrm_set_voltage(struct rpmh_vreg *vreg, int min_uv, int max_uv,
				   bool set_active, bool set_sleep);

/**
 * rpmh_regulator_vrm_set_mode() - set the mode of a VRM regulator
 * @vreg:	Pointer to the rpmh regulator
 * @mode:	PMIC mode value (use RPMH_REGULATOR_MODE_* defines)
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * This function sets the operating mode for a VRM type regulator.
 * The mode value should be one of the RPMH_REGULATOR_MODE_PMIC*_* defines
 * appropriate for the regulator type.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_vrm_set_mode(struct rpmh_vreg *vreg, u32 mode,
				bool set_active, bool set_sleep);

/**
 * rpmh_regulator_enable() - enable a regulator
 * @vreg:	Pointer to the rpmh regulator
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * This function enables the regulator. Works for VRM, ARC, XOB, and PBS types.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_enable(struct rpmh_vreg *vreg, bool set_active, bool set_sleep);

/**
 * rpmh_regulator_disable() - disable a regulator
 * @vreg:	Pointer to the rpmh regulator
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * This function disables the regulator. Works for VRM, ARC, XOB, and PBS types.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_disable(struct rpmh_vreg *vreg, bool set_active, bool set_sleep);

/**
 * rpmh_regulator_arc_set_level() - set the voltage level of an ARC regulator
 * @vreg:	Pointer to the rpmh regulator
 * @level:	ARC voltage level (0 to level_count-1)
 * @set_active:	Set in active state
 * @set_sleep:	Set in sleep state
 *
 * This function sets the voltage level for an ARC type regulator.
 * The level is an index into the voltage level table loaded from command DB.
 *
 * Return: 0 on success, -1 on failure
 */
int rpmh_regulator_arc_set_level(struct rpmh_vreg *vreg, u32 level,
				  bool set_active, bool set_sleep);

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
int rpmh_regulator_arc_get_level(struct rpmh_vreg *vreg);

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
u32 rpmh_regulator_arc_get_voltage_level(struct rpmh_vreg *vreg, u32 level);

#endif /* __SOC_QUALCOMM_RPMH_REGULATOR_H__ */
