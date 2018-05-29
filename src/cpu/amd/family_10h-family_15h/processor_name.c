/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2007 Advanced Micro Devices, Inc.
 * Copyright (C) 2008 Peter Stuge
 * Copyright (C) 2010 Marc Jones <marcj303@gmail.com>
 * Copyright (C) 2015 Timothy Pearson <tpearson@raptorengineeringinc.com>, Raptor Engineering
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

/*
 * This code sets the Processor Name String for AMD64 CPUs.
 *
 * Revision Guide for AMD Family 10h Processors
 * Publication # 41322 Revision: 3.17 Issue Date: February 2008
 */

#include <console/console.h>
#include <string.h>
#include <cpu/x86/msr.h>
#include <cpu/amd/mtrr.h>
#include <cpu/cpu.h>
#include <cpu/amd/model_10xxx_rev.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pnp.h>
#include <device/pci_ops.h>

/* The maximum length of CPU names is 48 bytes, including the final NULL byte.
 * If you change these names your BIOS will _NOT_ pass the AMD validation and
 * your mainboard will not be posted on the AMD Recommended Motherboard Website
 */

struct str_s {
	u8 Pg;
	u8 NC;
	u8 String;
	char const *value;
};


static const struct str_s String1_socket_F[] = {
	{0x00, 0x01, 0x00, "Dual-Core AMD Opteron(tm) Processor 83"},
	{0x00, 0x01, 0x01, "Dual-Core AMD Opteron(tm) Processor 23"},
	{0x00, 0x03, 0x00, "Quad-Core AMD Opteron(tm) Processor 83"},
	{0x00, 0x03, 0x01, "Quad-Core AMD Opteron(tm) Processor 23"},
	{0x00, 0x05, 0x00, "Six-Core AMD Opteron(tm) Processor 84"},
	{0x00, 0x05, 0x01, "Six-Core AMD Opteron(tm) Processor 24"},
	{0x00, 0x03, 0x02, "Embedded AMD Opteron(tm) Processor 83"},
	{0x00, 0x03, 0x03, "Embedded AMD Opteron(tm) Processor 23"},
	{0x00, 0x03, 0x04, "Embedded AMD Opteron(tm) Processor 13"},
	{0x00, 0x03, 0x05, "AMD Phenom(tm) FX-"},
	{0x01, 0x01, 0x01, "Embedded AMD Opteron(tm) Processor"},
	{0, 0, 0, NULL}
};

static const struct str_s String2_socket_F[] = {
	{0x00, 0xFF, 0x02, " EE"},
	{0x00, 0xFF, 0x0A, " SE"},
	{0x00, 0xFF, 0x0B, " HE"},
	{0x00, 0xFF, 0x0C, " EE"},
	{0x00, 0xFF, 0x0D, " Quad-Core Processor"},
	{0x00, 0xFF, 0x0F, ""},
	{0x01, 0x01, 0x01, "GF HE"},
	{0, 0, 0, NULL}
};


