/*
 * This file is part of the coreboot project.
 *
 * Copyright 2018 Google LLC
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

#include <stdint.h>
#include <string.h>
#include <arch/acpi.h>
#include <arch/acpi_pld.h>

int acpi_pld_fill_usb(struct acpi_pld *pld, enum acpi_upc_type type)
{
	if (!pld)
		return -1;

	memset(pld, 0, sizeof(struct acpi_pld));

	/* Set defaults */
	pld->ignore_color = 1;
	pld->panel = PLD_PANEL_UNKNOWN;
	pld->vertical_position = PLD_VERTICAL_POSITION_CENTER;
	pld->horizontal_position = PLD_HORIZONTAL_POSITION_CENTER;
	pld->rotation = PLD_ROTATE_0;
	pld->visible = 1;

	/* Set the shape based on port type */
	switch (type) {
	case UPC_TYPE_A:
	case UPC_TYPE_USB3_A:
	case UPC_TYPE_USB3_POWER_B:
		pld->shape = PLD_SHAPE_HORIZONTAL_RECTANGLE;
		break;
	case UPC_TYPE_MINI_AB:
	case UPC_TYPE_USB3_B:
		pld->shape = PLD_SHAPE_CHAMFERED;
		break;
	case UPC_TYPE_USB3_MICRO_B:
	case UPC_TYPE_USB3_MICRO_AB:
		pld->shape = PLD_SHAPE_HORIZONTAL_TRAPEZOID;
		break;
	case UPC_TYPE_C_USB2_ONLY:
	case UPC_TYPE_C_USB2_SS_SWITCH:
	case UPC_TYPE_C_USB2_SS:
		pld->shape = PLD_SHAPE_OVAL;
		break;
	case UPC_TYPE_INTERNAL:
	default:
		pld->shape = PLD_SHAPE_UNKNOWN;
		pld->visible = 0;
		break;
	}

	return 0;
}

int acpi_pld_to_buffer(const struct acpi_pld *pld, uint8_t *buf, int buf_len)
{
	if (!pld || !buf)
		return -1;

	memset(buf, 0, buf_len);

	/* [0] Revision (=2) */
	buf[0] = 0x2;

	if (pld->ignore_color) {
		/* [1] Ignore Color */
		buf[0] |= 0x80;
	} else {
		/* [15:8] Red Color */
		buf[1] = pld->color_red;
		/* [23:16] Green Color */
		buf[2] = pld->color_green;
		/* [31:24] Blue Color */
		buf[3] = pld->color_blue;
	}

	/* [47:32] Width */
	buf[4] = pld->width & 0xff;
	buf[5] = pld->width >> 8;

	/* [63:48] Height */
	buf[6] = pld->height & 0xff;
	buf[7] = pld->height >> 8;

	/* [64] User Visible */
	buf[8] |= (pld->visible & 0x1);

	/* [65] Dock */
	buf[8] |= (pld->dock & 0x1) << 1;

	/* [66] Lid */
	buf[8] |= (pld->lid & 0x1) << 2;

	/* [69:67] Panel */
	buf[8] |= (pld->panel & 0x7) << 3;

	/* [71:70] Vertical Position */
	buf[8] |= (pld->vertical_position & 0x3) << 6;

	/* [73:72] Horizontal Position */
	buf[9] |= (pld->horizontal_position & 0x3);

	/* [77:74] Shape */
	buf[9] |= (pld->shape & 0xf) << 2;

	/* [78] Group Orientation */
	buf[9] |= (pld->group_orientation & 0x1) << 6;

	/* [86:79] Group Token (incorrectly defined as 1 bit in ACPI 6.2A) */
	buf[9] |= (pld->group_token & 0x1) << 7;
	buf[10] |= (pld->group_token >> 0x1) & 0x7f;

	/* [94:87] Group Position */
	buf[10] |= (pld->group_position & 0x1) << 7;
	buf[11] |= (pld->group_position >> 0x1) & 0x7f;

	/* [95] Bay */
	buf[11] |= (pld->bay & 0x1) << 7;

	/* [96] Ejectable */
	buf[12] |= (pld->ejectable & 0x1);

	/* [97] Ejectable with OSPM help */
	buf[12] |= (pld->ejectable_ospm & 0x1) << 1;

	/* [105:98] Cabinet Number */
	buf[12] |= (pld->cabinet_number & 0x3f) << 2;
	buf[13] |= (pld->cabinet_number >> 6) & 0x3;

	/* [113:106] Card Cage Number */
	buf[13] |= (pld->card_cage_number & 0x3f) << 2;
	buf[14] |= (pld->card_cage_number >> 6) & 0x3;

	/* [114] PLD is a Reference Shape */
	buf[14] |= (pld->reference_shape & 0x1) << 2;

	/* [118:115] Rotation */
	buf[14] |= (pld->rotation & 0xf) << 3;

	/* [123:119] Draw Order */
	buf[14] |= (pld->draw_order & 0x1) << 7;
	buf[15] |= (pld->draw_order >> 1) & 0xf;

	/* [127:124] Reserved */

	/* Both 16 byte and 20 byte buffers are supported by the spec */
	if (buf_len == 20) {
		/* [143:128] Vertical Offset */
		buf[16] = pld->vertical_offset & 0xff;
		buf[17] = pld->vertical_offset >> 8;

		/* [159:144] Horizontal Offset */
		buf[18] = pld->horizontal_offset & 0xff;
		buf[19] = pld->horizontal_offset >> 8;
	}

	return 0;
}
