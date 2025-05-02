/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#include <assert.h>
#include <soc/mcu_common.h>
#include <soc/mtk_fsp.h>
#include <soc/pi_image.h>
#include <soc/symbols.h>

static struct mtk_mcu pi_image = {
	.firmware_name = CONFIG_PI_IMG_FIRMWARE
};

static size_t pi_image_load(void **buffer)
{
	*buffer = NULL;
	pi_image.load_buffer = _dram_dma;
	pi_image.buffer_size = REGION_SIZE(dram_dma);

	if (mtk_init_mcu(&pi_image))
		die("%s() failed\n", __func__);

	*buffer = pi_image.load_buffer;
	return pi_image.run_size;
}

void pi_image_add_mtk_fsp_params(void)
{
	void *pi_img;
	size_t pi_image_size;
	pi_image_size = pi_image_load(&pi_img);

	void *csram = (void *)PI_IMAGE_CSRAM;
	size_t csram_size = PI_IMAGE_CSRAM_SIZE;

	mtk_fsp_add_param(FSP_PARAM_TYPE_PI_IMG, pi_image_size, pi_img);
	mtk_fsp_add_param(FSP_PARAM_TYPE_PI_IMG_CSRAM, csram_size, csram);
}
