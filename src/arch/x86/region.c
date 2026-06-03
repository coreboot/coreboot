/* SPDX-License-Identifier: GPL-2.0-only */
#include <arch/region.h>
#include <cpu/x86/tsc.h>
#include <cpu/x86/cache.h>
#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <cbfs.h>
#include <delay.h>
#include <stdint.h>
#include <string.h>
#include <timer.h>
#include <thread.h>

/* Defines */
#define CACHELINE_SIZE 64 /* bytes */
#define MIN_PREFETCH_SIZE (CACHELINE_SIZE * 8) /* bytes */
#define PREFETCH_SIZE 64 /* Cachelines */

#define MAX_YIELD_FACTOR 10

/* Internal structs */
struct preload_region_device {
	const struct region_device *encapsuled_rdev;
	struct region_device rdev;
};

/* The delay to wait for the CPU to prefetch one cacheline */
static uint32_t prefetch_delay; /* usec */

/* Perfomance counters */
static uint32_t stat_max_loops = 0;
static uint32_t stat_loops = 0;

/*
 * Read from the encapsuled region device by preloading the data in the background.
 * The prefetching is done by the CPU using the prefetchnta instruction. The prefetching
 * is done in the background while the main thread is doing other work.
 *
 * @param rd The region device to read from.
 * @param b The buffer to read into.
 * @param offset The offset to read from.
 * @param size The size to read.
 * @return The number of bytes read or a negative error code.
 */
