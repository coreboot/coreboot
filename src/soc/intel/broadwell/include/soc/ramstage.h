/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_RAMSTAGE_H_
#define _BROADWELL_RAMSTAGE_H_

#include <device/device.h>
#include <soc/intel/broadwell/chip.h>

#if CONFIG(HAVE_REFCODE_BLOB)
void broadwell_run_reference_code(void);
#else
static inline void broadwell_run_reference_code(void) { }
#endif

#endif