static const struct str_s String1_socket_AM2[] = {
	{0x00, 0x00, 0x00, "AMD Athlon(tm) Processor LE-"},
	{0x00, 0x00, 0x01, "AMD Sempron(tm) Processor LE-"},
	{0x00, 0x00, 0x02, "AMD Sempron(tm) 1"},
	{0x00, 0x00, 0x03, "AMD Athlon(tm) II 1"},
	{0x00, 0x01, 0x00, "Dual-Core AMD Opteron(tm) Processor 13"},
	{0x00, 0x01, 0x01, "AMD Athlon(tm)"},
	{0x00, 0x01, 0x03, "AMD Athlon(tm) II X2 2"},
	{0x00, 0x01, 0x04, "AMD Athlon(tm) II X2 B"},
	{0x00, 0x01, 0x05, "AMD Athlon(tm) II X2"},
	{0x00, 0x01, 0x07, "AMD Phenom(tm) II X2 5"},
	{0x00, 0x01, 0x0A, "AMD Phenom(tm) II X2"},
	{0x00, 0x01, 0x0B, "AMD Phenom(tm) II X2 B"},
	{0x00, 0x02, 0x00, "AMD Phenom(tm)"},
	{0x00, 0x02, 0x03, "AMD Phenom(tm) II X3 B"},
	{0x00, 0x02, 0x04, "AMD Phenom(tm) II X3"},
	{0x00, 0x02, 0x07, "AMD Athlon(tm) II X3 4"},
	{0x00, 0x02, 0x08, "AMD Phenom(tm) II X3 7"},
	{0x00, 0x02, 0x0A, "AMD Athlon(tm) II X3"},
	{0x00, 0x03, 0x00, "Quad-Core AMD Opteron(tm) Processor 13"},
	{0x00, 0x03, 0x01, "AMD Phenom(tm) FX-"},
	{0x00, 0x03, 0x02, "AMD Phenom(tm)"},
	{0x00, 0x03, 0x03, "AMD Phenom(tm) II X4 9"},
	{0x00, 0x03, 0x04, "AMD Phenom(tm) II X4 8"},
	{0x00, 0x03, 0x07, "AMD Phenom(tm) II X4 B"},
	{0x00, 0x03, 0x08, "AMD Phenom(tm) II X4"},
	{0x00, 0x03, 0x0A, "AMD Athlon(tm) II X4 6"},
	{0x00, 0x03, 0x0F, "AMD Athlon(tm) II X4"},
	{0, 0, 0, NULL}
};

static const struct str_s String2_socket_AM2[] = {
	{0x00, 0x00, 0x00, "00"},
	{0x00, 0x00, 0x01, "10"},
	{0x00, 0x00, 0x02, "20"},
	{0x00, 0x00, 0x03, "30"},
	{0x00, 0x00, 0x04, "40"},
	{0x00, 0x00, 0x05, "50"},
	{0x00, 0x00, 0x06, "60"},
	{0x00, 0x00, 0x07, "70"},
	{0x00, 0x00, 0x08, "80"},
	{0x00, 0x00, 0x09, "90"},
	{0x00, 0x00, 0x09, " Processor"},
	{0x00, 0x00, 0x09, "u Processor"},
	{0x00, 0x01, 0x00, "00 Dual-Core Processor"},
	{0x00, 0x01, 0x01, "00e Dual-Core Processor"},
	{0x00, 0x01, 0x02, "00B Dual-Core Processor"},
	{0x00, 0x01, 0x03, "50 Dual-Core Processor"},
	{0x00, 0x01, 0x04, "50e Dual-Core Processor"},
	{0x00, 0x01, 0x05, "50B Dual-Core Processor"},
	{0x00, 0x01, 0x06, " Processor"},
	{0x00, 0x01, 0x07, "e Processor"},
	{0x00, 0x01, 0x09, "0 Processor"},
	{0x00, 0x01, 0x0A, "0e Processor"},
	{0x00, 0x01, 0x0B, "u Processor"},
	{0x00, 0x02, 0x00, "00 Triple-Core Processor"},
	{0x00, 0x02, 0x01, "00e Triple-Core Processor"},
	{0x00, 0x02, 0x02, "00B Triple-Core Processor"},
	{0x00, 0x02, 0x03, "50 Triple-Core Processor"},
	{0x00, 0x02, 0x04, "50e Triple-Core Processor"},
	{0x00, 0x02, 0x05, "50B Triple-Core Processor"},
	{0x00, 0x02, 0x06, " Processor"},
	{0x00, 0x02, 0x07, "e Processor"},
	{0x00, 0x02, 0x09, "0e Processor"},
	{0x00, 0x02, 0x0A, "0 Processor"},
	{0x00, 0x03, 0x00, "00 Quad-Core Processor"},
	{0x00, 0x03, 0x01, "00e Quad-Core Processor"},
	{0x00, 0x03, 0x02, "00B Quad-Core Processor"},
	{0x00, 0x03, 0x03, "50 Quad-Core Processor"},
	{0x00, 0x03, 0x04, "50e Quad-Core Processor"},
	{0x00, 0x03, 0x05, "50B Quad-Core Processor"},
	{0x00, 0x03, 0x06, " Processor"},
	{0x00, 0x03, 0x07, "e Processor"},
	{0x00, 0x03, 0x09, "0e Processor"},
	{0x00, 0x03, 0x0A, " SE"},
	{0x00, 0x03, 0x0B, " HE"},
	{0x00, 0x03, 0x0C, " EE"},
	{0x00, 0x03, 0x0D, " Quad-Core Processor"},
	{0x00, 0x03, 0x0E, "0 Processor"},
	{0x00, 0xFF, 0x0F, ""},
	{0, 0, 0, NULL}
};

