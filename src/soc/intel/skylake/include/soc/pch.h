/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef _SOC_PCH_H_
#define _SOC_PCH_H_

#include <device/device.h>

#if ENV_RAMSTAGE
void pch_disable_devfn(struct device *dev);
#endif

#endif /* _SOC_PCH_H_ */
