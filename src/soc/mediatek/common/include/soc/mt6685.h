/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT6685_H__
#define __SOC_MEDIATEK_MT6685_H__

#include <soc/spmi.h>
#include <types.h>

struct mt6685_setting {
	unsigned short addr;
	unsigned short val;
	unsigned short mask;
	unsigned char shift;
};

struct mt6685_key_setting {
	unsigned short addr;
	unsigned short val;
};

void mt6685_init(void);
void mt6685_init_pmif_arb(void);
void mt6685_write_field(u32 reg, u32 val, u32 mask, u32 shift);
u32 mt6685_read_field(u32 reg, u32 mask, u32 shift);
u8 mt6685_read8(u32 reg);
void mt6685_write8(u32 reg, u8 reg_val);
u16 mt6685_read16(u32 reg);
void mt6685_write16(u32 reg, u16 reg_val);
void mt6685_init_setting(void);

#endif
