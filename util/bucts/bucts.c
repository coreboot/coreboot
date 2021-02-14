/* SPDX-License-Identifier: GPL-2.0-only */

#include <getopt.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <sys/mman.h>
#if defined(__GLIBC__)
#include <sys/io.h>
#endif
#include <pci/pci.h>

#if defined(__sun) && (defined(__i386) || defined(__amd64))
#define MEM_DEV "/dev/xsvc"
#else
#define MEM_DEV "/dev/mem"
#endif

#define BUC_OFFSET 0x3414
#define GCS_OFFSET 0x3410

#define SPIBAR 0x3800
#define FDOC 0xb0
#define FDOD 0xb4

enum {
	DESCRIPTOR_MAP_SECTION = 0,
	COMPONENT_SECTION,
	REGION_SECTION,
	MASTER_SECTION,
	PCHSTRAPS_SECTION,
};

enum {
	BUCTS_UNINITIALIZED = -1,
	BUCTS_UNSET = 0,
	BUCTS_SET
};

static int fd_mem = -1;

static void print_usage(const char *name)
{
	printf("usage: %s [-h?]\n", name);
	printf("\n"
	       "   -s | --set:                       Set the top swap bit to invert the bootblock region decoding\n"
	       "   -u | --unset:                     Unset the top swap bit to have regular bootblock region decoding\n"
	       "   -p | --print:                     print the current status of the hardware\n"
	       "   -h | --help:                      print this help\n"
	       "\n");
}

static void *sys_physmap(unsigned long phys_addr, size_t len)
{
	void *virt_addr =
	    mmap(0, len, PROT_WRITE | PROT_READ, MAP_SHARED, fd_mem,
		 (off_t) phys_addr);
	return virt_addr == MAP_FAILED ? NULL : virt_addr;
}

static void physunmap(void *virt_addr, size_t len)
{
	if (!len) {
		printf("Not unmapping zero size at %p\n", virt_addr);
		return;
	}
	munmap(virt_addr, len);
}

static void *physmap(const char *descr, unsigned long phys_addr, size_t len)
{
	void *virt_addr;

	if (!descr)
		descr = "memory";

	if (!len) {
		printf("Not mapping %s, zero size at 0x%08lx.\n", descr,
		       phys_addr);
		return NULL;
	}

	if ((getpagesize() - 1) & len)
		fprintf(stderr, "Unaligned size 0x%lx for %s at 0x%08lx!\n",
			(unsigned long)len, descr, phys_addr);

	if ((getpagesize() - 1) & phys_addr)
		fprintf(stderr, "Unaligned address 0x%08lx for %s!\n",
			phys_addr, descr);

	virt_addr = sys_physmap(phys_addr, len);
	if (!virt_addr) {
		fprintf(stderr, "Error accessing 0x%lx bytes %s at 0x%08lx!\n",
			(unsigned long)len, descr, phys_addr);
		perror("mmap(" MEM_DEV ")");
		if (errno == EINVAL) {
			fprintf(stderr, "\n");
			fprintf(stderr,
				"In Linux this error can be caused by the CONFIG_NONPROMISC_DEVMEM (<2.6.27),\n");
			fprintf(stderr,
				"CONFIG_STRICT_DEVMEM (>=2.6.27) and CONFIG_X86_PAT kernel options.\n");
			fprintf(stderr,
				"Please check if either is enabled in your kernel before reporting a failure.\n");
			fprintf(stderr,
				"You can override CONFIG_X86_PAT at boot with the nopat kernel parameter but\n");
			fprintf(stderr,
				"disabling the other option unfortunately requires a kernel recompile. Sorry!\n");
		}
	}

	return virt_addr;
}

