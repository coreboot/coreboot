/*
 * This file is part of the superiotool project.
 *
 * Copyright (C) 2007 Uwe Hermann <uwe@hermann-uwe.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
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

#include "superiotool.h"

#define DEVICE_ID_REG_OLD	0x09

#define DEVICE_ID_REG		0x20
#define DEVICE_REV_REG		0x21

/**
 * The ID entries must be in 0xYYZ format, where YY is the device ID,
 * and Z is bits 7..4 of the device revision register. We do not match
 * bits 3..0 of the device revision here (at least for newer Super I/Os).
 *
 * But some of the older versions use both bytes (0x20 and 0x21), where
 * register 0x21 holds the ID and the full 8 bits of 0x21 hold the revision.
 *
 * Some other Super I/Os only use bits 3..0 of 0x09 as ID.
 */
static const struct superio_registers reg_table[] = {
	/* ID and rev[3..0] */
	{0x527, "W83977CTF", {	/* TODO: Not yet in sensors-detect */
		{EOT}}},
	{0x52f, "W83977EF/EG", {
		{NOLDN, NULL,
			{0x02,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x28,0x2a,
			 0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{RSVD,0x52,MISC,0xff,0xfe,MISC,0x00,MISC,0x00,0x00,
			 0x00,0x00,RSVD,RSVD,RSVD,EOT}},
		/* Some register defaults depend on the value of PNPCSV. */
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,
			 0xf5,EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x00,0xff,0x00,
			 0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x7, "GPIO 1",
			{0x30,0x60,0x61,0x62,0x63,0x64,0x65,0x70,0x72,0xe0,
			 0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xf1,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
			 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,EOT}},
		{0x8, "GPIO 2",
			{0x30,0x60,0x61,0x70,0x72,0xe8,0xe9,0xea,0xeb,0xec,
			 0xed,0xf0,0xf1,0xf2,0xf3,0xf4,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,
			 0x01,0x00,RSVD,0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xf0,0xf1,0xf3,0xf4,0xf6,0xf7,0xf9,0xfe,0xff,EOT},
			{0x00,0x00,0x00,0x00,MISC,MISC,MISC,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,RSVD,RSVD,EOT}},
		{EOT}}},
	{0x595, "W83627SF", {	/* TODO: Not yet in sensors-detect */
		{EOT}}},
	{0x601, "W83697HF/F/HG", { /* No G version? */
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x28,0x29,
			 0x2a,EOT},
			{0x60,NANA,0xff,0x00,0x00,0x00,0x00,0x00,0x00,
			 MISC,EOT}},
		/* Some register defaults depend on the value of PNPCSV. */
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,
			 0xf5,EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x00,0xff,0x00,
			 0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x6, "Consumer IR",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{0x7, "Game port, GPIO 1",
			{0x30,0x60,0x61,0x62,0x63,0xf0,0xf1,0xf2,EOT},
			{0x00,0x02,0x01,0x00,0x00,0xff,0x00,0x00,EOT}},
		{0x8, "MIDI port, GPIO 5",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 0xf4,0xf5,EOT},
			{0x00,0x03,0x30,0x00,0x00,0x09,0xff,0x00,0x00,0x00,
			 0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO 3, GPIO 4",
			{0x30,0x60,0x61,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,
			 0xf7,0xf8,0xf5,EOT},
			{0x00,0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0xff,
			 0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe5,0xe6,0xe7,
			 0xf0,0xf1,0xf3,0xf4,0xf6,0xf7,0xf9,EOT},
			{0x00,0x00,0x00,0x00,NANA,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{EOT}}},
	{0x610, "W83L517D/D-F", {
		{EOT}}},
	{0x708, "W83637HF/HG", {
		{EOT}}},
	{0x828, "W83627THF/THG", { /* We assume rev is bits 3..0 of 0x21. */
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x28,0x29,0x2a,
			 0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x82,NANA,0xff,0x00,MISC,0x00,MISC,0x00,0x00,0x00,
			 MISC,MISC,MISC,0x00,0x00,EOT}},
		/* Some register defaults depend on the value of PNPCSV. */
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,
			 0xf5,EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x00,0xff,0x00,
			 0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x80,EOT}},
		{0x7, "GPIO 1, GPIO 5, game port, MIDI port",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 0xf4,0xf5,EOT},
			{0x00,0x02,0x01,0x03,0x30,0x09,0xff,0x00,0x00,0xff,
			 0x00,0x00,EOT}},
		{0x8, "GPIO 2",
			{0x30,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,EOT},
			{0x00,0xff,0x00,0x00,0x00,RSVD,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 3, GPIO 4",
			{0x30,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,EOT},
			{0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,EOT}},
		{0xa, "ACPI",
			/* Note: Datasheet says 0xe2 can't be read/written. */
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xf0,0xf1,0xf3,0xf4,0xf6,0xf7,0xf9,0xfe,0xff,EOT},
			{0x00,0x00,0x00,0x00,MISC,MISC,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,RSVD,RSVD,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{EOT}}},
