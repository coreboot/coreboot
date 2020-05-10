/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <smmstore.h>
#include <stddef.h>
#include <stdint.h>

/*
 * Check that the given range is legal.
 *
 * Legal means:
 *  - not pointing into SMRAM
 *  - ...?
 *
 * returns 0 on success, -1 on failure
 */
static int range_check(void *start, size_t size)
{
	// TODO: fill in
	return 0;
}

/* Param is usually EBX, ret in EAX */
uint32_t smmstore_exec(uint8_t command, void *param)
{
	uint32_t ret = SMMSTORE_RET_FAILURE;

	switch (command) {
	case SMMSTORE_CMD_READ: {
		printk(BIOS_DEBUG, "Reading from SMM store\n");
		struct smmstore_params_read *params = param;

		if (range_check(params->buf, params->bufsize) != 0)
			break;

		if (smmstore_read_region(params->buf, &params->bufsize) == 0)
			ret = SMMSTORE_RET_SUCCESS;
		break;
	}

	case SMMSTORE_CMD_APPEND: {
		printk(BIOS_DEBUG, "Appending into SMM store\n");
		struct smmstore_params_append *params = param;

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
			"Unknown SMM store command: 0x%02x\n", command);
		ret = SMMSTORE_RET_UNSUPPORTED;
		break;
	}

	return ret;
}
