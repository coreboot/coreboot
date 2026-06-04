/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_PMIC_GPIO_H_
#define _SOC_QUALCOMM_PMIC_GPIO_H_

#include <stddef.h>
#include <stdint.h>
#include <types.h>

#define PMIC_GPIO_BASE(num)		(0x8800 + ((num - 1) * 0x100))
#define PMIC_GPIO_NUMBER_MIN		1
#define PMIC_GPIO_NUMBER_MAX		14
#define PMIC_PMK_GPIO_OFFSET		0x3000

#define PMIC_GPIO_MODE_CTL		0x40
 #define PMIC_GPIO_MODE_OUTPUT		0x01
 #define PMIC_GPIO_MODE_INPUT		0x00

#define PMIC_GPIO_DIG_VIN_CTL		0x41
 #define PMIC_GPIO_DIG_VIN_VIN0		0x00

#define PMIC_GPIO_DIG_PULL_CTL		0x42

#define PMIC_GPIO_DIG_OUT_SOURCE_CTL	0x44
 #define PMIC_GPIO_DIG_OUT_SOURCE_HIGH	0x80
 #define PMIC_GPIO_DIG_OUT_SOURCE_LOW	0x00

#define PMIC_GPIO_DIG_OUT_CTL		0x45

#define PMIC_GPIO_EN_CTL		0x46
 #define PMIC_GPIO_EN_PERPH_EN		0x80

void pmic_gpio_configure(uint8_t sid, uint8_t gpio_num,
				uint8_t source, uint8_t enable,
				uint8_t mode);

void pmic_gpio_output(uint8_t sid, uint8_t gpio_num, bool high);

#endif	// _SOC_QUALCOMM_PMIC_GPIO_H_
