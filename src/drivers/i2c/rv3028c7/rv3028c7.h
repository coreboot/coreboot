/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _I2C_RV3028C7_H_
#define _I2C_RV3028C7_H_

#define RV3028C7_ACPI_NAME		"MC28"
#define RV3028C7_HID_NAME		"MCRY3028"
#define RV3028C7_HID_DESC		"Real Time Clock"

/* Register layout */
#define SECOND_REG			0x00
#define MINUTE_REG			0x01
#define HOUR_REG			0x02
#define WEEK_REG			0x03
#define DAY_REG				0x04
#define MONTH_REG			0x05
#define YEAR_REG			0x06
#define STATUS_REG			0x0e
#define  PORF_BIT			(1 << 0)
#define  EE_BUSY_BIT			(1 << 7)
#define CTRL1_REG			0x0f
#define  EERD_BIT			(1 << 3)
#define CTRL2_REG			0x10
#define  HOUR_12_24_BIT			(1 << 1)
/* Registers for the internal EEPROM */
#define EEP_ADR_REG			0x25
#define EEP_DATA_REG			0x26
#define EEP_CMD_REG			0x27
#define  EEP_CMD_PREFIX			0x00
#define  EEP_CMD_UPDATE			0x11
#define EEP_BACKUP_REG			0x37
#define  FEDE_BIT			(1 << 4)
#define  BSM_BIT			2
#define  BSM_MASK			(3 << BSM_BIT)
#define  TCR_BIT			0
#define  TCR_MASK			(3 << TCR_BIT)
#define  TCE_BIT			(1 << 5)

#define EEP_REFRESH_EN			1
#define EEP_REFRESH_DIS			0

/* The longest mentioned timeout in the datasheet is 63 ms,
   round up to 70 ms to be on the safe side. */
#define EEP_SYNC_TIMEOUT_MS		70

#endif /* _I2C_RV3028C7_H_ */
