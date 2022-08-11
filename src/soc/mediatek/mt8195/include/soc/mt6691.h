/* SPDX-License-Identifier: GPL-2.0-only OR MIT */

#ifndef __SOC_MEDIATEK_MT8195_MT6691_H__
#define __SOC_MEDIATEK_MT8195_MT6691_H__

#include <stdint.h>

void mt6691_probe(uint8_t i2c_num);
int mt6691_set_voltage(uint8_t i2c_num, unsigned int volt_uv);
int mt6691_get_voltage(uint8_t i2c_num);

#define MT6691_CID_CODE		0x01

#define MT6691_VDD2_ID		0x0000
#define MT6691_VDDQ_ID		0x0000
#define MT6691_PGOOD_SHIFT	7
#define MT6691_DN_SR_MASK	0x7
#define MT6691_DN_SR_SHIFT	5

enum {
	/* Voltage setting */
	MT6691_VSEL0 =		0x00,
	MT6691_VSEL1 =		0x01,
	/* Control register */
	MT6691_CONTROL =	0x02,
	/* IC Type */
	MT6691_ID1 =		0x03,
	/* IC mask version */
	MT6691_ID2 =		0x04,
	/* Monitor register */
	MT6691_MONITOR =	0x05,
	MT6691_CTRL2 =		0x06,
	MT6691_CTRL3 =		0x07,
	MT6691_CTRL4 =		0x08,
};

#endif
