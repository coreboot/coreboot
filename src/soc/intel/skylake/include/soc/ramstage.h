/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_RAMSTAGE_H_
#define _SOC_RAMSTAGE_H_

#include <device/device.h>
#include <fsp/api.h>
#include <fsp/util.h>

#include "../../chip.h"

#define FSP_SIL_UPD FSP_S_CONFIG
#define FSP_MEM_UPD FSP_M_CONFIG

void mainboard_silicon_init_params(FSP_S_CONFIG *params);
void soc_init_pre_device(void *chip_info);
void soc_irq_settings(FSP_SIL_UPD *params);
const char *soc_acpi_name(const struct device *dev);

#endif
