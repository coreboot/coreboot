/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <delay.h>

#include "chip.h"
#include "nct6687d_hwm.h"

uint16_t nct6687d_hwm_base = 0;

static bool check_cond(const bool cond, const char *error)
{
	if (!cond)
		printk(BIOS_ERR, "NCT6687D: %s\n", error);

	return !cond;
}

static inline void print_status_bit(const char *status, const bool cond)
{
	printk(BIOS_DEBUG, "\t%-45s: %s\n", status, cond ? "yes" : "no");
}

static void print_fan_engine_status(void)
{
	uint8_t fan_eng_sts = hwm_reg_read(FAN_ENGINE_STS_REG);

	printk(BIOS_DEBUG, "NCT6687D: Fan Engine Status:\n");
	print_status_bit("PECI configuration adjusted", fan_eng_sts & FAN_PECI_CFG_ADJUSTED);
	print_status_bit("All enabled fan channels processed",
			 fan_eng_sts & FAN_UNFINISHED_FLAG);
	print_status_bit("Configuration phase", fan_eng_sts & FAN_CFG_PHASE);
	print_status_bit("Configuration invalid", fan_eng_sts & FAN_CFG_INVALID);
	print_status_bit("Configuration check done", fan_eng_sts & FAN_CFG_CHECK_DONE);
	print_status_bit("Configuration locked", fan_eng_sts & FAN_CFG_LOCK);
	printk(BIOS_DEBUG, "Fans in automatic mode driven by %s register\n",
	       (fan_eng_sts & FAN_DRIVE_BY_DEFAULT_VAL) ? "DEFAULT_VAL" : "MOD_SEL");
}

static bool unlock_fan_register_set(void)
{
	uint8_t fan_eng_sts;
	bool done = false;
	unsigned int i;

	fan_eng_sts = hwm_reg_read(FAN_ENGINE_STS_REG);

	if (!(fan_eng_sts & FAN_CFG_LOCK) && (fan_eng_sts & FAN_CFG_PHASE))
		return true;

	for (i = 1000; i > 0; i--) {
		/* Wait until EC exits config phase and config request is clear */
		if ((hwm_reg_read(FAN_ENGINE_STS_REG) & FAN_CFG_PHASE) == 0 &&
		    (hwm_reg_read(FAN_CFG_CTRL_REG) & FAN_CFG_REQUEST) == 0) {
			done = true;
			break;
		}
		mdelay(1);
	}

	if (!done) {
		printk(BIOS_WARNING, "Timeout waiting for EC to exit config phase or clear"
				     "config request\n");
		print_fan_engine_status();
		return false;
	}

	done = false;

	hwm_reg_write(FAN_CFG_CTRL_REG, FAN_CFG_REQUEST);

	for (i = 1000; i > 0; i--) {
		/* Wait until EC unlock the register set */
		if ((hwm_reg_read(FAN_ENGINE_STS_REG) & FAN_CFG_LOCK) == 0) {
			done = true;
			break;
		}
		mdelay(1);
	}

	if (!done) {
		printk(BIOS_WARNING, "Timeout waiting for EC to unlock the fan registers\n");
		print_fan_engine_status();
		return false;
	}

	return true;
}

static void print_last_err(uint8_t error_code)
{
	uint8_t debug_level = BIOS_WARNING;
	uint8_t last_err_code = hwm_reg_read(FAN_LAST_ERROR_CODE_REG);

	if (last_err_code == FAN_NO_ERROR) {
		printk(BIOS_DEBUG, "No error occurred.\n");
		return;
	}

	/* Lower the debug level if it is an error we care about */
	if (last_err_code == error_code)
		debug_level = BIOS_ERR;

	switch (last_err_code) {
	case FAN_ERR_MODE_SELECT:
		printk(debug_level, "Mode Select invalid configuration\n");
		break;
	case FAN_ERR_CRIT_TEMP_PROTECT:
		printk(debug_level, "Critical Temperature Protection invalid configuration\n");
		break;
	case FAN_ERR_ITL_FAN_CONTROL:
		printk(debug_level, "Intel DTS Sensor invalid configuration\n");
		break;
	case FAN_ERR_SMART_TRACKING:
		printk(debug_level, "Smart Tracking invalid configuration\n");
		break;
	case FAN_ERR_THERMAL_CRUISE:
		printk(debug_level, "Thermal Cruise invalid configuration\n");
		break;
	case FAN_ERR_SPEED_CRUISE:
		printk(debug_level, "Speed Cruise invalid configuration\n");
		break;
	case FAN_ERR_SMART_FAN_IV:
		printk(debug_level, "Smart Fan IV invalid configuration\n");
		break;
	case FAN_ERR_PID_CONTROL:
		printk(debug_level, "PID control invalid configuration\n");
		break;
	default:
		printk(debug_level, "Unknown fan configuration error %02x\n", last_err_code);
		break;
	}
}

