/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_TOUCH_H_
#define _SOC_PANTHERLAKE_TOUCH_H_

/* For THC-I2C: */

/*
 * I2C Standard Mode (100Kbps):
 * Requested Connection Speed: 186A0
 * I2C Speed Mode: Standard (100Kbps) = 0x186A0
 *
 * I2C Fast Mode (400Kbps):
 * Requested Connection Speed: 61A80
 * I2C Speed Mode: Fast (400Kbps) = 0x612B0
 *
 * I2C Fast-Plus Mode (1Mbps):
 * Requested Connection Speed: F4240
 * I2C Speed Mode: Fast-Plus (1Mbps) = 0xF4240
 * FM SCL HIGH Period: 34 Fast Mode (FM) CLK Signal HIGH Period
 * FM SCL LOW Period: 3E Fast Mode (FM) CLK Signal LOW Period
 */
#define SOC_PTL_THC_I2C_CONNECTION_SPEED_SM   0x186a0
#define SOC_PTL_THC_I2C_CONNECTION_SPEED_FM   0x61a80
#define SOC_PTL_THC_I2C_CONNECTION_SPEED_FMP  0xf4240

/*
 * 0 = 7-bit Addressing Mode for HID-I2C (Default)
 * 1 = 10-bit Addressing Mode for HID-I2C
 */
#define SOC_PTL_THC_I2C_ADDR_MODE                   0
#define SOC_PTL_THC_I2C_SM_SCL_HIGH_PERIOD      0x267
#define SOC_PTL_THC_I2C_SM_SCL_LOW_PERIOD       0x271
#define SOC_PTL_THC_I2C_SM_SDA_HOLD_TX_PERIOD       0
#define SOC_PTL_THC_I2C_SM_SDA_HOLD_RX_PERIOD       0
#define SOC_PTL_THC_I2C_FM_SCL_HIGH_PERIOD       0x92
#define SOC_PTL_THC_I2C_FM_SCL_LOW_PERIOD        0x9c
#define SOC_PTL_THC_I2C_FM_SDA_HOLD_TX_PERIOD       0
#define SOC_PTL_THC_I2C_FM_SDA_HOLD_RX_PERIOD       0
#define SOC_PTL_THC_I2C_SUPPRESSED_SPIKES_S_F_FP    0
#define SOC_PTL_THC_I2C_FMP_SCL_HIGH_PERIOD      0x34
#define SOC_PTL_THC_I2C_FMP_SCL_LOW_PERIOD       0x3e
#define SOC_PTL_THC_I2C_FMP_SDA_HOLD_TX_PERIOD      0
#define SOC_PTL_THC_I2C_FMP_SDA_HOLD_RX_PERIOD      0
#define SOC_PTL_THC_I2C_HM_SCL_HIGH_PERIOD          0
#define SOC_PTL_THC_I2C_HM_SCL_LOW_PERIOD           0
#define SOC_PTL_THC_I2C_HM_SDA_HOLD_TX_PERIOD       0
#define SOC_PTL_THC_I2C_HM_SDA_HOLD_RX_PERIOD       0
#define SOC_PTL_THC_I2C_SUPPRESSED_SPIKES_H_FP      0


/* For THC-SPI: */

/* unit: ms */
#define SOC_PTL_THC_RST_SEQ_DLY                   300

/* The initial default speed is 17000000 Hz. */
#define SOC_PTL_THC_CONNECTION_SPEED         17000000

/* 0 = no limit */
#define SOC_PTL_THC_HIDSPI_LIMIT_PKT_SZ             0
#define SOC_PTL_THC_PERFORMANCE_LIMIT               0

#endif /* _SOC_PANTHERLAKE_TOUCH_H_ */
