/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#include <baseboard/gpio.h>
#undef GPIO_PCH_WP
/* WP signal to PCH */
#define GPIO_PCH_WP		GPP_E12

#define WWAN_FCPO	GPP_D6 /* FULL_CARD_POWER_OFF# */
#define WWAN_RST	GPP_E17
#define T2_OFF_MS	5      /*spec request: 0 < t_off < 10ms*/
#define SLP_S0_HDMI_EN	GPP_A7
#endif
