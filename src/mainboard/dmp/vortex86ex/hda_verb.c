/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 DMP Electronics Inc.
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

#include <device/azalia_device.h>

const u32 cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0262,     // Codec Vendor / Device ID: Realtek ALC262
	0x10714700,     // Subsystem ID
	0x0000000f,     // Number of jacks

	/* ===== HDA Codec Subsystem ID Verb-table ===== */
	/* HDA Codec Subsystem ID  : 0x10EC0000 */
	AZALIA_SUBVENDOR(0x0, 0x10ec0000),

	/* ===== Pin Widget Verb-table ===== */
	/* Widget node 0x01 : */
	0x0017ff00,
	0x0017ff00,
	0x0017ff00,
	0x0017ff00,
	/* Pin widget 0x11 - S/PDIF-OUT2 */
	AZALIA_PIN_CFG(0x0, 0x11, 0x40000000),
	/* Pin widget 0x12 - DMIC */
	AZALIA_PIN_CFG(0x0, 0x12, 0x411111f0),
	/* Pin widget 0x14 - LINE-OUT (Port-D) */
	AZALIA_PIN_CFG(0x0, 0x14, 0x01014110),
	/* Pin widget 0x15 - HP-OUT (Port-A) */
	AZALIA_PIN_CFG(0x0, 0x15, 0x411111f0),
	/* Pin widget 0x16 - MONO-OUT */
	AZALIA_PIN_CFG(0x0, 0x16, 0x411111f0),
	/* Pin widget 0x18 - MIC1 (Port-B) */
	AZALIA_PIN_CFG(0x0, 0x18, 0x411111f0),
	/* Pin widget 0x19 - MIC2 (Port-F) */
	AZALIA_PIN_CFG(0x0, 0x19, 0x02a19130),
	/* Pin widget 0x1A - LINE1 (Port-C) */
	AZALIA_PIN_CFG(0x0, 0x1a, 0x01813140),
	/* Pin widget 0x1B - LINE2 (Port-E) */
	AZALIA_PIN_CFG(0x0, 0x1b, 0x411111f0),
	/* Pin widget 0x1C - CD-IN */
	AZALIA_PIN_CFG(0x0, 0x1c, 0x411111f0),
	/* Pin widget 0x1D - BEEP-IN */
	AZALIA_PIN_CFG(0x0, 0x1d, 0x40354629),
	/* Pin widget 0x1E - S/PDIF-OUT */
	AZALIA_PIN_CFG(0x0, 0x1e, 0x18561120),
	/* Pin widget 0x1F - S/PDIF-IN */
	AZALIA_PIN_CFG(0x0, 0x1f, 0x411111f0),
};

const u32 pc_beep_verbs[0] = {};

AZALIA_ARRAY_SIZES;
