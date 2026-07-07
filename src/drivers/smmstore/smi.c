/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <commonlib/region.h>
#include <cpu/x86/smm.h>
#include <smmstore.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Check that the given range is legal.
 *
 * Legal means:
 *  - not pointing into SMRAM
 *
 * returns 0 on success, -1 on failure
 */
static int range_check(void *start, size_t size)
{
	if (smm_points_to_smram(start, size))
		return -1;

	return 0;
}

uint32_t smmstore_exec(uint8_t command, void *param)
{
	uint32_t ret = SMMSTORE_RET_FAILURE;
	static bool initialized = false;

	if (smmstore_preprocess_cmd(&command, param))
		return SMMSTORE_RET_SUCCESS;

	if (!param)
		return SMMSTORE_RET_FAILURE;

	if (!initialized) {
		uintptr_t base;
		size_t size;
		smm_get_smmstore_com_buffer(&base, &size);

		if (smmstore_init((void *)base, size))
			return SMMSTORE_RET_FAILURE;
		initialized = true;
	}

	switch (command) {
	case SMMSTORE_CMD_RAW_READ: {
		printk(BIOS_DEBUG, "Raw read from SMM store, param = %p\n", param);
		struct smmstore_params_raw_read *params = param;

		if (range_check(params, sizeof(*params)) != 0)
			break;

		if (smmstore_rawread_region(params->block_id, params->bufoffset,
					    params->bufsize) == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}
	case SMMSTORE_CMD_RAW_WRITE: {
		printk(BIOS_DEBUG, "Raw write to SMM store, param = %p\n", param);
		struct smmstore_params_raw_write *params = param;

		if (range_check(params, sizeof(*params)) != 0)
			break;

		if (smmstore_rawwrite_region(params->block_id, params->bufoffset,
					     params->bufsize) == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}
	case SMMSTORE_CMD_RAW_CLEAR: {
		printk(BIOS_DEBUG, "Raw clear SMM store, param = %p\n", param);
		struct smmstore_params_raw_clear *params = param;

		if (range_check(params, sizeof(*params)) != 0)
			break;

		if (smmstore_rawclear_region(params->block_id) == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}
	default:
		printk(BIOS_DEBUG, "Unknown SMM store command: 0x%02x\n", command);
		ret = SMMSTORE_RET_UNSUPPORTED;
		break;
	}

	return ret;
}
