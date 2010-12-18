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
        0x111d76d5,     // Codec Vendor / Device ID: IDT / 92HD81
        0x00000000,     // Subsystem ID
        0x0000000a,     // Number of jacks

	//Codec 92HD81 Yangtze 4ch Pin Port A, data = 0x02a11040
	0x0A71C40,
	0x0A71D10,
	0x0A71EA1,
	0x0A71F02,

	//;Codec 92HD81 Yangtze 4ch Pin Port B, data = 0x0221101f
	0x0B71C1F,
	0x0B71D10,
	0x0B71E21,
	0x0B71F02,

	//;Codec 92HD81 Yangtze 4ch Pin Port C, data = 0x400000f0
	0x0C71CF0,
	0x0C71D00,
	0x0C71E00,
	0x0C71F40,

	//;Codec 92HD81 Yangtze 4ch Pin Port D, data = 0x10104110
	0x0D71C10,
	0x0D71D41,
	0x0D71E10,
	0x0D71F10,

	//;Codec 92HD81 Yangtze 4ch Pin Port E, data = 0x400000f0
	0x0E71CF0,
	0x0E71D00,
	0x0E71E00,
	0x0E71F40,

	//;Codec 92HD81 Yangtze 4ch Pin Port F, data = 0x400000f0
	0x0F71CF0,
	0x0F71D00,
	0x0F71E00,
	0x0F71F40,

	//;Codec 92HD81 Yangtze 4ch Pin MonoOut, data = 0x40f000f0
	0x1071CF0,
	0x1071D00,
	0x1071EF0,
	0x1071F40,

	//;Codec 92HD81 Yangtze 4ch Pin DMic0, data = 0x400000f0
	0x1171CF0,
	0x1171D00,
	0x1171E00,
	0x1171F40,

	//;Codec 92HD81 Yangtze 4ch Pin Dig0Pin, data = 0x10402150
	0x1F71C50,
	0x1F71D21,
	0x1F71E40,
	0x1F71F10,

	//;Codec 92HD81 Yangtze 4ch Pin Dig1Pin, data = 0x400000f0
	0x2071CF0,
	0x2071D00,
	0x2071E00,
	0x2071F40,

	//; BTL Gain
	0x017F417
	// ; Gain = 16.79dB
};

extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;

