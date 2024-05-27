/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef MAINBOARD_GPIO_H
#define MAINBOARD_GPIO_H

#ifndef PAD_CFG_GPIO_BIDIRECT
#define PAD_CFG_GPIO_BIDIRECT(pad, val, pull, rst, trig, own)		\
	_PAD_CFG_STRUCT(pad,						\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_TRIG(trig) |	\
		PAD_BUF(NO_DISABLE) | val,				\
		PAD_PULL(pull) | PAD_CFG_OWN_GPIO(own))
#endif

void mainboard_configure_early_gpios(void);
void mainboard_configure_gpios(void);

#endif
