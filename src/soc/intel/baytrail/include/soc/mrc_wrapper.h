/* SPDX-License-Identifier: BSD-3-Clause */
#ifndef _MRC_WRAPPER_H_
#define _MRC_WRAPPER_H_

#define MRC_PARAMS_VER  5

#define NUM_CHANNELS 2

/* Provide generic x86 calling conventions. */
#define ABI_X86 __attribute((regparm(0)))

enum {
	DRAM_INFO_SPD_SMBUS, /* Use the typical SPD smbus access. */
	DRAM_INFO_SPD_MEM,   /* SPD info in memory. */
	DRAM_INFO_DETAILED,  /* Timing info not in SPD format. */
};

enum dram_type {
	DRAM_DDR3,
	DRAM_DDR3L,
	DRAM_LPDDR3,
};

/* Errors returned by the MRC wrapper. */
enum mrc_wrapper_error {
	INVALID_VER = -1,
	INVALID_DRAM_TYPE = -2,
	INVALID_SLEEP_MODE = -3,
	PLATFORM_SETTINGS_FAIL = -4,
	DIMM_DETECTION_FAILURE = -5,
	MEMORY_CONFIG_FAILURE = -6,
	INVALID_CPU_ODT_SETTING = -7,
	INVALID_DRAM_ODT_SETTING = -8,
};

struct mrc_mainboard_params {
	int dram_type;
	int dram_info_location; /* DRAM_INFO_* */
	int dram_is_slotted; /* mobo has DRAM slots. */
	/*
	 * The below ODT settings are only honored when !dram_is_slotted.
	 * Additionally, weaker_odt_settings being non-zero causes
	 * cpu_odt_value to not be honored as weaker_odt_settings have a
	 * special training path.
	 */
	int weaker_odt_settings;
	/* Allowed settings: 60, 80, 100, 120, and 150. */
	int cpu_odt_value;
	/* Allowed settings: 60 and 120. */
	int dram_odt_value;
	int spd_addrs[NUM_CHANNELS];
	void *dram_data[NUM_CHANNELS]; /* SPD or Timing specific data. */
} __packed;

struct mrc_params {
	/* Mainboard Inputs */
	int version;

	struct mrc_mainboard_params mainboard;

	void ABI_X86 (*console_out)(unsigned char byte);

	int prev_sleep_state;

	int saved_data_size;
	const void *saved_data;

	int txe_size_mb; /* TXE memory size in megabytes. */
	int rmt_enabled; /* Enable RMT training + prints. */
	int io_hole_mb;  /* Size of IO hole in MiB. */

	/* Outputs */
	void *txe_base_address;
	int data_to_save_size;
	void *data_to_save;
} __packed;

/* Call into wrapper. */
typedef int ABI_X86 (*mrc_wrapper_entry_t)(struct mrc_params *);

#endif /* _MRC_WRAPPER_H_ */
