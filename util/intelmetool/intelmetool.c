/* intelmetool  Dump interesting things about Management Engine even if hidden
 * Copyright (C) 2014  Damien Zammit <damien@zamaudio.com>
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
#include <inttypes.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif

#include "me.h"
#include "mmap.h"
#include "intelmetool.h"

#define FD2 0x3428
#define ME_COMMAND_DELAY 10000

extern int fd_mem;
int debug = 0;

static uint32_t fd2 = 0;
static const int size = 0x4000;
static volatile uint8_t *rcba;

static void dumpmem(uint8_t *phys, uint32_t size)
{
	uint32_t i;
	printf("Dumping cloned ME memory:\n");
	for (i = 0; i < size; i++) {
		printf("%02X",*((uint8_t *) (phys + i)));
	}
	printf("\n");
}

static void zeroit(uint8_t *phys, uint32_t size)
{
	uint32_t i;
	for (i = 0; i < size; i++) {
		*((uint8_t *) (phys + i)) = 0x00;
	}
}

static void dumpmemfile(uint8_t *phys, uint32_t size)
{
	FILE *fp = fopen("medump.bin", "w");
	uint32_t i;
	for (i = 0; i < size; i++) {
		fprintf(fp, "%c", *((uint8_t *) (phys + i)));
	}
	fclose(fp);
}

static void rehide_me() {
	if (fd2 & 0x2) {
		printf("Re-hiding MEI device...");
		fd2 = *(uint32_t *)(rcba + FD2);
		*(uint32_t *)(rcba + FD2) = fd2 | 0x2;
		printf("done\n");
	}
}

/* You need >4GB total ram, in kernel cmdline, use 'mem=1000m'
 * then this code will clone to absolute memory address 0xe0000000
 * which can be read using a mmap tool at that offset.
 * Real ME memory is located around top of memory minus 64MB. (I think)
 * so we avoid cloning to this part.
 */
static void dump_me_memory() {
	uint32_t me_clone = 0x60000000;
	uint8_t *dump;

	dump = map_physical_exact((off_t)me_clone, (void *)me_clone, 0x2000000);
	zeroit(dump, 0x2000000);
	printf("Send magic command for memory clone\n");

	mei_reset();
	usleep(ME_COMMAND_DELAY);
	void* ptr = &me_clone;
	int err = mkhi_debug_me_memory(ptr);

	if (!err) {
		printf("Wait a second...");
		usleep(ME_COMMAND_DELAY);
		printf("done\n\nHere are the first bytes:\n");
		dumpmemfile(dump, 0x2000000);
		//printf("Try reading 0x%zx with other mmap tool...\n"
		//	"Press enter to quit, you only get one chance to run this tool before reboot required for some reason\n", me_clone);
		while (getc(stdin) != '\n') {};
		unmap_physical(dump, 0x2000000);
	}
}

static int pci_platform_scan() {
	struct pci_access *pacc;
	struct pci_dev *dev;
	char namebuf[1024];
	const char *name;

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;

	pci_init(pacc);
	pci_scan_bus(pacc);

	for (dev=pacc->devices; dev; dev=dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_SIZES | PCI_FILL_CLASS);
		name = pci_lookup_name(pacc, namebuf, sizeof(namebuf),
			PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
		if (dev->vendor_id == 0x8086) {
			if (PCI_DEV_HAS_ME_DISABLE(dev->device_id)) {
				printf(CGRN "Good news, you have a `%s` so ME is present but can be disabled, continuing...\n\n" RESET, name);
				break;
			} else if (PCI_DEV_HAS_ME_DIFFICULT(dev->device_id)) {
				printf(CRED "Bad news, you have a `%s` so you have ME hardware on board and you can't control or disable it, continuing...\n\n" RESET, name);
				break;
			} else if (PCI_DEV_CAN_DISABLE_ME_IF_PRESENT(dev->device_id)) {
				printf(CYEL "Not sure if ME hardware is present because you have a `%s`, but it is possible to disable it if you do, continuing...\n\n" RESET, name);
				break;
			} else if (PCI_DEV_ME_NOT_SURE(dev->device_id)) {
				printf(CYEL "Found `%s`. Not sure whether you have ME hardware, exiting\n\n" RESET, name);
				pci_cleanup(pacc);
				return 1;
				break;
			}
		}
	}

	if (dev != NULL &&
	!PCI_DEV_HAS_ME_DISABLE(dev->device_id) &&
	!PCI_DEV_HAS_ME_DIFFICULT(dev->device_id) &&
	!PCI_DEV_CAN_DISABLE_ME_IF_PRESENT(dev->device_id) &&
	!PCI_DEV_ME_NOT_SURE(dev->device_id)) {
		printf(CCYN "ME is not present on your board or unkown\n\n" RESET);
		pci_cleanup(pacc);
		return 1;
	}

	pci_cleanup(pacc);

	return 0;
}

