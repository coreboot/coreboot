/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_EBG_PM_H_
#define _SOC_EBG_PM_H_

#define GPE_CNTL                0x42
#define  SWGPE_CTRL             (1 << 1)

#define GPE0_STS(x)		(0x60 + ((x) * 4))
#define GPE0_EN(x)		(0x70 + ((x) * 4))

/* This is defined as ETR3 in EDS. We named it as ETR here for consistency */
#define ETR		0x1048
#define PRSTS		0x1810

#endif	/* _SOC_EBG_PM_H_ */
