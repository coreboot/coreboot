/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT6687D_CHIP_H
#define SUPERIO_NUVOTON_NCT6687D_CHIP_H

#include <stdbool.h>

#define MAX_TEMP_SPEED_LEVELS 7
#define MAX_TEMP_SRC 4
#define MAX_NUM_FANS 10
#define MAX_NUM_SENSORS 32
#define MAX_WEIGHT_POINTS 8
#define MAX_DTS_CTL_POINTS 5

enum nct6687d_sensor_src_select {
	/* TIN sources */
	SENSOR_DISABLE = 0x00,
	SENSOR_LOCAL = 0x01,
	TD0P_CURRENT_MODE = 0x02,
	TD1P_CURRENT_MODE = 0x03,
	TD2P_CURRENT_MODE = 0x04,
	TD0P_VOLTAGE_MODE = 0x05,
	TD1P_VOLTAGE_MODE = 0x06,
	TD2P_VOLTAGE_MODE = 0x07,
	THERMISTOR14 = 0x08,
	THERMISTOR15 = 0x09,
	THERMISTOR16 = 0x0A,
	THERMISTOR0 = 0x0B,
	THERMISTOR1 = 0x0C,
	THERMISTOR2 = 0x0D,
	THERMISTOR3 = 0x0E,
	THERMISTOR4 = 0x0F,
	THERMISTOR5 = 0x10,
	THERMISTOR6 = 0x11,
	THERMISTOR7 = 0x12,
	THERMISTOR8 = 0x13,
	THERMISTOR9 = 0x14,
	THERMISTOR10 = 0x15,
	THERMISTOR11 = 0x16,
	THERMISTOR12 = 0x17,
	THERMISTOR13 = 0x18,
	PECI_AGENT0_DOMAIN0 = 0x20,
	PECI_AGENT1_DOMAIN0 = 0x21,
	PECI_AGENT2_DOMAIN0 = 0x22,
	PECI_AGENT3_DOMAIN0 = 0x23,
	PECI_AGENT0_DOMAIN1 = 0x24,
	PECI_AGENT1_DOMAIN1 = 0x25,
	PECI_AGENT2_DOMAIN1 = 0x26,
	PECI_AGENT3_DOMAIN1 = 0x27,
	PECI_DIMM_TMP0 = 0x28,
	PECI_DIMM_TMP1 = 0x29,
	PECI_DIMM_TMP2 = 0x2A,
	PECI_DIMM_TMP3 = 0x2B,
	PCH_CPU = 0x30,
	PCH_CHIP = 0x31,
	PCH_CHIP_CPU_MAX = 0x32,
	PCH_MCH = 0x33,
	PCH_DIMM0 = 0x34,
	PCH_DIMM1 = 0x35,
	PCH_DIMM2 = 0x36,
	PCH_DIMM3 = 0x37,
	SMBUS_THERMAL_SENSOR0 = 0x38,
	SMBUS_THERMAL_SENSOR1 = 0x39,
	SMBUS_THERMAL_SENSOR2 = 0x3A,
	SMBUS_THERMAL_SENSOR3 = 0x3B,
	SMBUS_THERMAL_SENSOR4 = 0x3C,
	SMBUS_THERMAL_SENSOR5 = 0x3D,
	DIMM_THERMAL_SENSOR0 = 0x3E,
	DIMM_THERMAL_SENSOR1 = 0x3F,
	DIMM_THERMAL_SENSOR2 = 0x40,
	DIMM_THERMAL_SENSOR3 = 0x41,
	AMD_TSI_ADDRESS_0x90 = 0x42,
	AMD_TSI_ADDRESS_0x92 = 0x43,
	AMD_TSI_ADDRESS_0x94 = 0x44,
	AMD_TSI_ADDRESS_0x96 = 0x45,
	AMD_TSI_ADDRESS_0x98 = 0x46,
	AMD_TSI_ADDRESS_0x9A = 0x47,
	AMD_TSI_ADDRESS_0x9C = 0x48,
	AMD_TSI_ADDRESS_0x9D = 0x49,
	VIRTUAL_TEMPIN0 = 0x50,
	VIRTUAL_TEMPIN1 = 0x51,
	VIRTUAL_TEMPIN2 = 0x52,
	VIRTUAL_TEMPIN3 = 0x53,
	VIRTUAL_TEMPIN4 = 0x54,
	VIRTUAL_TEMPIN5 = 0x55,
	VIRTUAL_TEMPIN6 = 0x56,
	VIRTUAL_TEMPIN7 = 0x57,
	/* VIN sources */
	VCC = 0x60,
	VSB = 0x61,
	AVSB = 0x62,
	VTT = 0x63,
	VBAT = 0x64,
	VREF = 0x65,
	VIN0 = 0x66,
	VIN1 = 0x67,
	VIN2 = 0x68,
	VIN3 = 0x69,
	VIN4 = 0x6A,
	VIN5 = 0x6B,
	VIN6 = 0x6C,
	VIN7 = 0x6D,
	VIN8 = 0x6E,
	VIN9 = 0x6F,
	VIN10 = 0x70,
	VIN11 = 0x71,
	VIN12 = 0x72,
	VIN13 = 0x73,
	VIN14 = 0x74,
	VIN15 = 0x75,
	VIN16 = 0x76,
};