static struct pci_dev *pci_me_interface_scan(const char **name, char *namebuf, int namebuf_size) {
	struct pci_access *pacc;
	struct pci_dev *dev;
	int me = 0;

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;

	pci_init(pacc);
	pci_scan_bus(pacc);

	for (dev=pacc->devices; dev; dev=dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_SIZES | PCI_FILL_CLASS);
		*name = pci_lookup_name(pacc, namebuf, namebuf_size,
			PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
		if (dev->vendor_id == 0x8086) {
			if (PCI_DEV_HAS_SUPPORTED_ME(dev->device_id)) {
				me = 1;
				break;
			}
		}
	}

	if (!me) {
		rehide_me();

		printf("MEI device not found\n");
		pci_cleanup(pacc);
		return NULL;
	}

	return dev;
}

static int activate_me() {
	struct pci_access *pacc;
	struct pci_dev *sb;
	uint32_t rcba_phys;

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;

	pci_init(pacc);
	pci_scan_bus(pacc);

	sb = pci_get_dev(pacc, 0, 0, 0x1f, 0);
	if (!sb) {
		printf("Uh oh, southbridge not on BDF(0:31:0), please report this error, exiting.\n");
		pci_cleanup(pacc);
		return 1;
	}
	pci_fill_info(sb, PCI_FILL_IDENT | PCI_FILL_BASES | PCI_FILL_SIZES | PCI_FILL_CLASS);

	rcba_phys = pci_read_long(sb, 0xf0) & 0xfffffffe;
	rcba = map_physical((off_t)rcba_phys, size);

	//printf("RCBA at 0x%08" PRIx32 "\n", (uint32_t)rcba_phys);
	fd2 = *(uint32_t *)(rcba + FD2);
	*(uint32_t *)(rcba + FD2) = fd2 & ~0x2;
	if (fd2 & 0x2) {
		printf("MEI was hidden on PCI, now unlocked\n");
	} else {
		printf("MEI not hidden on PCI, checking if visible\n");
	}

	pci_cleanup(pacc);

	return 0;
}

static void dump_me_info() {
	struct pci_dev *dev;
	uint32_t stat, stat2;
	char namebuf[1024];
	const char *name;

	if (pci_platform_scan()) {
		exit(1);
	}

	if (activate_me()) {
		exit(1);
	}

	dev = pci_me_interface_scan(&name, namebuf, sizeof(namebuf));
	if (!dev) {
		exit(1);
	}

	printf("MEI found: [%x:%x] %s\n\n", dev->vendor_id, dev->device_id, name);
	stat = pci_read_long(dev, 0x40);
	printf("ME Status   : 0x%x\n", stat);
	stat2 = pci_read_long(dev, 0x48);
	printf("ME Status 2 : 0x%x\n\n", stat2);

	intel_me_status(stat, stat2);
	printf("\n");
	intel_me_extend_valid(dev);
	printf("\n");

	if ((stat & 0xf000) >> 12 != 0) {
		printf("ME: has a broken implementation on your board with this BIOS\n");
	}

	intel_mei_setup(dev);
	usleep(ME_COMMAND_DELAY);
	mei_reset();
	usleep(ME_COMMAND_DELAY);
	mkhi_get_fw_version();
	usleep(ME_COMMAND_DELAY);
	mei_reset();
	usleep(ME_COMMAND_DELAY);
	mkhi_get_fwcaps();
	usleep(ME_COMMAND_DELAY);

	rehide_me();

	munmap((void*)rcba, size);
}

static void print_version(void)
{
	printf("intelmetool v%s -- ", INTELMETOOL_VERSION);
	printf("Copyright (C) 2015 Damien Zammit\n\n");
	printf(
		"This program is free software: you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation, version 2 of the License.\n\n"
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU General Public License for more details.\n\n");
}

static void print_usage(const char *name)
{
	printf("usage: %s [-vh?sd]\n", name);
	printf("\n"
			 "   -v | --version:                   print the version\n"
			 "   -h | --help:                      print this help\n\n"
			 "   -s | --show:                      dump all me information on console\n"
			 "   -d | --debug:                     enable debug output\n"
			 "\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int opt, option_index = 0;
	unsigned cmd_exec = 0;

	static struct option long_options[] = {
		{"version", 0, 0, 'v'},
		{"help", 0, 0, 'h'},
		{"show", 0, 0, 's'},
		{"debug", 0, 0, 'd'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?sd",
	                                long_options, &option_index)) != EOF) {
	switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 's':
			cmd_exec = 1;
			break;
		case 'd':
			debug = 1;
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

	switch(cmd_exec) {
		case 1:
			dump_me_info();
			break;
		default:
			print_usage(argv[0]);
			break;
	}

	return 0;
}
