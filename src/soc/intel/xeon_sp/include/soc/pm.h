/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_PM_H_
#define _SOC_PM_H_

#include <soc/iomap.h>
#include <soc/pmc.h>

#define PM1_CNT                   0x04
#define PM1_STS                   0x00
#define PM1_TMR                   0x08
#define PM2_CNT                   0x50

#define GPE0_REG_MAX              4
#define GPE0_STS(x)              (0x80 + (x * 4))

#endif
