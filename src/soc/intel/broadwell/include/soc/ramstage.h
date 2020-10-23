/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_RAMSTAGE_H_
#define _BROADWELL_RAMSTAGE_H_

#include <device/device.h>
#include <soc/intel/broadwell/chip.h>

void broadwell_init_pre_device(void *chip_info);
void broadwell_init_cpus(struct device *dev);
void broadwell_pch_enable_dev(struct device *dev);

#if CONFIG(HAVE_REFCODE_BLOB)
void broadwell_run_reference_code(void);
#else
static inline void broadwell_run_reference_code(void) { }
#endif

#endif