enum nct6687d_peci_speed {
	PECI_2MHZ = 0,
	PECI_1200KHZ,
	PECI_800KHZ,
	PECI_400KHZ,
};

enum nct6687d_baud_rate {
	BAUD_12_5K = 0,
	BAUD_25K,
	BAUD_50K,
	BAUD_100K,
	BAUD_200K,
	BAUD_400K,
	BAUD_800K,
	BAUD_1200K,
};

enum nct6687d_fan_mode {
	FAN_IGNORE = 0,
	FAN_THERMAL_CRUISE,
	FAN_SPEED_CRUISE,
	FAN_SMART_FAN_IV,
	FAN_PID_CONTROL,
	FAN_MODE_MANUAL,
};

enum nct6687d_tach_pwm_sel {
	TACH_PWM0 = 0,
	TACH_PWM1,
	TACH_PWM2,
	TACH_PWM3,
	TACH_PWM4,
	TACH_PWM5,
	TACH_PWM6,
	TACH_PWM7,
	TACH_PWM8,
	TACH_PWM9,
	TACH_PWM10,
	TACH_PWM11,
	TACH_PWM12,
	TACH_PWM13,
	TACH_PWM14,
	TACH_PWM15,
	TACH_PWM16,
	TACH_PWM17,
	TACH_PWM18,
	TACH_PWM19,
	TACH_PWM20,
	TACH_PWM21,
	TACH_PWM22,
	TACH_PWM23,
};

enum nct6687d_fan_unit_sel {
	FAN_PWM = 0,
	FAN_RPM,
};

enum nct6687d_fast_track_weight {
	WEIGHT_DIV_1 = 0,
	WEIGHT_DIV_2,
	WEIGHT_DIV_4,
	WEIGHT_DIV_8,
	WEIGHT_DIV_16,
	WEIGHT_DIV_32,
	WEIGHT_DIV_64,
};

struct nct6687d_pch_smbus_sensor {
	bool	sensor_en;
	uint8_t	sensor_idx;
	bool	report_one_byte;
	uint8_t port_sel;
	enum	nct6687d_baud_rate baud_rate;
	uint8_t	sensor_addr;
	uint8_t	sensor_cmd;
};

struct nct6687d_dts_sensor_config {
	uint8_t ambient_temp[MAX_DTS_CTL_POINTS];
	uint16_t rpm_start_point[MAX_DTS_CTL_POINTS];
	uint16_t rpm_end_point[MAX_DTS_CTL_POINTS];
	uint8_t	temp_start;
	uint8_t	temp_end;
	uint16_t max_speed;
	bool peci_adjust;
	uint8_t peci_agent_idx;
};

struct nct6687d_dts2_sensor_config {
	int8_t	target_margin;
	uint8_t	target_margin_tolerance;
	int8_t	t_control;
	int8_t	t_control_offset;
	uint16_t step_speed;
	uint16_t min_speed;
	/* Unit is 1s */
	uint8_t delay_time;
	uint8_t divisor;
};

struct nct6687d_smart_fan_iv_config {
	uint8_t temp_src[MAX_TEMP_SRC];
	uint8_t temp_levels[MAX_TEMP_SPEED_LEVELS];
	uint16_t speed_levels[MAX_TEMP_SPEED_LEVELS];
	uint8_t temp_hystheresis;
	uint8_t temp_cut_off;
	uint8_t cut_off_delay;
	/* Time in 100ms units */
	uint8_t step_up_time;
	uint8_t step_down_time;
};