#if 0
	{0x85x, "W83687THF", {	/* TODO: sensors-detect: 0x85 */
		{EOT}}},
#endif
	{0xa02, "W83627DHG", {
		{NOLDN, NULL,
			{0x02,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
			 0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x00,0xa0,NANA,0xff,0x00,MISC,0x00,MISC,RSVD,0x50,
			 0x00,0x00,RSVD,0xe2,0x21,0x00,0x00,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,0xf5,
			 EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x8e,0x00,0xff,0x00,0x00,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x6, "Serial peripheral interface",
			{0x30,0x62,0x63,EOT},
			{0x00,0x00,0x00,EOT}},
		{0x7, "GPIO 6",
			{0x30,0xf4,0xf5,0xf6,0xf7,EOT},
			{0x00,0xff,0x00,0x00,0x00,EOT}},
		{0x8, "WDTO#, PLED",
			{0x30,0xf5,0xf6,0xf7,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO 3, GPIO 4, GPIO 5",
			{0x30,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,
			 0xe9,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xfe,
			 EOT},
			{0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,
			 0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,
			 EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xe8,0xe9,0xf2,0xf3,0xf4,0xf6,0xf7,0xfe,EOT},
			{0x00,0x00,0x01,0x00,0xff,0x08,0x00,RSVD,0x1c,0x00,
			 RSVD,RSVD,0x7c,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x00,0x00,0x00,0x81,0x00,0x00,EOT}},
		{0xc, "PECI, SST",
			{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe8,0xf1,0xfe,0xff,
			 EOT},
			{0x00,0x48,0x48,0x48,0x48,0x00,0x00,0x48,0x00,0x00,
			 EOT}},
		{EOT}}},
	{0xa23, "W83627UHG = NCT6627UD", { /* TODO: Not yet in sensors-detect */
		{NOLDN, NULL,    /* CR2B (485) is only valid for chip rev.E */
			{0x02,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
			 0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x00,0xa2,NANA,0xff,0xF0,MISC,0x00,MISC,RSVD,0x00,
			 0x00,0x00,0x00,0x02,0x00,0x00,0x00,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,0xf5,
			 EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x8e,0x00,0xff,0x00,0x00,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "UART A",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "UART B",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x6, "UART C",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xE0,0x04,0x00,EOT}},
		{0x7, "GPIO 3, GPIO 4",
			{0x30,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,EOT},
			{0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,EOT}},
		{0x8, "WDTO#, PLED, GPIO 5,6 & GPIO Base Address",
			{0x30,0x60,0x61,
			 0xE0,0xE1,0xE2,0xE3,0xE4,0xE5,0xE6,0xE7,
			 0xF5,0xF6,0xF7,EOT},
			{0x02,0x00,0x00,
                         0xFF,0x00,0x00,0x00,0xFF,0x1F,0x00,0x00,
			 0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 1, GPIO 2 and SUSLED",
			{0x30,
			 0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xF3,EOT},
			{0x00,
			 0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,
                         0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xe8,0xe9,0xf2,0xf3,0xf4,0xf6,0xf7,0xfe,EOT},
			{0x00,0x00,0x01,0x00,0xff,0x08,0x00,0x00,0x1c,0x00,
			 RSVD,RSVD,0x3e,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x00,0x00,0x00,RSVD,RSVD,0x00,EOT}},
		{0xc, "PECI, SST",
			{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,
			 0xf1,0xf2,0xf3,0xfe,0xff,EOT},
			{0x00,0x48,0x48,0x48,0x48,0x00,RSVD,RSVD,0x00,
			 0x48,0x50,0x10,0x23,0x5A,EOT}},
		{0xD, "UART D",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x02,0xe0,0x03,0x00,EOT}},
		{0xE, "UART E",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x03,0xe8,0x04,0x00,EOT}},
		{0xF, "UART F",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x02,0xe8,0x03,0x00,EOT}},
		{EOT}}},
	{0xa51, "W83667HG", {
		/* See also: http://lists.lm-sensors.org/pipermail/lm-sensors/2008-July/023683.html */
		{EOT}}},
	{0xb07, "W83627DHG-P/-PT", {
		{NOLDN, NULL,
			{0x02,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
			 0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x00,0xb0,NANA,0xff,0x00,MISC,0x00,MISC,RSVD,0x50,
			 0x00,0x00,RSVD,0xe2,0x21,0x00,0x00,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,0xf5,
			 EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x8e,0x00,0xff,0x00,0x00,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x6, "Serial peripheral interface",
			{0x30,0x62,0x63,EOT},
			{0x00,0x00,0x00,EOT}},
		{0x7, "GPIO 6",
			{0x30,0xf4,0xf5,0xf6,0xf7,0xf8,EOT},
			{0x00,0xff,0x00,0x00,0x00,0x00,EOT}},
		{0x8, "WDTO#, PLED",
			{0x30,0xf5,0xf6,0xf7,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO 3, GPIO 4, GPIO 5",
			{0x30,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,
			 0xe9,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,
			 0xf9,0xfa,0xfe,EOT},
			{0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,
			 0x00,0xff,0x00,0x00,0x00,0xff,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xe8,0xe9,0xf2,0xf3,0xf4,0xf6,0xf7,0xfe,EOT},
			{0x00,0x00,0x01,0x00,0xff,0x08,0x00,RSVD,0x1c,0x00,
			 RSVD,RSVD,0x7c,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x00,0x00,0x00,0x81,0x00,0x00,EOT}},
		{0xc, "PECI, SST",
			{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe8,0xe9,0xea,0xec,
			 0xee,0xef,0xf1,0xf2,0xf3,0xfe,0xff,EOT},
			{0x00,0x48,0x48,0x48,0x48,0x00,0x00,RSVD,0x00,0x00,
			 0x01,0x5a,0x48,0x50,0x10,0x00,0x00,EOT}},
		{EOT}}},
	{0xb35, "Nuvoton NCT5571D", {
		{NOLDN, NULL,
			{0x02,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
			 0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{RSVD,0xb3,NANA,0xff,0x00,0x40,0x00,MISC,0xff,0x20,0x00,
			 0x00,0x7f,0x0a,0x08,0x00,MISC,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x83,EOT}},
		{0x7, "GPIO 6, GPIO 8, GPIO 9",
			{0x30,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xea,0xeb,0xec,0xed,
			 0xee,0xf4,0xf5,0xf6,0xf7,0xf8,EOT},
			{0x18,0xef,MISC,0x00,0x00,0xff,MISC,0x00,0x00,RSVD,0x00,
			 0x00,0xff,MISC,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO 3, GPIO 5",
			{0x30,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe9,0xea,
			 0xeb,0xf4,0xf5,0xf6,0xf7,0xfe,EOT},
			{0x05,0xff,MISC,0x00,0x00,0xff,MISC,0x00,0x00,0x00,0x00,
			 0x00,0xff,MISC,0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xf2,
			 0xf3,0xf4,0xf6,0xf7,0xfe,0xff,EOT},
			{0x01,0x00,0xff,0x00,0x00,0x02,0x1c,0x00,RSVD,0x00,0x7c,
			 0x00,0x00,0x00,0x00,0x00,0x30,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xe0,0xe1,0xe2,0xe3,0xf0,0xf5,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xc1,0x10,EOT}},
		{0xc, "PECI",
			{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe8,0xe9,0xea,0xec,
			 0xee,0xef,0xf1,0xf2,0xf3,0xfe,0xff,EOT},
			{0x00,0x48,0x48,0x48,0x48,0x00,0x00,0x00,RSVD,0x00,0x00,
			 0x01,0x5a,0x48,0x50,0x10,0x80,0x01,EOT}},
		{0xd, "SUSLED",
			{0xec,EOT},
			{0x01,EOT}},
		{0xf, "GPIO Push-Pull/OD Select",
			{0xe0,0xe1,0xe3,0xe4,0xe5,0xe6,0xe7,0xe8,0xe9,0xf0,0xf1,
			 0xf2,0xf3,0xf4,0xf5,0xf6,0xf7,0xf8,0xf9,0xfa,0xfb,0xfc,
			 0xfd,0xfe,0xff,EOT},
			{RSVD,0xfd,RSVD,0xf7,0xcb,RSVD,0xff,0xff,0x00,MISC,MISC,
			 MISC,MISC,MISC,MISC,MISC,MISC,MISC,MISC,MISC,MISC,MISC,
			 MISC,MISC,MISC,EOT}},
		{EOT}}},

	/* ID and rev */
	{0x9771, "W83977F-A/G-A/AF-A/AG-A", {
		{EOT}}},
	{0x9777, "W83977AF", {
		/*
		 * W83977AF as found on the Advantech PCM-5820. We weren't able
		 * to find a datasheet (so far) which lists the 0x77 revision,
		 * but the hardware is there in the wild, so detect it...
		 */
		{EOT}}},
	{0x9773, "W83977TF", {
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x28,0x2a,0x2b,
			 0x2c,0x2d,0x2e,0x2f,EOT},
			{0x97,0x73,0xff,0xfe,MISC,0x00,MISC,0x00,0x00,0x00,
			 0x00,RSVD,RSVD,RSVD,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,0xf5,
			 EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x00,0xff,0x00,0x00,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard / mouse",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x7, "GPIO 1",
			{0x30,0x60,0x61,0x62,0x63,0x64,0x65,0x70,0x72,0xe0,
			 0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xf1,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
			 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,EOT}},
		{0x8, "GPIO 2",
			{0x30,0x60,0x61,0x70,0x72,0xe8,0xe9,0xea,0xeb,0xec,
			 0xed,0xee,0xf0,0xf1,0xf2,0xf3,0xf4,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x01,0x01,0x01,0x01,0x01,
			 0x01,0x01,0x00,RSVD,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 3",
			{0x30,0x60,0x61,0x62,0x63,0x64,0x65,0x70,0x72,0xe0,
			 0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xf1,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
			 0x01,0x01,0x01,0x01,0x01,0x01,0x01,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x60,0x61,0x62,0x63,0x64,0x65,0x70,0xe0,0xe1,
			 0xe2,0xe3,0xe4,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,
			 0xf7,0xfe,0xff,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 NANA,MISC,RSVD,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,RSVD,RSVD,EOT}},
		{EOT}}},
	{0x9774, "W83977ATF", {
		{EOT}}},

	{0xb07, "W83527HG", {	/* TODO: Not yet in sensors-detect */
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
			 0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0xB0,0x73,0xff,0x00,MISC,0x00,0x00,RSVD,0x50,0x00,
			 0x00,RSVD,0xe2,0x21,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x8, "WDTO#, PLED",
			{0x30,0xf5,0xf6,0xf7,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO3, GPIO5",
			{0x30,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,0xe9,
			 0xf0,0xf1,0xf2,0xf3,0xf8,0xf9,0xfa,0xfe,EOT},
			{0x00,0xff,0x00,0x00,0xff,0x00,0x00,0x00,0x00,0x00,
			 0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xe8,0xe9,0xf2,0xf3,0xf4,0xf6,0xf7,0xfe,EOT},
			{0x00,0x00,0x01,0x00,0xff,0x08,0x00,0x00,0x1c,0x00,
			 RSVD,RSVD,0x7c,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x00,0x00,0x00,0x81,RSVD,0x00,EOT}},
		{0xc, "PECI",
			{0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe8,0xe9,0xea,0xec,
			 0xfe,0xff,EOT},
			{0x00,0x48,0x48,0x48,0x48,0x00,0x00,RSVD,0x00,0x00,
			 0x00,0x00,EOT}},
		{EOT}}},
	/* ID only */
	{0x52, "W83627HF/F/HG/G", {
		{NOLDN, NULL,
			{0x02,0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x28,0x29,
			 0x2a,0x2b,0x2c,0x2e,0x2f,EOT},
			{0x00,0x52,NANA,0xff,0x00,MISC,0x00,0x00,0x00,0x00,
			 0x7c,0xc0,0x00,0x00,0x00,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,
			 0xf5,EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x00,0xff,0x00,
			 0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x80,EOT}},
		{0x6, "Consumer IR",
			{0x30,0x60,0x61,0x70,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{0x7, "Game port, MIDI port, GPIO 1",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,EOT},
			{0x00,0x02,0x01,0x03,0x30,0x09,0xff,0x00,0x00,EOT}},
		{0x8, "GPIO 2, watchdog timer",
			{0x30,0xf0,0xf1,0xf2,0xf3,0xf5,0xf6,0xf6,0xf7,EOT},
			{0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 3",
			{0x30,0xf0,0xf1,0xf2,0xf3,EOT},
			{0x00,0xff,0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xf0,0xf1,0xf3,0xf4,0xf6,0xf7,0xf9,0xfe,0xff,EOT},
			{0x00,0x00,0x00,0x00,NANA,NANA,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,EOT}},
		{EOT}}},
	{0x68, "W83697SF/UF/UG", {
		/* ID:  0x68 (for W83697SF/UF/UG)
		 * Rev: 0x1X (for W83697SF)
		 *      0x0X (for W83697SF) -- sic!
		 *      0x1X (for W83697UF/UG)
		 */
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x28,0x29,0x2a,
			 0x2b,0x2c,EOT},
			{0x68,NANA,0xef,0xfe,MISC,0x00,0x00,0x00,0x00,MISC,
			 0x00,0x30,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,0xf5,
			 EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x0e,0x00,0xff,0x00,0x00,
			 EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x03,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x7, "Game port, GPIO 1",
			{0x30,0x60,0x61,0x62,0x63,0xf0,0xf1,0xf2,EOT},
			{0x00,0x02,0x01,0x00,0x00,0xff,0x00,0x00,EOT}},
		{0x8, "MIDI port, GPIO 5",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 0xf4,0xf5,EOT},
			{0x00,0x03,0x30,0x00,0x00,0x09,0xff,0x00,0x00,0x00,
			 0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO 3, GPIO 4",
			{0x30,0x60,0x61,0xf0,0xf1,0xf2,0xf3,0xf4,0xf5,0xf6,
			 0xf7,0xf8,EOT},
			{0x00,0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,0xff,
			 0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xf0,0xf1,0xf2,0xf3,0xf4,0xf6,0xf7,0xf9,
			 0xfa,EOT},
			{0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,EOT}},
		{0xb, "PWM",
			{0x30,0x60,0x61,EOT},
			{0x00,0x00,0x00,EOT}},
		{0xc, "Smart card",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x00,0x00,0x00,0x00,0x00,EOT}},
		{0xd, "URC, GPIO 6",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 0xf4,EOT},
			{0x00,0x03,0xe8,0x00,0x00,0x00,0x00,0xff,0x00,0x00,
			 0x00,EOT}},
		{0xe, "URD, GPIO 7",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 EOT},
			{0x00,0x02,0xe8,0x00,0x00,0x00,0x00,0xff,0x00,0x00,
			 EOT}},
		{0xf, "GPIO 8",
			{0x30,0x60,0x61,0xf0,0xf1,0xf2,EOT},
			{0x00,0x00,0x00,0xff,0x00,0x00,EOT}},
		{EOT}}},
	{0x88, "W83627EHF/EF/EHG/EG", {
		/*
		 * As per datasheet the ID should be 0x886? here.
		 * Not mentioned in the datasheet, but sensors-detect says
		 * 0x8853 is also possible. Also, the ASUS A8V-E Deluxe
		 * (W83627EHF) has an ID of 0x8854 (verified on hardware).
		 * So we now assume all 0x88?? IDs to mean W83627EHF/EF/EHG/EG.
		 */
		{NOLDN, NULL,
			{0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,
			 0x29,0x2a,0x2b,0x2c,0x2d,0x2e,0x2f,EOT},
			{0x88,MISC,0xff,0x00,MISC,0x00,MISC,RSVD,0x50,
			 0x04,0x00,RSVD,0x00,0x21,0x00,0x00,EOT}},
		{0x0, "Floppy",
			{0x30,0x60,0x61,0x70,0x74,0xf0,0xf1,0xf2,0xf4,
			 0xf5,EOT},
			{0x01,0x03,0xf0,0x06,0x02,0x8e,0x00,0xff,0x00,
			 0x00,EOT}},
		{0x1, "Parallel port",
			{0x30,0x60,0x61,0x70,0x74,0xf0,EOT},
			{0x01,0x03,0x78,0x07,0x04,0x3f,EOT}},
		{0x2, "COM1",
			{0x30,0x60,0x61,0x70,0xf0,EOT},
			{0x01,0x03,0xf8,0x04,0x00,EOT}},
		{0x3, "COM2",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x01,0x02,0xf8,0x03,0x00,0x00,EOT}},
		{0x5, "Keyboard",
			{0x30,0x60,0x61,0x62,0x63,0x70,0x72,0xf0,EOT},
			{0x01,0x00,0x60,0x00,0x64,0x01,0x0c,0x83,EOT}},
		{0x6, "Serial flash interface",
			{0x30,0x62,0x63,EOT},
			{0x00,0x00,0x00,EOT}},
		{0x7, "GPIO 1, GPIO 6, game port, MIDI port",
			{0x30,0x60,0x61,0x62,0x63,0x70,0xf0,0xf1,0xf2,0xf3,
			 0xf4,0xf5,0xf6,0xf7,EOT},
			{0x00,0x02,0x01,0x03,0x30,0x09,0xff,0x00,0x00,0x00,
			 0xff,0x00,0x00,0x00,EOT}},
		{0x8, "WDTO#, PLED",
			{0x30,0xf5,0xf6,0xf7,EOT},
			{0x00,0x00,0x00,0x00,EOT}},
		{0x9, "GPIO 2, GPIO 3, GPIO 4, GPIO 5, SUSLED",
			{0x30,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xf0,0xf1,0xf2,
			 0xf3,0xf4,0xf5,0xf6,0xf7,EOT},
			{0x00,0xff,0x00,0x00,0xff,0x00,0x00,0xff,0x00,0x00,
			 0x00,0xff,0x00,0x00,0x00,EOT}},
		{0xa, "ACPI",
			{0x30,0x70,0xe0,0xe1,0xe2,0xe3,0xe4,0xe5,0xe6,0xe7,
			 0xe8,0xf2,0xf3,0xf4,0xf6,0xf7,EOT},
			{0x00,0x00,0x01,0x00,0xff,0x08,0x00,RSVD,0x00,0x00,
			 RSVD,0x7c,0x00,0x00,0x00,0x00,EOT}},
		{0xb, "Hardware monitor",
			{0x30,0x60,0x61,0x70,0xf0,0xf1,EOT},
			{0x00,0x00,0x00,0x00,0xc1,0x00,EOT}},
		{EOT}}},

	/* ID[3..0] */
	{0xa, "W83877F", {
		{EOT}}},
	{0xb, "W83877AF", {
		{NOLDN, NULL,
			{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
			 0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,
			 0x14,0x15,0x16,0x17,0x1e,0x20,0x21,0x22,0x23,0x24,
			 0x25,0x26,0x27,0x28,0x29,0x2a,0x2b,0x2c,0x2d,EOT},
			{0x00,0x00,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x0a,
			 0x1f,0x0c,0x28,0xa3,RSVD,RSVD,0x00,0x00,0x00,0x00,
			 0x00,0x00,0x0e,0x00,MISC,MISC,MISC,MISC,MISC,MISC,
			 MISC,MISC,MISC,MISC,MISC,MISC,MISC,MISC,MISC,EOT}},
		{EOT}}},
	{0xc, "W83877TF", {
		{EOT}}},
	{0xd, "W83877ATF/ATG", {
		{EOT}}},
	{EOT}
};

