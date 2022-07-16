/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SB800_FAN_H_
#define _SB800_FAN_H_

#include <device/device.h>

void init_sb800_IMC_fans(struct device *dev);
void init_sb800_MANUAL_fans(struct device *dev);

/* Fan Register Definitions */
#define FAN_0_OFFSET				0x00
#define FAN_1_OFFSET				0x10
#define FAN_2_OFFSET				0x20
#define FAN_3_OFFSET				0x30
#define FAN_4_OFFSET				0x40

#define FAN_INPUT_CONTROL_REG		0x00
#define FAN_CONTROL_REG				0x01
#define FAN_FREQUENCY_REG			0x02
#define FAN_LOW_DUTY_REG			0x03
#define FAN_MED_DUTY_REG			0x04
#define FAN_MULTIPLIER_REG			0x05
#define FAN_LOW_TEMP_LO_REG			0x06
#define FAN_LOW_TEMP_HI_REG			0x07
#define FAN_MED_TEMP_LO_REG			0x08
#define FAN_MED_TEMP_HI_REG			0x09
#define FAN_HIGH_TEMP_LO_REG		0x0A
#define FAN_HIGH_TEMP_HI_REG		0x0B
#define FAN_LINEAR_RANGE_REG		0x0C
#define FAN_LINEAR_HOLD_REG			0x0D

/* FanXInputControl Definitions */
#define FAN_INPUT_INTERNAL_DIODE	0
#define FAN_INPUT_TEMP0				1
#define FAN_INPUT_TEMP1				2
#define FAN_INPUT_TEMP2				3
#define FAN_INPUT_TEMP3				4
#define FAN_INPUT_TEMP0_FILTER		5
#define FAN_INPUT_ZERO				6
#define FAN_INPUT_DISABLED			7

/* FanXControl Definitions */
#define FAN_AUTOMODE				(1 << 0)
#define FAN_LINEARMODE				(1 << 1)
#define FAN_STEPMODE				0 /* ~(1 << 1) */
#define FAN_POLARITY_HIGH			(1 << 2)
#define FAN_POLARITY_LOW			0 /*~(1 << 2) */

/* FanXLowDuty Definitions */
#define FAN_POLARITY_HIGH_MAX_SPEED	0xff
#define FAN_POLARITY_LOW_MAX_SPEED	0x00

/* FanXFreq Definitions */
/* Typically, fans run at 25KHz */
#define FREQ_28KHZ			0x0
#define FREQ_25KHZ			0x1
#define FREQ_23KHZ			0x2
#define FREQ_21KHZ			0x3
#define FREQ_29KHZ			0x4
#define FREQ_18KHZ			0x5
/* Any value > 05h and < F7: Freq = 1/(FreqDiv * 2048 * 15ns) */
#define FREQ_100HZ			0xF7
#define FREQ_87HZ			0xF8
#define FREQ_58HZ			0xF9
#define FREQ_44HZ			0xFA
#define FREQ_35HZ			0xFB
#define FREQ_29HZ			0xFC
#define FREQ_22HZ			0xFD
#define FREQ_14HZ			0xFE
#define FREQ_11HZ			0xFF

/* IMC Fan Control Definitions */
#define IMC_MODE1_FAN_ENABLED					(1 << 0)
#define IMC_MODE1_FAN_IMC_CONTROLLED			(1 << 2)
#define IMC_MODE1_FAN_LINEAR_MODE				(1 << 4)
#define IMC_MODE1_FAN_STEP_MODE					0 /* ~( 1 << 4 ) */
#define IMC_MODE1_NO_FANOUT						0 /* ~( 7 << 5 ) */
#define IMC_MODE1_FANOUT0						(1 << 5)
#define IMC_MODE1_FANOUT1						(2 << 5)
#define IMC_MODE1_FANOUT2						(3 << 5)
#define IMC_MODE1_FANOUT3						(4 << 5)
#define IMC_MODE1_FANOUT4						(5 << 5)

#define IMC_MODE2_TEMPIN_NONE					0 /* ~( 7 << 0) */
#define IMC_MODE2_TEMPIN_0						1
#define IMC_MODE2_TEMPIN_1						2
#define IMC_MODE2_TEMPIN_2						3
#define IMC_MODE2_TEMPIN_3						4
#define IMC_MODE2_INT_TEMPIN					5
#define IMC_MODE2_TEMPIN_SB_TSI					6
#define IMC_MODE2_TEMPIN_OTHER					7
#define IMC_MODE2_FANIN_NONE					0 /* ~ (7 << 3) */
#define IMC_MODE2_FANIN0						(1 << 3)
#define IMC_MODE2_FANIN1						(2 << 3)
#define IMC_MODE2_FANIN2						(3 << 3)
#define IMC_MODE2_FANIN3						(4 << 3)
#define IMC_MODE2_FANIN4						(5 << 3)
#define IMC_MODE2_TEMP_AVERAGING_ENABLED		(1 << 6)
#define IMC_MODE2_TEMP_AVERAGING_DISABLED		0 /* ~( 1 << 6 ) */

#define IMC_TEMP_SENSOR_ON_SMBUS_0				0
#define IMC_TEMP_SENSOR_ON_SMBUS_2				1
#define IMC_TEMP_SENSOR_ON_SMBUS_3				2
#define IMC_TEMP_SENSOR_ON_SMBUS_4				3

#define IMC_ZONE0								0
#define IMC_ZONE1								1
#define IMC_ZONE2								2
#define IMC_ZONE3								3
#define IMC_ZONE4								4

#define IMC_TEMPIN_TUNING_DEFAULT_MODE			0
#define IMC_TEMPIN_TUNING_HIGH_CURRENT_RATIO	1
#define IMC_TEMPIN_TUNING_HIGH_CURRENT			2
#define IMC_TEMPIN_TUNING_DISABLE_FILTERING		(1 << 2)

/* IMCFUNSupportBitMap - Zone enable values */
#define IMC_ENABLE_ZONE0						0x111
#define IMC_ENABLE_ZONE1						0x222
#define IMC_ENABLE_ZONE2						0x333
#define IMC_ENABLE_ZONE3						0x444
#define IMC_ENABLE_TEMPIN0						(1 << 12)
#define IMC_ENABLE_TEMPIN1						(1 << 13)
#define IMC_ENABLE_TEMPIN2						(1 << 14)
#define IMC_ENABLE_TEMPIN3						(1 << 15)

/* Array size settings */
#define IMC_FAN_THRESHOLD_COUNT					9
#define IMC_FAN_SPEED_COUNT						8
#define IMC_FAN_CONFIG_COUNT					8
#define FAN_REGISTER_COUNT						15

#endif
