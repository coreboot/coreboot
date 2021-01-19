/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _COREBOOT_SRC_MAINBOARD_GOOGLE_HEROBRINE_BOARD_H_
#define _COREBOOT_SRC_MAINBOARD_GOOGLE_HEROBRINE_BOARD_H_

#include <boardid.h>
#include <gpio.h>

#define GPIO_SD_CD_L	GPIO(91)

void setup_chromeos_gpios(void);

#endif /* _COREBOOT_SRC_MAINBOARD_GOOGLE_HEROBRINE_BOARD_H_ */
