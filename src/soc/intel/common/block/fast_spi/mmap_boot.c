/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * This file provides a custom boot media device for the platforms that support additional
 * window for BIOS regions greater than 16MiB. If the mainboard uses a smaller BIOS region, then
 * the additional window is unused.
 */

#include <boot_device.h>
#include <commonlib/region.h>
#include <console/console.h>
#include <fmap.h>
#include <intelblocks/fast_spi.h>
#include <spi_flash.h>

enum window_type {
	/* Fixed decode window of max 16MiB size just below 4G boundary */
	FIXED_DECODE_WINDOW,
	/* Additional decode window for mapping BIOS region greater than 16MiB */
	EXT_BIOS_DECODE_WINDOW,
	TOTAL_DECODE_WINDOWS,
};

static struct xlate_region_device real_dev;
static struct mem_region_device shadow_devs[TOTAL_DECODE_WINDOWS];
static struct xlate_window real_dev_windows[TOTAL_DECODE_WINDOWS];

static void initialize_window(enum window_type type, uintptr_t host_base,
			      uintptr_t flash_base, size_t size)
{
	mem_region_device_ro_init(&shadow_devs[type], (void *)host_base, size);
	xlate_window_init(&real_dev_windows[type], &shadow_devs[type].rdev,
			  flash_base, size);
	printk(BIOS_INFO, "%s: ",
		(type == FIXED_DECODE_WINDOW) ?
		 "Fixed Decode Window" : "Extended Decode Window");
	printk(BIOS_INFO, "SPI flash base=0x%lx, Host base=0x%lx, Size=0x%zx\n",
	       flash_base, host_base, size);
}

/*
 *
 *                                                                      +--------------+
 *                                                                      |              |
 *                                                                      |              |
 *                                                                      |              |
 *      ^     +------------+--------------------------^--------------------------------+ 0xffffffff
 *      |     |            |                          |                 |              |
 *      |     |            |                          |                 |              |
 *      |     |            |                          +                 | FIXED        |
 *      |     |            |                     fixed_win_size         | DECODE       |
 *      |     |  BIOS      |                          +                 | WINDOW       |
 *      +     |  region    |                          |                 |              |
 *  bios_size | (Region 1) |                          |                 |              |
 *      +     |            |                          |                 |              |
 *      |     |            |                          |                 |              |
 *      |     |            | fixed_win_flash_base+----v--------------------------------+ fixed_win_host_base
 *      |     |            |                     |                      |              |
 *      |     |            |                     |                      |              |
 *      |     |            |                     |                      | Other MMIO   |
 *      v     |            |                     |                      |              |
 * bios_start +------------+ ext_win_flash_base  |                      |              |
 *            |            |            +        |                      |              |
 *            |            |            |        |                      |              |
 *            |            |            |        |                      |              |
 *            |            |            |        +-----^------------------------------------------------------------^
 *          0 +------------+            |              |                |              |                            |
 *                                      |              +                | EXT_BIOS     |                            |
 *            SPI flash                 |         ext_win_size          | DECODE       |                            |
 *            address                   |              +                | WINDOW       |                            +
 *            space                     |              |                |              |                    CONFIG_EXT_BIOS_WIN_SIZE
 *                                      +--------------v-------------------------------+ ext_win_host_base          +
 *                                                                      |              |                            |
 *                                                                      | Unused       |                            |
 *                                                                      |              |                            |
 *                                                                      +--------------+ CONFIG_EXT_BIOS_WIN_BASE+--v
 *                                                                      |              |
 *                                                                      |              |
 *                                                                      |              |
 *                                                                      +--------------+
 *
 *                                                                       Host address
 *                                                                       space
 */
static void bios_mmap_init(void)
{
	static bool init_done;

	size_t bios_size, bios_start;

	uintptr_t fixed_win_host_base, fixed_win_flash_base;
	uintptr_t ext_win_host_base, ext_win_flash_base;
	size_t fixed_win_size, ext_win_size;

	size_t win_count = 0;

	if (init_done)
		return;

	/* Read the offset and size of BIOS region in the SPI flash address space. */
	bios_start = fast_spi_get_bios_region(&bios_size);

	/*
	 * By default, fixed decode window (maximum size 16MiB) is mapped just below the 4G
	 * boundary. This window maps the top part of the BIOS region in the SPI flash address
	 * space to the host address space.
	 */
	fixed_win_size = MIN(16 * MiB, bios_size);
	fixed_win_host_base = 4ULL * GiB - fixed_win_size;
	fixed_win_flash_base = bios_start + bios_size - fixed_win_size;

	initialize_window(FIXED_DECODE_WINDOW, fixed_win_host_base, fixed_win_flash_base,
			  fixed_win_size);
	win_count++;

	_Static_assert(CONFIG_EXT_BIOS_WIN_BASE != 0, "Extended BIOS window base cannot be 0!");
	_Static_assert(CONFIG_EXT_BIOS_WIN_SIZE != 0, "Extended BIOS window size cannot be 0!");

	/*
	 * Remaining portion of the BIOS region up to a maximum of CONFIG_EXT_BIOS_WIN_SIZE is
	 * mapped at the top of the extended window if the BIOS region is greater than 16MiB.
	 *
	 * If the BIOS region is not greater than 16MiB, then the extended window is not
	 * enabled.
	 */
	ext_win_size = MIN(CONFIG_EXT_BIOS_WIN_SIZE, bios_size - fixed_win_size);

	if (ext_win_size) {
		ext_win_host_base = CONFIG_EXT_BIOS_WIN_BASE + CONFIG_EXT_BIOS_WIN_SIZE -
			ext_win_size;
		ext_win_flash_base = fixed_win_flash_base - ext_win_size;
		initialize_window(EXT_BIOS_DECODE_WINDOW, ext_win_host_base,
				  ext_win_flash_base, ext_win_size);
		win_count++;
	}

	xlate_region_device_ro_init(&real_dev, win_count, real_dev_windows, CONFIG_ROM_SIZE);

	init_done = true;
}

const struct region_device *boot_device_ro(void)
{
	bios_mmap_init();

	return &real_dev.rdev;
}

void fast_spi_get_ext_bios_window(uintptr_t *base, size_t *size)
{
	const struct region_device *rd = &shadow_devs[EXT_BIOS_DECODE_WINDOW].rdev;

	bios_mmap_init();

	*size = region_device_sz(rd);

	if (*size == 0) {
		*base = 0;
	} else {
		/*
		 * This is a memory region device. So, mmap returns the base address of the
		 * device. Also, as this is a memory region device, unmap is a no-op.
		 */
		*base = (uintptr_t)rdev_mmap_full(rd);
	}
}

uint32_t spi_flash_get_mmap_windows(struct flash_mmap_window *table)
{
	int i;
	uint32_t count = 0;

	bios_mmap_init();

	for (i = 0; i < TOTAL_DECODE_WINDOWS; i++) {
		if (region_sz(&real_dev_windows[i].sub_region) == 0)
			continue;

		count++;
		table->flash_base = region_offset(&real_dev_windows[i].sub_region);
		table->host_base = (uintptr_t)rdev_mmap_full(&shadow_devs[i].rdev);
		table->size = region_sz(&real_dev_windows[i].sub_region);

		table++;
	}

	return count;
}
