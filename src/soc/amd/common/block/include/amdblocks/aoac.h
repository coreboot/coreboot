/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_AOAC_H
#define AMD_BLOCK_AOAC_H

#include <types.h>

/* FCH AOAC Registers 0xfed81e00 */
#define AOAC_DEV_D3_CTL(device)		(0x40 + device * 2)
#define AOAC_DEV_D3_STATE(device)	(AOAC_DEV_D3_CTL(device) + 1)

/* Bit definitions for Device D3 Control AOACx0000[40...7E; even byte addresses] */
#define   FCH_AOAC_TARGET_DEVICE_STATE (BIT(0) | BIT(1))
#define     FCH_AOAC_D0_UNINITIALIZED	0
#define     FCH_AOAC_D0_INITIALIZED	1
#define     FCH_AOAC_D1_2_3_WARM	2
#define     FCH_AOAC_D3_COLD		3
#define   FCH_AOAC_DEVICE_STATE		BIT(2)
#define   FCH_AOAC_PWR_ON_DEV		BIT(3)
#define   FCH_AOAC_SW_PWR_ON_RSTB	BIT(4)
#define   FCH_AOAC_SW_REF_CLK_OK	BIT(5)
#define   FCH_AOAC_SW_RST_B		BIT(6)
#define   FCH_AOAC_IS_SW_CONTROL	BIT(7)

/* Bit definitions for Device D3 State AOACx0000[41...7f; odd byte addresses] */
#define   FCH_AOAC_PWR_RST_STATE	BIT(0)
#define   FCH_AOAC_REF_CLK_OK_STATE	BIT(1)
#define   FCH_AOAC_RST_B_STATE		BIT(2)
#define   FCH_AOAC_DEV_OFF_GATING_STATE	BIT(3)
#define   FCH_AOAC_D3COLD		BIT(4)
#define   FCH_AOAC_CLK_OK_STATE		BIT(5)
#define   FCH_AOAC_STAT0		BIT(6)
#define   FCH_AOAC_STAT1		BIT(7)

bool is_aoac_device_enabled(unsigned int dev);
void power_on_aoac_device(unsigned int dev);
void power_off_aoac_device(unsigned int dev);
void enable_aoac_devices(void);
void wait_for_aoac_enabled(unsigned int dev);

/*
 * SoC provides the list of AOAC devices to power on during early FCH init.
 * Include the console UART device in this list when AMD_SOC_CONSOLE_UART is
 * enabled (see FCH_AOAC_UART_FOR_CONSOLE in soc/aoac_defs.h).
 */
const unsigned int *soc_get_aoac_devices(size_t *num);

#endif /* AMD_BLOCK_AOAC_H */
