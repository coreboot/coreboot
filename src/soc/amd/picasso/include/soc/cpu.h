/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Advanced Micro Devices, Inc.
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

#ifndef __STONEYRIDGE_CPU_H__
#define __STONEYRIDGE_CPU_H__

#include <device/device.h>

/*
 *  Set a variable MTRR in bootblock and/or romstage.  AGESA will use the lowest
 *  numbered registers.  Any values defined below are subtracted from the
 *  highest numbered registers.
 *
 *  todo: Revisit this once AGESA no longer programs MTRRs.
 */
#define SOC_EARLY_VMTRR_FLASH 1
#define SOC_EARLY_VMTRR_CAR_HEAP 2
#define SOC_EARLY_VMTRR_TEMPRAM 3

void stoney_init_cpus(struct device *dev);
void check_mca(void);

#endif /* __STONEYRIDGE_CPU_H__ */