static ssize_t mdev_readat(const struct region_device *rd, void *b, size_t offset, size_t size)
{
	const struct preload_region_device *pdev;
	const struct region_device *rdev;
	struct mono_time t1, t2;
	ssize_t ret;

	pdev = container_of(rd, __typeof__(*pdev), rdev);
	rdev = pdev->encapsuled_rdev;

	/* Opt out early if not preloading or too small */
	if ((size < MIN_PREFETCH_SIZE) || !cbfs_preload_is_preload_thread())
		return rdev_readat(rdev, b, offset, size);

	/* Is a preload thread */
	size_t remaining = size;

	/* Align to cacheline boundary */
	if (offset % CACHELINE_SIZE) {
		/* Align to CACHELINE_SIZE */
		const size_t align = CACHELINE_SIZE - (offset % CACHELINE_SIZE);
		ret = rdev_readat(rdev, b, offset, align);
		if (ret != align)
			return ret;

		b += align;
		offset += align;
		remaining -= align;
	}


	/* Perform initial benchmark of SPI flash */
	if (!prefetch_delay) {
		char *src = rdev_mmap(rdev, offset, CACHELINE_SIZE);

		/* Sanity check. Should not happen. */
		if (!src) {
			ret = rdev_readat(rdev, b, offset, remaining);
			if (ret != remaining)
				return ret;
			return size;
		}

		/* Discard data in cache. Need raw I/O latency. */
		if (clflush_supported())
			clflush_region((uintptr_t)src, CACHELINE_SIZE);

		/* Start real benchmark */
		timer_monotonic_get(&t1);
		memcpy(b, src, CACHELINE_SIZE);
		timer_monotonic_get(&t2);

		/* MMIO should not need to be unmapped. Just in case. */
		rdev_munmap(rdev, src);

		prefetch_delay = mono_time_diff_microseconds(&t1, &t2);

		printk(BIOS_SPEW, "spi_preload: Initial read latency %u us\n", prefetch_delay);
		/* Sanity check. To small or too big values delay the boot. */
		prefetch_delay = clamp_u32(1, prefetch_delay, 500);

		b += CACHELINE_SIZE;
		offset += CACHELINE_SIZE;
		remaining -= CACHELINE_SIZE;
	}
//works
	timer_monotonic_get(&t1);
	for (size_t loops = 0; remaining >= CACHELINE_SIZE; remaining -= CACHELINE_SIZE) {
		/* Get pointer to chached MMIO SPI ROM mapping */
		char *src = rdev_mmap(rdev, offset, CACHELINE_SIZE);
		/* Sanity check. Should not happen. */
		if (!src) {
			ret = rdev_readat(rdev, b, offset, remaining);
			if (ret != remaining)
				return ret;
			return size;
		}

		/* Copy chunk of data from MMIO. Should take 0 usec if it was cached. */
		memcpy(b, src, CACHELINE_SIZE);
		timer_monotonic_get(&t2);

		/* MMIO should not need to be unmapped. Just in case. */
		rdev_munmap(rdev, src);

		b += CACHELINE_SIZE;
		offset += CACHELINE_SIZE;
		loops++;

		/* Check how long access took. If it wasn't cached or reached
		 * the prefetch size, abort and yield.
		 */
		if ((mono_time_diff_microseconds(&t1, &t2) > 1) ||
		    (loops == PREFETCH_SIZE)) {
			loops = 0;
			/*
			 * Reached prefetched size or it wasn't cached. Possible reasons:
			 * - Time since last yield was to small to allow prefetching to complete
			 * - BSP might have cleared the cache
			 * - Cacheline was evicted due to memory pressure
			 * - no MTRR covering this region
			 * - Concurrent SPI flash access on another thread
			 */
			timer_monotonic_get(&t2);

			/*
			 * Prefetch the next cache lines. The prefetch will be done in the
			 * background and hopefully complete before the next memcpy call.
			 *
			 * When SPIROM is not marked cachable, the cache was evicted by
			 * a 'wbinvd' or 'invd' instruction, or the prefetch was not completed in time,
			 * the next memcpy call will take longer, but still load the correct data.
			 */
			src = rdev_mmap(rdev, offset, MIN(PREFETCH_SIZE, remaining));
			for (size_t j = 1; j <= PREFETCH_SIZE && remaining >= (j * CACHELINE_SIZE); j++)
				asm volatile("prefetchnta %0" :: "m" (src[CACHELINE_SIZE * j]));
			rdev_munmap(rdev, src);

			/* Yield to continue work in main thread. When main tread takes more time the prefetch will
			 * load more cache-lines. */
			udelay(prefetch_delay);

			timer_monotonic_get(&t1);

			u64 last_yield_time = mono_time_diff_microseconds(&t2, &t1);

			/*
			 * For developers: When you see this investigate what blocks for so long
			 * on the main thread without calling thread_yield() or udelay().
			 */
			if (last_yield_time > (MAX_YIELD_FACTOR * prefetch_delay))
				printk(BIOS_SPEW, "spi_preload: Last udelay(%u) call took %llu usec\n",
				       prefetch_delay, last_yield_time);

			/* Reset performance counter */
			stat_loops = 0;
		} else {
			stat_loops++;
			if (stat_loops > stat_max_loops)
				stat_max_loops = stat_loops;
		}
	}

	printk(BIOS_SPEW, "spi_preload: Max prefetched cache lines: %u\n", stat_max_loops);

	if (remaining) {
		ret = rdev_readat(rdev, b, offset, remaining);
		if (ret != remaining)
			return ret;
	}

	return size;
}

static void *mdev_mmap(const struct region_device *rd, size_t offset, size_t size)
{
	const struct preload_region_device *pdev;
	pdev = container_of(rd, __typeof__(*pdev), rdev);

	return rdev_mmap(pdev->encapsuled_rdev, offset, size);
}

static int mdev_munmap(const struct region_device *rd __always_unused, void *mapping)
{
	const struct preload_region_device *pdev;
	pdev = container_of(rd, __typeof__(*pdev), rdev);

	return rdev_munmap(pdev->encapsuled_rdev, mapping);
}

static const struct region_device_ops preload_mem_rdev_ro_ops = {
	.mmap = mdev_mmap,
	.munmap = mdev_munmap,
	.readat = mdev_readat,
};

const struct region_device *
mmio_region_device_preload_ro_init(const struct region_device *orig_rdev)
{
	static struct preload_region_device ctx;

	if (!ctx.encapsuled_rdev) {
		ctx.encapsuled_rdev = orig_rdev;
		memcpy(&ctx.rdev, orig_rdev, sizeof(*orig_rdev));
		ctx.rdev.ops = &preload_mem_rdev_ro_ops;
	}
	return &ctx.rdev;
}