static void lock_fan_register_set_and_check(uint8_t error_code)
{
	uint8_t fan_eng_sts;
	bool done = false;
	unsigned int i;

	fan_eng_sts = hwm_reg_read(FAN_ENGINE_STS_REG);

	if (fan_eng_sts & FAN_CFG_LOCK || !(fan_eng_sts & FAN_CFG_PHASE)) {
		printk(BIOS_DEBUG, "Fan register set already locked or not in config phase\n");
		return;
	}

	hwm_reg_write(FAN_CFG_CTRL_REG, FAN_CFG_DONE);

	for (i = 1000; i > 0; i--) {
		fan_eng_sts = hwm_reg_read(FAN_ENGINE_STS_REG);
		/* Wait until EC checks the configuration */
		if (fan_eng_sts & FAN_CFG_CHECK_DONE) {
			done = true;
			break;
		}
		mdelay(1);
	}

	if (!done) {
		printk(BIOS_WARNING, "Timeout waiting for configuration check done\n");
		print_fan_engine_status();
		return;
	}

	fan_eng_sts = hwm_reg_read(FAN_ENGINE_STS_REG);

	if (fan_eng_sts & FAN_CFG_INVALID) {
		printk(BIOS_WARNING, "NCT6687D: Configuration error ocurred\n");
		print_last_err(error_code);
	}

	if (!(fan_eng_sts & FAN_CFG_LOCK)) {
		printk(BIOS_WARNING, "NCT6687D: Configuration registers did not lock\n");
		print_fan_engine_status();
	}
}

static void init_pch_smbus_sensor(const struct nct6687d_pch_smbus_sensor *smbus_sensor)
{
	if (!smbus_sensor->sensor_addr || !smbus_sensor->sensor_cmd) {
		printk(BIOS_ERR, "NCT6687D SMBus sensor CMD or ADDR missing!\n");
		return;
	}

	/* Enable SMBUS first */
	hwm_reg_write(SMBUS_MASTER_CFG2_REG, 0x04);
	hwm_reg_write(SMBUS_MASTER_BAUD_RATE_SEL_REG, SMB_MASTER_BAUD_100K);
	hwm_reg_set_bits(SMBUS_MASTER_CFG1_REG, SMB_MASTER_EN);

	hwm_reg_and_or(PCH_THERMAL_DATA_CFG_REG, ~PCH_BAUD_SEL_MASK,
		       smbus_sensor->baud_rate & PCH_BAUD_SEL_MASK);
	hwm_reg_and_or(PCH_THERMAL_DATA_CFG_REG, ~PCH_PORT_SEL_MASK,
		       PCH_THERMAL_PORT(smbus_sensor->port_sel) & PCH_PORT_SEL_MASK);

	if (smbus_sensor->report_one_byte)
		hwm_reg_set_bits(PCH_THERMAL_DATA_CFG_REG, PCH_ONE_BYTE_REPORT);
	else
		hwm_reg_and_or(PCH_THERMAL_DATA_CFG_REG, ~PCH_ONE_BYTE_REPORT & 0xff, 0);


	hwm_reg_write(PCH_DEVICE_ADDR_REG, smbus_sensor->sensor_addr);
	hwm_reg_write(PCH_THERMAL_CMD_REG, smbus_sensor->sensor_cmd);
}

static void init_sensors(const struct superio_nuvoton_nct6687d_config *conf)
{
	unsigned int i;
	bool peci_en = false;
	const enum nct6687d_sensor_src_select *sensors = conf->sensors;

	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping sensors programming\n");
		return;
	}

	printk(BIOS_DEBUG, "NCT6687D programming sensors\n");
	/* Start monitoring */
	hwm_reg_set_bits(HWM_CONFIG_REG, HWM_EN);

	for (i = 0; i < MAX_NUM_SENSORS; i++) {
		hwm_reg_write(SENSOR_CFG_REG(i), sensors[i] & SENSOR_SRC_SEL_MASK);

		if (sensors[i] >= PECI_AGENT0_DOMAIN0 &&
		    sensors[i] <= PECI_AGENT3_DOMAIN1) {
			hwm_reg_and_or(PECI_AGENT_EN_REG, ~PECI_AGENT_EN_MASK,
				       PECI_AGENT_EN(sensors[i] & 0x3));
			peci_en = true;
		}

		/* Only PCH SMBus sensor supported right now */
		if (sensors[i] >= PCH_CPU && sensors[i] <= PCH_DIMM3 &&
		    i == conf->smbus_sensor.sensor_idx &&
		    conf->smbus_sensor.sensor_en) {
			init_pch_smbus_sensor(&conf->smbus_sensor);
		}
	}

	if (peci_en) {
		hwm_reg_and_or(PECI_CFG_REG, PECI_SPEED_SEL_MASK,
			       conf->peci_speed & PECI_SPEED_SEL_MASK);
		hwm_reg_set_bits(PECI_CFG_REG, PECI_AGENT_INIT | PECI_EN);
	}

	lock_fan_register_set_and_check(FAN_NO_ERROR);
}

static bool intel_dts_sensor_control_point_check(const struct nct6687d_dts_sensor_config *dts,
						 unsigned int i)
{
	bool failure = false;

	failure |= check_cond(dts->ambient_temp[i] <= 127,
			      "DTS Sensor Ambient Temperature Levels should be <= 127");

	if (i < MAX_DTS_CTL_POINTS - 1) {
		failure |= check_cond(dts->ambient_temp[i + 1] >= dts->ambient_temp[i],
				      "DTS Sensor next Ambient Temperature Level must be "
				      "higher or equal to previous Ambient Temperature Level");
		failure |= check_cond(dts->rpm_start_point[i + 1] >  dts->rpm_start_point[i],
				      "DTS Sensor next RPM Start Point must be higher than "
				      "previous RPM Start Point");
		failure |= check_cond(dts->rpm_end_point[i + 1] >  dts->rpm_end_point[i],
				      "DTS Sensor next RPM End Point must be higher than "
				      "previous RPM End Point");
	}

	failure |= check_cond(dts->rpm_end_point[i] > dts->rpm_start_point[i],
			      "DTS Sensor RPM End Point must be higher than corresponding RPM "
			      "Start Point");

	return failure;
}

