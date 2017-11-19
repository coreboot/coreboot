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
#include "rcba.h"

#define FD2 0x3428
#define ME_COMMAND_DELAY 10000

extern int fd_mem;
int debug = 0;

static uint32_t fd2 = 0;

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

/* You need >4GB total ram, in kernel cmdline, use 'mem=1000m'
 * then this code will clone to absolute memory address 0xe0000000
 * which can be read using a mmap tool at that offset.
 * Real ME memory is located around top of memory minus 64MB. (I think)
 * so we avoid cloning to this part.
 */
static void dump_me_memory(void)
{
	uintptr_t me_clone = 0x60000000;
	uint8_t *dump;

	dump = map_physical_exact((off_t)me_clone, (void *)me_clone, 0x2000000);
	if (dump == NULL) {
		printf("Could not map ME memory\n");
		return;
	}
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
		//       "Press enter to quit, you only get one chance to run"
		//       "this tool before reboot required for some reason\n",
		//       me_clone);
		while (getc(stdin) != '\n') {};
		unmap_physical(dump, 0x2000000);
	}
}

static int pci_platform_scan(void)
{
	struct pci_access *pacc;
	struct pci_dev *dev;
	char namebuf[1024];
	const char *name;

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;

	pci_init(pacc);
	pci_scan_bus(pacc);

	for (dev=pacc->devices; dev; dev=dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES |
				   PCI_FILL_SIZES | PCI_FILL_CLASS);
		name = pci_lookup_name(pacc, namebuf, sizeof(namebuf),
			PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
		if (name == NULL)
			name = "<unknown>";
		if (dev->vendor_id != PCI_VENDOR_ID_INTEL)
			continue;

		if (PCI_DEV_HAS_ME_DISABLE(dev->device_id)) {
			printf(CGRN "Good news, you have a `%s` so ME is "
			       "present but can be disabled, continuing...\n\n"
			       RESET, name);
			break;
		} else if (PCI_DEV_HAS_ME_DIFFICULT(dev->device_id)) {
			printf(CRED "Bad news, you have a `%s` so you have ME "
			       "hardware on board and you can't control or "
			       "disable it, continuing...\n\n" RESET, name);
			break;
		} else if (PCI_DEV_CAN_DISABLE_ME_IF_PRESENT(dev->device_id)) {
			printf(CYEL "Not sure if ME hardware is present "
			       "because you have a `%s`, but it is possible to "
			       "disable it if you do, continuing...\n\n" RESET,
			       name);
			break;
		} else if (PCI_DEV_ME_NOT_SURE(dev->device_id)) {
			printf(CYEL "Found `%s`. Not sure whether you have ME "
			       "hardware, exiting\n\n" RESET, name);
			pci_cleanup(pacc);
			return 1;
		}
	}

	if (dev != NULL &&
	    !PCI_DEV_HAS_ME_DISABLE(dev->device_id) &&
	    !PCI_DEV_HAS_ME_DIFFICULT(dev->device_id) &&
	    !PCI_DEV_CAN_DISABLE_ME_IF_PRESENT(dev->device_id) &&
	    !PCI_DEV_ME_NOT_SURE(dev->device_id)) {
		printf(CCYN "ME is not present on your board or unknown\n\n"
		       RESET);
		pci_cleanup(pacc);
		return 1;
	}

	pci_cleanup(pacc);

	return 0;
}

static int activate_me(void)
{
	if (read_rcba32(FD2, &fd2)) {
		printf("Error reading RCBA\n");
		return 1;
	}
	if (write_rcba32(FD2, fd2 & ~0x2)) {
		printf("Error writing RCBA\n");
		return 1;
	}
	if (debug) {
		if (fd2 & 0x2)
			printf("MEI was hidden on PCI, now unlocked\n");
		else
			printf("MEI not hidden on PCI, checking if visible\n");
	}
	return 0;
}

static void rehide_me(void)
{
	if (fd2 & 0x2) {
		if (debug)
			printf("Re-hiding MEI device...");
		if (read_rcba32(FD2, &fd2)) {
			printf("Error reading RCBA\n");
			return;
		}
		if (write_rcba32(FD2, fd2 | 0x2)) {
			printf("Error writing RCBA\n");
			return;
		}
		if (debug)
			printf("done\n");
	}
}

static struct pci_dev *pci_me_interface_scan(const char **name, char *namebuf,
                                             int namebuf_size)
{
	struct pci_access *pacc;
	struct pci_dev *dev;
	int me = 0;

	pacc = pci_alloc();
	pacc->method = PCI_ACCESS_I386_TYPE1;

	pci_init(pacc);
	pci_scan_bus(pacc);

	for (dev=pacc->devices; dev; dev=dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_BASES |
				   PCI_FILL_SIZES | PCI_FILL_CLASS);
		*name = pci_lookup_name(pacc, namebuf, namebuf_size,
			PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id);
		if (dev->vendor_id != PCI_VENDOR_ID_INTEL)
			continue;

		if (PCI_DEV_HAS_SUPPORTED_ME(dev->device_id)) {
			me = 1;
			break;
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

static void dump_me_info(void)
{
	struct pci_dev *dev;
	uint32_t stat, stat2;
	char namebuf[1024];
	const char *name = NULL;

	if (pci_platform_scan())
		exit(1);

	if (activate_me())
		exit(1);

	dev = pci_me_interface_scan(&name, namebuf, sizeof(namebuf));
	if (!dev)
		exit(1);

	if (name == NULL)
		name = "<unknown>";

	printf("MEI found: [%x:%x] %s\n\n",
	       dev->vendor_id, dev->device_id, name);
	stat = pci_read_long(dev, 0x40);
	printf("ME Status   : 0x%x\n", stat);
	stat2 = pci_read_long(dev, 0x48);
	printf("ME Status 2 : 0x%x\n\n", stat2);

	intel_me_status(stat, stat2);
	printf("\n");
	intel_me_extend_valid(dev);
	printf("\n");

	if (stat & 0xf000)
		printf("ME: has a broken implementation on your board with"
		       "this firmware\n");

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
}

static void print_version(void)
{
	printf("intelmetool v%s -- ", INTELMETOOL_VERSION);
	printf("Copyright (C) 2015 Damien Zammit\n\n");
	printf(GPLV2COPYRIGHT);
}

static void print_usage(const char *name)
{
	printf("usage: %s [-vh?sd]\n", name);
	printf("\n"
	       "   -v | --version:         print the version\n"
	       "   -h | --help:            print this help\n\n"
	       "   -s | --show:            dump all me information on console\n"
	       "   -d | --debug:           enable debug output\n"
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
		fd_mem = open("/dev/mem", O_RDWR);
		if (fd_mem < 0) {
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
