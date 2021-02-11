/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <device/dram/spd.h>

const char *spd_manufacturer_name(const uint16_t mod_id)
{
	switch (mod_id) {
	case 0x9b85:
		return "Crucial";
	case 0x4304:
		return "Ramaxel";
	case 0x4f01:
		return "Transcend";
	case 0x9801:
		return "Kingston";
	case 0x987f:
		return "Hynix";
	case 0x9e02:
		return "Corsair";
	case 0xb004:
		return "OCZ";
	case 0xad80:
		return "Hynix/Hyundai";
	case 0x3486:
		return "Super Talent";
	case 0xcd04:
		return "GSkill";
	case 0xce80:
		return "Samsung";
	case 0xfe02:
		return "Elpida";
	case 0x2c80:
		return "Micron";
	default:
		return NULL;
	}
}
