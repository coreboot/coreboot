/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_SOUTHBRIDGE_H
#define AMD_CEZANNE_SOUTHBRIDGE_H

#include <soc/iomap.h>

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PWR_RESET_CFG			0x10
#define   TOGGLE_ALL_PWR_GOOD		(1 << 1)
#define PM_ACPI_SMI_CMD			0x6a

#define FCH_LEGACY_UART_DECODE		(ALINK_AHB_ADDRESS + 0x20) /* 0xfedc0020 */

/* FCH AOAC device offsets for AOAC_DEV_D3_CTL/AOAC_DEV_D3_STATE */
#define FCH_AOAC_DEV_CLK_GEN		0
#define FCH_AOAC_DEV_I2C0		5
#define FCH_AOAC_DEV_I2C1		6
#define FCH_AOAC_DEV_I2C2		7
#define FCH_AOAC_DEV_I2C3		8
#define FCH_AOAC_DEV_I2C4		9
#define FCH_AOAC_DEV_I2C5		10
#define FCH_AOAC_DEV_UART0		11
#define FCH_AOAC_DEV_UART1		12
#define FCH_AOAC_DEV_UART2		16
#define FCH_AOAC_DEV_AMBA		17
#define FCH_AOAC_DEV_UART3		26
#define FCH_AOAC_DEV_ESPI		27

/* IO 0xf0 NCP Error */
#define   NCP_WARM_BOOT			(1 << 7) /* Write-once */

void enable_aoac_devices(void);
void wait_for_aoac_enabled(unsigned int dev);
void fch_pre_init(void);
void fch_early_init(void);

#endif /* AMD_CEZANNE_SOUTHBRIDGE_H */
