/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
