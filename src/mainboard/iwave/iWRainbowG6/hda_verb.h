/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2009-2010 iWave Systems
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

static u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x111d76d5,		// Codec Vendor / Device ID: IDT 92HD81
	0x00000000,		// Subsystem ID
	0x0000000a,		// Number of jacks

	/* NID 0x0a, Port A (capless headphone) */
	0x0A71C40,
	0x0A71D10,
	0x0A71EA1,
	0x0A71F02,

	/* NID 0x0b, Port B (capless headphone) */
	0x0B71C1F,
	0x0B71D10,
	0x0B71E21,
	0x0B71F02,

	/*
	 * NID 0x0c, Port C (Line IN/OUT+MIC for YD/UA revisions, and
	 * Line IN+MIC for TA revision)
	 */
	0x0C71CF0,
	0x0C71D00,
	0x0C71E00,
	0x0C71F40,

	/* NID 0x0d, Port D (BTL output - EAPD control) */
	0x0D71C10,
	0x0D71D41,
	0x0D71E10,
	0x0D71F10,

	/* NID 0x0e, Port E (Line IN/OUT) */
	0x0E71CF0,
	0x0E71D00,
	0x0E71E00,
	0x0E71F40,

	/* NID 0x0f, Port F (Line IN/OUT, MIC) */
	0x0F71CF0,
	0x0F71D00,
	0x0F71E00,
	0x0F71F40,

	/* NID 0x10, MonoOut (output-only) */
	0x1071CF0,
	0x1071D00,
	0x1071EF0,
	0x1071F40,

	/* NID 0x10, DigMic0 (Digital Microphone 0) */
	0x1171CF0,
	0x1171D00,
	0x1171E00,
	0x1171F40,

	/* NID 0x1f, Dig0Pin (First Digital Output Pin) */
	0x1F71C50,
	0x1F71D21,
	0x1F71E40,
	0x1F71F10,

	/* NID 0x20, Dig1Pin (Second Digital Output Pin / DMIC Input Pin) */
	0x2071CF0,
	0x2071D00,
	0x2071E00,
	0x2071F40,

	/* BTL Gain */
	0x017F417, /* Gain = 16.79dB */
};

extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;
