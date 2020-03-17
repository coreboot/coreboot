/*
 * This file is part of the coreboot project.
 *
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <string.h>
#include "soc/dramc_param.h"

struct dramc_param *get_dramc_param_from_blob(void *blob)
{
	return (struct dramc_param *)blob;
}

int validate_dramc_param(const void *blob)
{
	const struct dramc_param *param = blob;
	const struct dramc_param_header *hdr = &param->header;

	if (hdr->magic != DRAMC_PARAM_HEADER_MAGIC)
		return DRAMC_ERR_INVALID_MAGIC;

	if (hdr->version != DRAMC_PARAM_HEADER_VERSION)
		return DRAMC_ERR_INVALID_VERSION;

	if (hdr->size != sizeof(*param))
		return DRAMC_ERR_INVALID_SIZE;

	return DRAMC_SUCCESS;
}

int is_valid_dramc_param(const void *blob)
{
	return validate_dramc_param(blob) == DRAMC_SUCCESS;
}

int initialize_dramc_param(void *blob, u16 config)
{
	struct dramc_param *param = blob;
	struct dramc_param_header *hdr = &param->header;

	memset(blob, 0, sizeof(*param));
	hdr->magic = DRAMC_PARAM_HEADER_MAGIC;
	hdr->size = sizeof(*param);
	hdr->version = DRAMC_PARAM_HEADER_VERSION;
	hdr->config = config;
	return 0;
}