static bool intel_dts_sensor_config_check(const struct nct6687d_dts_sensor_config *dts)
{
	bool failure = false;
	unsigned int i;

	failure |= check_cond(dts->temp_end > dts->temp_start,
			      "DTS Sensor Temperature End Point should be > "
			      "Temperature End Point");
	failure |= check_cond(dts->temp_start <= 127,
			      "DTS Sensor Temperature Start Point should be <= 127");
	failure |= check_cond(dts->temp_end <= 127,
			      "DTS Sensor Temperature End Point should be <= 127");

	if (dts->peci_adjust) {
		failure |= check_cond(dts->peci_agent_idx < 8,
				      "DTS Sensor PECI Agent Index should be < 8");
	}

	for (i = 0; i < MAX_DTS_CTL_POINTS; i++)
		failure |= intel_dts_sensor_control_point_check(dts, i);

	return failure;
}

static void init_intel_dts_sensor(const struct nct6687d_dts_sensor_config *dts)
{
	static int init_once = 0;
	unsigned int i;

	if (init_once)
		return;

	if (intel_dts_sensor_config_check(dts)) {
		printk(BIOS_ERR, "NCT6687D DTS Sensor invalid config, "
				 "skipping sensor programming\n");
		return;
	}

	printk(BIOS_DEBUG, "NCT6687D programming DTS sensor\n");

	hwm_reg_write(ITL_TEMP_START_POINT_REG, dts->temp_start);
	hwm_reg_write(ITL_TEMP_END_POINT_REG, dts->temp_end);

	hwm_reg_write(ITL_RPM_MAX_SPEED_HI_REG, dts->max_speed >> 8);
	hwm_reg_write(ITL_RPM_MAX_SPEED_LO_REG, dts->max_speed & 0xff);

	for (i = 0; i < MAX_DTS_CTL_POINTS; i++) {
		hwm_reg_write(ITL_AMB_TEMP_START_POINT_REG(i), dts->ambient_temp[i]);
		hwm_reg_write(ITL_RPM_START_POINT_HI_REG(i), dts->rpm_start_point[i] >> 8);
		hwm_reg_write(ITL_RPM_START_POINT_LO_REG(i), dts->rpm_start_point[i] & 0xff);
		hwm_reg_write(ITL_RPM_END_POINT_HI_REG(i), dts->rpm_end_point[i] >> 8);
		hwm_reg_write(ITL_RPM_END_POINT_LO_REG(i), dts->rpm_end_point[i] & 0xff);
	}

	if (dts->peci_adjust) {
		hwm_reg_and_or(ITL_DTS_CFG_REG, ~ITL_DTS_PECI_AGENT_IDX_SEL_MASK,
			       dts->peci_agent_idx & ITL_DTS_PECI_AGENT_IDX_SEL_MASK);
		hwm_reg_set_bits(ITL_DTS_CFG_REG, ITL_DTS_PECI_CFG_ADJUSTMENT);
	}

	init_once = 1;
}

static void init_intel_dts2_sensor(const struct nct6687d_dts2_sensor_config *dts2)
{
	static int init_once = 0;

	if (init_once)
		return;

	printk(BIOS_DEBUG, "NCT6687D programming DTS2 sensor\n");

	hwm_reg_write(DTS2_TARGET_MARGIN_REG, (uint8_t)dts2->target_margin);
	hwm_reg_write(DTS2_TCONTROL_REG, (uint8_t)dts2->t_control);
	hwm_reg_write(DTS2_TCONTROL_OFFSET_REG, (uint8_t)dts2->t_control_offset);
	hwm_reg_write(DTS2_TARGET_TOLERANCE_REG, dts2->target_margin_tolerance);

	hwm_reg_write(DTS2_STEP_SPEED_HI_REG, dts2->step_speed >> 8);
	hwm_reg_write(DTS2_STEP_SPEED_LO_REG, dts2->step_speed & 0xff);
	hwm_reg_write(DTS2_MIN_SPEED_HI_REG, dts2->min_speed >> 8);
	hwm_reg_write(DTS2_MIN_SPEED_LO_REG, dts2->min_speed & 0xff);

	hwm_reg_write(DTS2_DELAY_TIME_COUNTER_REG, dts2->delay_time);
	hwm_reg_write(DTS2_DIVISOR_REG, dts2->divisor);

	init_once = 1;
}

static void lock_sensor_config(void)
{
	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping misc fan config programming\n");
		return;
	}
	hwm_reg_set_bits(HWM_CONFIG_REG, LOCK_SENSOR_CFG);
	lock_fan_register_set_and_check(FAN_NO_ERROR);
}

