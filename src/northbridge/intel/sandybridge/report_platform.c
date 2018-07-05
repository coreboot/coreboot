/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2012 Google Inc.
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

#include <console/console.h>
#include <arch/cpu.h>
#include <string.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <arch/io.h>
#include "sandybridge.h"

static void report_cpu_info(void)
{
	struct cpuid_result cpuidr;
	u32 i, index;
	char cpu_string[50], *cpu_name = cpu_string; /* 48 bytes are reported */
	int vt, txt, aes;
	const char *mode[] = {"NOT ", ""};

	index = 0x80000000;
	cpuidr = cpuid(index);
	if (cpuidr.eax < 0x80000004) {
		strcpy(cpu_string, "Platform info not available");
	} else {
		u32 *p = (u32*) cpu_string;
		for (i = 2; i <= 4; i++) {
			cpuidr = cpuid(index + i);
			*p++ = cpuidr.eax;
			*p++ = cpuidr.ebx;
			*p++ = cpuidr.ecx;
			*p++ = cpuidr.edx;
		}
	}
	/* Skip leading spaces in CPU name string */
	while (cpu_name[0] == ' ')
		cpu_name++;

	cpuidr = cpuid(1);
	printk(BIOS_DEBUG, "CPU id(%x): %s\n", cpuidr.eax, cpu_name);
	aes = (cpuidr.ecx & (1 << 25)) ? 1 : 0;
	txt = (cpuidr.ecx & (1 << 6)) ? 1 : 0;
	vt = (cpuidr.ecx & (1 << 5)) ? 1 : 0;
	printk(BIOS_DEBUG, "AES %ssupported, TXT %ssupported, VT %ssupported\n",
	       mode[aes], mode[txt], mode[vt]);
}

static struct {
	u16 dev_id;
	const char *dev_name;
} pch_table [] = {
	/* 6-series PCI ids from
	 * Intel® 6 Series Chipset and
	 * Intel® C200 Series Chipset
	 * Specification Update - NDA
	 * October 2013
	 * CDI / IBP#: 440377
	 */
	{0x1C41, "SFF Sample"},
	{0x1C42, "Desktop Sample"},
	{0x1C43, "Mobile Sample"},
	{0x1C44, "Z68"},
	{0x1C46, "P67"},
	{0x1C47, "UM67"},
	{0x1C49, "HM65"},
	{0x1C4A, "H67"},
	{0x1C4B, "HM67"},
	{0x1C4C, "Q65"},
	{0x1C4D, "QS67"},
	{0x1C4E, "Q67"},
	{0x1C4F, "QM67"},
	{0x1C50, "B65"},
	{0x1C52, "C202"},
	{0x1C54, "C204"},
	{0x1C56, "C206"},
	{0x1C5C, "H61"},
	/* 7-series PCI ids from Intel document 472178 */
	{0x1E41, "Desktop Sample"},
	{0x1E42, "Mobile Sample"},
	{0x1E43, "SFF Sample"},
	{0x1E44, "Z77"},
	{0x1E45, "H71"},
	{0x1E46, "Z75"},
	{0x1E47, "Q77"},
	{0x1E48, "Q75"},
	{0x1E49, "B75"},
	{0x1E4A, "H77"},
	{0x1E53, "C216"},
	{0x1E55, "QM77"},
	{0x1E56, "QS77"},
	{0x1E58, "UM77"},
	{0x1E57, "HM77"},
	{0x1E59, "HM76"},
	{0x1E5D, "HM75"},
	{0x1E5E, "HM70"},
	{0x1E5F, "NM70"},
};

static void report_pch_info(void)
{
	int i;
	u16 dev_id = pci_read_config16(PCH_LPC_DEV, 2);


	const char *pch_type = "Unknown";
	for (i = 0; i < ARRAY_SIZE(pch_table); i++) {
		if (pch_table[i].dev_id == dev_id) {
			pch_type = pch_table[i].dev_name;
			break;
		}
	}
	printk (BIOS_DEBUG, "PCH type: %s, device id: %x, rev id %x\n",
		pch_type, dev_id, pci_read_config8(PCH_LPC_DEV, 8));
}

void report_platform_info(void)
{
	report_cpu_info();
	report_pch_info();
}
