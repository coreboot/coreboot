/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __TEGRA_MISC_TYPES_H__
#define __TEGRA_MISC_TYPES_H__

#define EFAULT		1
#define EINVAL		2
#define ETIMEDOUT	3
#define ENOSPC		4
#define ENOSYS		5
#define EPTR		6

#define IS_ERR_PTR(ptr) \
		(ptr == (void *)-EPTR)

#endif /* __TEGRA_MISC_TYPES_H__ */
