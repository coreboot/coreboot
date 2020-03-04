/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __BL31_H__
#define __BL31_H__

#include <types.h>

#include <arm-trusted-firmware/include/export/lib/bl_aux_params/bl_aux_params_exp.h>

/* Load and enter BL31, set it up to exit to payload according to arguments. */
void run_bl31(u64 payload_entry, u64 payload_arg0, u64 payload_spsr);

/* Return platform-specific bl31_plat_params. SoCs should avoid overriding this
   and stick with the default BL aux parameter framework if possible. */
void *soc_get_bl31_plat_params(void);

/* Add a BL aux parameter to the list to be passed to BL31. Only works for SoCs
   that use the default soc_get_bl31_plat_params() implementation. */
void register_bl31_aux_param(struct bl_aux_param_header *param);

#endif /* __BL31_H__ */
