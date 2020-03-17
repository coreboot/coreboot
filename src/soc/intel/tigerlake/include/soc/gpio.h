/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_TIGERLAKE_GPIO_H_
#define _SOC_TIGERLAKE_GPIO_H_

#include <soc/gpio_defs.h>
#include <intelblocks/gpio.h>

#if CONFIG(SOC_INTEL_TIGERLAKE)

	#define CROS_GPIO_NAME		"INT34C5"
	#define CROS_GPIO_COMM0_NAME    "INT34C5:00"
	#define CROS_GPIO_COMM1_NAME    "INT34C5:01"
	#define CROS_GPIO_COMM4_NAME    "INT34C5:02"
	#define CROS_GPIO_COMM5_NAME    "INT34C5:03"

#elif CONFIG(SOC_INTEL_JASPERLAKE)

	#define CROS_GPIO_NAME		"INT34C8"
	#define CROS_GPIO_COMM0_NAME    "INT34C8:00"
	#define CROS_GPIO_COMM1_NAME    "INT34C8:01"
	#define CROS_GPIO_COMM4_NAME    "INT34C8:02"
	#define CROS_GPIO_COMM5_NAME    "INT34C8:03"
#endif

#endif