static const struct str_s String1_socket_G34[] = {
	{0x00, 0x07, 0x00, "AMD Opteron(tm) Processor 61"},
	{0x00, 0x0B, 0x00, "AMD Opteron(tm) Processor 61"},
	{0x01, 0x07, 0x01, "Embedded AMD Opteron(tm) Processor "},
	{0, 0, 0, NULL}
};

static const struct str_s String2_socket_G34[] = {
	{0x00, 0x07, 0x00, " HE"},
	{0x00, 0x07, 0x01, " SE"},
	{0x00, 0x0B, 0x00, " HE"},
	{0x00, 0x0B, 0x01, " SE"},
	{0x00, 0x0B, 0x0F, ""},
	{0x01, 0x07, 0x01, " QS"},
	{0x01, 0x07, 0x02, " KS"},
	{0, 0, 0, NULL}
};

static const struct str_s String1_socket_C32[] = {
	{0x00, 0x03, 0x00, "AMD Opteron(tm) Processor 41"},
	{0x00, 0x05, 0x00, "AMD Opteron(tm) Processor 41"},
	{0x01, 0x03, 0x01, "Embedded AMD Opteron(tm) Processor "},
	{0x01, 0x05, 0x01, "Embedded AMD Opteron(tm) Processor "},
	{0, 0, 0, NULL}
};

static const struct str_s String2_socket_C32[] = {
	{0x00, 0x03, 0x00, " HE"},
	{0x00, 0x03, 0x01, " EE"},
	{0x00, 0x05, 0x00, " HE"},
	{0x00, 0x05, 0x01, " EE"},
	{0x01, 0x03, 0x01, "QS HE"},
	{0x01, 0x03, 0x02, "LE HE"},
	{0x01, 0x05, 0x01, "KX HE"},
	{0x01, 0x05, 0x02, "GL EE"},
	{0, 0, 0, NULL}
};

const char *unknown = "AMD Processor model unknown";
const char *unknown2 = " type unknown";
const char *sample = "AMD Engineering Sample";
const char *thermal = "AMD Thermal Test Kit";


static int strcpymax(char *dst, const char *src, int buflen)
{
	int i;
	for (i = 0; i < buflen && src[i]; i++)
		dst[i] = src[i];
	if (i >= buflen)
		i--;
	dst[i] = 0;
	return i;
}

#define NAME_STRING_MAXLEN 48

