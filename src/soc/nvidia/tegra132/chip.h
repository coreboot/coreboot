/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SOC_NVIDIA_TEGRA132_CHIP_H__
#define __SOC_NVIDIA_TEGRA132_CHIP_H__
#include <arch/cache.h>
#include <soc/addressmap.h>

#define EFAULT  1
#define EINVAL  2

struct soc_nvidia_tegra132_config {
	int bring_up_secondary_cpu;
};

#endif /* __SOC_NVIDIA_TEGRA132_CHIP_H__ */
