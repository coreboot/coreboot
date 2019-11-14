/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __STDBOOL_H__
#define __STDBOOL_H__

#include <stdint.h>

#ifdef __ROMCC__
typedef uint8_t bool;
#else
typedef _Bool   bool;
#endif
#define true    1
#define false   0

#endif /* __STDBOOL_H__ */
