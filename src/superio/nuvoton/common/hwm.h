/* SPDX-License-Identifier: GPL-2.0-or-later */
/* This file is part of the coreboot project. */

#ifndef SUPERIO_NUVOTON_COMMON_HWM_H
#define SUPERIO_NUVOTON_COMMON_HWM_H

/* Nuvoton is a Winbond spin-off, so this code is for both */

#include <stdint.h>

void nuvoton_hwm_select_bank(const u16 base, const u8 bank);

#endif /* SUPERIO_NUVOTON_COMMON_HWM_H */