static void init_ambient_floor_alg(const struct superio_nuvoton_nct6687d_config *conf,
				   unsigned int idx)
{
	static int init_once = 0;
	const struct nct6687d_ambient_floor_fan_config *amb_floor_fan;
	const struct nct6687d_ambient_floor_config *amb_floor_cfg = &conf->ambient_floor;

	amb_floor_fan = &conf->fans[idx - 1].amb_floor_fan_cfg;

	printk(BIOS_DEBUG, "NCT6687D initializing ambient floor algorithm\n");

	if (!init_once) {
		hwm_reg_write(AMBIENT_FLOOR_TEMP_START_POINT_REG, amb_floor_cfg->temp_start);
		hwm_reg_write(AMBIENT_FLOOR_TEMP_END_POINT_REG, amb_floor_cfg->temp_end);
		init_once = 1;
	}

	hwm_reg_write(FAN_AMB_FLOOR_MIN_OUT_START_REG(idx),
		      FAN_AMB_FLOOR_RPM_TO_REG(amb_floor_fan->minout_start));
	hwm_reg_write(FAN_AMB_FLOOR_MIN_OUT_END_REG(idx),
		      FAN_AMB_FLOOR_RPM_TO_REG(amb_floor_fan->minout_end));
	hwm_reg_write(FAN_AMB_FLOOR_MAX_OUT_REG(idx),
		      FAN_AMB_FLOOR_RPM_TO_REG(amb_floor_fan->minout_max));
}

static void init_smart_tracking_alg(const struct superio_nuvoton_nct6687d_config *conf,
				    unsigned int idx)
{
	static int init_once = 0;
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];
	const struct nct6687d_smart_tracking_config *smart_track = &conf->smart_tracking;

	printk(BIOS_DEBUG, "NCT6687D initializing smart tracking algorithm\n");

	if (!init_once) {
		hwm_reg_write(FAN_LOW_RPM_SPEED_BOUNDARY_HI_REG,
			      smart_track->speed_boundary_low >> 8);
		hwm_reg_write(FAN_LOW_RPM_SPEED_BOUNDARY_LO_REG,
			      smart_track->speed_boundary_low & 0xff);
		hwm_reg_write(FAN_HIGH_RPM_SPEED_BOUNDARY_HI_REG,
			      smart_track->speed_boundary_high >> 8);
		hwm_reg_write(FAN_HIGH_RPM_SPEED_BOUNDARY_LO_REG,
			      smart_track->speed_boundary_high & 0xff);
		hwm_reg_write(FAN_LOW_RPM_TOLERANCE_REG, smart_track->rpm_tolerance_low);
		hwm_reg_write(FAN_MID_RPM_TOLERANCE_REG, smart_track->rpm_tolerance_mid);
		hwm_reg_write(FAN_HIGH_RPM_TOLERANCE_REG, smart_track->rpm_tolerance_high);

		hwm_reg_write(FAN_TRACKING_STEP_REG, ((smart_track->step_up & 0xf) << 4) |
						     (smart_track->step_down & 0xf));

		hwm_reg_write(FAN_FAST_TRACK_TEMP_BOUNDARY_REG, smart_track->temp_boundary);

		hwm_reg_write(FAN_LOW_RPM_FAST_TRACK_WEIGHT_REG,
			      ((smart_track->weight_up_low & 7) << 4) |
			      (smart_track->weight_down_low & 7));
		hwm_reg_write(FAN_MID_RPM_FAST_TRACK_WEIGHT_REG,
			      ((smart_track->weight_up_mid & 7) << 4) |
			      (smart_track->weight_down_mid & 7));
		hwm_reg_write(FAN_HIGH_RPM_FAST_TRACK_WEIGHT_REG,
			      ((smart_track->weight_up_high & 7) << 4) |
			      (smart_track->weight_down_high & 7));

		hwm_reg_write(FAN_LOW_RPM_FAST_TRACK_DUTY_STEP_REG,
			      smart_track->duty_step_low & 0xf);
		hwm_reg_write(FAN_MID_RPM_FAST_TRACK_DUTY_STEP_REG,
			      smart_track->duty_step_mid & 0xf);
		hwm_reg_write(FAN_HIGH_RPM_FAST_TRACK_DUTY_STEP_REG,
			      smart_track->duty_step_high & 0xf);

		hwm_reg_write(FAN_MARKUP_TRACK_AMB_TEMP_BOUNDARY_REG,
			      smart_track->ambient_temp_boundary);

		hwm_reg_write(FAN_MARKUP_TRACK_WEIGHT_REG, smart_track->weight_val & 7);

		init_once = 1;
	}

	if (fan->fast_tracking_en)
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_FAST_TRACK_EN);


	if (fan->markup_tracking_en)
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_MARKUP_TRACK_EN);
}

