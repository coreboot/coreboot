/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __STDBOOL_H__
#define __STDBOOL_H__

#if __STDC_VERSION__ <= 201710L
typedef _Bool   bool;
#define true	1
#define false	0
#endif

#endif /* __STDBOOL_H__ */
