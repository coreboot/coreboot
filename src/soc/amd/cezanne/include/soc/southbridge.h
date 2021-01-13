/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_CEZANNE_SOUTHBRIDGE_H
#define AMD_CEZANNE_SOUTHBRIDGE_H

#include <soc/iomap.h>

/* Power management registers:  0xfed80300 or index/data at IO 0xcd6/cd7 */
#define PWR_RESET_CFG			0x10
#define   TOGGLE_ALL_PWR_GOOD		(1 << 1)
#define PM_ACPI_SMI_CMD			0x6a

#define FCH_LEGACY_UART_DECODE		(ALINK_AHB_ADDRESS + 0x20) /* 0xfedc0020 */

/* IO 0xf0 NCP Error */
#define   NCP_WARM_BOOT			(1 << 7) /* Write-once */

void fch_pre_init(void);
void fch_early_init(void);

#endif /* AMD_CEZANNE_SOUTHBRIDGE_H */
