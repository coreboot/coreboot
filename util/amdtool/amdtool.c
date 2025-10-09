/* amdtool - dump all registers on an AMD CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <getopt.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

#include "acpimmio.h"
#include "amdtool.h"
#include "smn.h"

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif

static const struct {
	uint16_t vendor_id, device_id;
	char *name;
} supported_chips_list[] = {
	/* Host bridges/DRAM controllers (Northbridges) */
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_ROOT_COMPLEX, "Turin Root Complex" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_0, "Turin Data Fabric 0" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_1, "Turin Data Fabric 1" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_2, "Turin Data Fabric 2" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_3, "Turin Data Fabric 3" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_4, "Turin Data Fabric 4" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_5, "Turin Data Fabric 5" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_6, "Turin Data Fabric 6" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_BRH_DATA_FABRIC_7, "Turin Data Fabric 7" },

	/* FCHs (Southbridges) */
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_1, "FCH SMBus Controller" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_LPC_1, "FCH LPC Bridge" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_SMB_2, "FCH SMBus Controller" },
	{ PCI_VENDOR_ID_AMD, PCI_DEVICE_ID_AMD_FCH_LPC_2, "FCH LPC Bridge" },
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

static struct pci_access *pacc;

static struct pci_dev *pci_dev_find(uint16_t vendor, uint16_t device)
{
	struct pci_dev *temp;
	struct pci_filter filter;

	pci_filter_init(NULL, &filter);
	filter.vendor = vendor;
	filter.device = device;

	for (temp = pacc->devices; temp; temp = temp->next)
		if (pci_filter_match(&filter, temp))
			return temp;

	return NULL;
}

int find_smbus_dev_rev(uint16_t vendor, uint16_t device)
{
	struct pci_dev *smbus_dev = pci_dev_find(vendor, device);
	if (!smbus_dev) {
		printf("No SMBus device with ID %04X:%04X found.\n", vendor, device);
		perror("ERROR: SMBus device not found.\n");
		return -1;
	}
	return pci_read_byte(smbus_dev, PCI_REVISION_ID);
}

static void print_version(void)
{
	printf("amdtool v%s -- ", AMDTOOL_VERSION);
	printf("Copyright (C) 2024 3mdeb\n\n");
	printf("This program is free software: you can redistribute it and/or modify\n"
	       "it under the terms of the GNU General Public License as published by\n"
	       "the Free Software Foundation, version 2 of the License.\n\n"
	       "This program is distributed in the hope that it will be useful,\n"
	       "but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	       "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	       "GNU General Public License for more details.\n\n");
}

static void print_usage(const char *name)
{
	printf("usage: %s [-vh?gicspGlMAa]\n", name);
	printf("\n"
	     "   -v | --version:                   print the version\n"
	     "   -h | --help:                      print this help\n\n"
	     "   -s | --spi:                       dump southbridge spi and bios_cntrl registers\n"
	     "   -g | --gpio:                      dump southbridge GPIO registers\n"
	     "   -G | --gpio-diffs:                show GPIO differences from defaults\n"
	     "   -i | --irq-routing                dump IRQ routing registers\n"
	     "   -l | --lpc:                       dump southbridge LPC/eSPI Interface registers\n\n"
	     "   -c | --cpu:                       dump CPU information and features\n\n"
	     "   -M | --msrs:                      dump CPU MSRs\n"
	     "   -A | --acpimmio:                  dump southbridge ACPI MMIO registers\n"
	     "   -p | --psb:                       dump Platform Secure Boot state\n"
	     "   -a | --all:                       dump all known (safe) registers\n"
	     "\n");
	exit(1);
}

static void print_system_info(struct pci_dev *nb, struct pci_dev *sb,
			      struct pci_dev *smb, struct pci_dev *gfx)
{
	unsigned int id, i;
	char *sbname = "unknown", *nbname = "unknown", *gfxname = "unknown", *smbname = "unknown";

	id = cpuid(1);

	/* Determine names */
	for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++) {
		if (nb->device_id == supported_chips_list[i].device_id)
			nbname = supported_chips_list[i].name;
		if (sb->device_id == supported_chips_list[i].device_id)
			sbname = supported_chips_list[i].name;
		if (smb->device_id == supported_chips_list[i].device_id)
			smbname = supported_chips_list[i].name;
	}
	if (gfx) {
		for (i = 0; i < ARRAY_SIZE(supported_chips_list); i++)
			if (gfx->device_id == supported_chips_list[i].device_id)
				gfxname = supported_chips_list[i].name;
	}
	printf("CPU: ID 0x%x, Processor Type 0x%x, Family 0x%x, Model 0x%x, Stepping 0x%x\n",
			id, (id >> 12) & 0x3, ((id >> 8) & 0xf) + ((id >> 20) & 0xff),
			((id >> 12) & 0xf0) + ((id >> 4) & 0xf), (id & 0xf));

	printf("Northbridge: %04x:%04x (%s)\n",
		nb->vendor_id, nb->device_id, nbname);

	printf("Southbridge SMBus: %04x:%04x rev %02x (%s)\n",
		smb->vendor_id, smb->device_id, pci_read_byte(smb, PCI_REVISION_ID), smbname);

	printf("Southbridge LPC: %04x:%04x rev %02x (%s)\n",
		sb->vendor_id, sb->device_id, pci_read_byte(sb, PCI_REVISION_ID), sbname);

	if (gfx)
		printf("Integrated Graphics: %04x:%04x (%s)\n",
		       gfx->vendor_id, gfx->device_id, gfxname);
}

