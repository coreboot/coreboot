/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef RAMINIT_TABLES_H
#define RAMINIT_TABLES_H

#include <stdint.h>

extern const u32 frq_refi_map[2][8];

extern const u8 frq_xs_map[2][8];

extern const u8 frq_mod_map[2][8];

extern const u8 frq_wlo_map[2][8];

extern const u8 frq_cke_map[2][8];

extern const u8 frq_xpdll_map[2][8];

extern const u8 frq_xp_map[2][8];

extern const u8 frq_aonpd_map[2][8];

extern const u32 frq_comp2_map[2][8];

extern const u32 pattern[32][16];

extern const u8 use_base[63][32];

extern const u8 invert[63][32];

#endif /* RAMINIT_TABLES_H */
