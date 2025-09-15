/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <arch/mmu.h>
#include <cbfs.h>
#include <cbmem.h>
#include <commonlib/bsd/mem_chip_info.h>
#include <console/cbmem_console.h>
#include <console/console.h>
#include <fmap.h>
#include <mrc_cache.h>
#include <option.h>
#include <reset.h>
#include <security/vboot/misc.h>
#include <soc/mmu.h>
#include <soc/mmu_common.h>
#include <soc/qclib_common.h>
#include <soc/symbols_common.h>
#include <string.h>
#include <vb2_api.h>

#define QCLIB_VERSION 0

/* store QcLib return data until CBMEM_CREATION_HOOK runs */
static struct mem_chip_info *mem_chip_info;

static void write_mem_chip_information(struct qclib_cb_if_table_entry *te)
{
	struct mem_chip_info *info = (void *)te->blob_address;
	if (te->size > sizeof(struct mem_chip_info) &&
	    te->size == mem_chip_info_size(info->num_entries)) {
		/* Save mem_chip_info in global variable ahead of hook running */
		mem_chip_info = info;
	}
}

static void add_mem_chip_info(int unused)
{
	void *mem_region_base = NULL;
	size_t size;

	if (!mem_chip_info || !mem_chip_info->num_entries ||
	    mem_chip_info->struct_version != MEM_CHIP_STRUCT_VERSION) {
		printk(BIOS_ERR, "Did not receive valid mem_chip_info from QcLib!\n");
		return;
	}

	size = mem_chip_info_size(mem_chip_info->num_entries);

	/* Add cbmem table */
	mem_region_base = cbmem_add(CBMEM_ID_MEM_CHIP_INFO, size);
	ASSERT(mem_region_base != NULL);

	/* Migrate the data into CBMEM */
	memcpy(mem_region_base, mem_chip_info, size);
}

CBMEM_CREATION_HOOK(add_mem_chip_info);

struct qclib_cb_if_table qclib_cb_if_table;

static inline void init_qclib_cb_if_table(struct qclib_cb_if_table *tbl)
{
	if (!tbl)
		return;

	memcpy(tbl->magic, QCLIB_MAGIC_NUMBER, sizeof(tbl->magic));
	tbl->version = QCLIB_INTERFACE_VERSION;
	tbl->num_entries = 0;
	tbl->max_entries = QCLIB_MAX_NUMBER_OF_ENTRIES;
	tbl->global_attributes = 0;
	tbl->reserved = 0;
}

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
	case QCLIB_CBFS_DTB:
		return CONFIG_CBFS_PREFIX "/dtb";
	case QCLIB_CBFS_CPR:
		return CONFIG_CBFS_PREFIX "/cpr";
	case QCLIB_CBFS_SHRM_META:
		return CONFIG_CBFS_PREFIX "/shrm_meta";
	case QCLIB_CBFS_AOP_META:
		return CONFIG_CBFS_PREFIX "/aop_meta";
	case QCLIB_CBFS_AOP_DEVCFG_META:
		return CONFIG_CBFS_PREFIX "/aop_devcfg_meta";
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
	ddr_size = te->size;
	*ddr_region = region_create(te->blob_address, ddr_size * MiB);

	/* Use DDR info to configure MMU */
	qc_mmu_dram_config_post_dram_init(region_sz(ddr_region));
}

