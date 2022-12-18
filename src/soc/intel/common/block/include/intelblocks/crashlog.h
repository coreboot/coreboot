/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_CRASHLOG_H
#define SOC_INTEL_COMMON_BLOCK_CRASHLOG_H

#include <fsp/util.h>
#include <types.h>

/* PMC  CrashLog Command */
#define PMC_IPC_CMD_CRASHLOG			0xA6
#define PMC_IPC_CMD_ID_CRASHLOG_DISCOVERY	0x01
#define PMC_IPC_CMD_ID_CRASHLOG_DISABLE		0x02
#define PMC_IPC_CMD_ID_CRASHLOG_ERASE		0x04
#define PMC_IPC_CMD_ID_CRASHLOG_ON_RESET	0x05
#define PMC_IPC_CMD_ID_CRASHLOG_RE_ARM_ON_RESET 0x06

/* CPU CrashLog Mailbox commands */
#define CPU_CRASHLOG_CMD_DISABLE		0
#define CPU_CRASHLOG_CMD_CLEAR			2
#define CPU_CRASHLOG_MAILBOX_WAIT_STALL		1
#define CPU_CRASHLOG_MAILBOX_WAIT_TIMEOUT	1000
#define CPU_CRASHLOG_DISC_TAB_GUID_VALID	0x1600

#define CRASHLOG_SIZE_DEBUG_PURPOSE		0x640

/* PMC crashlog discovery structs */
typedef union {
	struct {
		u16 offset;
		u16 size;
	} bits;
	u32 data;
} __packed pmc_crashlog_discov_region_t;

typedef struct {
	u32 numb_regions;
	pmc_crashlog_discov_region_t regions[256];
} __packed pmc_crashlog_desc_table_t;

typedef union {
	struct {
		u32 supported		:1;
		u32 dis			:1;
		u32 discov_mechanism	:2;
		u32 size		:12;
		u32 base_offset		:16; /* Start offset of CrashLog in PMC SSRAM */
		u32 rsv			:16;
		u32 desc_tabl_offset	:16; /* start offset of descriptor table */
	} bits;
	u64  val_64_bits;
} __packed pmc_ipc_discovery_buf_t;


/* CPU/TELEMETRY crashlog discovery structs */

typedef union {
	struct {
		u32 pcie_cap_id		:16;
		u32 cap_ver		:4;
		u32 next_cap_offset	:12;
	} fields;
	u32 data;
} __packed cap_data_t;

typedef union {
	struct {
		u64 devsc_ven_id	:16;
		u64 devsc_ver		:4;
		u64 devsc_len		:12;
		u64 devsc_id		:16;
		u64 num_entries		:8; /*Numb of telemetry aggregators in lookup table. */
		u64 entries_size	:8; /* Entry Size in DWORDS */
	} fields;
	u64 data_64;
	u32 data_32[2];
} __packed devsc_data_t;

typedef union {
	struct {
		u32 t_bir_q			:3;  /* tBIR, The BAR to be used */
		u32 discovery_table_offset	:29;
	} fields;
	u32 data;
} __packed discovery_data_t;

typedef struct {
	cap_data_t	cap_data;
	devsc_data_t	devsc_data;
	discovery_data_t  discovery_data;
} __packed tel_crashlog_devsc_cap_t;

typedef union {
	struct {
		u64 access_type	:4;
		u64 crash_type	:4;
		u64 count	:8;
		u64 reserved	:16;
		u64 guid	:32;
	} fields;
	u64 data;
} __packed cpu_crashlog_header_t;


/* Structures for CPU CrashLog mailbox interface */
typedef union {
	struct {
		u32 command	:8;
		u32 param	:8;
		u32 reserved	:15;
		u32 busy	:1;
	} fields;
	u32 data;
} __packed cpu_crashlog_mailbox_t;

typedef union {
	struct {
		u32 offset	:32;
		u32 size	:16;
		u32 reserved	:16;
	} fields;
	u64 data;
} __packed cpu_crashlog_buffer_info_t;

typedef struct {
	cpu_crashlog_header_t header;
	cpu_crashlog_mailbox_t cmd_mailbox;
	u32 mailbox_data;
	cpu_crashlog_buffer_info_t buffers[256];
} __packed cpu_crashlog_discovery_table_t;

int cl_get_cpu_record_size(void);
int cl_get_pmc_record_size(void);
u32 cl_get_cpu_bar_addr(void);
u32 cl_get_cpu_tmp_bar(void);
u32 cl_get_cpu_mb_int_addr(void);
int cl_get_total_data_size(void);
bool cl_pmc_sram_has_mmio_access(void);
bool cpu_crashlog_support(void);
bool pmc_crashlog_support(void);
bool cl_cpu_data_present(void);
bool cl_pmc_data_present(void);
void cl_get_cpu_sram_data(void);
void cl_get_pmc_sram_data(void);
void reset_discovery_buffers(void);
void update_new_pmc_crashlog_size(u32 *pmc_crash_size);
void update_new_cpu_crashlog_size(u32 *cpu_crash_size);
pmc_ipc_discovery_buf_t cl_get_pmc_discovery_buf(void);
pmc_crashlog_desc_table_t cl_get_pmc_descriptor_table(void);
cpu_crashlog_discovery_table_t cl_get_cpu_discovery_table(void);
u32 cl_gen_cpu_bar_addr(void);
int cpu_cl_poll_mailbox_ready(u32 cl_mailbox_addr);
int cpu_cl_mailbox_cmd(u8 cmd, u8 param);
int cpu_cl_clear_data(void);
int pmc_cl_gen_descriptor_table(u32 desc_table_addr,
				pmc_crashlog_desc_table_t *descriptor_table);
bool pmc_cl_discovery(void);
bool cpu_cl_discovery(void);
int cl_pmc_re_arm_after_reset(void);
int cl_pmc_clear(void);
int cl_pmc_en_gen_on_all_reboot(void);
bool discover_crashlog(void);
bool cl_copy_data_from_sram(u32 src_bar,
			u32 offset,
			u32 size,
			u32 *dest_addr,
			u32 buffer_index,
			bool pmc_sram);
void collect_pmc_and_cpu_crashlog_from_srams(void);
bool cl_fill_cpu_records(void *cl_record);
bool cl_fill_pmc_records(void *cl_record);

static const EFI_GUID FW_ERR_SECTION_GUID = {
	0x81212a96, 0x09ed, 0x4996,
	{ 0x94, 0x71, 0x8d, 0x72, 0x9c, 0x8e, 0x69, 0xed }
};

#endif /* SOC_INTEL_COMMON_BLOCK_CRASHLOG */
