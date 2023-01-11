/* SPDX-License-Identifier: BSD-3-Clause */

#include <gpio.h>
#include <soc/blsp.h>
#include <soc/qup.h>

#define IPQ40XX_I2C0_PINGROUP_1		1
#define IPQ40XX_I2C0_PINGROUP_2		(!IPQ40XX_I2C0_PINGROUP_1)
#define IPQ40XX_I2C1_PINGROUP_1         1

#if IPQ40XX_I2C0_PINGROUP_1

#define SCL_GPIO		20
#define SDA_GPIO		21
#define GPIO_FUNC_SCL		0x1
#define GPIO_FUNC_SDA		0x1

#elif IPQ40XX_I2C0_PINGROUP_2

#define SCL_GPIO		58
#define SDA_GPIO		59
#define GPIO_FUNC_SCL		0x3
#define GPIO_FUNC_SDA		0x2

#else

#warning "TPM: I2C pingroup not specified"

#endif

#if IPQ40XX_I2C1_PINGROUP_1

#define SCL_GPIO_I2C1                34
#define SDA_GPIO_I2C1                35
#define GPIO_I2C1_FUNC_SCL           0x1
#define GPIO_I2C1_FUNC_SDA           0x1

#endif

int blsp_i2c_init_board(blsp_qup_id_t id)
{
	switch (id) {
	case BLSP_QUP_ID_0:
	case BLSP_QUP_ID_1:
	case BLSP_QUP_ID_2:
	case BLSP_QUP_ID_3:
#if defined(IPQ40XX_I2C0_PINGROUP_1) || defined(IPQ40XX_I2C0_PINGROUP_2)
		gpio_tlmm_config_set(SDA_GPIO, GPIO_FUNC_SDA,
				     GPIO_NO_PULL, GPIO_2MA, 1);
		gpio_tlmm_config_set(SCL_GPIO, GPIO_FUNC_SCL,
				     GPIO_NO_PULL, GPIO_2MA, 1);
#endif /* Pin Group 1 or 2 */

#if defined(IPQ40XX_I2C1_PINGROUP_1)
		gpio_tlmm_config_set(SDA_GPIO_I2C1, GPIO_I2C1_FUNC_SDA,
				     GPIO_NO_PULL, GPIO_2MA, 1);
		gpio_tlmm_config_set(SCL_GPIO_I2C1, GPIO_I2C1_FUNC_SCL,
				     GPIO_NO_PULL, GPIO_2MA, 1);
#endif
		break;
	default:
		return 1;
	}

	return 0;
}