struct nct6687d_manual_fan_config {
	uint8_t manual_duty;
};

struct nct6687d_fan_thermal_cruise_config {
	bool keep_min_fan_output;
	uint8_t target_temp;
	uint8_t temp_tolerance;
	uint8_t initial_value;
	uint8_t stop_value;
	/* Time in 100ms units */
	uint8_t stop_time;
	uint8_t step_up_time;
	uint8_t step_down_time;
};

struct nct6687d_fan_speed_cruise_config {
	uint16_t target_rpm;
	uint16_t rpm_tolerance;
	/* Time in 100ms units */
	uint8_t step_up_time;
	uint8_t step_down_time;
};

struct nct6687d_ambient_floor_fan_config {
	/* Units is RPM */
	uint16_t minout_start;
	uint16_t minout_end;
	uint16_t minout_max;
};

struct nct6687d_ambient_floor_config {
	uint8_t temp_start;
	uint8_t temp_end;
};

struct nct6687d_smart_tracking_config {
	uint8_t rpm_tolerance_low;
	uint8_t rpm_tolerance_mid;
	uint8_t rpm_tolerance_high;

	uint16_t speed_boundary_low;
	uint16_t speed_boundary_high;

	/* Step tracking setting */
	uint8_t step_up;
	uint8_t step_down;

	/* Fast tracking settings */
	uint8_t temp_boundary;

	enum nct6687d_fast_track_weight weight_up_low;
	enum nct6687d_fast_track_weight weight_up_mid;
	enum nct6687d_fast_track_weight weight_up_high;
	enum nct6687d_fast_track_weight weight_down_low;
	enum nct6687d_fast_track_weight weight_down_mid;
	enum nct6687d_fast_track_weight weight_down_high;

	uint8_t duty_step_low;
	uint8_t duty_step_mid;
	uint8_t duty_step_high;

	/* Markup tracking setting */
	uint8_t ambient_temp_boundary;
	uint8_t weight_val;
};


struct nct6687d_fan_config {
	enum nct6687d_fan_mode mode;
	enum nct6687d_fan_unit_sel unit_sel;
	enum nct6687d_tach_pwm_sel fanin_sel;
	enum nct6687d_tach_pwm_sel fanout_sel;

	struct nct6687d_smart_fan_iv_config smart_fan;
	struct nct6687d_manual_fan_config manual_fan;
	struct nct6687d_fan_thermal_cruise_config thermal_cruise_fan;
	struct nct6687d_fan_speed_cruise_config speed_cruise_fan;

	/* Fan ALG_FUNCTRL */
	uint8_t fan_alg_weight;

	uint8_t crit_temp;
	uint8_t crit_temp_tolerance;

	uint8_t temp_err_duty;

	bool dts1_en;
	bool dts2_en;
	bool dts_ub_en;

	/* Fan FUN_CTRL */
	bool smart_tracking_en;
	bool fast_tracking_en;
	bool markup_tracking_en;

	bool ambient_floor_en;
	struct nct6687d_ambient_floor_fan_config amb_floor_fan_cfg;

	uint8_t startup_duty;
	uint8_t manual_offset;
	uint8_t min_duty;
};

struct superio_nuvoton_nct6687d_config {
	struct nct6687d_fan_config fans[MAX_NUM_FANS];
	enum nct6687d_sensor_src_select sensors[MAX_NUM_SENSORS];

	enum nct6687d_peci_speed peci_speed;

	struct nct6687d_pch_smbus_sensor smbus_sensor;
	struct nct6687d_dts_sensor_config dts_sensor;
	struct nct6687d_dts2_sensor_config dts2_sensor;

	struct nct6687d_ambient_floor_config ambient_floor;
	struct nct6687d_smart_tracking_config smart_tracking;

	uint8_t fan_default_val;
};

#define FAN1	fans[0]
#define FAN2	fans[1]
#define FAN3	fans[2]
#define FAN4	fans[3]
#define FAN5	fans[4]
#define FAN6	fans[5]
#define FAN7	fans[6]
#define FAN8	fans[7]
#define FAN9	fans[8]
#define FAN10	fans[9]

void nct6687d_hwm_init(uint16_t hwm_base, const struct superio_nuvoton_nct6687d_config *conf);

#endif /* SUPERIO_NUVOTON_NCT6687D_CHIP_H */
