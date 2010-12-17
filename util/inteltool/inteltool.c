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
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include "inteltool.h"
#if defined(__FreeBSD__)
#include <unistd.h>
#endif

static const struct {
	uint16_t vendor_id, device_id;
	char *name;
} supported_chips_list[] = {
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443LX, "82443LX" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443BX, "82443BX" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82443BX_NO_AGP, "82443BX without AGP" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810, "i810" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810DC, "i810-DC100" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82810E_MC, "i810E DC-133" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82830M, "i830M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82845, "i845" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82865, "i865" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82915, "82915G/P/GV/GL/PL/910GL" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945P, "i945P" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GM, "i945GM" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82945GSE, "i945GSE" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_PM965, "PM965" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_Q965, "Q963/965" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82975X, "i975X" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q35, "Q35" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82G33, "P35/G33/G31/P31" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82Q33, "Q33" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_X58, "X58" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_GS45, "GS45ME" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ATOM_DXXX, "Atom D400/500 Series" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ATOM_NXXX, "Atom N400 Series" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SCH_POULSBO, "SCH Poulsbo" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_SCH_POULSBO_LPC, "SCH Poulsbo" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH10R, "ICH10R" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9DH, "ICH9DH" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9DO, "ICH9DO" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9R, "ICH9R" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9, "ICH9" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9M, "ICH9M" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH9ME, "ICH9M-E" },
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_ICH8M, "ICH8-M" },
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
	{ PCI_VENDOR_ID_INTEL, PCI_DEVICE_ID_INTEL_82371XX, "82371AB/EB/MB" },
};

#ifndef __DARWIN__
static int fd_mem;

void *map_physical(unsigned long phys_addr, size_t len)
{
	void *virt_addr;

	virt_addr = mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED,
		    fd_mem, (off_t) phys_addr);

	if (virt_addr == MAP_FAILED) {
		printf("Error mapping physical memory 0x%08lx[0x%x]\n", phys_addr, len);
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
    "GNU General Public License for more details.\n\n"
    "You should have received a copy of the GNU General Public License\n"
    "along with this program.  If not, see <http://www.gnu.org/licenses/>.\n\n");
}

void print_usage(const char *name)
{
	printf("usage: %s [-vh?grpmedPMa]\n", name);
	printf("\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n\n"
	     "   -g | --gpio:                      dump soutbridge GPIO registers\n"
	     "   -r | --rcba:                      dump soutbridge RCBA registers\n"
	     "   -p | --pmbase:                    dump soutbridge Power Management registers\n\n"
	     "   -m | --mchbar:                    dump northbridge Memory Controller registers\n"
	     "   -e | --epbar:                     dump northbridge EPBAR registers\n"
	     "   -d | --dmibar:                    dump northbridge DMIBAR registers\n"
	     "   -P | --pciexpress:                dump northbridge PCIEXBAR registers\n\n"
	     "   -M | --msrs:                      dump CPU MSRs\n"
	     "   -a | --all:                       dump all known registers\n"
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

	int dump_gpios = 0, dump_mchbar = 0, dump_rcba = 0;
	int dump_pmbase = 0, dump_epbar = 0, dump_dmibar = 0;
	int dump_pciexbar = 0, dump_coremsrs = 0;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"gpios", 0, 0, 'g'},
		{"mchbar", 0, 0, 'm'},
		{"rcba", 0, 0, 'r'},
		{"pmbase", 0, 0, 'p'},
		{"epbar", 0, 0, 'e'},
		{"dmibar", 0, 0, 'd'},
		{"pciexpress", 0, 0, 'P'},
		{"msrs", 0, 0, 'M'},
		{"all", 0, 0, 'a'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?grpmedPMa",
                                  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'g':
			dump_gpios = 1;
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
			dump_mchbar = 1;
			dump_rcba = 1;
			dump_pmbase = 1;
			dump_epbar = 1;
			dump_dmibar = 1;
			dump_pciexbar = 1;
			dump_coremsrs = 1;
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
	int io_fd;
#endif

#if defined(__FreeBSD__)
	if ((io_fd = open("/dev/io", O_RDWR)) < 0) {
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

	id = cpuid(1);

	/* Intel has suggested applications to display the family of a CPU as
	 * the sum of the "Family" and the "Extended Family" fields shown
	 * above, and the model as the sum of the "Model" and the 4-bit
	 * left-shifted "Extended Model" fields.
	 * http://download.intel.com/design/processor/applnots/24161832.pdf
	 */
	printf("Intel CPU: Processor Type: %x, Family %x, Model %x, Stepping %x\n",
			(id >> 12) & 0x3, ((id >> 8) & 0xf) + ((id >> 20) & 0xff),
			((id >> 12) & 0xf0) + ((id >> 4) & 0xf), (id & 0xf));

	/* Determine names */
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
		if (nb->device_id == supported_chips_list[i].device_id)
			nbname = supported_chips_list[i].name;
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
		if (sb->device_id == supported_chips_list[i].device_id)
			sbname = supported_chips_list[i].name;

	printf("Intel Northbridge: %04x:%04x (%s)\n",
		nb->vendor_id, nb->device_id, nbname);

	printf("Intel Southbridge: %04x:%04x (%s)\n",
		sb->vendor_id, sb->device_id, sbname);

	/* Now do the deed */

	if (dump_gpios) {
		print_gpios(sb);
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
		print_mchbar(nb, pacc);
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

	/* Clean up */
	pci_free_dev(nb);
	// pci_free_dev(sb); // TODO: glibc detected "double free or corruption"
	pci_cleanup(pacc);

	return 0;
}
