/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_MSR_H_
#define _SOC_SNOWRIDGE_MSR_H_

#include <intelblocks/msr.h>

/**
 * @brief Force serialized SMM relocation by hardcoding `SMM_CPU_SVRSTR` feature as not supported.
 */
#ifdef SMM_CPU_SVRSTR_MASK
#undef SMM_CPU_SVRSTR_MASK
#endif
#define SMM_CPU_SVRSTR_MASK 0

#define MSR_BIOS_DONE       0x151
#define ENABLE_IA_UNTRUSTED BIT(0)

#endif // _SOC_SNOWRIDGE_MSR_H_
