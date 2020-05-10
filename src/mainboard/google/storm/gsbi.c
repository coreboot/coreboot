/* SPDX-License-Identifier: BSD-3-Clause */

#include <soc/gpio.h>
#include <soc/gsbi.h>
#include <soc/qup.h>

#define GPIO_FUNC_I2C		0x1

int gsbi_init_board(gsbi_id_t gsbi_id)
{
	switch (gsbi_id) {
	case GSBI_ID_7:
			gpio_tlmm_config_set(8, GPIO_FUNC_I2C,
					     GPIO_NO_PULL, GPIO_2MA, 1);
			gpio_tlmm_config_set(9, GPIO_FUNC_I2C,
					     GPIO_NO_PULL, GPIO_2MA, 1);
		break;
	case GSBI_ID_4:
			/* Configure GPIOs 13 - SCL, 12 - SDA, 2mA gpio_en */
			gpio_tlmm_config_set(12, GPIO_FUNC_I2C,
					     GPIO_NO_PULL, GPIO_2MA, 1);
			gpio_tlmm_config_set(13, GPIO_FUNC_I2C,
					     GPIO_NO_PULL, GPIO_2MA, 1);
		break;
	case GSBI_ID_1:
			/* Configure GPIOs 54 - SCL, 53 - SDA, 2mA gpio_en */
			gpio_tlmm_config_set(54, GPIO_FUNC_I2C,
					     GPIO_NO_PULL, GPIO_2MA, 1);
			gpio_tlmm_config_set(53, GPIO_FUNC_I2C,
					     GPIO_NO_PULL, GPIO_2MA, 1);
		break;
	default:
		return 1;
	}

	return 0;
}
