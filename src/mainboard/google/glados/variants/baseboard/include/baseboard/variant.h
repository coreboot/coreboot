/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef GLADOS_VARIANT_H
#define GLADOS_VARIANT_H

#include <fsp/soc_binding.h>

int is_dual_channel(const int spd_index);
void mainboard_gpio_smi_sleep(void);
void variant_memory_init_params(FSPM_UPD *mupd, const int spd_index);

#endif /* GLADOS_VARIANT_H */