static int get_platform_info(struct pci_dev *sb, int *has_var_bb,
			     uint32_t *rcba_addr)
{
	*has_var_bb = 0;
	switch (sb->device_id) {
	case 0x1c44: /* Z68 */
	case 0x1c46: /* P67 */
	case 0x1c47: /* UM67 */
	case 0x1c49: /* HM68 */
	case 0x1c4a: /* H67 */
	case 0x1c4b: /* HM67 */
	case 0x1c4c: /* Q65 */
	case 0x1c4d: /* QS67 */
	case 0x1c4e: /* Q67 */
	case 0x1c4f: /* QM67 */
	case 0x1c50: /* B65 */
	case 0x1c52: /* C202 */
	case 0x1c54: /* C204 */
	case 0x1c56: /* C026 */
	case 0x1c5c: /* H61 */
	case 0x1d40: /* C60x/X79 */
	case 0x1c41: /* C60x/X79 */
	case 0x1e44: /* Z77 */
	case 0x1e46: /* Z75 */
	case 0x1e47: /* Q77 */
	case 0x1e48: /* Q75 */
	case 0x1e49: /* B75 */
	case 0x1e4a: /* H77 */
	case 0x1e53: /* C216 */
	case 0x1e55: /* QM77 */
	case 0x1e56: /* QS77 */
	case 0x1e57: /* HM77 */
	case 0x1e58: /* UM77 */
	case 0x1e59: /* HM76 */
	case 0x1e5d: /* HM75 */
	case 0x1e5e: /* HM70 */
	case 0x1e5f: /* NM70 */
	case 0x3b02: /* P55 */
	case 0x3b03: /* PM55 */
	case 0x3b06: /* H55 */
	case 0x3b07: /* QM55 */
	case 0x3b08: /* H57 */
	case 0x3b09: /* HM55 */
	case 0x3b0a: /* Q57 */
	case 0x3b0b: /* HM57 */
	case 0x3b0d: /* PCH 3400 Mobile SFF */
	case 0x3b0e: /* B55 */
	case 0x3b0f: /* QS57 */
	case 0x3b12: /* PCH 3400 */
	case 0x3b14: /* PCH 3420 */
	case 0x3b16: /* PCH 3450 */
	case 0x3b1e: /* B55 */
	case 0x8c40: /* Lynx Point */
	case 0x8c41: /* Lynx Point Mobile Eng. Sample */
	case 0x8c42: /* Lynx Point Desktop Eng. Sample */
	case 0x8c43: /* Lynx Point */
	case 0x8c44: /* Z87 */
	case 0x8c45: /* Lynx Point */
		*has_var_bb = 1;
		/* fallthrough */
	case 0x2640: /* ICH6/ICH6R */
	case 0x2641: /* ICH6-M */
	case 0x2642: /* ICH6W/ICH6RW */
	case 0x2670: /* 631xESB/632xESB/3100 */
	case 0x27b0: /* ICH7DH */
	case 0x27b8: /* ICH7/ICH7R */
	case 0x27b9: /* ICH7M */
	case 0x27bd: /* ICH7MDH */
	case 0x27bc: /* NM10 */
	case 0x2810: /* ICH8/ICH8R */
	case 0x2811: /* ICH8M-E */
	case 0x2812: /* ICH8DH */
	case 0x2814: /* ICH8DO */
	case 0x2815: /* ICH8M */
	case 0x2910: /* ICH9 Eng. Sample */
	case 0x2912: /* ICH9DH */
	case 0x2914: /* ICH9DO */
	case 0x2916: /* ICH9R */
	case 0x2917: /* ICH9M-E */
	case 0x2918: /* ICH9 */
	case 0x2919: /* ICH9M */
	case 0x3a10: /* ICH10R Eng. Sample */
	case 0x3a14: /* ICH10D0 */
	case 0x3a16: /* ICH10R */
	case 0x3a18: /* ICH10 */
	case 0x3a1a: /* ICH10D */
	case 0x3a1e: /* ICH10 Eng. Sample*/
	case 0x3b00: /* PCH 3400 Desktop */
	case 0x3b01: /* PCH 3400 Mobile */
		*rcba_addr = pci_read_long(sb, 0xf0) & ~1;
		return 0;
	default:
		fprintf(stderr, "Unsupported LPC bridge. Sorry.\n");
		return 1;
	}
}

int print_status(uint32_t rcba_addr, int has_var_bb)
{
	int ret = 0;
	volatile uint8_t *rcba;
	int bild, ts, ts_size = 64 << 10;
	rcba = physmap("RCBA", rcba_addr, 0x4000);
	if (!rcba)
		return 1;

	bild = rcba[GCS_OFFSET] & 1;
	ts = rcba[BUC_OFFSET] & 1;
	int spi_is_bootdevice =
		((*(uint16_t *)(rcba + GCS_OFFSET) >> 10) & 3) == 3;

	if (has_var_bb && spi_is_bootdevice) {
		*(uint32_t *)(rcba + SPIBAR + FDOC) =
			(PCHSTRAPS_SECTION << 12) | 0;
		uint32_t pchstrap0 = *(uint32_t *)(rcba + SPIBAR + FDOD);
		switch (pchstrap0 >> 29) {
		case 0:
			ts_size = 64 << 10;
			break;
		case 1:
			ts_size = 128 << 10;
			break;
		case 2:
			ts_size = 256 << 10;
			break;
		default:
			printf("Unknown BIOS Boot-Block size (BBBS)\n");
			ret = 1;
			goto out;
		}
	}

	printf("\nSystem status\n=============\n");
	printf("BILD (BIOS Interface Lock-Down) : %s\n", bild
	       ? "set, changing top swap not possible" : "unset, changing top swap is possible");
	printf("Top Swap                        : %s\n", ts ? "Enabled" : "Disabled");
	printf("BIOS Boot-block Size (BBDS)     : %dK\n", ts_size >> 10);
	if (ts) {
		printf("Ranges swapped                  : 0x%llx-0x%08llx <-> 0x%08llx-0x%08llx",
		       ((4ULL << 30) - ts_size),
		       (4ULL << 30) - 1,
		       (4ULL << 30) - 2 * ts_size,
		       (4ULL << 30) - ts_size - 1);
	}

out:
	physunmap((void *)rcba, 0x4000);
	return ret;
}