int main(int argc, char *argv[])
{
	struct pci_dev *sb = NULL, *nb, *gfx = NULL, *smb = NULL, *dev;
	int opt, option_index = 0;

	int dump_gpios = 0, dump_coremsrs = 0, dump_acpimmio = 0, dump_cpu = 0;
	int dump_spi = 0, dump_lpc = 0, show_gpio_diffs = 0, dump_psb = 0, dump_irq = 0;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"gpios", 0, 0, 'g'},
		{"gpio-diffs", 0, 0, 'G'},
		{"irq-routing", 0, 0, 'i'},
		{"lpc", 0, 0, 'l'},
		{"cpu", 0, 0, 'c'},
		{"msrs", 0, 0, 'M'},
		{"acpimmio", 0, 0, 'A'},
		{"psb", 0, 0, 'p'},
		{"spi", 0, 0, 's'},
		{"all", 0, 0, 'a'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?gGilcMApsa",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 'g':
			dump_gpios = 1;
			break;
		case 'G':
			show_gpio_diffs = 1;
			break;
		case 'i':
			dump_irq = 1;
			break;
		case 'l':
			dump_lpc = 1;
			break;
		case 'c':
			dump_cpu = 1;
			break;
		case 'M':
			dump_coremsrs = 1;
			break;
		case 'A':
			dump_acpimmio = 1;
			break;
		case 'p':
			dump_psb = 1;
			break;
		case 's':
			dump_spi = 1;
			break;
		case 'a':
			dump_gpios = 1;
			show_gpio_diffs = 1;
			dump_irq = 1;
			dump_lpc = 1;
			dump_cpu = 1;
			dump_coremsrs = 1;
			dump_acpimmio = 1;
			dump_spi = 1;
			dump_psb = 1;
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
	pacc->method = PCI_ACCESS_I386_TYPE1;
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

	pci_fill_info(sb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if (sb->vendor_id != PCI_VENDOR_ID_AMD) {
		printf("Not an AMD southbridge.\n");
		exit(1);
	}

	nb = pci_get_dev(pacc, 0, 0, 0x00, 0);
	if (!nb) {
		printf("No northbridge found.\n");
		exit(1);
	}

	pci_fill_info(nb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if (nb->vendor_id != PCI_VENDOR_ID_AMD) {
		printf("Not an AMD northbridge.\n");
		exit(1);
	}

	smb = pci_get_dev(pacc, 0, 0, 0x14, 0);
	if (!smb) {
		printf("No SMBus Controller found.\n");
		exit(1);
	}

	pci_fill_info(smb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_CLASS);

	if (smb->vendor_id != PCI_VENDOR_ID_AMD) {
		printf("Not an AMD southbridge.\n");
		exit(1);
	}

	gfx = pci_get_dev(pacc, 0, 0, 0x02, 0);
	if (gfx) {
		pci_fill_info(gfx, PCI_FILL_IDENT | PCI_FILL_BASES |
				   PCI_FILL_CLASS);
		if ((gfx->device_class & 0xff00) != 0x0300)
			gfx = NULL;
		else if (gfx->vendor_id != PCI_VENDOR_ID_AMD)
			gfx = NULL;
	}

	print_system_info(nb, sb, smb, gfx);

	init_smn(nb);

	/* Now do the deed */

	if (dump_lpc) {
		print_lpc(sb);
		printf("\n\n");
		print_espi(sb);
		printf("\n\n");
	}

	if (dump_cpu) {
		print_cpu_info();
		printf("\n\n");
	}

	if (dump_coremsrs) {
		print_amd_msrs();
		printf("\n\n");
	}

	if (dump_acpimmio) {
		print_acpimmio(sb);
		printf("\n\n");
	}

	if (dump_gpios) {
		print_gpios(sb, 1, show_gpio_diffs);
		printf("\n\n");
	} else if (show_gpio_diffs) {
		print_gpios(sb, 0, show_gpio_diffs);
		printf("\n\n");
	}

	if (dump_spi) {
		print_spi(sb);
		printf("\n\n");
	}

	if (dump_irq) {
		print_irq_routing(sb);
		printf("\n\n");
	}

	if (dump_psb) {
		print_psb(nb);
		printf("\n\n");
	}

	/* Clean up */
	acpimmio_cleanup();
	pci_free_dev(nb);
	/* `sb` wasn't allocated by pci_get_dev() */
	pci_cleanup(pacc);

	return 0;
}
