/* SPDX-License-Identifier: GPL-2.0-only */

#include <boot_device.h>
#include <spi_flash.h>
#include <stdint.h>
#include <amdblocks/lpc.h>

enum window_type {
	/* Fixed decode window of max 16MiB size just below 4G boundary */
	ROM2_DECODE_WINDOW,
	/* Additional decode window for mapping BIOS region greater than 16MiB */
	ROM3_DECODE_WINDOW,
	TOTAL_DECODE_WINDOWS,
};

static struct xlate_region_device real_dev;
static struct mem_region_device shadow_devs[TOTAL_DECODE_WINDOWS];
static struct xlate_window real_dev_windows[TOTAL_DECODE_WINDOWS];

static void initialize_window(const size_t win_idx, const enum window_type type,
			      uintptr_t host_base, uintptr_t flash_base, size_t size)
{
	mem_region_device_ro_init(&shadow_devs[win_idx], (void *)host_base, size);
	xlate_window_init(&real_dev_windows[win_idx], &shadow_devs[win_idx].rdev,
			  flash_base, size);
	printk(BIOS_INFO, "ROM%d Decode Window: ",
	       (type == ROM2_DECODE_WINDOW) ? 2 : 3);
	printk(BIOS_INFO, "SPI flash base=0x%lx, Host base=0x%lx, Size=0x%zx\n",
	       flash_base, host_base, size);
}

/*
 *
 * Note: This configuration assumes no flash remapping (fch_spi_rom_remapping() = 0).
 *
 *                                                                           +--------------+
 *                                                                           |              |
 *                                                                           |              |
 *                                                                           |              |
 * CONFIG_ROM_SIZE +------------+--------------------------+--------------------------------+- rom3_end
 *                 |            |                          ^                 |              |
 *                 |            |                          |                 |     ROM3     |
 *                 |            |                          |                 |              |
 *                 |            |                          |                 |              |
 *                 |            |                          +                 |              |
 *                 |            |                       rom3_size            |              |
 *                 |            |                          +                 |              |
 *                 |  SPI ROM   |                          |                 |              |
 *                 |            |                     +----+-----------------+--------------+
 *                 |            |                     |    |                 |              |
 *                 |            |                     |    |                 |              |
 *                 |            |                     |    V                 |              |
 *                 |            |                     |    +---0xfd00000000--+--------------+- rom3_start
 *                 |            |                     |                      |              |
 *                 |            |                     |                      |              |
 *                 |            |                     |                      | Other MMIO   |
 *                 |            |                     |                      |              |
 *                 +------------+ --------------------+---------0x100000000--+--------------+- rom2_end
 *                 |            |              ^                             |              |
 *                 |            |              |                             |              |
 *                 |            |          rom2_size                         |     ROM2     |
 *                 |            |              |                             |              |
 *                 |            |              V                             |              |
 *             0   +------------+ ------------------------------0xFF0000000--+--------------+- rom2_start
 *                                                                           |              |
 *                  SPI flash                                                |              |
 *                  address                                                  |              |
 *                  space                                                    |              |
 *                                                                           +--------------+
 *
 *                                                                            Host address
 *                                                                            space
 */
static void bios_mmap_init(void)
{
	static bool init_done;
	size_t win_count = 0;
	size_t map_win_size = 0;

	if (init_done)
		return;

	/*
	 * By default, fixed decode window (maximum size 16MiB) is mapped just
	 * below the 4G boundary. This window maps the bottom part of the BIOS
	 * region in the SPI flash address space to the host address space.
	 */
	size_t rom2_size = 0;
	const uintptr_t rom2_start = lpc_get_rom2_region(&rom2_size);
	if (rom2_start && rom2_size) {
		initialize_window(win_count, ROM2_DECODE_WINDOW, rom2_start, 0, rom2_size);
		win_count++;
		map_win_size += rom2_size;
	}

	/*
	 * Remaining portion of the BIOS region up to a maximum of 64MiB is
	 * mapped at the bottom of the ROM3 if the BIOS region is greater than 16MiB.
	 * The ROM3 window is only used when it's inside the identity mapped
	 * page tables used by x86_64.
	 *
	 * If the BIOS region is not greater than 16MiB, then the ROM3 window is not
	 * enabled.
	 */
	size_t rom3_size;
	const uint64_t rom3_start = lpc_get_rom3_region(&rom3_size);
	const uint64_t rom3_end = rom3_start + rom3_size;

	if (CONFIG_ROM_SIZE > 16 * MiB) {
		assert(rom3_start);
		assert(rom3_size > 16 * MiB);
		assert(DIV_ROUND_UP(rom3_end, GiB) < CONFIG_CPU_PT_ROM_MAP_GB);
	}

	if (CONFIG_ROM_SIZE > 16 * MiB &&
	    rom3_start > 0 &&
	    rom3_size > 16 * MiB &&
	    DIV_ROUND_UP(rom3_end, GiB) < CONFIG_CPU_PT_ROM_MAP_GB) {

		const size_t ext_win_size = MIN(rom3_size, CONFIG_ROM_SIZE - rom2_size);

		initialize_window(win_count, ROM3_DECODE_WINDOW,
				  rom3_start + rom2_size, rom2_size, ext_win_size);
		win_count++;
		map_win_size += ext_win_size;
	}

	assert(map_win_size == CONFIG_ROM_SIZE);
	xlate_region_device_ro_init(&real_dev, win_count, real_dev_windows, map_win_size);

	init_done = true;
}

const struct region_device *boot_device_ro(void)
{
	bios_mmap_init();

	return &real_dev.rdev;
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
		/* FIXME: Allow SPI mmap in 64-bit address space */
		if ((uintptr_t)rdev_mmap_full(&shadow_devs[i].rdev) < 4ULL * GiB)
			table->host_base = (uintptr_t)rdev_mmap_full(&shadow_devs[i].rdev);
		table->size = region_sz(&real_dev_windows[i].sub_region);

		table++;
	}

	return count;
}