static void write_qclib_log_to_cbmemc(struct qclib_cb_if_table_entry *te)
{
	int i;
	char *ptr = (char *)te->blob_address;

	for (i = 0; i < te->size; i++) {
		char c = *ptr++;
		if (c != '\r')
			__cbmemc_tx_byte(c);
	}
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

__weak int qclib_soc_override(struct qclib_cb_if_table *table) { return 0; }

static bool qclib_debug_log_level(void)
{
	return get_uint_option("qclib_debug_level", 1);
}

struct prog qclib; /* This will be re-used by qclib_rerun() */

static void qclib_prepare_and_run(void)
{
	struct mmu_context pre_qclib_mmu_context;
	int i;

	/* output area, QCLib copies console log buffer out */
	if (CONFIG(CONSOLE_CBMEM))
		qclib_add_if_table_entry(QCLIB_TE_QCLIB_LOG_BUFFER,
				_qclib_serial_log,
				REGION_SIZE(qclib_serial_log), 0);

	/* Enable QCLib serial output, if below condition is met */
	if (CONFIG(CONSOLE_SERIAL) && qclib_debug_log_level())
		qclib_cb_if_table.global_attributes =
			QCLIB_GA_ENABLE_UART_LOGGING;

	dump_te_table();

	printk(BIOS_DEBUG, "Global Attributes[%#x]..Table Entries Count[%d]\n",
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

	printk(BIOS_DEBUG, "QCLib completed\n\n\n");
}

void qclib_load_and_run(void)
{
	ssize_t data_size;

	/* zero ddr_information SRAM region, needs new data each boot */
	memset(ddr_region, 0, sizeof(struct region));

	init_qclib_cb_if_table(&qclib_cb_if_table);

	/* output area, QCLib fills in DDR details */
	qclib_add_if_table_entry(QCLIB_TE_DDR_INFORMATION, NULL, 0, 0);

	/* Attempt to load DDR Training Blob */
	data_size = mrc_cache_load_current(MRC_TRAINING_DATA, QCLIB_VERSION,
					   _ddr_training, REGION_SIZE(ddr_training));
	if (data_size < 0) {
		printk(BIOS_ERR, "Unable to load previous training data.\n");
		memset(_ddr_training, 0, REGION_SIZE(ddr_training));
		data_size = REGION_SIZE(ddr_training);
	}
	qclib_add_if_table_entry(QCLIB_TE_DDR_TRAINING_DATA,
			_ddr_training, data_size, 0);

	/* Address and size of this entry will be filled in by QcLib. */
	qclib_add_if_table_entry(QCLIB_TE_MEM_CHIP_INFO, NULL, 0, 0);

	if (_pmic) {
		/* Attempt to load PMICCFG Blob */
		data_size = cbfs_load(qclib_file(QCLIB_CBFS_PMICCFG),
				_pmic, REGION_SIZE(pmic));
		if (!data_size) {
			printk(BIOS_ERR, "[%s] /pmiccfg failed\n", __func__);
			goto fail;
		}
		qclib_add_if_table_entry(QCLIB_TE_PMIC_SETTINGS, _pmic, data_size, 0);
	}

	/* Attempt to load DCB Blob */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_DCB),
			_dcb, REGION_SIZE(dcb));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /dcb failed\n", __func__);
		goto fail;
	}
	qclib_add_if_table_entry(QCLIB_TE_DCB_SETTINGS, _dcb, data_size, 0);

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

	/* hook for SoC specific binary blob loads */
	if (qclib_soc_override(&qclib_cb_if_table)) {
		printk(BIOS_ERR, "qclib_soc_override failed\n");
		goto fail;
	}

	/* Attempt to load QCLib elf */
	qclib = (struct prog)
		PROG_INIT(PROG_REFCODE, qclib_file(QCLIB_CBFS_QCLIB));

	if (cbfs_prog_stage_load(&qclib))
		goto fail;

	prog_set_entry(&qclib, prog_entry(&qclib), &qclib_cb_if_table);

	/* Set up the system and jump into QcLib */
	printk(BIOS_DEBUG, "\n\n\nEnter QcLib to Initialize DDR and bring up SHRM\n");
	qclib_prepare_and_run();

	/* confirm that we received valid ddr information from QCLib */
	assert((uintptr_t)_dram == region_offset(ddr_region) &&
		region_sz(ddr_region) >= (u8 *)cbmem_top() - _dram);

	return;

fail:
	die("Couldn't run QCLib.\n");
}

void qclib_rerun(void)
{
	ssize_t data_size;

	assert(prog_type(&qclib) == PROG_REFCODE)

	init_qclib_cb_if_table(&qclib_cb_if_table);

	struct prog aop_cfg_fw_prog =
				PROG_INIT(PROG_PAYLOAD, CONFIG_CBFS_PREFIX "/aop_cfg");

	if (!selfload(&aop_cfg_fw_prog))
		die("SOC image: AOP load failed");

	/* Attempt to load aop_meta Blob (reuse the qc_blob_meta region). */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_AOP_META),
			_qc_blob_meta, REGION_SIZE(qc_blob_meta));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /aop_meta failed\n", __func__);
		goto fail;
	}

	qclib_add_if_table_entry(QCLIB_TE_AOP_META_SETTINGS, _qc_blob_meta, data_size, 0);

	/* Attempt to load aop_devcfg_meta Blob. */
	data_size = cbfs_load(qclib_file(QCLIB_CBFS_AOP_DEVCFG_META),
			_aop_blob_meta, REGION_SIZE(aop_blob_meta));
	if (!data_size) {
		printk(BIOS_ERR, "[%s] /aop_devcfg_meta failed\n", __func__);
		goto fail;
	}

	qclib_add_if_table_entry(QCLIB_TE_AOP_DEVCFG_META_SETTINGS, _aop_blob_meta, data_size, 0);

	/* Set up the system and jump into QcLib */
	printk(BIOS_DEBUG, "\n\n\nRe-enter QCLib to bring up AOP\n");
	qclib_prepare_and_run();

	return;

fail:
	die("Couldn't reload QCLib.\n");
}
