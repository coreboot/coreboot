/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SUPERIO_NUVOTON_NCT5525D_HWM_H
#define SUPERIO_NUVOTON_NCT5525D_HWM_H

#include <stdint.h>

#include "chip.h"

void nct5525d_hwm_init(u16 base, const struct superio_nuvoton_nct5525d_config *conf);
void nct5525d_tsi_init(u16 base, const struct superio_nuvoton_nct5525d_config *conf);

#endif /* SUPERIO_NUVOTON_NCT5525D_HWM_H */
