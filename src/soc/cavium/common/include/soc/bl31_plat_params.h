/*
 * This file is part of the coreboot project.
 *
 * Copyright 2016 Rockchip Inc.
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
 */

#ifndef __BL31_PLAT_PARAMS_H__
#define __BL31_PLAT_PARAMS_H__

// FIXME: use correct path one ATF is upstream
#include <arm-trusted-firmware/plat/rockchip/common/include/plat_params.h>

void register_bl31_param(struct bl31_plat_param *param);

#endif/* __BL31_PLAT_PARAMS_H__ */