static const struct superio_registers hwm_table[] = {
	{0x828, "W83627THF/THG", {
		{NOLDN, NULL,
			{0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,
			 0x0a,0x0b,0x0c,0x0d,0x0e,0x0f,0x10,0x11,0x12,0x13,
			 0x14,0x15,0x16,0x17,0x18,0x19,0x1a,0x1b,0x1c,0x1d,
			 0x1e,0x1f,
			 0x20,0x21,0x22,0x23,0x24,0x25,0x26,0x27,0x28,0x29,
			 0x2b,0x2c,0x2d,0x2e,0x2f,0x30,0x31,0x32,0x33,0x34,
			 0x35,0x36,0x37,0x38,0x39,0x3a,0x3b,0x3c,0x3d,0x3e,
			 0x3f,
			 0x40,0x41,0x42,0x43,0x44,0x45,0x46,0x47,0x48,0x49,
			 0x4a,0x4b,0x4c,0x4d,0x4e,0x4f,EOT},
			{RSVD,0xff,RSVD,0xff,0x00,0x00,0x00,0x00,0x01,0x01,
			 0x01,0x01,0x3c,0x3c,0x0a,0x0a,RSVD,0xff,0x00,0x00,
			 0x00,0x01,0x01,0x3c,0x43,RSVD,0xff,0xff,RSVD,RSVD,
			 NANA,NANA,
			 NANA,NANA,NANA,NANA,NANA,RSVD,RSVD,NANA,NANA,NANA,
			 NANA,NANA,NANA,NANA,NANA,NANA,NANA,NANA,NANA,NANA,
			 RSVD,RSVD,RSVD,RSVD,NANA,NANA,NANA,NANA,NANA,RSVD,
			 RSVD,
			 0x03,0x00,0x00,0xfe,0xff,RSVD,RSVD,0x5f,NANA,0x03,
			 RSVD,0x44,0x18,0x15,0x80,0x5c,EOT}},
		{0x0, "Bank 0",
			{0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,0x58,0x59,
			 0x5d,0x5e,0x5f,EOT},
			{NANA,NANA,NANA,NANA,NANA,NANA,0x00,0x80,0x90,0x70,
			 0x00,RSVD,RSVD,EOT}},
		{0x1, "Bank 1",
			{0x50,0x51,0x52,0x53,0x54,0x55,0x56,EOT},
			{NANA,NANA,0x00,0x4b,0x00,0x50,0x00,EOT}},
		{0x2, "Bank 2",
			{0x50,0x51,0x52,0x53,0x54,0x55,0x56,EOT},
			{NANA,NANA,0x00,0x4b,0x00,0x50,0x00,EOT}},
		{0x4, "Bank 4",
			{0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x59,0x5a,
			 0x5b,EOT},
			{0x00,0xff,RSVD,0x00,0x00,0x00,0x00,0x00,0x00,
			 0x00,EOT}},
		{0x5, "Bank 5",
			{0x50,0x51,0x52,0x53,0x54,0x55,0x56,0x57,EOT},
			{NANA,NANA,RSVD,RSVD,NANA,NANA,NANA,NANA,EOT}},
		{0x6, "Bank 6",
			{0x50,EOT},
			{RSVD,EOT}},
		{EOT}}},
	{EOT}
};

