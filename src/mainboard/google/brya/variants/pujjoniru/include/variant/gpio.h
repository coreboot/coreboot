/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef VARIANT_GPIO_H
#define VARIANT_GPIO_H

#undef GPIO_PCH_WP
/* WP signal to PCH */
#define GPIO_PCH_WP		GPP_E17

/* FPR feature need add SLP_S0_GATE_R to control.
Pujjoniru reference baord did not have HAVE_SLP_S0_GATE.
So we add a variant specific S0ix hook to fill the SSDT
table to control FPR feature. GPP_A7 pull down when do
suspend and pull high when resume. */
#define SLP_S0_FP_EN	GPP_A7
#endif
