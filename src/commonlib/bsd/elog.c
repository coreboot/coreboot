/* SPDX-License-Identifier: BSD-3-Clause */

#include <commonlib/bsd/elog.h>
#include <stddef.h>

/*
 * verify and validate if header is a valid coreboot Event Log header.
 * return CB_ERR if invalid, otherwise CB_SUCCESS.
 */
enum cb_err elog_verify_header(const struct elog_header *header)
{
	if (header == NULL)
		return CB_ERR;

	if (header->magic != ELOG_SIGNATURE)
		return CB_ERR;

	if (header->version != ELOG_VERSION)
		return CB_ERR;

	if (header->header_size != sizeof(*header))
		return CB_ERR;

	return CB_SUCCESS;
}
