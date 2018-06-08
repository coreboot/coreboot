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

#ifndef __ACPI_PLD_H
#define __ACPI_PLD_H

#include <arch/acpi.h>
#include <stdint.h>

enum acpi_pld_panel {
	PLD_PANEL_TOP,
	PLD_PANEL_BOTTOM,
	PLD_PANEL_LEFT,
	PLD_PANEL_RIGHT,
	PLD_PANEL_FRONT,
	PLD_PANEL_BACK,
	PLD_PANEL_UNKNOWN
};

enum acpi_pld_vertical_position {
	PLD_VERTICAL_POSITION_UPPER,
	PLD_VERTICAL_POSITION_CENTER,
	PLD_VERTICAL_POSITION_LOWER
};

/*
 * The ACPI spec 6.2A does not define the horizontal position field.
 * These values are taken from the IASL compiler:
 * https://github.com/acpica/acpica/blob/master/source/components/utilities/utglobal.c#L321
 */

enum acpi_pld_horizontal_position {
	PLD_HORIZONTAL_POSITION_LEFT,
	PLD_HORIZONTAL_POSITION_CENTER,
	PLD_HORIZONTAL_POSITION_RIGHT
};

enum acpi_pld_shape {
	PLD_SHAPE_ROUND,
	PLD_SHAPE_OVAL,
	PLD_SHAPE_SQUARE,
	PLD_SHAPE_VERTICAL_RECTANGLE,
	PLD_SHAPE_HORIZONTAL_RECTANGLE,
	PLD_SHAPE_VERTICAL_TRAPEZOID,
	PLD_SHAPE_HORIZONTAL_TRAPEZOID,
	PLD_SHAPE_UNKNOWN,
	PLD_SHAPE_CHAMFERED
};

enum acpi_pld_orientation {
	PLD_ORIENTATION_HORIZONTAL,
	PLD_ORIENTATION_VERTICAL,
};

enum acpi_pld_rotate {
	PLD_ROTATE_0,
	PLD_ROTATE_45,
	PLD_ROTATE_90,
	PLD_ROTATE_135,
	PLD_ROTATE_180,
	PLD_ROTATE_225,
	PLD_ROTATE_270,
	PLD_ROTATE_315
};

struct acpi_pld {
	/* Color field can be explicitly ignored */
	bool ignore_color;
	uint8_t color_red;
	uint8_t color_blue;
	uint8_t color_green;

	/* Port characteristics */
	bool visible;		/* Can be seen by the user */
	bool lid;		/* Port is on lid of device */
	bool dock;		/* Port is in a docking station */
	bool bay;		/* Port is in a bay */
	bool ejectable;		/* Device is ejectable, has _EJx objects */
	bool ejectable_ospm;	/* Device needs OSPM to eject */
	uint16_t width;		/* Width in mm */
	uint16_t height;	/* Height in mm */
	uint16_t vertical_offset;
	uint16_t horizontal_offset;
	enum acpi_pld_panel panel;
	enum acpi_pld_horizontal_position horizontal_position;
	enum acpi_pld_vertical_position vertical_position;
	enum acpi_pld_shape shape;
	enum acpi_pld_rotate rotation;

	/* Port grouping */
	enum acpi_pld_orientation group_orientation;
	uint8_t group_token;
	uint8_t group_position;
	uint8_t draw_order;
	uint8_t cabinet_number;
	uint8_t card_cage_number;

	/* Set if this PLD defines a reference shape */
	bool reference_shape;
};

/* Fill out PLD structure with defaults based on USB port type */
int acpi_pld_fill_usb(struct acpi_pld *pld, enum acpi_upc_type type);

/* Turn PLD structure into a 20 byte ACPI buffer */
int acpi_pld_to_buffer(const struct acpi_pld *pld, uint8_t *buf, int buf_len);

#endif
