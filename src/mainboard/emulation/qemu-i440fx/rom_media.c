/* SPDX-License-Identifier: GPL-2.0-only */

/* Inspired by OvmfPkg/QemuFlashFvbServicesRuntimeDxe/QemuFlash.c from edk2 */

#include <arch/mmio.h>
#include <boot_device.h>
#include <console/console.h>
#include <commonlib/helpers.h>
#include <commonlib/region.h>
#include <string.h>

#define WRITE_BYTE_CMD		0x10
#define BLOCK_ERASE_CMD		0x20
#define CLEAR_STATUS_CMD	0x50
#define READ_STATUS_CMD		0x70
#define BLOCK_ERASE_CONFIRM_CMD	0xD0
#define READ_ARRAY_CMD		0xFF

#define CLEARED_ARRAY_STATUS	0x00

#define QEMU_FLASH_BLOCK_SIZE	0x1000

#if CONFIG(ELOG)
#include <southbridge/intel/common/pmutil.h>

/*
 * ELOG and VBOOT options are automatically enabled when building with
 * CHROMEOS=y. While the former allows for logging PCH state (not that there is
 * much to log on QEMU), the latter currently forces 16 MiB ROM size, which in
 * turn doesn't allow mounting as pflash in QEMU. Using pflash is required to
 * have writable flash, which means that the following function will not be
 * able to write to the flash based log until ROM size and layout is changed in
 * Flashmap used when building for vboot.
 */
void pch_log_state(void) {}
#endif

static ssize_t qemu_writeat(const struct region_device *rd, const void *b,
				size_t offset, size_t size)
{
	const struct mem_region_device *mdev;
	size_t i;
	volatile char *ptr;
	const char *buf = b;

	mdev = container_of(rd, __typeof__(*mdev), rdev);
	ptr = &mdev->base[offset];

	for (i = 0; i < size; i++) {
		write8(ptr, WRITE_BYTE_CMD);
		write8(ptr, buf[i]);
		ptr++;
	}

	/* Restore flash to read mode. */
	if (size > 0) {
		write8(ptr - 1, READ_ARRAY_CMD);
	}

	return size;
}

static ssize_t qemu_eraseat(const struct region_device *rd, size_t offset,
				size_t size)
{
	const struct mem_region_device *mdev;
	size_t i;
	volatile char *ptr;

	mdev = container_of(rd, __typeof__(*mdev), rdev);
	ptr = &mdev->base[offset];

	if (!IS_ALIGNED(offset, QEMU_FLASH_BLOCK_SIZE)) {
		printk(BIOS_ERR, "%s: erased offset isn't multiple of block size\n",
		       __func__);
		return -1;
	}

	if (!IS_ALIGNED(size, QEMU_FLASH_BLOCK_SIZE)) {
		printk(BIOS_ERR, "%s: erased size isn't multiple of block size\n",
		       __func__);
		return -1;
	}

	for (i = 0; i < size; i += QEMU_FLASH_BLOCK_SIZE) {
		write8(ptr, BLOCK_ERASE_CMD);
		write8(ptr, BLOCK_ERASE_CONFIRM_CMD);
		ptr += QEMU_FLASH_BLOCK_SIZE;
	}

	/* Restore flash to read mode. */
	if (size > 0) {
		write8(ptr - QEMU_FLASH_BLOCK_SIZE, READ_ARRAY_CMD);
	}

	return size;
}

static struct region_device_ops flash_ops;
static const struct mem_region_device boot_dev =
	MEM_REGION_DEV_INIT(0x100000000ULL - CONFIG_ROM_SIZE, CONFIG_ROM_SIZE, &flash_ops);

