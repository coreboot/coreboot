/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_SNOWRIDGE_GPMR_H_
#define _SOC_SNOWRIDGE_GPMR_H_

#define GPMR_LPCLGIR1      0xe30
#define GPMR_DMICTL        0xefc
#define GPMR_DMICTL_SRLOCK BIT(31)

#define GPMR_LPCGMR   0xe40
#define GPMR_GCS      0xe4c
#define GPMR_GCS_BILD BIT(0)

#define GPMR_LPCIOD  0xe70
#define GPMR_LPCIOE  0xe74
#define GPMR_TCOBASE 0xe78
#define GPMR_TCOEN   BIT(1)

#endif // _SOC_SNOWRIDGE_GPMR_H_
