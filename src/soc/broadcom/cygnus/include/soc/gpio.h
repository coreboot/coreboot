/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef __SOC_BROADCOM_CYGNUS_GPIO_H__
#define __SOC_BROADCOM_CYGNUS_GPIO_H__

#include <types.h>

#define ENOTSUPP	524	/* Operation is not supported */

/* Supported GPIO types. Not all of these types are supported on all boards. */
enum iproc_gpio_types {
	IPROC_GPIO_CCA_ID,
	IPROC_GPIO_CMICM_ID,
	IPROC_GPIO_ASIU_ID
};

typedef u32 gpio_t;


void *cygnus_pinmux_init(void);
int cygnus_gpio_request_enable(void *priv, unsigned pin);
void cygnus_gpio_disable_free(void *priv, unsigned pin);
void gpio_init(void);
void gpio_free(gpio_t gpio);

#endif	/* __SOC_BROADCOM_CYGNUS_GPIO_H__ */
