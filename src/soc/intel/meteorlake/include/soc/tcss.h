/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_TCSS_H_
#define _SOC_TCSS_H_

/* Thunderbolt firmware IMR status */
#define IOM_CSME_IMR_TBT_STATUS			0x14
#define TBT_VALID_AUTHENTICATION		(1 << 30)

/* IOM aux bias control registers in REGBAR MMIO space */
#define IOM_AUX_BIAS_CTRL_PULLUP_OFFSET_0	0x1070
#define IOM_AUX_BIAS_CTRL_PULLUP_OFFSET(x)	(IOM_AUX_BIAS_CTRL_PULLUP_OFFSET_0 + (x) * 4)
#define IOM_AUX_BIAS_CTRL_PULLDOWN_OFFSET_0	0x1088
#define IOM_AUX_BIAS_CTRL_PULLDOWN_OFFSET(x)	(IOM_AUX_BIAS_CTRL_PULLDOWN_OFFSET_0 + (x) * 4)

#endif /* _SOC_TCSS_H_ */
