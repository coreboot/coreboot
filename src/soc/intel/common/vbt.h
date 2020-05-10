/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _INTEL_COMMON_VBT_H_
#define _INTEL_COMMON_VBT_H_

#include <commonlib/region.h>
#include <types.h>

/*
 * Returns VBT pointer and mapping after checking prerequisites for Pre OS
 * Graphics initialization
 */
void *vbt_get(void);
#endif
