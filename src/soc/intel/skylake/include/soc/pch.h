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

#ifndef _SOC_PCH_H_
#define _SOC_PCH_H_

#include <device/device.h>

#if ENV_RAMSTAGE
void pch_disable_devfn(struct device *dev);
#endif

#endif /* _SOC_PCH_H_ */