int init_processor_name(void)
{
	msr_t msr;
	ssize_t i;
	char program_string[NAME_STRING_MAXLEN];
	u32 *p_program_string = (u32 *)program_string;
	uint8_t fam15h = 0;
	uint32_t family;

	family = cpuid_eax(0x80000001);
	family = ((family & 0xf00000) >> 16) | ((family & 0xf00) >> 8);

	if (family >= 0x6f)
		/* Family 15h or later */
		fam15h = 1;

	/* null the string */
	memset(program_string, 0, sizeof(program_string));

	if (fam15h) {
		/* Family 15h or later */
		uint32_t dword;
		struct device *cpu_fn5_dev = dev_find_slot(0, PCI_DEVFN(0x18, 5));
		pci_write_config32(cpu_fn5_dev, 0x194, 0);
		dword = pci_read_config32(cpu_fn5_dev, 0x198);
		if (dword == 0) {
			strcpymax(program_string, sample, sizeof(program_string));
		} else {
			/* Assemble the string from PCI configuration register contents */
			for (i = 0; i < 12; i++) {
				pci_write_config32(cpu_fn5_dev, 0x194, i);
				p_program_string[i] = pci_read_config32(cpu_fn5_dev, 0x198);
			}

			/* Correctly place the null terminator */
			for (i = (NAME_STRING_MAXLEN - 2); i > 0; i--) {
				if (program_string[i] != 0x20)
					break;
			}
			program_string[i + 1] = 0;
		}
	} else {
		/* variable names taken from fam10 revision guide for clarity */
		u32 BrandId;	/* CPUID Fn8000_0001_EBX */
		u8 String1;	/* BrandID[14:11] */
		u8 String2;	/* BrandID[3:0] */
		u8 Model;	/* BrandID[10:4] */
		u8 Pg;		/* BrandID[15] */
		u8 PkgTyp;	/* BrandID[31:28] */
		u8 NC;		/* CPUID Fn8000_0008_ECX */
		const char *processor_name_string = unknown;
		int j = 0, str2_checkNC = 1;
		const struct str_s *str, *str2;

		/* Find out which CPU brand it is */
		BrandId = cpuid_ebx(0x80000001);
		String1 = (u8)((BrandId >> 11) & 0x0F);
		String2 = (u8)((BrandId >> 0) & 0x0F);
		Model = (u8)((BrandId >> 4) & 0x7F);
		Pg = (u8)((BrandId >> 15) & 0x01);
		PkgTyp = (u8)((BrandId >> 28) & 0x0F);
		NC = (u8)(cpuid_ecx(0x80000008) & 0xFF);

		if (!Model) {
			processor_name_string = Pg ? thermal : sample;
			goto done;
		}

		switch (PkgTyp) {
		case 0:		/* F1207 */
			str = String1_socket_F;
			str2 = String2_socket_F;
			str2_checkNC = 0;
			break;
		case 1:		/* AM2 */
			str = String1_socket_AM2;
			str2 = String2_socket_AM2;
			break;
		case 3:		/* G34 */
			str = String1_socket_G34;
			str2 = String2_socket_G34;
			str2_checkNC = 0;
			break;
		case 5:		/* C32 */
			str = String1_socket_C32;
			str2 = String2_socket_C32;
			break;
		default:
			goto done;
		}

		/* String1 */
		for (i = 0; str[i].value; i++) {
			if ((str[i].Pg == Pg) &&
			(str[i].NC == NC) &&
			(str[i].String == String1)) {
				processor_name_string = str[i].value;
				break;
			}
		}

		if (!str[i].value)
			goto done;

		j = strcpymax(program_string, processor_name_string,
			sizeof(program_string));

		/* Translate Model from 01-99 to ASCII and put it on the end.
		* Numbers less than 10 should include a leading zero, e.g., 09.*/
		if (Model < 100 && j < sizeof(program_string) - 2) {
			program_string[j++] = (Model / 10) + '0';
			program_string[j++] = (Model % 10) + '0';
		}

		processor_name_string = unknown2;

		/* String 2 */
		for (i = 0; str2[i].value; i++) {
			if ((str2[i].Pg == Pg) &&
			((str2[i].NC == NC) || !str2_checkNC) &&
			(str2[i].String == String2)) {
				processor_name_string = str2[i].value;
				break;
			}
		}

done:
		strcpymax(&program_string[j], processor_name_string,
			sizeof(program_string) - j);
	}

	printk(BIOS_DEBUG, "CPU model: %s\n", program_string);

	for (i = 0; i < 6; i++) {
		msr.lo = p_program_string[(2 * i) + 0];
		msr.hi = p_program_string[(2 * i) + 1];
		wrmsr_amd(0xC0010030 + i, msr);
	}

	return 0;
}