static void enter_conf_mode_winbond_88(uint16_t port)
{
	OUTB(0x88, port);
}

static void enter_conf_mode_winbond_89(uint16_t port)
{
	OUTB(0x89, port);
}

static void enter_conf_mode_winbond_86(uint16_t port)
{
	OUTB(0x86, port);
	OUTB(0x86, port);
}

static int chip_found_at_port;

static void probe_idregs_winbond_helper(const char *init, uint16_t port)
{
	uint16_t id, hwmport;
	uint8_t devid, rev, olddevid;

	probing_for("Winbond", init, port);

	devid = regval(port, DEVICE_ID_REG);
	rev = regval(port, DEVICE_REV_REG);
	olddevid = regval(port, DEVICE_ID_REG_OLD) & 0x0f;

	if (devid == 0x52 && (rev & 0xf0) != 0xf0)
		id = devid;				 /* ID only */
	else if (devid == 0x68 || devid == 0x88)
		id = devid;				 /* ID only */
	else if ((devid == 0x97) && ((rev & 0xf0) == 0x70))
		id = (devid << 8) | rev;		 /* ID and rev */
	else
		id = (devid << 4) | ((rev & 0xf0) >> 4); /* ID and rev[7..4] */

	if (olddevid >= 0x0a && olddevid <= 0x0d)
		id = olddevid & 0x0f;			 /* ID[3..0] */

	if (superio_unknown(reg_table, id)) {
		if (verbose)
			printf(NOTFOUND "id/oldid=0x%02x/0x%02x, rev=0x%02x\n",
			       devid, olddevid, rev);
		return;
	}

	if (olddevid >= 0x0a && olddevid <= 0x0d)
		printf("Found Winbond %s (id=0x%02x) at 0x%x\n",
		       get_superio_name(reg_table, id), olddevid, port);
	else
		printf("Found Winbond %s (id=0x%02x, rev=0x%02x) at 0x%x\n",
		       get_superio_name(reg_table, id), devid, rev, port);
	chip_found = 1;
	chip_found_at_port = 1;

	dump_superio("Winbond", reg_table, port, id, LDN_SEL);

	if (extra_dump) {
		regwrite(port, LDN_SEL, 0x0b); /* Select LDN 0xb (HWM). */

		if ((regval(port, 0x30) & (1 << 0)) != (1 << 0)) {
			printf("Hardware Monitor disabled or does not exist.\n");
			return;
		}

		/* Get HWM base address (stored in LDN 0xb, index 0x60/0x61). */
		hwmport = regval(port, 0x60) << 8;
		hwmport |= regval(port, 0x61);

		/* HWM address register = HWM base address + 5. */
		hwmport += 5;

		printf("Hardware monitor (0x%04x)\n", hwmport);
		dump_superio("Winbond-HWM", hwm_table, hwmport, id,
			     WINBOND_HWM_SEL);
	}
}

void probe_idregs_winbond(uint16_t port)
{
	chip_found_at_port = 0;

	enter_conf_mode_winbond_88(port);
	probe_idregs_winbond_helper("(init=0x88) ", port);
	exit_conf_mode_winbond_fintek_ite_8787(port);
	if (chip_found_at_port)
		return;

	enter_conf_mode_winbond_89(port);
	probe_idregs_winbond_helper("(init=0x89) ", port);
	exit_conf_mode_winbond_fintek_ite_8787(port);
	if (chip_found_at_port)
		return;

	enter_conf_mode_winbond_86(port);
	probe_idregs_winbond_helper("(init=0x86,0x86) ", port);
	exit_conf_mode_winbond_fintek_ite_8787(port);
	if (chip_found_at_port)
		return;

	enter_conf_mode_winbond_fintek_ite_8787(port);
	probe_idregs_winbond_helper("(init=0x87,0x87) ", port);
	exit_conf_mode_winbond_fintek_ite_8787(port);
	if (chip_found_at_port)
		return;
}

void print_winbond_chips(void)
{
	print_vendor_chips("Winbond", reg_table);
	print_vendor_chips("Winbond-HWM", hwm_table);
}
