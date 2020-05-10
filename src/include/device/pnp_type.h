/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __DEVICE_PNP_TYPE_H__
#define __DEVICE_PNP_TYPE_H__

#include <stdint.h>

typedef u32 pnp_devfn_t;

#define PNP_DEV(PORT, FUNC) (((PORT) << 8) | (FUNC))

#if defined(__SIMPLE_DEVICE__)
#define ENV_PNP_SIMPLE_DEVICE 1
#else
#define ENV_PNP_SIMPLE_DEVICE 0
#endif

#endif /* __DEVICE_PNP_TYPE_H__ */
