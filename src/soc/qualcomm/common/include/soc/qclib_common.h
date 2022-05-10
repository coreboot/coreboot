/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_QCLIB_COMMON_H__
#define _SOC_QUALCOMM_QCLIB_COMMON_H__

/* coreboot & QCLib I/F definitions */

/* string field lengths */
#define QCLIB_MAGIC_NUMBER_LENGTH 8
#define QCLIB_FMAP_NAME_LENGTH 24
#define QCLIB_TE_NAME_LENGTH 24

/* FMAP_REGION names */
#define QCLIB_FR_LIMITS_CFG_DATA   "RO_LIMITS_CFG"

/* TE_NAME (table entry name) */
#define QCLIB_TE_DDR_INFORMATION   "ddr_information"
#define QCLIB_TE_QCLIB_LOG_BUFFER  "qclib_log_buffer"
#define QCLIB_TE_DCB_SETTINGS      "dcb_settings"
#define QCLIB_TE_CDT_SETTINGS      "cdt_settings"
#define QCLIB_TE_PMIC_SETTINGS     "pmic_settings"
#define QCLIB_TE_DDR_TRAINING_DATA "ddr_training_data"
#define QCLIB_TE_LIMITS_CFG_DATA   "limits_cfg_data"
#define QCLIB_TE_QCSDI             "qcsdi"
#define QCLIB_TE_MEM_CHIP_INFO     "mem_chip_info"

/* BA_BMASK_VALUES (blob_attributes bit mask values) */
#define QCLIB_BA_SAVE_TO_STORAGE 0x00000001

enum qclib_cbfs_file {
	QCLIB_CBFS_PMICCFG,
	QCLIB_CBFS_QCSDI,
	QCLIB_CBFS_QCLIB,
	QCLIB_CBFS_DCB,
	QCLIB_CBFS_MAX
};

struct qclib_cb_if_table_entry {
	char       name[QCLIB_TE_NAME_LENGTH];  /* 0x00 TE_NAME */
	uint64_t   blob_address;		/* 0x18 blob addr in SRAM */
	uint32_t   size;                        /* 0x20 blob size in SRAM */
	uint32_t   blob_attributes;		/* 0x24 BA_BMASK_VALUES */
};

/* GA_BMASK_VALUES (global_attributes bit mask values) */
#define QCLIB_GA_ENABLE_UART_LOGGING   0x00000001

#define QCLIB_INTERFACE_VERSION 0x00000001
#define QCLIB_MAX_NUMBER_OF_ENTRIES 16

#define QCLIB_MAGIC_NUMBER "QCLIB_CB"

struct qclib_cb_if_table {
	char          magic[8];				/* 0x00 */
	uint32_t      version;				/* 0x08 */
	uint32_t      num_entries;			/* 0x0C */
	uint32_t      max_entries;			/* 0x10 */
	uint32_t      global_attributes;		/* 0x14 */
	uint64_t      reserved;				/* 0x18 */
	struct qclib_cb_if_table_entry
		te[QCLIB_MAX_NUMBER_OF_ENTRIES];	/* 0x20 */
};

extern struct qclib_cb_if_table qclib_cb_if_table;

void qclib_add_if_table_entry(const char *name, void *base,
			      uint32_t size, uint32_t attrs);
void qclib_load_and_run(void);
int  qclib_soc_blob_load(void);

const char *qclib_file_default(enum qclib_cbfs_file file);
const char *qclib_file(enum qclib_cbfs_file file);

#endif  // _SOC_QUALCOMM_QCLIB_COMMON_H_
