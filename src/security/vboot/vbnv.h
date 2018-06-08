/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2016 Google Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef __VBOOT_VBNV_H__
#define __VBOOT_VBNV_H__

#include <types.h>

/* Generic functions */
void read_vbnv(uint8_t *vbnv_copy);
void save_vbnv(const uint8_t *vbnv_copy);
int verify_vbnv(uint8_t *vbnv_copy);
void regen_vbnv_crc(uint8_t *vbnv_copy);
int get_recovery_mode_from_vbnv(void);
void set_recovery_mode_into_vbnv(int recovery_reason);
int vboot_wants_oprom(void);

/* Read the USB Device Controller(UDC) enable flag from VBNV. */
int vbnv_udc_enable_flag(void);

/* Initialize and read vbnv. This is used in the main vboot logic path. */
void vbnv_init(uint8_t *vbnv_copy);
/* Reset vbnv snapshot to a known state. */
void vbnv_reset(uint8_t *vbnv_copy);

/* CMOS backend */
/* Initialize the vbnv cmos backing store. The vbnv_copy pointer is used for
   optional temporary storage in the init function. */
void vbnv_init_cmos(uint8_t *vbnv_copy);
/* Return non-zero if cmos power was lost. */
int vbnv_cmos_failed(void);
void read_vbnv_cmos(uint8_t *vbnv_copy);
void save_vbnv_cmos(const uint8_t *vbnv_copy);

/* Flash backend */
void read_vbnv_flash(uint8_t *vbnv_copy);
void save_vbnv_flash(const uint8_t *vbnv_copy);

/* EC backend */
void read_vbnv_ec(uint8_t *vbnv_copy);
void save_vbnv_ec(const uint8_t *vbnv_copy);

#endif /* __VBOOT_VBNV_H__ */