static void misc_fan_config(const struct superio_nuvoton_nct6687d_config *conf,
			    unsigned int idx)
{
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];

	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping misc fan config programming\n");
		return;
	}

	if (fan->fan_alg_weight) {
		printk(BIOS_DEBUG, "Programming algorithm weight for FAN%d\n", idx);
		hwm_reg_and_or(FAN_ALG_ENGINE_WEIGHT_VAL_REG(idx),
			       ~FAN_ALG_ENGINE_WEIGHT_MASK(idx),
			       (fan->fan_alg_weight & 0xf) <<
					FAN_ALG_ENGINE_WEIGHT_SHIFT(idx));
		hwm_reg_set_bits(FAN_ALG_ENGINE_WEIGHT_EN_REG(idx),
				 FAN_ALG_ENGINE_WEIGHT_CHANNEL_EN(idx));
		hwm_reg_set_bits(FAN_ALG_FUNCTRL_REG(idx), FAN_ALG_FUNCTRL_ALG_EN);
	}

	if (fan->crit_temp) {
		if (!check_cond(fan->crit_temp >= fan->crit_temp_tolerance,
				"Fan Critical Temperature should be >= "
				"Critical Temperature Tolerance")) {
			printk(BIOS_DEBUG, "Enabling critical temperature for FAN%d\n", idx);
			hwm_reg_write(FAN_CRIT_TEMP_CFG_REG(idx), fan->crit_temp & 0x7f);
			if (fan->crit_temp_tolerance)
				hwm_reg_write(FAN_CRIT_TEMP_TOLERANCE_REG(idx),
					      fan->crit_temp_tolerance & 0xf);

			hwm_reg_set_bits(FAN_ALG_FUNCTRL_REG(idx),
					 FAN_ALG_FUNCTRL_CRIT_TEMP_EN);
		}
	}

	if (fan->temp_err_duty) {
		hwm_reg_write(FAN_TEMP_ERR_DUTY_VAL_REG(idx), fan->temp_err_duty);
		hwm_reg_set_bits(FAN_ALG_FUNCTRL_REG(idx), FAN_ALG_FUNCTRL_TEMP_ERR);
	}

	if (fan->dts1_en && fan->dts2_en) {
		printk(BIOS_ERR, "DTS and DTS2 sensors can't coexist!\n");
		printk(BIOS_ERR, "Skipping DTS configuration\n");
	} else {
		if (fan->dts1_en) {
			init_intel_dts_sensor(&conf->dts_sensor);
			hwm_reg_set_bits(FAN_ALG_FUNCTRL_REG(idx), FAN_ALG_FUNCTRL_DTS1_EN);
		}

		if (fan->dts2_en) {
			init_intel_dts2_sensor(&conf->dts2_sensor);
			hwm_reg_set_bits(FAN_ALG_FUNCTRL_REG(idx), FAN_ALG_FUNCTRL_DTS2_EN);
		}
	}

	if (fan->dts_ub_en)
		hwm_reg_set_bits(FAN_ALG_FUNCTRL_REG(idx), FAN_ALG_FUNCTRL_DTS_UB_EN);


	if (fan->smart_tracking_en) {
		init_smart_tracking_alg(conf, idx);
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_SMART_TRACK_EN);
	}

	if (conf->fan_default_val && conf->fan_default_val <= 100)
		hwm_reg_write(FAN_DEFAULT_VAL_REG,
			      FAN_PWM_PERCENT_TO_HEX(conf->fan_default_val));

	/* Some registers for functionalities below are missing for fan 9 and 10 */
	if (idx > 8) {
		lock_fan_register_set_and_check(FAN_NO_ERROR);
		return;
	}

	if (fan->ambient_floor_en) {
		init_ambient_floor_alg(conf, idx);
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_AMBIENT_FLOOR_EN);
	}

	if (fan->startup_duty && fan->startup_duty <= 100) {
		hwm_reg_write(FAN_STARTUP_DUTY_REG(idx),
			      FAN_PWM_PERCENT_TO_HEX(fan->startup_duty));
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_STARTUP_EN);
	}

	if (fan->manual_offset) {
		hwm_reg_write(FAN_MANUAL_OFFSET_REG(idx), fan->manual_offset);
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_MANUAL_OFFSET_EN);
	}

	if (fan->min_duty && fan->min_duty <= 100) {
		hwm_reg_write(FAN_MIN_DUTY_REG(idx), FAN_PWM_PERCENT_TO_HEX(fan->min_duty));
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_MINDUTY_EN);
	}

	lock_fan_register_set_and_check(FAN_NO_ERROR);
}

static void set_fan_pins_and_mode(const struct nct6687d_fan_config *fan, unsigned int idx)
{
	enum nct6687d_fan_mode mode = fan->mode;

	/* FAN_IGNORE takes the mapping of manual mode to detect uninitialized fans */
	if (mode == FAN_MODE_MANUAL)
		mode = 0;

	hwm_reg_and_or(FAN_MODE_SEL_REG(idx), ~FAN_MODE_SEL_MASK, mode & FAN_MODE_SEL_MASK);
	hwm_reg_and_or(FANIN_CFG_REG(idx), ~FANIN_PIN_SEL_MASK,
		       fan->fanin_sel & FANIN_PIN_SEL_MASK);
	hwm_reg_set_bits(FANIN_CFG_REG(idx), FANIN_MONITOR_EN);

	hwm_reg_and_or(FANOUT_CFG_REG(idx), ~FANOUT_PIN_SEL_MASK,
		       fan->fanout_sel & FANOUT_PIN_SEL_MASK);
	hwm_reg_set_bits(FANOUT_CFG_REG(idx), FANOUT_EN);
}

static bool smart_fan_level_check(const struct nct6687d_fan_config *fan,
				  const struct nct6687d_smart_fan_iv_config *s_fan,
				  int i)
{
	bool failure = false;

	failure |= check_cond(s_fan->temp_levels[i] <= 127,
			      "Smart Fan IV Temperature Levels should be <= 127");
	if (i < MAX_TEMP_SPEED_LEVELS - 1) {
		failure |= check_cond(s_fan->temp_levels[i + 1] >= s_fan->temp_levels[i],
				      "Smart Fan IV next Temperature Level must be higher"
				      " or equal to previous Temperature Level");
		failure |= check_cond(s_fan->speed_levels[i + 1] >=  s_fan->speed_levels[i],
				      "Smart Fan IV next Speed Level must be higher or "
				      "equal to previous Speed Level");
	}

	if (fan->unit_sel == FAN_PWM) {
		failure |= check_cond(s_fan->speed_levels[i] <= 100,
				      "Smart Fan IV PWM Speed Levels should be <= 100");
	} else if (fan->unit_sel == FAN_RPM) {
		failure |= check_cond(s_fan->speed_levels[i] <= 0x3fff,
				      "Smart Fan IV RPM Speed Levels should be"
				      "<= 16383 RPM");
	} else {
		failure |= check_cond(false, "Smart Fan IV invalid Fan Unit Selection");
	}

	return failure;

}

