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
#ifndef _SOC_TIGERLAKE_GPIO_SOC_DEFS_H_
#define _SOC_TIGERLAKE_GPIO_SOC_DEFS_H_

#if CONFIG(SOC_INTEL_TIGERLAKE)
	#include "gpio_soc_defs_tgl.h"
#elif CONFIG(SOC_INTEL_JASPERLAKE)
	#include "gpio_soc_defs_jsl.h"
#endif

#endif
