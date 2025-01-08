/* SPDX-License-Identifier: GPL-2.0-only */

#include <cbfs.h>
#include <endian.h>
#include <option.h>

unsigned int get_uint_option(const char *name, const unsigned int fallback)
{
	size_t size;
	uint64_t value;
	char full_name[CBFS_METADATA_MAX_SIZE];
	snprintf(full_name, sizeof(full_name), "option/%s", name);

	void *p = cbfs_ro_map(full_name, &size);
	if (!p || size < sizeof(value)) {
		value = fallback;
	} else {
		value = le64dec(p);
		cbfs_unmap(p);
	}
	return (unsigned int)value;
}

enum cb_err set_uint_option(const char *name, unsigned int value)
{
	return CB_ERR_NOT_IMPLEMENTED;
}