static int set_bucts(uint32_t rcba_addr, int bucts_state)
{
	int ret = 0;
	volatile uint8_t *rcba;
	int bild;
	rcba = physmap("RCBA", rcba_addr, 0x4000);
	if (!rcba)
		return 1;

	bild = rcba[GCS_OFFSET] & 1;
	if (bild) {
		printf("BIOS Interface Lock-Down set, Changing Top Swap not possible.\n");
		ret = 1;
		goto out;
	}


	rcba[BUC_OFFSET] = bucts_state & 1;
	if ((rcba[BUC_OFFSET] & 1) != bucts_state) {
		fprintf(stderr, "Hu?! setting the top swap bit failed!\n");
		ret = 1;
	} else {
		printf("Setting top swap bit succeeded.\n");
		ret = 0;
	}

out:
	physunmap((void *)rcba, 0x4000);
	return ret;
}

int main(int argc, char *argv[])
{
	struct pci_access *pacc;
	struct pci_dev *dev, *sb = NULL;
#if defined(__FreeBSD__)
	int io_fd;
#endif
	int opt, option_index = 0;
	int print_state = 0;
	int bucts_state = BUCTS_UNINITIALIZED;

	static struct option long_options[] = {
		{"unset", 0, 0, 'u'},
		{"set", 0, 0, 's'},
		{"print", 0, 0, 'p'},
		{"help", 0, 0, 'h'},
		{0, 0, 0, 0}
	};


	printf("bucts utility version '" VERSION "'\n");

	if (argv[1] == NULL) {
		print_usage(argv[0]);
		exit(1);
	}

	while ((opt = getopt_long(argc, argv, "usph?:",
				  long_options, &option_index)) != EOF) {
		switch (opt) {
		case 'u':
			if (bucts_state != BUCTS_UNINITIALIZED) {
				printf("Invalid setting: multiple set/unset arguments\n");
				exit(1);
			}
			if (print_state) {
				printf("Print and Set are mutually exclusive!\n");
				exit(1);
			}
			bucts_state = BUCTS_UNSET;
			break;
		case 's':
			if (bucts_state != BUCTS_UNINITIALIZED) {
				printf("Invalid setting: multiple set/unset arguments\n");
				exit(1);
			}
			if (print_state) {
				printf("Print and Set are mutually exclusive!\n");
				exit(1);
			}
			bucts_state = BUCTS_SET;
			break;
		case 'p':
			if (bucts_state != BUCTS_UNINITIALIZED) {
				printf("Print and Set are mutually exclusive!\n");
				exit(1);
			}
			print_state = 1;
			break;
		case 'h':
			print_usage(argv[0]);
			exit(0);
		case '?':
		default:
			print_usage(argv[0]);
			exit(1);
		}
	}

	if (optind < argc) {
		fprintf(stderr, "Error: Extra parameter found.\n");
		print_usage(argv[0]);
		exit(1);
	}


#if defined(__FreeBSD__)
	if ((io_fd = open("/dev/io", O_RDWR)) < 0) {
		perror("open(/dev/io)");
#else
	if (iopl(3)) {
		perror("iopl");
#endif
		printf("You need to be root.\n");
		return 1;
	}

	if ((fd_mem = open(MEM_DEV, O_RDWR | O_SYNC)) == -1) {
		perror("Error: open(" MEM_DEV ")");
		return 1;
	}

	pacc = pci_alloc();
	pci_init(pacc);
	pci_scan_bus(pacc);
	for (dev = pacc->devices; dev && !sb; dev = dev->next) {
		pci_fill_info(dev, PCI_FILL_IDENT | PCI_FILL_CLASS);
		if (dev->vendor_id != 0x8086 || dev->device_class != 0x0601)
			continue;
		sb = dev;
	}
	if (!sb) {
		fprintf(stderr, "Error: LPC bridge not found!\n");
		return 1;
	}

	printf("Using LPC bridge %04x:%04x at %02x%02x:%02x.%02x\n",
	       sb->vendor_id, sb->device_id, sb->domain, sb->bus, sb->dev,
	       sb->func);

	int has_var_bb;
	uint32_t rcba_addr;

	if (get_platform_info(sb, &has_var_bb, &rcba_addr))
		exit(1);

	if (print_state)
		print_status(rcba_addr, has_var_bb);
	if (bucts_state != BUCTS_UNINITIALIZED) {
		set_bucts(rcba_addr, bucts_state);
		print_status(rcba_addr, has_var_bb);
	}

	close(fd_mem);
	return 0;
}
