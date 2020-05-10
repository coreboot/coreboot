/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef _I2C_PCF8523_H_
#define _I2C_PCF8523_H_

/* The address of this RTC is fixed. */
#define PCF8523_SLAVE_ADR		0x68

/* Register layout */
#define CTRL_REG_1			0x00
#define  STOP_BIT			(1 << 5)
#define  CAP_SEL			(1 << 7)
#define CTRL_REG_2			0x01
#define CTRL_REG_3			0x02
#define  PM_MASK			(7 << 5)
#define SECOND_REG			0x03
#define  OS_BIT				(1 << 7)
#define MINUTE_REG			0x04
#define HOUR_REG			0x05
#define DAY_REG				0x06
#define WEEKDAY_REG			0x07
#define MONTH_REG			0x08
#define YEAR_REG			0x09
#define ALARM_MINUTE_REG		0x0A
#define ALARM_HOUR_REG			0x0B
#define ALARM_DAY_REG			0x0C
#define ALARM_WEEKDAY_REG		0x0D
#define OFFSET_REG			0x0E
#define TMR_CLKOUT_REG			0x0F
#define  COF_MASK			0x38
#define TMR_A_FREQ_REG			0x10
#define TMR_A_REG			0x11
#define TMR_B_FREQ_REG			0x12
#define TMR_B_REG			0x13

/* Define used capacitor modes */
/* Valid for parameter cap_sel */
#define CAP_SEL_7_PF			0x00
#define CAP_SEL_12_PF			0x01

/* Define supported power modes */
/* Valid for parameter power_mode */
#define PM_BAT_SW_STD_LOW_DETECT	0x00
#define PM_BAT_SW_DIRECT_LOW_DETECT	0x01
#define PM_BAT_SW_OFF_LOW_DETECT	0x02
#define PM_BAT_SW_STD_LOW_DETECT_OFF	0x04
#define PM_BAT_SW_DIRECT_LOW_DETECT_OFF	0x05
#define PM_BAT_SW_OFF_LOW_DETECT_OFF	0x07

/* Define CLKOUT frequency divider values */
/* Valid for parameter cof_selection */
#define COF_32768_HZ			0x00
#define COF_16384_HZ			0x01
#define COF_8192_HZ			0x02
#define COF_4096_HZ			0x03
#define COF_1024_HZ			0x04
#define COF_32_HZ			0x05
#define COF_1_HZ			0x06
#define COF_OFF				0x07

/* Define timer A & B set up values */
/* Valid for parameter tmrA_prescaler and tmrB_prescaler */
#define TMR_CLK_4096_HZ			0x00
#define TMR_CLK_64_HZ			0x01
#define TMR_CLK_1_HZ			0x02
#define TMR_CLK_1_60_HZ			0x03
#define TMR_CLK_1_3600_HZ		0x07

/* Valid for parameter tmrA_mode and tmrB_mode */
#define TMR_DISABLED			0x00
#define TMR_A_MODE_COUNTDOWN		0x01
#define TMR_A_MODE_WATCHDOG		0x02
#define TMR_B_MODE_ENABLED		0x01

/* Valid for parameter tmrB_pulse_cfg */
#define TMR_B_PULSE_WIDTH_46_MS		0x00
#define TMR_B_PULSE_WIDTH_62_MS		0x01
#define TMR_B_PULSE_WIDTH_78_MS		0x02
#define TMR_B_PULSE_WIDTH_93_MS		0x03
#define TMR_B_PULSE_WIDTH_125_MS	0x04
#define TMR_B_PULSE_WIDTH_156_MS	0x05
#define TMR_B_PULSE_WIDTH_187_MS	0x06
#define TMR_B_PULSE_WIDTH_218_MS	0x07

#endif /* _I2C_PCF8523_H_ */
