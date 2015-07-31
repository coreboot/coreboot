/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 MediaTek Inc.
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
#ifndef __BL31_PLAT_PARAMS_H__
#define __BL31_PLAT_PARAMS_H__

#if IS_ENABLED(CONFIG_ARM64_USE_ARM_TRUSTED_FIRMWARE)
#include <arm-trusted-firmware/plat/mediatek/mt8173/include/plat_params.h>

void register_bl31_param(struct bl31_plat_param *param);
#endif

#endif /* __BL31_PLAT_PARAMS_H__ */
