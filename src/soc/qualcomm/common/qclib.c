/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/cbmem_console.h>
#include <cbmem.h>
#include <string.h>
#include <fmap.h>
#include <assert.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <console/console.h>
#include <mrc_cache.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/qclib_common.h>
#include <soc/symbols_common.h>
#include <security/vboot/misc.h>
#include <vb2_api.h>
#include <commonlib/bsd/mem_chip_info.h>
#include <include/reset.h>

#define QCLIB_VERSION 0

/* store QcLib return data until CBMEM_CREATION_HOOK runs */
static void *mem_chip_addr;

static void write_mem_chip_information(struct qclib_cb_if_table_entry *te)
{
	if (te->size > sizeof(struct mem_chip_info) &&
	    te->size == mem_chip_info_size((void *)te->blob_address)) {
		/* Save mem_chip_addr in global variable ahead of hook running */
		mem_chip_addr = (void *)te->blob_address;
	}
}

static void add_mem_chip_info(int unused)
{
	void *mem_region_base = NULL;
	size_t size;

	if (!mem_chip_addr) {
		printk(BIOS_ERR, "Did not receive valid mem_chip_info from QcLib!");
		return;
	}

	size = mem_chip_info_size(mem_chip_addr);

	/* Add cbmem table */
	mem_region_base = cbmem_add(CBMEM_ID_MEM_CHIP_INFO, size);
	ASSERT(mem_region_base != NULL);

	/* Migrate the data into CBMEM */
	memcpy(mem_region_base, mem_chip_addr, size);
}

CBMEM_CREATION_HOOK(add_mem_chip_info);

struct qclib_cb_if_table qclib_cb_if_table = {
	.magic = QCLIB_MAGIC_NUMBER,
	.version = QCLIB_INTERFACE_VERSION,
	.num_entries = 0,
	.max_entries = QCLIB_MAX_NUMBER_OF_ENTRIES,
	.global_attributes = 0,
	.reserved = 0,
};

const char *qclib_file_default(enum qclib_cbfs_file file)
{
	switch (file) {
	case QCLIB_CBFS_PMICCFG:
		return CONFIG_CBFS_PREFIX "/pmiccfg";
	case QCLIB_CBFS_QCSDI:
		return CONFIG_CBFS_PREFIX "/qcsdi";
	case QCLIB_CBFS_QCLIB:
		return CONFIG_CBFS_PREFIX "/qclib";
	case QCLIB_CBFS_DCB:
		return CONFIG_CBFS_PREFIX "/dcb";
	default:
		die("unknown QcLib file %d", file);
	}
}

const char *qclib_file(enum qclib_cbfs_file file)
	__attribute__((weak, alias("qclib_file_default")));

void qclib_add_if_table_entry(const char *name, void *base,
				uint32_t size, uint32_t attrs)
{
	struct qclib_cb_if_table_entry *te =
		&qclib_cb_if_table.te[qclib_cb_if_table.num_entries++];
	assert(qclib_cb_if_table.num_entries <= qclib_cb_if_table.max_entries);
	strncpy(te->name, name, sizeof(te->name) - 1);
	te->blob_address = (uintptr_t)base;
	te->size = size;
	te->blob_attributes = attrs;
}

static void write_ddr_information(struct qclib_cb_if_table_entry *te)
{
	uint64_t ddr_size;

	/* Save DDR info in SRAM region to share with ramstage */
	ddr_region->offset = te->blob_address;
	ddr_size = te->size;
	ddr_region->size = ddr_size * MiB;

	/* Use DDR info to configure MMU */
	qc_mmu_dram_config_post_dram_init((void *)ddr_region->offset,
		(size_t)ddr_region->size);
}

static void write_qclib_log_to_cbmemc(struct qclib_cb_if_table_entry *te)
{
	int i;
	char *ptr = (char *)te->blob_address;

	for (i = 0; i < te->size; i++)
		__cbmemc_tx_byte(*ptr++);
}

static void write_table_entry(struct qclib_cb_if_table_entry *te)
{

	if (!strncmp(QCLIB_TE_DDR_INFORMATION, te->name,
			sizeof(te->name))) {

		write_ddr_information(te);

	} else if (!strncmp(QCLIB_TE_DDR_TRAINING_DATA, te->name,
			sizeof(te->name))) {
		assert(!mrc_cache_stash_data(MRC_TRAINING_DATA, QCLIB_VERSION,
					     (const void *)te->blob_address, te->size));

	} else if (!strncmp(QCLIB_TE_LIMITS_CFG_DATA, te->name,
			sizeof(te->name))) {

		assert(fmap_overwrite_area(QCLIB_FR_LIMITS_CFG_DATA,
			(const void *)te->blob_address, te->size));

	} else if (!strncmp(QCLIB_TE_QCLIB_LOG_BUFFER, te->name,
			sizeof(te->name))) {

		write_qclib_log_to_cbmemc(te);

	} else if (!strncmp(QCLIB_TE_MEM_CHIP_INFO, te->name,
			sizeof(te->name))) {
		write_mem_chip_information(te);

	} else {

		printk(BIOS_WARNING, "%s write not implemented\n", te->name);
		printk(BIOS_WARNING, "  blob_address[%llx]..size[%x]\n",
			te->blob_address, te->size);

	}
}

static void dump_te_table(void)
{
	struct qclib_cb_if_table_entry *te;
	int i;

	for (i = 0; i < qclib_cb_if_table.num_entries; i++) {
		te = &qclib_cb_if_table.te[i];
		printk(BIOS_DEBUG, "[%s][%llx][%x][%x]\n",
			te->name, te->blob_address,
			te->size, te->blob_attributes);
	}
}

