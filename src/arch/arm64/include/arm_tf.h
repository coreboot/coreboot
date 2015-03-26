/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc.
 */

#ifndef __ARM_TF_H__
#define __ARM_TF_H__

#include <types.h>

/* TODO: Pull in directly from ARM TF once its headers have been reorganized. */
#include <arm_tf_temp.h>

/* Load and enter BL31, set it up to exit to payload according to arguments. */
void arm_tf_run_bl31(u64 payload_entry, u64 payload_arg0, u64 payload_spsr);

/* Return platform-specific bl31_plat_params. May update bl31_params. */
void *soc_get_bl31_plat_params(bl31_params_t *bl31_params);

#endif /* __ARM_TF_H__ */
