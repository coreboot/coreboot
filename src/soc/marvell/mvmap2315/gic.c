/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Marvell, Inc.
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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include <arch/io.h>
#include <soc/gic.h>

void enable_bcm_gic(void)
{
	setbits_le32(&mvmap2315_bcm_gicc->ctrl, MVMAP2315_BCM_GICC_EN0);
	setbits_le32(&mvmap2315_bcm_gicc->ctrl, MVMAP2315_BCM_GICD_FIQ_EN);
	setbits_le32(&mvmap2315_bcm_gicd->ctrl, MVMAP2315_BCM_GICD_EN0);
}
