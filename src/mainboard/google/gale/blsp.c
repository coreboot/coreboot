/*
 * This file is part of the depthcharge project.
 *
 * Copyright (C) 2014 - 2015 The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <gpio.h>
#include <soc/gpio.h>
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
