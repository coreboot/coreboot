/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007-2009 coresystems GmbH
 *               2012 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of the License.
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

#define PIN_CFG(pin, val) \
	(pin << 20) | ( 0x71c << 8) | (val & 0xff), \
	(pin << 20) | ( 0x71d << 8) | ((val >> 8) & 0xff), \
	(pin << 20) | ( 0x71e << 8) | ((val >> 16) & 0xff), \
	(pin << 20) | ( 0x71f << 8) | ((val >> 24) & 0xff), \

static const u32 mainboard_cim_verb_data[] = {
	/* coreboot specific header */
	0x10ec0262,	// Codec Vendor / Device ID: Realtek ALC262
	0x43528986,	// Subsystem ID
	0x0000000c,	// Number of entries

	/* Pin Widget Verb Table */

	/* Pin Complex (NID 0x11), S/PDIF-OUT2: not connected */
	PIN_CFG(0x11, 0x411111f0)
	/* Pin Complex (NID 0x14), LINE_OUT (port D): Speakers */
	PIN_CFG(0x14, 0x99130110)
	/* Pin Complex (NID 0x15), HP_OUT (port A): Head phones */
	PIN_CFG(0x15, 0x0121411f)
	/* Pin Complex (NID 0x16), MONO-OUT: not connected */
	PIN_CFG(0x16, 0x411111f0)
	/* Pin Complex (NID 0x18), MIC1 (port B): Microphone */
	PIN_CFG(0x18, 0x01a19920)
	/* Pin Complex (NID 0x19), MIC2 (port F): not connected */
	PIN_CFG(0x19, 0x411111f0)
	/* Pin Complex (NID 0x1a), LINE1 (port C): Line-In */
	PIN_CFG(0x1a, 0x01813121)
	/* Pin Complex (NID 0x1b), LINE2 (port E): MDC */
	PIN_CFG(0x1b, 0x9983012f)
	/* Pin Complex (NID 0x1c), CD_IN */
	PIN_CFG(0x1c, 0x593301f0)
	/* Pin Complex (NID 0x1d), PCBEEP */
	PIN_CFG(0x1d, 0x4014022d)
	/* Pin Complex (NID 0x1e), S/PDIF-OUT: not connected */
	PIN_CFG(0x1e, 0x411111f0)
	/* Pin Complex (NID 0x1f), S/PDIF-IN: not connected */
	PIN_CFG(0x1f, 0x411111f0)
};
extern const u32 *cim_verb_data;
extern u32 cim_verb_data_size;

static const u32 mainboard_pc_beep_verbs[] = {
	0x00170500,	/* power up codec */
	0x01470500,	/* power up speakers */
	0x01470100,	/* select lout1 (input 0x0) for speakers */
	0x01470740,	/* enable speakers output */
	0x00b37517,	/* unmute beep (mixer's input 0x5), set amp 0dB */
	0x00c37100,	/* unmute mixer in lout1 (lout1 input 0x1) */
	0x00c3b015,	/* set lout1 output volume -15dB */
	0x0143b000,	/* unmute speakers */
};
extern const u32 * pc_beep_verbs;
extern u32 pc_beep_verbs_size;
