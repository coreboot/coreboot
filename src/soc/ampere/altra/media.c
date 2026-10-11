/* SPDX-License-Identifier: GPL-2.0-only */

#include "atf.h"
#include <lib.h>
#include <boot_device.h>
#include <stdint.h>
#include <symbols.h>
#include <console/console.h>
#include <assert.h>
#include <cbfs.h>

/*
 * Boot media access on Ampere Altra
 *
 * The SPI-NOR flash is not memory mapped for the non-secure world. TF-A only
 * loads the BL33 image (the bootblock) from the FIP at flash offset 0x600000
 * to DRAM. Everything else has to be read through the SPI-NOR service of the
 * Standalone MM secure partition, see "Altra ATF Interface Specification",
 * chapter "SPI-NOR MM Interface".
 *
 * Calling convention:
 *  - The request is written to the non-secure communication buffer at the
 *    fixed address 0x88300000 in EFI_MM_COMMUNICATE_HEADER format: the
 *    SPI_NOR_MM_GUID, the message length (5 * 8 bytes) and the five 64-bit
 *    words DW0-DW4.
 *  - DW0 holds the subfunction (MM_SPINOR_FUNC_*), DW1-DW3 its arguments.
 *  - The request is issued with the SMC64 MM_COMMUNICATE call. On return the
 *    secure partition has overwritten DW0 with the status
 *    (MM_SPINOR_RES_SUCCESS or MM_SPINOR_RES_FAIL) and DW1-DW4 with the
 *    results of the subfunction.
 *  - READ: DW1 = flash address, DW2 = length, DW3 = physical address of the
 *    non-secure destination buffer.
 *
 * Flash addresses:
 *  - Despite the specification calling DW1 an offset, the service expects
 *    addresses in a window the flash is mapped to (e.g. EDK2 reads the UEFI
 *    extra region at flash offset 0x1700000 via address 0x93100000).
 *  - GET_INFO reports a device base of 0, so it can't be used to find that
 *    window. GET_NVRAM_INFO returns the base of the UEFI ENV region (0x91a00000)
 *    instead. That region is at flash offset 0, so its base is the window base.
 *
 * Access restrictions (see "Altra Family Interface Firmware Requirements",
 * table "SPI-NOR Layout Requirements"):
 *  - At boot time only the UEFI ENV, failsafe, BERT and UEFI extra
 *    (0x1700000 - 0x1ffffff) regions may be read. Reads of the UEFI (code)
 *    region at 0x600000, where the bootblock lives, fail with
 *    MM_SPINOR_RES_FAIL. Hence FMAP and CBFS are placed in the UEFI extra
 *    region, see flash.fmd.
 *  - Writes and erases are only granted to the ENV, failsafe and upper 16 MiB
 *    regions.
 */

//extern char _mm_comm_buffer[];
volatile struct mm_communicate *com = (struct mm_communicate *)0x88300000;

#define SPI_NOR_MM_GUID \
	GUID_INIT(0xC8D76438, 0x4D3C, 0x4BEA, \
	0xBF, 0x86, 0x92, 0x6B, 0x83, 0x07, 0xA2, 0x39)

#define MM_SPINOR_FUNC_GET_INFO          0x00
#define MM_SPINOR_FUNC_READ              0x01
#define MM_SPINOR_FUNC_WRITE             0x02
#define MM_SPINOR_FUNC_ERASE             0x03
#define MM_SPINOR_FUNC_GET_NVRAM_INFO    0x04
#define MM_SPINOR_FUNC_GET_NVRAM2_INFO   0x05
#define MM_SPINOR_FUNC_GET_FAILSAFE_INFO 0x06

#define MM_SPINOR_RES_SUCCESS            0xAABBCC00
#define MM_SPINOR_RES_FAIL               0xAABBCCFF

/* Same chunk size as EDK2's FlashLib (EFI_MM_MAX_TMP_BUF_SIZE) */
#define MM_SPINOR_MAX_READ_SIZE          (16 * KiB)

/* Address of flash offset 0 as expected by the SPI-NOR MM service */
static uint64_t flash_window_base;

static int spinor_mm_call(uint64_t func, uint64_t arg1, uint64_t arg2, uint64_t arg3)
{
	com->guid = SPI_NOR_MM_GUID;
	com->msg_length = 5 * sizeof(uint64_t);
	com->dw0 = func;
	com->dw1 = arg1;
	com->dw2 = arg2;
	com->dw3 = arg3;
	com->dw4 = 0;

	int32_t ret = atf_mm_communicate();
	if (ret != MM_SUCCESS) {
		printk(BIOS_ERR, "SPI-NOR MM func %lld: MM_COMMUNICATE returned %d\n",
		       func, ret);
		return -1;
	}

	if (com->status != MM_SPINOR_RES_SUCCESS) {
		printk(BIOS_ERR, "SPI-NOR MM func %lld: status 0x%llx\n", func, com->status);
		return -1;
	}

	return 0;
}

static ssize_t tfa_mm_readat(const struct region_device *rdev, void *dest, size_t offset,
			     size_t count)
{
	size_t done = 0;

	while (done < count) {
		size_t len = MIN(count - done, MM_SPINOR_MAX_READ_SIZE);

		if (spinor_mm_call(MM_SPINOR_FUNC_READ, flash_window_base + offset + done, len,
				   (uintptr_t)dest + done)) {
			printk(BIOS_ERR, "SPI-NOR read of 0x%zx bytes at offset 0x%zx to %p failed\n",
			       len, offset + done, (uint8_t *)dest + done);
			return -1;
		}

		done += len;
	}

	return count;
}

static const struct region_device_ops tfa_mm_ops = {
	.mmap = mmap_helper_rdev_mmap,
	.munmap = mmap_helper_rdev_munmap,
	.readat = tfa_mm_readat,
};

static struct mmap_helper_region_device tfa_mm_mdev = MMAP_HELPER_DEV_INIT(&tfa_mm_ops, 0,
								       CONFIG_ROM_SIZE,
								       &cbfs_cache);


void boot_device_init(void)
{
	static bool init_done = false;
	if (init_done)
		return;

	union mm_version version = { .raw = atf_mm_version() };
	if (version.must_be_zero != 0 || version.minor != 0 || version.major != 1)
		die("ATF MM_VERSION is incompatible 0x%x\n", version.raw);

	if (spinor_mm_call(MM_SPINOR_FUNC_GET_INFO, 0, 0, 0))
		die("Failed to get SPI-NOR info\n");
	printk(BIOS_DEBUG, "SPI-NOR: base 0x%llx, page size 0x%llx, sector size 0x%llx, size 0x%llx\n",
	       com->device_base, com->page_size, com->sector_size, com->device_size);

	if (spinor_mm_call(MM_SPINOR_FUNC_GET_NVRAM_INFO, 0, 0, 0))
		die("Failed to get SPI-NOR NVRAM info\n");
	flash_window_base = com->dw1;
	printk(BIOS_DEBUG, "SPI-NOR: NVRAM base 0x%llx, size 0x%llx\n", com->dw1, com->dw2);

	init_done = true;
}

const struct region_device *boot_device_ro(void)
{
	return &tfa_mm_mdev.rdev;
}
