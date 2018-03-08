/*
 * Copyright (c) 2014 The Chromium OS Authors. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE file.
 */

#include <console/console.h>
#include <cbmem.h>
#include <fmap.h>
#include <stdlib.h>
#include <string.h>
#include <timestamp.h>

#include "vpd.h"
#include "lib_vpd.h"
#include "vpd_tables.h"

/* Currently we only support Google VPD 2.0, which has a fixed offset. */
enum {
	GOOGLE_VPD_2_0_OFFSET = 0x600,
	CROSVPD_CBMEM_MAGIC = 0x43524f53,
	CROSVPD_CBMEM_VERSION = 0x0001,
};

struct vpd_gets_arg {
	const uint8_t *key;
	const uint8_t *value;
	int32_t key_len, value_len;
	int matched;
};

struct vpd_cbmem {
	uint32_t magic;
	uint32_t version;
	uint32_t ro_size;
	uint32_t rw_size;
	uint8_t blob[0];
	/* The blob contains both RO and RW data. It starts with RO (0 ..
	 * ro_size) and then RW (ro_size .. ro_size+rw_size).
	 */
};

/* returns the size of data in a VPD 2.0 formatted fmap region, or 0 */
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

static void cbmem_add_cros_vpd(int is_recovery)
{
	struct region_device vpd;
	struct vpd_cbmem *cbmem;
	int32_t ro_vpd_base = 0, rw_vpd_base = 0;
	int32_t ro_vpd_size, rw_vpd_size;

	timestamp_add_now(TS_START_COPYVPD);

	ro_vpd_size = get_vpd_size("RO_VPD", &ro_vpd_base);
	rw_vpd_size = get_vpd_size("RW_VPD", &rw_vpd_base);

	/* no VPD at all? nothing to do then */
	if ((ro_vpd_size == 0) && (rw_vpd_size == 0))
		return;

	cbmem = cbmem_add(CBMEM_ID_VPD, sizeof(*cbmem) + ro_vpd_size +
		rw_vpd_size);
	if (!cbmem) {
		printk(BIOS_ERR, "%s: Failed to allocate CBMEM (%u+%u).\n",
			__func__, ro_vpd_size, rw_vpd_size);
		return;
	}

	cbmem->magic = CROSVPD_CBMEM_MAGIC;
	cbmem->version = CROSVPD_CBMEM_VERSION;
	cbmem->ro_size = 0;
	cbmem->rw_size = 0;

	if (ro_vpd_size) {
		if (fmap_locate_area_as_rdev("RO_VPD", &vpd)) {
			/* shouldn't happen, but let's be extra defensive */
			printk(BIOS_ERR, "%s: No RO_VPD FMAP section.\n",
				__func__);
			return;
		}
		rdev_chain(&vpd, &vpd, GOOGLE_VPD_2_0_OFFSET,
			region_device_sz(&vpd) - GOOGLE_VPD_2_0_OFFSET);


		if (rdev_readat(&vpd, cbmem->blob, ro_vpd_base, ro_vpd_size) ==
			ro_vpd_size) {
			cbmem->ro_size = ro_vpd_size;
		} else {
			printk(BIOS_ERR,
				"%s: Reading RO_VPD FMAP section failed.\n",
				__func__);
			ro_vpd_size = 0;
		}
		timestamp_add_now(TS_END_COPYVPD_RO);
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

		if (rdev_readat(&vpd, cbmem->blob + ro_vpd_size, rw_vpd_base,
			rw_vpd_size) == rw_vpd_size) {
			cbmem->rw_size = rw_vpd_size;
		} else {
			printk(BIOS_ERR,
				"%s: Reading RW_VPD FMAP section failed.\n",
				__func__);
		}
		timestamp_add_now(TS_END_COPYVPD_RW);
	}
}

static int vpd_gets_callback(const uint8_t *key, int32_t key_len,
			       const uint8_t *value, int32_t value_len,
			       void *arg)
{
	struct vpd_gets_arg *result = (struct vpd_gets_arg *)arg;
	if (key_len != result->key_len ||
	    memcmp(key, result->key, key_len) != 0)
		/* Returns VPD_OK to continue parsing. */
		return VPD_OK;

	result->matched = 1;
	result->value = value;
	result->value_len = value_len;
	/* Returns VPD_FAIL to stop parsing. */
	return VPD_FAIL;
}

const void *vpd_find(const char *key, int *size, enum vpd_region region)
{
	struct vpd_gets_arg arg = {0};
	int consumed = 0;
	const struct vpd_cbmem *vpd;

	vpd = cbmem_find(CBMEM_ID_VPD);
	if (!vpd || !vpd->ro_size)
		return NULL;

	arg.key = (const uint8_t *)key;
	arg.key_len = strlen(key);

	if (region == VPD_ANY || region == VPD_RO)
		while (VPD_OK == decodeVpdString(vpd->ro_size, vpd->blob,
		       &consumed, vpd_gets_callback, &arg)) {
		/* Iterate until found or no more entries. */
		}

	if (!arg.matched && region != VPD_RO)
		while (VPD_OK == decodeVpdString(vpd->rw_size,
		       vpd->blob + vpd->ro_size, &consumed,
		       vpd_gets_callback, &arg)) {
		/* Iterate until found or no more entries. */
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

	if (size > (string_size + 1)) {
		memcpy(buffer, string_address, string_size);
		buffer[string_size] = '\0';
	} else {
		memcpy(buffer, string_address, size - 1);
		buffer[size - 1] = '\0';
	}
	return buffer;
}

RAMSTAGE_CBMEM_INIT_HOOK(cbmem_add_cros_vpd)
