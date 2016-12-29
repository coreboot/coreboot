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

/* CMOS backend */
void read_vbnv_cmos(uint8_t *vbnv_copy);
void save_vbnv_cmos(const uint8_t *vbnv_copy);
void init_vbnv_cmos(int rtc_fail);

/* Flash backend */
void read_vbnv_flash(uint8_t *vbnv_copy);
void save_vbnv_flash(const uint8_t *vbnv_copy);

/* EC backend */
void read_vbnv_ec(uint8_t *vbnv_copy);
void save_vbnv_ec(const uint8_t *vbnv_copy);

#endif /* __VBOOT_VBNV_H__ */
