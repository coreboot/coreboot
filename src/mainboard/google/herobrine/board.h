/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COREBOOT_SRC_MAINBOARD_GOOGLE_HEROBRINE_BOARD_H_
#define _COREBOOT_SRC_MAINBOARD_GOOGLE_HEROBRINE_BOARD_H_

#include <boardid.h>
#include <gpio.h>

#define GPIO_SD_CD_L	GPIO(91)

#define QCOM_SC7280_SKU1 0x0
#define QCOM_SC7280_SKU2 0x1
#define QCOM_SC7280_SKU3 0x2

void setup_chromeos_gpios(void);

#endif /* _COREBOOT_SRC_MAINBOARD_GOOGLE_HEROBRINE_BOARD_H_ */