static bool smart_fan_config_check(const struct nct6687d_fan_config *fan,
				   const struct nct6687d_smart_fan_iv_config *s_fan)
{
	bool failure = false;
	unsigned int i;

	for (i = 0; i < MAX_TEMP_SPEED_LEVELS; i++)
		failure |= smart_fan_level_check(fan, s_fan, i);

	failure |= check_cond(s_fan->temp_levels[0] >= s_fan->temp_cut_off,
			      "Smart Fan IV Temperature Level 1 should be >= "
			      "Temperature Cut Off");
	failure |= check_cond(s_fan->temp_levels[0] >=
			      (s_fan->temp_cut_off + s_fan->temp_hystheresis),
			      "Smart Fan IV Temperature Level 1 should be >= "
			      "Temperature Cut Off + Temperature Hysteresis");

	return failure;
}

static void init_smart_fan(const struct superio_nuvoton_nct6687d_config *conf, unsigned int idx)
{
	unsigned int i;
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];
	const struct nct6687d_smart_fan_iv_config *sfan = &fan->smart_fan;

	if (smart_fan_config_check(fan, sfan)) {
		printk(BIOS_ERR, "NCT6687D Smart Fan IV invalid config, "
				 "skipping fan programming\n");
		return;
	}

	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping Smart Fan IV config programming\n");
		return;
	}

	set_fan_pins_and_mode(fan, idx);

	if (fan->unit_sel == FAN_PWM)
		hwm_reg_and_or(FAN_FUNCTION_CTRL(idx), ~FAN_FUN_UNIT_RPM & 0xff,
			       FAN_FUN_UNIT_PWM_DUTY);
	else if (fan->unit_sel == FAN_RPM)
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_UNIT_RPM);

	/* Registers start from MSB */
	for (i = 0; i < MAX_TEMP_SRC; i++) {
		hwm_reg_write(FAN_MTZ_DCS_DATA_REG(MAX_TEMP_SRC - 1 - i, idx),
			      sfan->temp_src[i]);
	}

	for (i = 0; i < MAX_TEMP_SPEED_LEVELS; i++) {
		if (sfan->temp_levels[i])
			hwm_reg_write(FAN_SF4_TEMP_LVL_REG(idx, i),
				      sfan->temp_levels[i]);
		if (sfan->speed_levels[i]) {
			if (fan->unit_sel == FAN_PWM) {
				hwm_reg_write(FAN_SF4_PWM_RPM_LVL_HI_REG(idx, i), 0);
				hwm_reg_write(FAN_SF4_PWM_RPM_LVL_LO_REG(idx, i),
					      FAN_PWM_PERCENT_TO_HEX(sfan->speed_levels[i]));
			}

			if (fan->unit_sel == FAN_RPM) {
				hwm_reg_write(FAN_SF4_PWM_RPM_LVL_HI_REG(idx, i),
					      sfan->speed_levels[i] >> 8);
				hwm_reg_write(FAN_SF4_PWM_RPM_LVL_LO_REG(idx, i),
					      sfan->speed_levels[i] & 0xff);
			}
		}
	}

	hwm_reg_write(FAN_SF4_TEMP_OFF_HYSTHERESIS_REG(idx), sfan->temp_hystheresis);
	hwm_reg_write(FAN_SF4_TEMP_CUT_OFF_REG(idx), sfan->temp_cut_off);
	hwm_reg_write(FAN_SF4_TEMP_OFF_DELAY_REG(idx), sfan->cut_off_delay);

	hwm_reg_write(FAN_STEP_UP_TIME_REG(idx), sfan->step_up_time);
	hwm_reg_write(FAN_STEP_DOWN_TIME_REG(idx), sfan->step_down_time);

	/* Set fan mode to automatic */
	hwm_reg_and_or(FAN_MANUAL_EN_REG(idx), ~FAN_MANUAL_EN(idx), 0x00);

	hwm_reg_set_bits(FAN_CHANNEL_EN_REG(idx), FAN_CHANNEL_EN(idx));

	lock_fan_register_set_and_check(FAN_ERR_SMART_FAN_IV);

	misc_fan_config(conf, idx);
}

static void init_manual_fan(const struct superio_nuvoton_nct6687d_config *conf,
			    unsigned int idx)
{
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];
	const struct nct6687d_manual_fan_config *manual_fan = &fan->manual_fan;

	if (manual_fan->manual_duty || manual_fan->manual_duty > 100) {
		printk(BIOS_ERR, "NCT6687D: invalid duty cycle for manual fan mode\n");
		return;
	}

	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping manual fan config programming\n");
		return;
	}

	set_fan_pins_and_mode(fan, idx);

	if (fan->unit_sel == FAN_PWM)
		hwm_reg_and_or(FAN_FUNCTION_CTRL(idx), ~FAN_FUN_UNIT_RPM & 0xff,
			       FAN_FUN_UNIT_PWM_DUTY);
	else if (fan->unit_sel == FAN_RPM)
		hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_UNIT_RPM);

	hwm_reg_write(FAN_MANUAL_VALUE_REG(idx),
		      FAN_PWM_PERCENT_TO_HEX(manual_fan->manual_duty));

	hwm_reg_set_bits(FAN_MANUAL_EN_REG(idx), FAN_MANUAL_EN(idx));

	hwm_reg_set_bits(FAN_CHANNEL_EN_REG(idx), FAN_CHANNEL_EN(idx));

	lock_fan_register_set_and_check(FAN_NO_ERROR);

	misc_fan_config(conf, idx);
}

