/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_METEORLAKE_CNVI_H_
#define _SOC_METEORLAKE_CNVI_H_

#if CONFIG(SOC_INTEL_METEORLAKE_S)
#define CNVI_ABORT_PLDR		0x44
#else
#define CNVI_ABORT_PLDR		0x80
#endif

#endif /* _SOC_METEORLAKE_CNVI_H_ */
