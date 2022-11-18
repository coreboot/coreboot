/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <device/mmio.h>
#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <timer.h>

#include <soc/iomap.h>
#include <soc/soc_services.h>

#include "mbn_header.h"

static void *load_ipq_blob(const char *file_name)
{
	struct mbn_header *blob_mbn;
	void *blob_dest;
	size_t blob_size;

	blob_mbn = cbfs_map(file_name, &blob_size);
	if (!blob_mbn)
		return NULL;

	/* some sanity checks on the headers */
	if ((blob_mbn->mbn_version != 3) ||
	    (blob_mbn->mbn_total_size > blob_size))
		return NULL;

	blob_dest = (void *)blob_mbn->mbn_destination;
	if (blob_mbn->mbn_destination) {
		/* Copy the blob to the appropriate memory location. */
		memcpy(blob_dest, blob_mbn + 1, blob_mbn->mbn_total_size);
		cache_sync_instructions();
		return blob_dest;
	}

	/*
	 * The blob did not have to be relocated, return its address in CBFS
	 * cache.
	 */
	return blob_mbn + 1;
}

#define DDR_VERSION() ((const char *)0x2a03f600)
#define MAX_DDR_VERSION_SIZE 48

int initialize_dram(void)
{
	void *cdt;
	int (*ddr_init_function)(void *cdt_header);

	cdt = load_ipq_blob("cdt.mbn");
	ddr_init_function = load_ipq_blob("ddr.mbn");

	if (!cdt || !ddr_init_function) {
		printk(BIOS_ERR, "cdt: %p, ddr_init_function: %p\n",
		       cdt, ddr_init_function);
		die("could not find DDR initialization blobs\n");
	}

	if (ddr_init_function(cdt) < 0)
		die("Fail to Initialize DDR\n");

	/*
	 * Once DDR initializer finished, its version can be found at a fixed
	 * address in SRAM.
	 */
	printk(BIOS_INFO, "DDR version %.*s initialized\n",
	       MAX_DDR_VERSION_SIZE, DDR_VERSION());

	return 0;
}

void start_tzbsp(void)
{
	void *tzbsp = load_ipq_blob("tz.mbn");

	if (!tzbsp)
		die("could not find or map TZBSP\n");

	printk(BIOS_INFO, "Starting TZBSP\n");

	tz_init_wrapper(0, 0, tzbsp);
}

/* RPM version is encoded in a 32 bit word at the fixed address */
#define RPM_VERSION() (*((u32 *)(0x00108008)))
void start_rpm(void)
{
	u32 load_addr;
	u32 ready_mask = 1 << 10;
	u32 rpm_version;

	struct stopwatch sw;

	if (read32(RPM_SIGNAL_COOKIE) == RPM_FW_MAGIC_NUM) {
		printk(BIOS_INFO, "RPM appears to have already started\n");
		return;
	}

	load_addr = (u32)load_ipq_blob("rpm.mbn");
	if (!load_addr)
		die("could not find or map RPM code\n");

	printk(BIOS_INFO, "Starting RPM\n");

	/* Clear 'ready' indication. */
	/*
	 * RPM_INT_ACK is clear-on-write type register,
	 * read-modify-write is not recommended.
	 */
	write32(RPM_INT_ACK, ready_mask);

	/* Set RPM entry address */
	write32(RPM_SIGNAL_ENTRY, load_addr);
	/* Set cookie */
	write32(RPM_SIGNAL_COOKIE, RPM_FW_MAGIC_NUM);

	/* Wait for RPM start indication, up to 100ms. */
	stopwatch_init_usecs_expire(&sw, 100000);
	while (!(read32(RPM_INT) & ready_mask))
		if (stopwatch_expired(&sw))
			die("RPM Initialization failed\n");

	/* Acknowledge RPM initialization */
	write32(RPM_INT_ACK, ready_mask);

	/* Report RPM version, it is encoded in a 32 bit value. */
	rpm_version = RPM_VERSION();
	printk(BIOS_INFO, "Started RPM version %d.%d.%d\n",
	       rpm_version >> 24,
	       (rpm_version >> 16) & 0xff,
	       rpm_version & 0xffff);
}
