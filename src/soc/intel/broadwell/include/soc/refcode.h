/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef BROADWELL_REFCODE_H
#define BROADWELL_REFCODE_H

#if CONFIG(HAVE_REFCODE_BLOB)
void broadwell_run_reference_code(void);
#else
static inline void broadwell_run_reference_code(void) { }
#endif

#endif
