/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
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

#ifndef LYNXPOINT_HDA_VERB_H
#define LYNXPOINT_HDA_VERB_H

#define HDA_GCAP_REG		0x00
#define HDA_GCTL_REG		0x08
#define   HDA_GCTL_CRST		(1 << 0)
#define HDA_STATESTS_REG	0x0e
#define HDA_IC_REG		0x60
#define HDA_IR_REG		0x64
#define HDA_ICII_REG		0x68
#define   HDA_ICII_BUSY		(1 << 0)
#define   HDA_ICII_VALID	(1 << 1)

int hda_codec_detect(u8 *base);
int hda_codec_write(u8 *base, u32 size, const u32 *data);
int hda_codec_init(u8 *base, int addr, int verb_size, const u32 *verb_data);

#endif
