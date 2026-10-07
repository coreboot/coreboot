/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5525D_CHIP_H
#define SUPERIO_NUVOTON_NCT5525D_CHIP_H

#include <stdbool.h>
#include <stdint.h>

#define NCT5525D_FAN_CURVE_POINTS	4

/* Function of pins 46/48 (CR1B[1:0], CR2A[1]) */
enum nct5525d_pins_46_48 {
	NCT5525D_PINS_46_48_KEEP = 0,
	NCT5525D_PINS_46_48_GPIO,		/* GP63/GP64 */
};

/* For every enum below, 0 keeps the hardware default. */

enum nct5525d_fan_mode {
	NCT5525D_FAN_KEEP = 0,
	NCT5525D_FAN_SMART_FAN_IV,
};

enum nct5525d_fan_output {
	NCT5525D_FAN_OUT_KEEP = 0,
	NCT5525D_FAN_OUT_PWM_OPEN_DRAIN,
};

/* Fan control temperature source, HWM bank 1 index 10h/20h [4:0] */
enum nct5525d_temp_source {
	NCT5525D_TEMP_KEEP		= 0x00,
	NCT5525D_TEMP_SYSTIN		= 0x01,
	NCT5525D_TEMP_CPUTIN		= 0x02,
	NCT5525D_TEMP_AUXTIN		= 0x03,
	NCT5525D_TEMP_SMBUSMASTER0	= 0x04,	/* SB-TSI agent 0 */
	NCT5525D_TEMP_SMBUSMASTER1	= 0x05,	/* SB-TSI agent 1 */
	NCT5525D_TEMP_SMBUSMASTER2	= 0x06,
	NCT5525D_TEMP_SMBUSMASTER3	= 0x07,
	NCT5525D_TEMP_SMBUSMASTER4	= 0x08,
	NCT5525D_TEMP_SMBUSMASTER5	= 0x09,
	NCT5525D_TEMP_SMBUSMASTER6	= 0x0a,
	NCT5525D_TEMP_SMBUSMASTER7	= 0x0b,
	NCT5525D_TEMP_PECI_AGENT0	= 0x0c,
	NCT5525D_TEMP_PECI_AGENT1	= 0x0d,
	NCT5525D_TEMP_PCH_CHIP_CPU_MAX	= 0x0e,
	NCT5525D_TEMP_PCH_CHIP		= 0x0f,
	NCT5525D_TEMP_PCH_CPU		= 0x10,
	NCT5525D_TEMP_PCH_MCH		= 0x11,
	NCT5525D_TEMP_PCH_DIM0		= 0x12,
	NCT5525D_TEMP_PCH_DIM1		= 0x13,
	NCT5525D_TEMP_PCH_DIM2		= 0x14,
	NCT5525D_TEMP_PCH_DIM3		= 0x15,
	NCT5525D_TEMP_BYTE_TEMP		= 0x16,
};

enum nct5525d_sensor_type {
	NCT5525D_SENSOR_KEEP = 0,
	NCT5525D_SENSOR_DIODE,		/* Thermal diode, voltage mode */
	NCT5525D_SENSOR_DIODE_CURRENT,	/* Thermal diode, current mode (e.g. CPU diode) */
};

struct nct5525d_fan_config {
	enum nct5525d_fan_mode mode;
	enum nct5525d_fan_output output;
	enum nct5525d_temp_source source;

	/*
	 * SMART FAN IV: temperatures in degrees C, duty cycles as 0-255.
	 * All temperatures are real values; the offset of AMD's extended
	 * temperature range is added for SB-TSI sources. The registers are
	 * signed, so the maximum is 127, or 78 with that offset.
	 */
	uint8_t temp[NCT5525D_FAN_CURVE_POINTS];
	uint8_t curve_duty[NCT5525D_FAN_CURVE_POINTS];

	bool stop_duty_en;		/* Never go below stop_duty */
	uint8_t stop_duty;
	uint8_t step_up_time;		/* 0.1 s units, 0 keeps the default */
	uint8_t step_down_time;		/* 0.1 s units, 0 keeps the default */
	uint8_t crit_temp;		/* 0 keeps the default */
};

struct superio_nuvoton_nct5525d_config {
	enum nct5525d_pins_46_48 pins_46_48;

	/* Route pins 39/40 to TSIC/TSID for AMD SB-TSI */
	bool tsi_pins;

	/* Hardware monitor, programmed through LDN B */
	enum nct5525d_sensor_type systin_type;
	enum nct5525d_sensor_type cputin_type;
	enum nct5525d_sensor_type auxtin_type;

	/* AMD SB-TSI agent 0, needs the LDN B SMBus master base (io 0x62) */
	bool tsi_agent0_enable;

	struct nct5525d_fan_config sysfan;
	struct nct5525d_fan_config cpufan;
};

#endif /* SUPERIO_NUVOTON_NCT5525D_CHIP_H */
