/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_ALDERLAKE_CNVI_H_
#define _SOC_ALDERLAKE_CNVI_H_

#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
#define CNVI_ABORT_PLDR		0x44
#else
#define CNVI_ABORT_PLDR		0x80
#endif

#endif /* _SOC_ALDERLAKE_CNVI_H_ */
