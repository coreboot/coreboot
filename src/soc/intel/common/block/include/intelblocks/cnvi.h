/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_COMMON_CNVI_H_
#define _SOC_INTEL_COMMON_CNVI_H_

/* CNVi WiFi Register */
#define CNVI_DEV_CAP		0x44
#define CNVI_DEV_CONTROL	0x48
#define CNVI_POWER_STATUS	0xcc

/* CNVi PLDR Results */
#define CNVI_PLDR_COMPLETE	0x02
#define CNVI_PLDR_NOT_COMPLETE	0x03
#define CNVI_PLDR_TIMEOUT	0x04

/* CNVi PLDR Control */
#if CONFIG(SOC_INTEL_ALDERLAKE_PCH_S)
#define CNVI_ABORT_PLDR		0x44
#else
#define CNVI_ABORT_PLDR		0x80
#endif

#define CNVI_ABORT_ENABLE	BIT(0)
#define CNVI_ABORT_REQUEST	BIT(1)
#define CNVI_READY		BIT(2)

/* CNVi Sideband Port ID */
#if CONFIG(SOC_INTEL_METEORLAKE)
#define CNVI_SIDEBAND_ID	0x29
#else
#define CNVI_SIDEBAND_ID	0x73
#endif

#endif	// _SOC_INTEL_COMMON_CNVI_H_
