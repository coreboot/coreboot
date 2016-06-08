/*
 * inteltool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 *  written by Stefan Reinauer <stepan@coresystems.de>
 * Copyright (C) 2009 Carl-Daniel Hailfinger
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

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "inteltool.h"

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif

/*
 * http://pci-ids.ucw.cz/read/PC/8086
 * http://en.wikipedia.org/wiki/Intel_Tick-Tock
 * http://en.wikipedia.org/wiki/List_of_Intel_chipsets
 * http://en.wikipedia.org/wiki/Intel_Xeon_chipsets
 */
static const struct {
	uint16_t vendor_id, device_id;
	char *name;
} supported_chips_list[] = {
	/* Host bridges/DRAM controllers (Northbridges) */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443LX, "443LX" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443BX, "443BX" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443BX_NO_AGP, "443BX without AGP" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810, "810" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810_DC, "810-DC100" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810E_DC, "810E DC-133" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82830M, "830M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82845, "845" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82865, "865" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82915, "915G/P/GV/GL/PL/910GL" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945P, "945P" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GM, "945GM" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GSE, "945GSE" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82946, "946GZ/PL" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82965PM, "965PM" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q965, "Q963/82Q965" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82975X, "975X" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G33, "P35/G33/G31/P31" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q33, "Q33" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q35, "Q35" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82X38, "X38/X48" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_32X0, "3200/3210" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82XX4X, "GL40/GS40/GM45/GS45/PM45" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q45, "Q45/Q43" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G45, "G45/G43/P45/P43" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G41, "G41" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82B43, "B43 (Base)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82B43_2, "B43 (Soft)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82X58, "X58" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000P, "Intel i5000P Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000X, "Intel i5000X Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000Z, "Intel i5000Z Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I5000V, "Intel i5000V Memory Controller Hub" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SCH_POULSBO, "SCH Poulsbo" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ATOM_DXXX, "Atom D400/500 Series" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ATOM_NXXX, "Atom N400 Series" },
	/* Host bridges /DRAM controllers integrated in CPUs */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_0TH_GEN, "0th generation (Nehalem family) Core Processor" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_1ST_GEN, "1st generation (Westmere family) Core Processor" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_D, "2nd generation (Sandy Bridge family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_M, "2nd generation (Sandy Bridge family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_2ND_GEN_E3, "2nd generation (Sandy Bridge family) Core Processor (Xeon E3)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_D, "3rd generation (Ivy Bridge family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_M, "3rd generation (Ivy Bridge family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_E3, "3rd generation (Ivy Bridge family) Core Processor (Xeon E3 v2)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_3RD_GEN_015c, "3rd generation (Ivy Bridge family) Core Processor" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_D, "4th generation (Haswell family) Core Processor (Desktop)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_M, "4th generation (Haswell family) Core Processor (Mobile)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_E3, "4th generation (Haswell family) Core Processor (Xeon E3 v3)" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_4TH_GEN_U, "4th generation (Haswell family) Core Processor ULT" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_CORE_5TH_GEN_U, "5th generation (Broadwell family) Core Processor ULT" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL, "Bay Trail" },
	/* Southbridges (LPC controllers) */
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82371XX, "371AB/EB/MB" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10R, "ICH10R" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9DH, "ICH9DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9DO, "ICH9DO" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9R, "ICH9R" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9, "ICH9" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9M, "ICH9M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9ME, "ICH9M-E" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8M, "ICH8-M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8ME, "ICH8M-E" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8, "ICH8" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_NM10, "NM10" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7MDH, "ICH7-M DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7M, "ICH7-M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7, "ICH7" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH7DH, "ICH7DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH6, "ICH6" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH5, "ICH5" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH4M, "ICH4-M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH4, "ICH4" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH2, "ICH2" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH0, "ICH0" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH, "ICH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_I63XX, "631xESB/632xESB/3100" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC, "SCH Poulsbo" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400_DESKTOP, "3400 Desktop" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400_MOBILE, "3400 Mobile" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_P55, "P55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PM55, "PM55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H55, "H55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM57, "QM57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H57, "H57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM55, "HM55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q57, "Q57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM57, "HM57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400_MOBILE_SFF, "3400 Mobile SFF" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B55_A, "B55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QS57, "QS57" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3400, "3400" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3420, "3420" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_3450, "3450" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B55_B, "B55" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z68, "Z68" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_P67, "P67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UM67, "UM67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM65, "HM65" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H67, "H67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM67, "HM67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q65, "Q65" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QS67, "QS67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q67, "Q67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM67, "QM67" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B65, "B65" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C202, "C202" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C204, "C204" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C206, "C206" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H61, "H61" },
	{ PCI_VENDOR_ID_INTEL, 0x1d40, "X79" },
	{ PCI_VENDOR_ID_INTEL, 0x1d41, "X79" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z77, "Z77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Z75, "Z75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q77, "Q77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q75, "Q75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_B75, "B75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_H77, "H77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_C216, "C216" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QM77, "QM77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_QS77, "QS77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM77, "HM77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_UM77, "UM77" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM76, "HM76" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM75, "HM75" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_HM70, "HM70" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_NM70, "NM70" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL, "Lynx Point Low Power Full Featured Engineering Sample" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM, "Lynx Point Low Power Premium SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE, "Lynx Point Low Power Base SKU" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP, "Wildcat Point Low Power SKU" },
	{ PCI_VENDOR_ID_INTEL, 0x2310, "DH89xxCC" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_BAYTRAIL_LPC, "Bay Trail" },
};

