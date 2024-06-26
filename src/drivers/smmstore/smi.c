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

static uint32_t smmstorev1_exec(uint8_t command, void *param)
{
	uint32_t ret = SMMSTORE_RET_FAILURE;

	switch (command) {
	case SMMSTORE_CMD_READ: {
		printk(BIOS_DEBUG, "Reading from SMM store\n");
		struct smmstore_params_read *params = param;

		if (range_check(params, sizeof(*params)) != 0)
			break;

		if (range_check(params->buf, params->bufsize) != 0)
			break;

		if (smmstore_read_region(params->buf, &params->bufsize) == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}

	case SMMSTORE_CMD_APPEND: {
		printk(BIOS_DEBUG, "Appending into SMM store\n");
		struct smmstore_params_append *params = param;

		if (range_check(params, sizeof(*params)) != 0)
			break;
		if (range_check(params->key, params->keysize) != 0)
			break;
		if (range_check(params->val, params->valsize) != 0)
			break;

		if (smmstore_append_data(params->key, params->keysize,
					 params->val, params->valsize) == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}

	case SMMSTORE_CMD_CLEAR: {
		if (smmstore_clear_region() == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}
	default:
		printk(BIOS_DEBUG,
		       "Unknown SMM store v1 command: 0x%02x\n", command);
		ret = SMMSTORE_RET_UNSUPPORTED;
		break;
	}

	return ret;
}

static uint32_t smmstorev2_exec(uint8_t command, void *param)
{
	uint32_t ret = SMMSTORE_RET_FAILURE;
	static bool initialized = false;

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
		printk(BIOS_DEBUG,
			"Unknown SMM store v2 command: 0x%02x\n", command);
		ret = SMMSTORE_RET_UNSUPPORTED;
		break;
	}

	return ret;
}

uint32_t smmstore_exec(uint8_t command, void *param)
{
	if (smmstore_preprocess_cmd(&command, param))
		return SMMSTORE_RET_SUCCESS;

	if (command != SMMSTORE_CMD_CLEAR && !param)
		return SMMSTORE_RET_FAILURE;

	if (CONFIG(SMMSTORE_V2))
		return smmstorev2_exec(command, param);
	else
		return smmstorev1_exec(command, param);
}