/*
 * Depending on how firmware image was passed to QEMU, it may behave as:
 *
 * - ROM - memory mapped reads, writes are ignored (FW image mounted with
 *   '-bios');
 * - RAM - memory mapped reads and writes (FW image mounted with e.g.
 *   '-device loader');
 * - flash - memory mapped reads, write and erase possible through commands.
 *   Contrary to physical flash devices erase is not required before writing,
 *   but it also doesn't hurt. Flash may be split into read-only and read-write
 *   parts, like OVMF_CODE.fd and OVMF_VARS.fd. Maximal combined size of system
 *   firmware is hardcoded (QEMU < 5.0.0) or set by default to 8 MiB. On QEMU
 *   version 5.0.0 or newer, it is configurable with `max-fw-size` machine
 *   configuration option, up to 16 MiB to not overlap with IOAPIC memory range
 *   (FW image(s) mounted with '-drive if=pflash').
 *
 * This function detects which of the above applies and fills region_device_ops
 * accordingly.
 */
void boot_device_init(void)
{
	volatile char *ptr;
	char original, readback;
	static bool initialized = false;

	if (initialized)
		return;

	/*
	 * mmap, munmap and readat are always identical to mem_rdev_rw_ops, other
	 * functions may vary.
	 */
	flash_ops = mem_rdev_rw_ops;

	/*
	 * Find first byte different than any of the commands, simplified.
	 *
	 * Detection code few lines below writes commands and tries to read back
	 * the response. To make that code simpler, make sure that original byte
	 * is different than any of the commands or expected responses. It is
	 * expected that such byte will always be found - it is virtually
	 * impossible to write valid x86 code with just bytes ending with 0, and
	 * there are also ASCII characters in metadata (CBFS, FMAP) that has bytes
	 * matching those assumptions.
	 */
	ptr = (volatile char *)boot_dev.base;
	original = read8(ptr);
	while (original == (char)0xFF || (original & 0x0F) == 0)
		original = read8(++ptr);

	/*
	 * Detect what type of flash we're dealing with. This also clears any stale
	 * status bits, so the next read of status register should return known
	 * value (if pflash is used).
	 */
	write8(ptr, CLEAR_STATUS_CMD);
	readback = read8(ptr);
	if (readback == CLEAR_STATUS_CMD) {
		printk(BIOS_DEBUG, "QEMU flash behaves as RAM\n");
		/* Restore original content. */
		write8(ptr, original);
	} else {
		/* Either ROM or QEMU flash implementation. */
		write8(ptr, READ_STATUS_CMD);
		readback = read8(ptr);
		if (readback == original) {
			printk(BIOS_DEBUG, "QEMU flash behaves as ROM\n");
			/* ROM means no writing nor erasing. */
			flash_ops.writeat = NULL;
			flash_ops.eraseat = NULL;
		} else if (readback == CLEARED_ARRAY_STATUS) {
			/* Try writing original value to test whether flash is writable. */
			write8(ptr, WRITE_BYTE_CMD);
			write8(ptr, original);
			write8(ptr, READ_STATUS_CMD);
			readback = read8(ptr);
			if (readback & 0x10 /* programming error */) {
				printk(BIOS_DEBUG,
				       "QEMU flash behaves as write-protected flash\n");
				flash_ops.writeat = NULL;
				flash_ops.eraseat = NULL;
			} else {
				printk(BIOS_DEBUG, "QEMU flash behaves as writable flash\n");
				flash_ops.writeat = qemu_writeat;
				flash_ops.eraseat = qemu_eraseat;
			}
			/* Restore flash to read mode. */
			write8(ptr, READ_ARRAY_CMD);
		} else {
			printk(BIOS_ERR, "Unexpected QEMU flash behavior, assuming ROM\n");
			/*
			 * This shouldn't happen and first byte of flash may already be
			 * corrupted by testing, but don't take any further risk.
			 */
			flash_ops.writeat = NULL;
			flash_ops.eraseat = NULL;
		}
	}

	initialized = true;
}

/* boot_device_ro() is defined in arch/x86/mmap_boot.c */
const struct region_device *boot_device_rw(void)
{
	return &boot_dev.rdev;
}
