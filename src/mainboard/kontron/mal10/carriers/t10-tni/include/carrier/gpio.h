/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef T10_TNI_CFG_GPIO_H
#define T10_TNI_CFG_GPIO_H

#include <gpio.h>
#include <stddef.h>

/*
 * Bidirectional GPIO port when both RX and TX buffer is enabled
 * TODO: move this macros to src/soc/intel/common/block/include/intelblocks/gpio_defs.h
 */
#ifndef PAD_CFG_GPIO_BIDIRECT_IOS
#define PAD_CFG_GPIO_BIDIRECT_IOS(pad, val, pull, rst, trig, iosstate, iosterm, own) \
	_PAD_CFG_STRUCT(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_TRIG(trig) |	\
		PAD_BUF(NO_DISABLE) | val,				\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own) |		\
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))
#endif

#ifndef PAD_CFG_GPIO_BIDIRECT
#define PAD_CFG_GPIO_BIDIRECT(pad, val, pull, rst, trig, own)		\
	_PAD_CFG_STRUCT(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_TRIG(trig) |	\
		PAD_BUF(NO_DISABLE) | val,				\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own))
#endif

void carrier_gpio_configure(void);

#endif /* T10_TNI_CFG_GPIO_H */
