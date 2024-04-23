/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __COMMONLIB_SELF_TEST_H__
#define __COMMONLIB_SELF_TEST_H__

/*
 * Self-test ID ranges pre-allocated by SoC vendor:
 * - Intel:     1   - 128
 * - Qualcomm:  129 - 192
 * - MediaTek:  193 - 256
 *
 * Note: Each executed test logs an 8-byte entry (4-byte ID + 4-byte st_status)
 * to CBMEM. The default CBMEM buffer size (4096 bytes) holds up to 512 test
 * execution logs per boot.
 */
#define ST_INVALID_ID			0x00

/* Intel self-test IDs (1 - 128) */
#define ST_INTEL_START			1
#define ST_CPU_INTEL_MICROCODE		(ST_INTEL_START + 0)

/* Qualcomm self-test IDs (129 - 192) */
#define ST_QUALCOMM_START		129

/* MediaTek self-test IDs (193 - 256) */
#define ST_MEDIATEK_START		193

#define ST_ID_TO_NAME_TABLE \
	{ST_INVALID_ID,			"Invalid Test"},\
	{ST_CPU_INTEL_MICROCODE,	"Intel_microcode_patch_loaded"}

#define ST_PASSED	0
#define ST_SKIPPED	1
#define ST_WARNING	2
#define ST_FAILED	3
#define ST_STATUS_MAX	4

/*
 * Structure representing the return status and optional error code of a
 * self-test execution.
 */
struct st_status {
	uint16_t status;
	uint16_t error_code;
};

/*
 * Structure representing a single self-test execution log entry stored in CBMEM
 * (CBMEM_ID_SELFTEST). Payloads or the OS can parse this buffer to inspect
 * test execution results.
 */
struct self_test_log {
	uint32_t id;
	struct st_status result;
};

/*
 * Mapping structure to associate a self-test ID with its human-readable name,
 * used for console logging and debugging.
 */
struct st_id_to_name {
	uint32_t id;
	const char *name;
};

/*
 * Human-readable string representations for each self-test status code,
 * indexed by st_status value.
 */
#define ST_POPULATE_MAP_ST(a) [ST_##a] = #a
static const char * const st_status_str[] = {
	ST_POPULATE_MAP_ST(PASSED),
	ST_POPULATE_MAP_ST(SKIPPED),
	ST_POPULATE_MAP_ST(WARNING),
	ST_POPULATE_MAP_ST(FAILED),
	[ST_STATUS_MAX] = NULL
};

/*
 * Lookup table of all available self-tests and their names, populated
 * from ST_ID_TO_NAME_TABLE.
 */
static const struct st_id_to_name st_ids[] = { ST_ID_TO_NAME_TABLE };

#endif /* __COMMONLIB_SELF_TEST_H__ */
