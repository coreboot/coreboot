/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <assert.h>
#include <console/console.h>
#include <cbmem.h>
#include <fmap.h>
#include <stdlib.h>
#include <string.h>
#include <timestamp.h>

#include "vpd.h"
#include "vpd_decode.h"
#include "vpd_tables.h"

struct vpd_gets_arg {
	const uint8_t *key;
	const uint8_t *value;
	int32_t key_len, value_len;
	int matched;
};

struct vpd_blob vpd_blob;

/*
 * returns the size of data in a VPD 2.0 formatted fmap region, or 0.
 * Also sets *base as the region's base address.
 */
static int32_t get_vpd_size(const char *fmap_name, int32_t *base)
{
	struct google_vpd_info info;
	struct region_device vpd;
	int32_t size;

	if (fmap_locate_area_as_rdev(fmap_name, &vpd)) {
		printk(BIOS_ERR, "%s: No %s FMAP section.\n", __func__,
			fmap_name);
		return 0;
	}

	size = region_device_sz(&vpd);

	if ((size < GOOGLE_VPD_2_0_OFFSET + sizeof(info)) ||
	    rdev_chain(&vpd, &vpd, GOOGLE_VPD_2_0_OFFSET,
			size - GOOGLE_VPD_2_0_OFFSET)) {
		printk(BIOS_ERR, "%s: Too small (%d) for Google VPD 2.0.\n",
		       __func__, size);
		return 0;
	}

	/* Try if we can find a google_vpd_info, otherwise read whole VPD. */
	if (rdev_readat(&vpd, &info, *base, sizeof(info)) != sizeof(info)) {
		printk(BIOS_ERR, "ERROR: Failed to read %s header.\n",
		       fmap_name);
		return 0;
	}

	if (memcmp(info.header.magic, VPD_INFO_MAGIC, sizeof(info.header.magic))
	    == 0 && size >= info.size + sizeof(info)) {
		*base += sizeof(info);
		size = info.size;
	} else if (info.header.tlv.type == VPD_TYPE_TERMINATOR ||
		   info.header.tlv.type == VPD_TYPE_IMPLICIT_TERMINATOR) {
		printk(BIOS_WARNING, "WARNING: %s is uninitialized or empty.\n",
		       fmap_name);
		size = 0;
	} else {
		size -= GOOGLE_VPD_2_0_OFFSET;
	}

	return size;
}

static void vpd_get_blob(void)
{
	int32_t ro_vpd_base = 0;
	int32_t rw_vpd_base = 0;
	int32_t ro_vpd_size = get_vpd_size("RO_VPD", &ro_vpd_base);
	int32_t rw_vpd_size = get_vpd_size("RW_VPD", &rw_vpd_base);

	/* Return if no VPD at all */
	if (ro_vpd_size == 0 && rw_vpd_size == 0)
		return;

	vpd_blob.ro_base = NULL;
	vpd_blob.ro_size = 0;
	vpd_blob.rw_base = NULL;
	vpd_blob.rw_size = 0;

	struct region_device vpd;

	if (ro_vpd_size) {
		if (fmap_locate_area_as_rdev("RO_VPD", &vpd)) {
			/* shouldn't happen, but let's be extra defensive */
			printk(BIOS_ERR, "%s: No RO_VPD FMAP section.\n",
				__func__);
			return;
		}
		rdev_chain(&vpd, &vpd, GOOGLE_VPD_2_0_OFFSET,
			region_device_sz(&vpd) - GOOGLE_VPD_2_0_OFFSET);
		vpd_blob.ro_base = (uint8_t *)(rdev_mmap_full(&vpd) +
			sizeof(struct google_vpd_info));
		vpd_blob.ro_size = ro_vpd_size;
	}
	if (rw_vpd_size) {
		if (fmap_locate_area_as_rdev("RW_VPD", &vpd)) {
			/* shouldn't happen, but let's be extra defensive */
			printk(BIOS_ERR, "%s: No RW_VPD FMAP section.\n",
				__func__);
			return;
		}
		rdev_chain(&vpd, &vpd, GOOGLE_VPD_2_0_OFFSET,
			region_device_sz(&vpd) - GOOGLE_VPD_2_0_OFFSET);
		vpd_blob.rw_base = (uint8_t *)(rdev_mmap_full(&vpd) +
			sizeof(struct google_vpd_info));
		vpd_blob.rw_size = rw_vpd_size;
	}
	vpd_blob.initialized = true;
}

const struct vpd_blob *vpd_load_blob(void)
{
	if (vpd_blob.initialized == false)
		vpd_get_blob();

	return &vpd_blob;
}

static int vpd_gets_callback(const uint8_t *key, uint32_t key_len,
			     const uint8_t *value, uint32_t value_len,
			     void *arg)
{
	struct vpd_gets_arg *result = (struct vpd_gets_arg *)arg;
	if (key_len != result->key_len ||
	    memcmp(key, result->key, key_len) != 0)
		/* Returns VPD_DECODE_OK to continue parsing. */
		return VPD_DECODE_OK;

	result->matched = 1;
	result->value = value;
	result->value_len = value_len;
	/* Returns VPD_DECODE_FAIL to stop parsing. */
	return VPD_DECODE_FAIL;
}

const void *vpd_find(const char *key, int *size, enum vpd_region region)
{
	struct vpd_blob blob = {0};

	vpd_get_buffers(&blob);
	if (blob.ro_size == 0 && blob.rw_size == 0)
		return NULL;

	struct vpd_gets_arg arg = {0};
	uint32_t consumed = 0;

	arg.key = (const uint8_t *)key;
	arg.key_len = strlen(key);

	if ((region == VPD_ANY || region == VPD_RO) && blob.ro_size != 0) {
		while (vpd_decode_string(blob.ro_size, blob.ro_base,
			&consumed, vpd_gets_callback, &arg) == VPD_DECODE_OK) {
		/* Iterate until found or no more entries. */
		}
	}

	if ((!arg.matched && region != VPD_RO) && blob.rw_size != 0) {
		while (vpd_decode_string(blob.rw_size, blob.rw_base,
			&consumed, vpd_gets_callback, &arg) == VPD_DECODE_OK) {
		/* Iterate until found or no more entries. */
		}
	}

	if (!arg.matched)
		return NULL;

	*size = arg.value_len;
	return arg.value;
}

char *vpd_gets(const char *key, char *buffer, int size, enum vpd_region region)
{
	const void *string_address;
	int string_size;

	string_address = vpd_find(key, &string_size, region);

	if (!string_address)
		return NULL;

	assert(size > 0);
	int copy_size = MIN(size - 1, string_size);
	memcpy(buffer, string_address, copy_size);
	buffer[copy_size] = '\0';
	return buffer;
}

/*
 * Find value of boolean type vpd key.
 *
 * During the process, necessary checking is done, such as making
 * sure the value length is 1, and value is either '1' or '0'.
 */
bool vpd_get_bool(const char *key, enum vpd_region region, uint8_t *val)
{
	int size;
	const char *value;

	value = vpd_find(key, &size, region);
	if (!value) {
		printk(BIOS_CRIT, "problem returning from vpd_find.\n");
		return false;
	}

	if (size != 1)
		return false;

	/* Make sure the value is either '1' or '0' */
	if (*value == '1') {
		*val = 1;
		return true;
	} else if (*value == '0') {
		*val = 0;
		return true;
	} else
		return false;
}
