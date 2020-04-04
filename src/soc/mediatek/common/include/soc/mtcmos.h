/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __SOC_MEDIATEK_COMMON_MTCMOS_H__
#define __SOC_MEDIATEK_COMMON_MTCMOS_H__

void mtcmos_audio_power_on(void);
void mtcmos_display_power_on(void);

void mtcmos_protect_display_bus(void);

#endif /* __SOC_MEDIATEK_COMMON_MTCMOS_H__ */
