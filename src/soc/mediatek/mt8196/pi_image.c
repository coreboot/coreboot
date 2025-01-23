/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <arch/cache.h>
#include <assert.h>
#include <soc/mcu_common.h>
#include <soc/pi_image.h>
#include <soc/symbols.h>
#include <string.h>
#include <sys/types.h>

static struct mtk_mcu pi_image = {
	.firmware_name = CONFIG_PI_IMG_FIRMWARE
};

size_t pi_image_load(void **buffer)
{
	*buffer = NULL;
	pi_image.load_buffer = _dram_dma;
	pi_image.buffer_size = REGION_SIZE(dram_dma);

	if (mtk_init_mcu(&pi_image))
		die("%s() failed\n", __func__);

	*buffer = pi_image.load_buffer;
	return pi_image.run_size;
}
