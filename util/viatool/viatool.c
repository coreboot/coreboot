/*
 * viatool - dump all registers on an Intel CPU + chipset based system.
 *
 * Copyright (C) 2008-2010 by coresystems GmbH
 *  written by Stefan Reinauer <stepan@coresystems.de>
 * Copyright (C) 2009 Carl-Daniel Hailfinger
 * Copyright (C) 2013 Alexandru Gagniuc
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
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include "viatool.h"

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
	{ PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX900_SATA, "VX900 SATA"},
	/* Host bridges/DRAM controllers (Northbridges) */
	{ PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX900, "VX900"},
	/* Southbridges (LPC controllers) */
	{ PCI_VENDOR_ID_VIA, PCI_DEVICE_ID_VIA_VX900_LPC, "VX900" },
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
	printf("inteltool v%s -- ", VIATOOL_VERSION);
	printf("Copyright (C) 2013 Alexandru Gagniuc\n\n");
	printf(
    "This program is free software: you can redistribute it and/or modify\n"
    "it under the terms of the GNU General Public License as published by\n"
    "the Free Software Foundation, version 2 of the License.\n\n"
    "This program is distributed in the hope that it will be useful,\n"
    "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
    "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

void print_usage(const char *name)
{
	printf("usage: %s [-vh?gGrpmedPMa]\n", name);
	printf("\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n\n"
	     "   -M | --msrs:                      dump CPU MSRs\n"
	     "   -a | --all:                       dump all known registers\n"
	     "   -q | --quirks:                    dump hierarchical configs\n"
	     "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	struct pci_access *pacc;
	struct pci_dev *sb = NULL, *nb, *dev;
	int i, opt, option_index = 0;
	unsigned int id;

	char *sbname = "unknown", *nbname = "unknown";

	int dump_coremsrs = 0, dump_quirks = 0;

	static struct option long_options[] = {
		{"version",	0, 0, 'v'},
		{"help",	0, 0, 'h'},
		{"mchbar",	0, 0, 'm'},
		{"msrs",	0, 0, 'M'},
		{"quirks",	0, 0, 'q'},
		{"all",		0, 0, 'a'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?gGrpmedPMaA",
                                  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'M':
			dump_coremsrs = 1;
			break;
		case 'q':
			dump_quirks = 1;
			break;
		case 'a':
			dump_coremsrs = 1;
			dump_quirks = 1;
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
			if (sb == NULL)
				sb = dev;
			else
				fprintf(stderr, "Multiple devices with class ID"
					" 0x0601, using %02x%02x:%02x.%02x\n",
					dev->domain, dev->bus, dev->dev,
					dev->func);
		}
	}

	if (!sb) {
		printf("No southbridge found.\n");
		exit(1);
	}

	pci_fill_info(sb, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

	if (sb->vendor_id != PCI_VENDOR_ID_VIA) {
		printf("Not a VIA southbridge.\n");
		exit(1);
	}

	nb = pci_get_dev(pacc, 0, 0, 0x00, 0);
	if (!nb) {
		printf("No northbridge found.\n");
		exit(1);
	}

	pci_fill_info(nb, PCI_FILL_IDENT|PCI_FILL_BASES|PCI_FILL_SIZES|PCI_FILL_CLASS);

	if (nb->vendor_id != PCI_VENDOR_ID_VIA) {
		printf("Not a VIA northbridge.\n");
		exit(1);
	}

	id = cpuid(1);

	/* Intel has suggested applications to display the family of a CPU as
	 * the sum of the "Family" and the "Extended Family" fields shown
	 * above, and the model as the sum of the "Model" and the 4-bit
	 * left-shifted "Extended Model" fields.
	 * http://download.intel.com/design/processor/applnots/24161832.pdf
	 */
	printf("CPU: Processor Type: %x, Family %x, Model %x, Stepping %x\n",
			(id >> 12) & 0x3, ((id >> 8) & 0xf) + ((id >> 20) & 0xff),
			((id >> 12) & 0xf0) + ((id >> 4) & 0xf), (id & 0xf));

	/* Determine names */
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
		if (nb->device_id == supported_chips_list[i].device_id)
			nbname = supported_chips_list[i].name;
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
		if (sb->device_id == supported_chips_list[i].device_id)
			sbname = supported_chips_list[i].name;

	printf("Northbridge: %04x:%04x (%s)\n",
		nb->vendor_id, nb->device_id, nbname);

	printf("Southbridge: %04x:%04x (%s)\n",
		sb->vendor_id, sb->device_id, sbname);

	/* Now do the deed */

	if (dump_coremsrs) {
		print_intel_core_msrs();
		printf("\n\n");
	}

	if (dump_quirks) {
		print_quirks_north(nb, pacc);
		print_quirks_south(sb, pacc);
	}

	/* Clean up */
	pci_free_dev(nb);
	// pci_free_dev(sb); // TODO: glibc detected "double free or corruption"
	pci_cleanup(pacc);

	return 0;
}