__weak int qclib_soc_blob_load(void) { return 0; }

void qclib_load_and_run(void)
{
	int i;
	ssize_t data_size;
	struct mmu_context pre_qclib_mmu_context;

	/* zero ddr_information SRAM region, needs new data each boot */
	memset(ddr_region, 0, sizeof(struct region));

	/* output area, QCLib copies console log buffer out */
	if (CONFIG(CONSOLE_CBMEM))
		qclib_add_if_table_entry(QCLIB_TE_QCLIB_LOG_BUFFER,
				_qclib_serial_log,
				REGION_SIZE(qclib_serial_log), 0);

	/* output area, QCLib fills in DDR details */
	qclib_add_if_table_entry(QCLIB_TE_DDR_INFORMATION, NULL, 0, 0);

	/* Attempt to load DDR Training Blob */
	data_size = mrc_cache_load_current(MRC_TRAINING_DATA, QCLIB_VERSION,
					   _ddr_training, REGION_SIZE(ddr_training));
	if (data_size < 0) {
		printk(BIOS_ERR, "Unable to load previous training data.\n");
		memset(_ddr_training, 0, REGION_SIZE(ddr_training));
	}
	qclib_add_if_table_entry(QCLIB_TE_DDR_TRAINING_DATA,
				 _ddr_training, REGION_SIZE(ddr_training), 0);

	/* Address and size of this entry will be filled in by QcLib. */
	qclib_add_if_table_entry(QCLIB_TE_MEM_CHIP_INFO, NULL, 0, 0);

	/* Attempt to load PMICCFG Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_PMICCFG),
			_pmic, REGION_SIZE(pmic));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /pmiccfg failed\n", __func__);
		goto fail;
	}
	qclib_add_if_table_entry(QCLIB_TE_PMIC_SETTINGS, _pmic, data_size, 0);

	/* Attempt to load DCB Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_DCB),
			_dcb, REGION_SIZE(dcb));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /dcb failed\n", __func__);
		goto fail;
	}
	qclib_add_if_table_entry(QCLIB_TE_DCB_SETTINGS, _dcb, data_size, 0);

	/* hook for SoC specific binary blob loads */
	if (qclib_soc_blob_load()) {
		printk(BIOS_ERR, "qclib_soc_blob_load failed\n");
		goto fail;
	}

	/* Enable QCLib serial output, based on Kconfig */
	if (CONFIG(CONSOLE_SERIAL))
		qclib_cb_if_table.global_attributes =
			QCLIB_GA_ENABLE_UART_LOGGING;

	if (CONFIG(QC_SDI_ENABLE) && (!CONFIG(VBOOT) ||
		!vboot_is_gbb_flag_set(VB2_GBB_FLAG_RUNNING_FAFT))) {
		struct prog qcsdi =
			PROG_INIT(PROG_REFCODE,
				qclib_file(QCLIB_CBFS_QCSDI));

		/* Attempt to load QCSDI elf */
		if (cbfs_prog_stage_load(&qcsdi))
			goto fail;

		qclib_add_if_table_entry(QCLIB_TE_QCSDI,
			prog_entry(&qcsdi), prog_size(&qcsdi), 0);
		printk(BIOS_INFO, "qcsdi.entry[%p]\n", qcsdi.entry);
	}

	dump_te_table();

	/* Attempt to load QCLib elf */
	struct prog qclib =
		PROG_INIT(PROG_REFCODE, qclib_file(QCLIB_CBFS_QCLIB));

	if (cbfs_prog_stage_load(&qclib))
		goto fail;

	prog_set_entry(&qclib, prog_entry(&qclib), &qclib_cb_if_table);

	printk(BIOS_DEBUG, "\n\n\nQCLib is about to Initialize DDR\n");
	printk(BIOS_DEBUG, "Global Attributes[%x]..Table Entries Count[%d]\n",
		qclib_cb_if_table.global_attributes,
		qclib_cb_if_table.num_entries);
	printk(BIOS_DEBUG, "Jumping to QCLib code at %p(%p)\n",
		prog_entry(&qclib), prog_entry_arg(&qclib));

	/* back-up mmu context before disabling mmu and executing qclib */
	mmu_save_context(&pre_qclib_mmu_context);
	/* disable mmu before jumping to qclib. mmu_disable also
	   flushes and invalidates caches before disabling mmu. */
	mmu_disable();

	prog_run(&qclib);

	/* Before returning, QCLib flushes cache and disables mmu.
	   Explicitly disable mmu (flush, invalidate and disable mmu)
	   before re-enabling mmu with backed-up mmu context */
	mmu_disable();
	mmu_restore_context(&pre_qclib_mmu_context);
	mmu_enable();

	if (qclib_cb_if_table.global_attributes & QCLIB_GA_FORCE_COLD_REBOOT) {
		printk(BIOS_NOTICE, "QcLib requested cold reboot\n");
		board_reset();
	}

	/* step through I/F table, handling return values */
	for (i = 0; i < qclib_cb_if_table.num_entries; i++)
		if (qclib_cb_if_table.te[i].blob_attributes &
				QCLIB_BA_SAVE_TO_STORAGE)
			write_table_entry(&qclib_cb_if_table.te[i]);

	/* confirm that we received valid ddr information from QCLib */
	assert((uintptr_t)_dram == region_offset(ddr_region) &&
		region_sz(ddr_region) >= (u8 *)cbmem_top() - _dram);

	printk(BIOS_DEBUG, "QCLib completed\n\n\n");

	return;

fail:
	die("Couldn't run QCLib.\n");
}