static bool thermal_cruise_config_check(const struct nct6687d_fan_thermal_cruise_config *tc_fan)
{
	bool failure = false;

	failure |= check_cond(tc_fan->step_down_time > 0,
			      "Thermal Cruise Step-Down Time should be > 0");
	failure |= check_cond(tc_fan->step_up_time > 0,
			      "Thermal Cruise Step-Up Time should be > 0");
	failure |= check_cond(tc_fan->initial_value > tc_fan->stop_value,
			      "Thermal Cruise Initial value should be > Stop Value");
	failure |= check_cond(tc_fan->target_temp >= tc_fan->temp_tolerance,
			      "Thermal Cruise Target Temperature should be"
			      " >= Temperature Tolerance");
	if (tc_fan->keep_min_fan_output) {
		failure |= check_cond(tc_fan->stop_value > 0,
				      "Thermal Cruise Stop Value should be > 0");
	} else {
		failure |= check_cond(tc_fan->stop_time > 0,
				      "Thermal Cruise Stop Time should be > 0");
	}

	return failure;
}

static void init_thermal_cruise_fan(const struct superio_nuvoton_nct6687d_config *conf,
				    unsigned int idx)
{
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];
	const struct nct6687d_fan_thermal_cruise_config *tc_fan = &fan->thermal_cruise_fan;

	if (thermal_cruise_config_check(tc_fan)) {
		printk(BIOS_ERR, "NCT6687D Thermal Cruise invalid config, "
				 "skipping fan programming\n");
		return;
	}

	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping Thermal Cruise fan config programming\n");
		return;
	}

	set_fan_pins_and_mode(fan, idx);

	hwm_reg_write(FAN_TC_TARGET_TEMP_REG(idx),
		      tc_fan->target_temp & FAN_TC_TARGET_TEMP_MASK);

	if (tc_fan->keep_min_fan_output) {
		hwm_reg_set_bits(FAN_TC_TARGET_TEMP_REG(idx), FAN_TC_KEEP_MIN_OUTPUT);
		hwm_reg_write(FAN_TC_STOP_VALUE_REG(idx), tc_fan->stop_value);
	}

	hwm_reg_write(FAN_TC_TARGET_TEMP_TOLERANCE_REG(idx),
		      tc_fan->temp_tolerance & FAN_TC_TEMP_TOLERANCE_MASK);

	hwm_reg_write(FAN_INITIAL_VALUE_REG(idx), tc_fan->initial_value);

	hwm_reg_write(FAN_TC_STOP_TIME_REG(idx), tc_fan->stop_time);
	hwm_reg_write(FAN_TC_STEP_DOWN_TIME_REG(idx), tc_fan->step_up_time);
	hwm_reg_write(FAN_TC_STEP_UP_TIME_REG(idx), tc_fan->step_down_time);

	/* Set fan mode to automatic */
	hwm_reg_and_or(FAN_MANUAL_EN_REG(idx), ~FAN_MANUAL_EN(idx), 0);

	hwm_reg_set_bits(FAN_CHANNEL_EN_REG(idx), FAN_CHANNEL_EN(idx));

	lock_fan_register_set_and_check(FAN_ERR_THERMAL_CRUISE);

	misc_fan_config(conf, idx);
}

static bool speed_cruise_config_check(const struct nct6687d_fan_speed_cruise_config *sc_fan)
{
	bool failure = false;

	failure |= check_cond(sc_fan->target_rpm <= 0x3fff,
			      "Speed Cruise Target RPM should be <= 16838 RPM");
	failure |= check_cond(sc_fan->target_rpm >= sc_fan->rpm_tolerance,
			      "Speed Cruise Target RPM should be >= RPM Tolerance");
	failure |= check_cond(sc_fan->step_down_time > 0,
			      "Speed Cruise Step-Down Time should be > 0");
	failure |= check_cond(sc_fan->step_up_time > 0,
			      "Speed Cruise Step-Up Time should be > 0");

	return failure;
}

static void init_speed_cruise_fan(const struct superio_nuvoton_nct6687d_config *conf,
				  unsigned int idx)
{
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];
	const struct nct6687d_fan_speed_cruise_config *sc_fan = &fan->speed_cruise_fan;

	if (speed_cruise_config_check(sc_fan)) {
		printk(BIOS_ERR, "NCT6687D Speed Cruise invalid config, "
				 "skipping fan programming\n");
		return;
	}

	if (!unlock_fan_register_set()) {
		printk(BIOS_ERR, "NCT6687D failed to unlock registers, "
				 "skipping Speed Cruise fan config programming\n");
		return;
	}

	set_fan_pins_and_mode(fan, idx);

	/* Speed cruise should only enable RPM units */
	hwm_reg_set_bits(FAN_FUNCTION_CTRL(idx), FAN_FUN_UNIT_RPM);

	hwm_reg_write(FAN_SC_TARGET_RPM_HI_REG(idx), sc_fan->target_rpm >> 8);
	hwm_reg_write(FAN_SC_TARGET_RPM_LO_REG(idx), sc_fan->target_rpm & 0xff);

	hwm_reg_write(FAN_SC_TARGET_RPM_TOLERANCE_HI_REG(idx), sc_fan->rpm_tolerance >> 8);
	hwm_reg_write(FAN_SC_TARGET_RPM_TOLERANCE_LO_REG(idx), sc_fan->rpm_tolerance & 0xff);

	hwm_reg_write(FAN_SC_STEP_DOWN_TIME_REG(idx), sc_fan->step_up_time);
	hwm_reg_write(FAN_SC_STEP_UP_TIME_REG(idx), sc_fan->step_down_time);

	/* Set fan mode to automatic */
	hwm_reg_and_or(FAN_MANUAL_EN_REG(idx), ~FAN_MANUAL_EN(idx), 0x00);

	hwm_reg_set_bits(FAN_CHANNEL_EN_REG(idx), FAN_CHANNEL_EN(idx));

	lock_fan_register_set_and_check(FAN_ERR_SPEED_CRUISE);

	misc_fan_config(conf, idx);
};

