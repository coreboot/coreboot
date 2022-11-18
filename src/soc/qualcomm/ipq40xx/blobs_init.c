/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/cache.h>
#include <device/mmio.h>
#include <cbfs.h>
#include <console/console.h>
#include <string.h>
#include <program_loading.h>
#include <soc/iomap.h>
#include <soc/soc_services.h>

#include "mbn_header.h"

struct cdt_info {
	uint32_t	size;		/* size of the whole table */
	uint8_t		*cdt_ptr;	/* pointer to CDT */
};

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

	return blob_mbn;
}

#define DDR_VERSION() ((const char *)"private build")
#define MAX_DDR_VERSION_SIZE 48

typedef struct {
	uint64_t	entry_point; /* Write only for Core Boot */
	uint32_t	elf_class;
} sys_debug_qsee_info_type_t;

typedef struct {
	sys_debug_qsee_info_type_t	*qsee_info;
	uint64_t			sdi_entry; /* Read only for Core Boot */
} sbl_rw_ret_info_t;

sbl_rw_ret_info_t *sbl_rw_ret_info;

int initialize_dram(void)
{
	struct mbn_header *cdt;
	struct cdt_info cdt_header;
	uint32_t sw_entry;
	/*
	 * FIXME: Hard coding the address. Have to somehow get it
	 * automatically
	 */
	void *tzbsp = (uint8_t *)0x87e80000;

	sbl_rw_ret_info_t (*(*ddr_init_function)(struct cdt_info *cdt_header));

	cdt = load_ipq_blob(CONFIG_CDT_MBN);
	ddr_init_function = load_ipq_blob(CONFIG_DDR_MBN);

	if (!cdt || !ddr_init_function) {
		printk(BIOS_ERR, "cdt: %p, ddr_init_function: %p\n",
		       cdt, ddr_init_function);
		die("could not find DDR initialization blobs\n");
	}

	cdt_header.size = cdt->mbn_total_size;
	cdt_header.cdt_ptr = (uint8_t *)(cdt + 1);

	sbl_rw_ret_info = ddr_init_function(&cdt_header);
	if (sbl_rw_ret_info == NULL)
		die("Fail to Initialize DDR\n");

	/*
	 * Once DDR initializer finished, its version can be found at a fixed
	 * address in SRAM.
	 */
	printk(BIOS_INFO, "DDR version %.*s initialized\n",
	       MAX_DDR_VERSION_SIZE, DDR_VERSION());

	printk(BIOS_INFO, "SDI Entry: 0x%llx\n", sbl_rw_ret_info->sdi_entry);
	sw_entry = read32(TCSR_RESET_DEBUG_SW_ENTRY) & 0x1;
	sw_entry |= (sbl_rw_ret_info->sdi_entry & ~0x1);
	write32(TCSR_RESET_DEBUG_SW_ENTRY, sw_entry);
	sbl_rw_ret_info->qsee_info->entry_point = (uint32_t)tzbsp;

	return 0;
}

void start_tzbsp(void)
{
	void *tzbsp = load_ipq_blob(CONFIG_TZ_MBN);

	if (!tzbsp)
		die("could not find or map TZBSP\n");

	printk(BIOS_INFO, "Starting TZBSP\n");

	tz_init_wrapper(0, 0, tzbsp);

}
