/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _I2C_RX6110SA_H_
#define _I2C_RX6110SA_H_

#define RX6110SA_ACPI_NAME		"ERX6"
#define RX6110SA_HID_NAME		"RX6110SA"
#define RX6110SA_HID_DESC		"Real Time Clock"

/* Register layout */
#define SECOND_REG			0x10
#define MINUTE_REG			0x11
#define HOUR_REG			0x12
#define WEEK_REG			0x13
#define DAY_REG				0x14
#define MONTH_REG			0x15
#define YEAR_REG			0x16
#define RESERVED_BIT_REG		0x17
#define  RTC_INIT_VALUE			0xA8
#define ALARM_MINUTE_REG		0x18
#define ALARM_HOUR_REG			0x19
#define ALARM_WEEKDAY_REG		0x1A
#define TMR_COUNTER_0_REG		0x1B
#define TMR_COUNTER_1_REG		0x1C
#define EXTENSION_REG			0x1D
#define  TE_BIT				(1 << 4)
#define  FSEL_MASK			0xC0
#define  TSEL_MASK			0x07
#define FLAG_REGISTER			0x1E
#define  VLF_BIT			(1 << 1)
#define CTRL_REG			0x1F
#define  AIE_BIT			(1 << 3)
#define  TIE_BIT			(1 << 4)
#define  UIE_BIT			(1 << 5)
#define  STOP_BIT			(1 << 6)
#define  TEST_BIT			(1 << 7)
#define  TMR_MODE_MASK			0x03
#define  TMR_MODE_NORM_BACKUP		0x00
#define  TMR_MODE_NORM_ONLY		0x01
#define  TMR_MODE_BACKUP_ONLY		0x03
#define DIGITAL_REG			0x30
#define BATTERY_BACKUP_REG		0x31
#define  PMON_SAMPL_MASK		0x03
#define IRQ_CONTROL_REG			0x32
#define  FOUT_OUTPUT_PIN_MASK		0x03

/* Define CLKOUT frequency divider values valid for parameter cof_selection */
#define COF_OFF				0x00
#define COF_1_HZ			0x01
#define COF_1024_HZ			0x02
#define COF_32768_HZ			0x03

/* Define valid clock rates for the internal timer */
#define TMR_CLK_4096_HZ			0x00
#define TMR_CLK_64_HZ			0x01
#define TMR_CLK_1_HZ			0x02
#define TMR_CLK_1_60_HZ			0x03
#define TMR_CLK_1_3600_HZ		0x04

/*
 * Define possible power monitor sampling times. This value decides for how
 * long the power supply is sampled every second to detect a power down
 * condition.
 */
#define PMON_SAMPL_2_MS			0x00
#define PMON_SAMPL_16_MS		0x01
#define PMON_SAMPL_128_MS		0x02
#define PMON_SAMPL_256_MS		0x03

/* Define on which pin of the RTC the generated square wave will be driven. */
#define FOUT_IRQ2			0x00	/* IRQ2 pin used for Fout */
#define FOUT_IRQ1			0x01	/* IRQ1 pin used for Fout */
#define FOUT_DO_FOUT			0x02	/* DO/FOUT pin used for Fout */

#define AFTER_RESET_DELAY_MS		2	/* Delay after reset sequence */

#endif /* _I2C_RX6110SA_H_ */