static void init_one_fan(const struct superio_nuvoton_nct6687d_config *conf, unsigned int idx)
{
	const struct nct6687d_fan_config *fan = &conf->fans[idx - 1];

	switch (fan->mode) {
	case FAN_THERMAL_CRUISE:
		printk(BIOS_DEBUG, "Initializing Thermal Cruise for FAN%d\n", idx);
		init_thermal_cruise_fan(conf, idx);
		break;
	case FAN_SPEED_CRUISE:
		printk(BIOS_DEBUG, "Initializing Speed Cruise for FAN%d\n", idx);
		init_speed_cruise_fan(conf, idx);
		break;
	case FAN_SMART_FAN_IV:
		printk(BIOS_DEBUG, "Initializing Smart FAN IV for FAN%d\n", idx);
		init_smart_fan(conf, idx);
		break;
	case FAN_PID_CONTROL:
		printk(BIOS_WARNING, "NCT6687D: PID Control fan mode not yet supported\n");
		break;
	case FAN_MODE_MANUAL:
		printk(BIOS_DEBUG, "Initializing manual fan mode for FAN%d\n", idx);
		init_manual_fan(conf, idx);
		break;
	default:
		printk(BIOS_ERR, "NCT6687D: unknown fan mode detected!\n");
	}
}

static void init_fans(const struct superio_nuvoton_nct6687d_config *conf)
{
	unsigned int i;
	const struct nct6687d_fan_config *fans = conf->fans;

	for (i = 1; i <= MAX_NUM_FANS; i++) {
		if (fans[i - 1].mode == FAN_IGNORE)
			continue;

		init_one_fan(conf, i);
	}
}

static void report_ec_info(void)
{
	unsigned int i;

	printk(BIOS_DEBUG, "NCT6687D EC info:\n");
	printk(BIOS_DEBUG, "\tChip ID: %04x\n",
	       (uint16_t)hwm_reg_read(CHIP_ID0_REG) |
	       (uint16_t)(hwm_reg_read(CHIP_ID1_REG) << 8));
	printk(BIOS_DEBUG, "\tCustomer ID: %04x\n",
	       (uint16_t)hwm_reg_read(CUSTOMER_ID0_REG) |
	       (uint16_t)(hwm_reg_read(CUSTOMER_ID1_REG) << 8));
	printk(BIOS_DEBUG, "\tFW build date: %02d/%02d/20%02d\n",
	       hwm_reg_read(FW_BUILD_DAY_REG),
	       hwm_reg_read(FW_BUILD_MONTH_REG),
	       hwm_reg_read(FW_BUILD_YEAR_REG));
	printk(BIOS_DEBUG, "\tFW build serial number: %d\n",
	       hwm_reg_read(FW_BUILD_SERIALNUM_REG));
	printk(BIOS_DEBUG, "\tFW version: %d.%d\n",
	       hwm_reg_read(FW_VER0_REG),
	       hwm_reg_read(FW_VER1_REG));
	printk(BIOS_DEBUG, "\tProfile version: %d\n",
	       hwm_reg_read(PROFILE_VER_REG));
	printk(BIOS_DEBUG, "\tROM version: %d.%d.%d.%d\n",
	       hwm_reg_read(ROM_VER0_REG), hwm_reg_read(ROM_VER1_REG),
	       hwm_reg_read(ROM_VER2_REG), hwm_reg_read(ROM_VER3_REG));
	printk(BIOS_DEBUG, "\tISP build date: %02d/%02d/20%02d\n",
	       hwm_reg_read(ISP_BUILD_DAY_REG),
	       hwm_reg_read(ISP_BUILD_MONTH_REG),
	       hwm_reg_read(ISP_BUILD_YEAR_REG));
	printk(BIOS_DEBUG, "\tISP build serial number: %d\n",
	       hwm_reg_read(ISP_BUILD_SERIALNUM_REG));
	printk(BIOS_DEBUG, "\tISP version: %d.%d\n",
	       hwm_reg_read(ISP_VER0_REG),
	       hwm_reg_read(ISP_VER1_REG));
	printk(BIOS_DEBUG, "\tOEM version:");

	for (i = 0; i < OEM_VER_LEN; i++)
		printk(BIOS_DEBUG, "%02x", hwm_reg_read(OEM_VER_REG + i));

	printk(BIOS_DEBUG, "\n");
}

void nct6687d_hwm_init(uint16_t hwm_base, const struct superio_nuvoton_nct6687d_config *conf)
{
	nct6687d_hwm_base = hwm_base;

	report_ec_info();

	init_sensors(conf);
	init_fans(conf);

	lock_sensor_config();
	print_fan_engine_status();
}
