/*
 * This file is part of the coreboot project.
 *
 * Copyright 2014 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

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
