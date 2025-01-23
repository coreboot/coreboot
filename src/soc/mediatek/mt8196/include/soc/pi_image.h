/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8196_PI_IMAGE_H__
#define __SOC_MEDIATEK_MT8196_PI_IMAGE_H__

#include <commonlib/bsd/helpers.h>
#include <soc/mcu_common.h>

#define PI_IMAGE_CSRAM		0x00120000
#define PI_IMAGE_CSRAM_SIZE	(16 * KiB)

size_t pi_image_load(void **buffer);

#endif  /* __SOC_MEDIATEK_MT8196_PI_IMAGE_H__ */
