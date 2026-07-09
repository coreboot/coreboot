/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_MCAX_DEF_H
#define AMD_BLOCK_MCAX_DEF_H

#include <types.h>

enum cb_err amd_mca_fill_fru_from_synd(const int bank, char fru_text[17]);
u16 mcax_bank_hardware_id(unsigned int bank);
u32 mcax_bank_instance_id(unsigned int bank);
bool mcax_bank_is_umc(unsigned int bank);
bool mcax_bank_skip(unsigned int bank);

#endif /* AMD_BLOCK_MCAX_DEF_H */
