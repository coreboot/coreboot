/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <string.h>
#include <soc/dramc_param.h>

#define print(_x_...) printk(BIOS_INFO, _x_)

struct dramc_param *get_dramc_param_from_blob(void *blob)
{
	return (struct dramc_param *)blob;
}

static const char *const status_strings[] = {
	[DRAMC_SUCCESS] = "success",
	[DRAMC_ERR_INVALID_VERSION] = "invalid version",
	[DRAMC_ERR_INVALID_SIZE] = "invalid size",
	[DRAMC_ERR_INVALID_FLAGS] = "invalid flags",
	[DRAMC_ERR_RECALIBRATE] = "full calibration needed",
	[DRAMC_ERR_INIT_DRAM] = "calibration error",
	[DRAMC_ERR_COMPLEX_RW_MEM_TEST] = "mem test failure",
	[DRAMC_ERR_1ST_COMPLEX_RW_MEM_TEST] = "1st mem test failure",
	[DRAMC_ERR_2ND_COMPLEX_RW_MEM_TEST] = "2nd mem test failure",
	[DRAMC_ERR_FAST_CALIBRATION] = "fast calibration error",
};

const char *get_status_string(u16 status)
{
	const char *s = NULL;
	if (status < ARRAY_SIZE(status_strings))
		s = status_strings[status];
	return s ? s : "UNKNOWN STATUS";
}

void dump_param_header(const void *blob)
{
	const struct dramc_param *dparam = blob;
	const struct dramc_param_header *header = &dparam->header;

	print("header.status = %#x\n", header->status);
	print("header.version = %#x (expected: %#x)\n",
	      header->version, DRAMC_PARAM_HEADER_VERSION);
	print("header.size = %#x (expected: %#lx)\n",
	      header->size, sizeof(*dparam));
	print("header.flags = %#x\n", header->flags);
}

int initialize_dramc_param(void *blob)
{
	struct dramc_param *param = blob;
	struct dramc_param_header *hdr = &param->header;

	memset(hdr, 0, sizeof(*hdr));
	hdr->version = DRAMC_PARAM_HEADER_VERSION;
	hdr->size = sizeof(*param);
	return 0;
}