#ifndef __DARWIN__
static int fd_mem;

void *map_physical(uint64_t phys_addr, size_t len)
{
	void *virt_addr;

	virt_addr = mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) phys_addr);

	if (virt_addr == MAP_FAILED) {
		printf("Error mapping physical memory 0x%08" PRIx64 "[0x%zx]\n",
			phys_addr, len);
		return NULL;
	}

	return virt_addr;
}

void unmap_physical(void *virt_addr, size_t len)
{
	munmap(virt_addr, len);
}
#endif

void print_version(void)
{
	printf("inteltool v%s -- ", INTELTOOL_VERSION);
	printf("Copyright (C) 2008 coresystems GmbH\n\n");
	printf(
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, version 2 of the License.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n");
}

void print_usage(const char *name)
{
	printf("usage: %s [-vh?gGrpmedPMaAsfSR]\n", name);
	printf("\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n\n"
	     "   -s | --spi:                       dump southbridge spi and bios_cntrl registers\n"
	     "   -f | --gfx:                       dump graphics registers (UNSAFE: may hang system!)\n"
	     "   -R | --ahci:                      dump AHCI registers\n"
	     "   -g | --gpio:                      dump southbridge GPIO registers\n"
	     "   -G | --gpio-diffs:                show GPIO differences from defaults\n"
	     "   -r | --rcba:                      dump southbridge RCBA registers\n"
	     "   -p | --pmbase:                    dump southbridge Power Management registers\n\n"
	     "   -m | --mchbar:                    dump northbridge Memory Controller registers\n"
	     "   -S FILE | --spd=FILE:             create a file storing current timings (implies -m)\n"
	     "   -e | --epbar:                     dump northbridge EPBAR registers\n"
	     "   -d | --dmibar:                    dump northbridge DMIBAR registers\n"
	     "   -P | --pciexpress:                dump northbridge PCIEXBAR registers\n\n"
	     "   -M | --msrs:                      dump CPU MSRs\n"
	     "   -A | --ambs:                      dump AMB registers\n"
	     "   -a | --all:                       dump all known (safe) registers\n"
	     "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	struct pci_access *pacc;
	struct pci_dev *sb = NULL, *nb, *gfx = NULL, *ahci = NULL, *dev;
	const char *dump_spd_file = NULL;
	int i, opt, option_index = 0;
	unsigned int id;

	char *sbname = "unknown", *nbname = "unknown", *gfxname = "unknown";

	int dump_gpios = 0, dump_mchbar = 0, dump_rcba = 0;
	int dump_pmbase = 0, dump_epbar = 0, dump_dmibar = 0;
	int dump_pciexbar = 0, dump_coremsrs = 0, dump_ambs = 0;
	int dump_spi = 0, dump_gfx = 0, dump_ahci = 0;
	int show_gpio_diffs = 0;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"gpios", 0, 0, 'g'},
		{"gpio-diffs", 0, 0, 'G'},
		{"mchbar", 0, 0, 'm'},
		{"rcba", 0, 0, 'r'},
		{"pmbase", 0, 0, 'p'},
		{"epbar", 0, 0, 'e'},
		{"dmibar", 0, 0, 'd'},
		{"pciexpress", 0, 0, 'P'},
		{"msrs", 0, 0, 'M'},
		{"ambs", 0, 0, 'A'},
		{"spi", 0, 0, 's'},
		{"spd", 0, 0, 'S'},
		{"all", 0, 0, 'a'},
		{"gfx", 0, 0, 'f'},
		{"ahci", 0, 0, 'R'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?gGrpmedPMaAsfRS:",
                                  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'S':
			dump_spd_file = optarg;
			dump_mchbar = 1;
			break;
		case 'g':
			dump_gpios = 1;
			break;
		case 'f':
			dump_gfx = 1;
			break;
		case 'R':
			dump_ahci = 1;
			break;
		case 'G':
			show_gpio_diffs = 1;
			break;
		case 'm':
			dump_mchbar = 1;
			break;
		case 'r':
			dump_rcba = 1;
			break;
		case 'p':
			dump_pmbase = 1;
			break;
		case 'e':
			dump_epbar = 1;
			break;
		case 'd':
			dump_dmibar = 1;
			break;
		case 'P':
			dump_pciexbar = 1;
			break;
		case 'M':
			dump_coremsrs = 1;
			break;
		case 'a':
			dump_gpios = 1;
			show_gpio_diffs = 1;
			dump_mchbar = 1;
			dump_rcba = 1;
			dump_pmbase = 1;
			dump_epbar = 1;
			dump_dmibar = 1;
			dump_pciexbar = 1;
			dump_coremsrs = 1;
			dump_ambs = 1;
			dump_spi = 1;
			dump_ahci = 1;
			break;
		case 'A':
			dump_ambs = 1;
			break;
		case 's':
			dump_spi = 1;
			break;
		case 'h':
		case '?':
		default:
			print_usage(argv[0]);
			exit(0);
			break;
		}
	}

#if defined(__FreeBSD__)
	if (open("/dev/io", O_RDWR) < 0) {
		perror("/dev/io");
#elif defined(__NetBSD__)
# ifdef __i386__
	if (i386_iopl(3)) {
		perror("iopl");
# else
	if (x86_64_iopl(3)) {
		perror("iopl");
# endif
#else
	if (iopl(3)) {
		perror("iopl");
#endif
		printf("You need to be root.\n");
		exit(1);
	}

#ifndef __DARWIN__
	if ((fd_mem = open("/dev/mem", O_RDWR)) < 0) {
		perror("Can not open /dev/mem");
		exit(1);
	}
#endif

	pacc = pci_alloc();
	pci_init(pacc);
	pci_scan_bus(pacc);

	/* Find the required devices */
	for (dev = pacc->devices; dev; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_CLASS);
		/* The ISA/LPC bridge can be 0x1f, 0x07, or 0x04 so we probe. */
		if (dev->device_class == 0x0601) { /* ISA/LPC bridge */
			if (sb == NULL) {
				sb = dev;
			} else {
				fprintf(stderr, "Multiple devices with class ID"
					" 0x0601, using %02x%02x:%02x.%02x\n",
					sb->domain, sb->bus, sb->dev, sb->func);
				break;
			}
		}
	}

	if (!sb) {
		printf("No southbridge found.\n");
		exit(1);
	}

	pci_fill_info(sb, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

	if (sb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) southbridge.\n");
		exit(1);
	}

	nb = pci_get_dev(pacc, 0, 0, 0x00, 0);
	if (!nb) {
		printf("No northbridge found.\n");
		exit(1);
	}

	pci_fill_info(nb, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

	if (nb->vendor_id != PCI_VENDOR_ID_INTEL) {
		printf("Not an Intel(R) northbridge.\n");
		exit(1);
	}

	gfx = pci_get_dev(pacc, 0, 0, 0x02, 0);

	if (gfx) {
		pci_fill_info(gfx, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

		if (gfx->vendor_id != PCI_VENDOR_ID_INTEL)
			gfx = 0;
	}

	ahci = pci_get_dev(pacc, 0, 0, 0x1f, 2);

	if (ahci) {
		pci_fill_info(ahci, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

		if (ahci->vendor_id != PCI_VENDOR_ID_INTEL)
			ahci = 0;
	}

	id = cpuid(1);

	/* Intel has suggested applications to display the family of a CPU as
	 * the sum of the "Family" and the "Extended Family" fields shown
	 * above, and the model as the sum of the "Model" and the 4-bit
	 * left-shifted "Extended Model" fields.
	 * http://download.intel.com/design/processor/applnots/24161832.pdf
	 */
	printf("CPU: ID 0x%x, Processor Type 0x%x, Family 0x%x, Model 0x%x, Stepping 0x%x\n",
			id, (id >> 12) & 0x3, ((id >> 8) & 0xf) + ((id >> 20) & 0xff),
			((id >> 12) & 0xf0) + ((id >> 4) & 0xf), (id & 0xf));

	/* Determine names */
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
		if (nb->device_id == supported_chips_list[i].device_id)
			nbname = supported_chips_list[i].name;
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
		if (sb->device_id == supported_chips_list[i].device_id)
			sbname = supported_chips_list[i].name;
	if (gfx) {
		for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
			if (gfx->device_id == supported_chips_list[i].device_id)
				gfxname = supported_chips_list[i].name;
	}

	printf("Northbridge: %04x:%04x (%s)\n",
		nb->vendor_id, nb->device_id, nbname);

	printf("Southbridge: %04x:%04x (%s)\n",
		sb->vendor_id, sb->device_id, sbname);

	if (gfx) {
		printf("IGD: %04x:%04x (%s)\n",
		       gfx->vendor_id, gfx->device_id, gfxname);
	}

	/* Now do the deed */

	if (dump_gpios) {
		print_gpios(sb, 1, show_gpio_diffs);
		printf("\n\n");
	} else if (show_gpio_diffs) {
		print_gpios(sb, 0, show_gpio_diffs);
		printf("\n\n");
	}

	if (dump_rcba) {
		print_rcba(sb);
		printf("\n\n");
	}

	if (dump_pmbase) {
		print_pmbase(sb, pacc);
		printf("\n\n");
	}

	if (dump_mchbar) {
		print_mchbar(nb, pacc, dump_spd_file);
		printf("\n\n");
	}

	if (dump_epbar) {
		print_epbar(nb);
		printf("\n\n");
	}

	if (dump_dmibar) {
		print_dmibar(nb);
		printf("\n\n");
	}

	if (dump_pciexbar) {
		print_pciexbar(nb);
		printf("\n\n");
	}

	if (dump_coremsrs) {
		print_intel_core_msrs();
		printf("\n\n");
	}

	if (dump_ambs) {
		print_ambs(nb, pacc);
	}

	if (dump_spi) {
		print_spi(sb);
	}

	if (dump_gfx) {
		print_gfx(gfx);
	}

	if (dump_ahci) {
		print_ahci(ahci);
	}

	/* Clean up */
	pci_free_dev(nb);
	// pci_free_dev(sb); // TODO: glibc detected "double free or corruption"
	pci_cleanup(pacc);

	return 0;
}
