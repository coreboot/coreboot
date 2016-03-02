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
