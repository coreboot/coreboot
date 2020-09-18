/* intelmetool  Dump interesting things about Management Engine even if hidden */
/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <string.h>
#include <cpuid.h>
#include <sys/io.h>

#ifdef __NetBSD__
#include <machine/sysarch.h>
#endif

#include "intelmetool.h"
#include "me.h"
#include "mmap.h"
#include "msr.h"
#include "rcba.h"

extern int fd_mem;
int debug = 0;

static uint32_t fd2 = 0;
static int ME_major_ver = 0;
static int ME_minor_ver = 0;

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

static int isCPUGenuineIntel(void)
{
	regs_t regs;
	unsigned int level = 0;
	unsigned int eax = 0;

	__get_cpuid(level, &eax, &regs.ebx, &regs.ecx, &regs.edx);

	return !strncmp((char *)&regs, "GenuineIntel", CPU_ID_SIZE-1);
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

		if (PCI_DEV_NO_ME(dev->device_id)) {
			printf(CGRN "Good news, you have a `%s` so you have "
			       "no ME present at all, continuing...\n\n"
			       RESET, name);
			break;
		} else if (PCI_DEV_HAS_ME_DISABLE(dev->device_id)) {
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
	const uint32_t rcba = get_rcba_phys();
	if (debug)
		printf("RCBA addr: 0x%08x\n", rcba);
	if (rcba > 0) {
		if (read_rcba32(FD2, &fd2)) {
			printf("Error reading RCBA\n");
			return 1;
		}
		if (write_rcba32(FD2, fd2 & ~0x2)) {
			printf("Error writing RCBA\n");
			return 1;
		}
		if (debug && (fd2 & 0x2))
			printf("MEI was hidden on PCI, now unlocked\n");
		else if (debug)
			printf("MEI not hidden on PCI, checking if visible\n");
	}

	return 0;
}

static void rehide_me(void)
{
	const uint32_t rcba = get_rcba_phys();
	if (rcba > 0) {
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
		return;

	dev = pci_me_interface_scan(&name, namebuf, sizeof(namebuf));
	if (!dev) {
		if (debug)
			printf("ME PCI device is hidden\n");

		if (activate_me())
			return;
		dev = pci_me_interface_scan(&name, namebuf, sizeof(namebuf));
		if (!dev) {
			printf("Can't find ME PCI device\n");
			return;
		}
	}

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
		printf("ME: has a broken implementation on your board with "
		       "this firmware\n");

	if (intel_mei_setup(dev))
		goto out;
	usleep(ME_COMMAND_DELAY);
	mei_reset();
	usleep(ME_COMMAND_DELAY);
	if (mkhi_get_fw_version(&ME_major_ver, &ME_minor_ver))
		goto out;
	usleep(ME_COMMAND_DELAY);
	mei_reset();
	usleep(ME_COMMAND_DELAY);
	if (mkhi_get_fwcaps())
		goto out;
	usleep(ME_COMMAND_DELAY);

out:
	rehide_me();
}

static void print_btg_bool_param(const char *name, u8 state)
{
	printf("%-20s : %s\n", name, state ? "ON" : "OFF");
}

static void dump_bootguard_info(void)
{
	struct pci_dev *dev;
	char namebuf[1024];
	const char *name = NULL;

	if (pci_platform_scan())
		return;

	dev = pci_me_interface_scan(&name, namebuf, sizeof(namebuf));
	if (!dev) {
		if (debug)
			printf("ME PCI device is hidden\n");

		if (activate_me())
			return;
		dev = pci_me_interface_scan(&name, namebuf, sizeof(namebuf));
		if (!dev) {
			printf("Can't find ME PCI device\n");
			return;
		}
	}

	/* ME_major_ver is zero on some platforms (Mac) */
	if (ME_major_ver &&
	    (ME_major_ver < 9 ||
	     (ME_major_ver == 9 && ME_minor_ver < 5))) {
		printf(CGRN "Your system isn't BootGuard ready.\n"
		       "You can flash other firmware!\n" RESET);
		rehide_me();
		return;
	}

	if (pci_read_long(dev, 0x40) & 0x10)
		printf(CYEL "Your southbridge configuration is insecure!!\n"
		       "BootGuard keys can be overwritten or wiped, or you are "
		       "in developer mode.\n"
		       RESET);
	rehide_me();

	union {
		struct {
			u8  nem_enabled    :  1; /* [ 0.. 0] */
			u8  tpm_type       :  2; /* [ 2.. 1] */
			u8  tpm_success    :  1; /* [ 3.. 3] */
			u8  facb_fpf       :  1; /* [ 4.. 4] */
			u8  measured_boot  :  1; /* [ 5.. 5] */
			u8  verified_boot  :  1; /* [ 6.. 6] */
			u8  module_revoked :  1; /* [ 7.. 7] */
			u32                : 24;
			u8  btg_capability :  1; /* [32..32] */
			u32                : 31;
		};
		u64 raw;
	} btg;

	if (msr_bootguard(&btg.raw) < 0) {
		printf("Could not read the BOOTGUARD_SACM_INFO MSR.\n");
		return;
	}

	printf("BootGuard MSR Output : 0x%" PRIx64 "\n", btg.raw);

	if (!btg.btg_capability) {
		printf(CGRN "Your system isn't BootGuard ready.\n"
		       "You can flash other firmware!\n" RESET);
		return;
	}

	print_btg_bool_param("Measured boot",  btg.measured_boot);
	print_btg_bool_param("Verified boot",  btg.verified_boot);
	print_btg_bool_param("FACB in FPFs",   btg.facb_fpf);
	print_btg_bool_param("Module revoked", btg.module_revoked);
	if (btg.measured_boot) {
		const char *const tpm_type_strs[] = {
			"None",
			"TPM 1.2",
			"TPM 2.0",
			"PTT",
		};
		printf("%-20s : %s\n", "TPM type", tpm_type_strs[btg.tpm_type]);
		print_btg_bool_param("TPM success", btg.tpm_success);
	}
	if (btg.verified_boot) {
		print_btg_bool_param("NEM enabled", btg.nem_enabled);
		if (btg.nem_enabled)
			printf(CRED "Verified boot is enabled and ACM has enabled "
			       "Cache-As-RAM.\nYou can't flash other firmware!\n" RESET);
		else
			printf(CYEL "Verified boot is enabled, but ACM did not enable "
			       "Cache-As-RAM.\nIt might be possible to flash other firmware.\n"
			       RESET);
	} else {
		printf(CGRN "Your system is BootGuard ready but verified boot is disabled.\n"
		       "You can flash other firmware!\n" RESET);
	}
}

static void print_version(void)
{
	printf("intelmetool v%s -- ", INTELMETOOL_VERSION);
	printf("Copyright (C) 2015 Damien Zammit\n");
	printf("Copyright (C) 2017 Philipp Deppenwiese\n");
	printf("Copyright (C) 2017 Patrick Rudolph\n\n");
	printf(GPLV2COPYRIGHT);
}

static void print_usage(const char *name)
{
	printf("usage: %s [-vh?smdb]\n", name);
	printf("\n"
	       "   -v | --version:       print the version\n"
	       "   -h | --help:          print this help\n\n"
	       "   -d | --debug:         enable debug output\n"
	       "   -m | --me             dump all me information on console\n"
	       "   -b | --bootguard      dump bootguard state of the platform\n"
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
		{"me", 0, 0, 'm'},
		{"bootguard", 0, 0, 'b'},
		{"debug", 0, 0, 'd'},
		{0, 0, 0, 0}
	};

	while ((opt = getopt_long(argc, argv, "vh?smdb",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'v':
			print_version();
			exit(0);
			break;
		case 's': /* Legacy fallthrough */
		case 'm':
			cmd_exec = 1;
			break;
		case 'b':
			cmd_exec = 2;
			break;
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

	if (!cmd_exec)
		print_usage(argv[0]);

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
			perror("Can not open /dev/mem. Do you have disabled "
			       "Secure Boot ?");
			exit(1);
		}

		if (!isCPUGenuineIntel()) {
			perror("Error CPU is not from Intel.");
			exit(1);
		}
	#endif

	if (cmd_exec & 3)
		dump_me_info();
	if (cmd_exec & 2)
		dump_bootguard_info();

	return 0;
}
