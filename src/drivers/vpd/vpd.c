/* SPDX-License-Identifier: BSD-3-Clause */

#include <assert.h>
#include <console/console.h>
#include <cbmem.h>
#include <ctype.h>
#include <fmap.h>
#include <program_loading.h>
#include <string.h>
#include <timestamp.h>
#include <types.h>

#include "vpd.h"
#include "vpd_decode.h"
#include "vpd_tables.h"

/* Currently we only support Google VPD 2.0, which has a fixed offset. */
enum {
	CROSVPD_CBMEM_MAGIC = 0x43524f53,
	CROSVPD_CBMEM_VERSION = 0x0001,
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

struct vpd_gets_arg {
	const uint8_t *key;
	const uint8_t *value;
	int32_t key_len, value_len;
	int matched;
};

static struct region_device ro_vpd, rw_vpd;

/*
 * Initializes a region_device to represent the requested VPD 2.0 formatted
 * region on flash. On errors rdev->size will be set to 0.
 */
static void init_vpd_rdev(const char *fmap_name, struct region_device *rdev)
{
	struct google_vpd_info info;
	int32_t size;

	if (fmap_locate_area_as_rdev(fmap_name, rdev)) {
		printk(BIOS_ERR, "%s: No %s FMAP section.\n", __func__,
			fmap_name);
		goto fail;
	}

	size = region_device_sz(rdev);

	if ((size < GOOGLE_VPD_2_0_OFFSET + sizeof(info)) ||
	    rdev_chain(rdev, rdev, GOOGLE_VPD_2_0_OFFSET,
			size - GOOGLE_VPD_2_0_OFFSET)) {
		printk(BIOS_ERR, "%s: Too small (%d) for Google VPD 2.0.\n",
		       __func__, size);
		goto fail;
	}

	/* Try if we can find a google_vpd_info, otherwise read whole VPD. */
	if (rdev_readat(rdev, &info, 0, sizeof(info)) != sizeof(info)) {
		printk(BIOS_ERR, "Failed to read %s header.\n",
		       fmap_name);
		goto fail;
	}

	if (memcmp(info.header.magic, VPD_INFO_MAGIC, sizeof(info.header.magic))
	    == 0) {
		if (rdev_chain(rdev, rdev, sizeof(info), info.size)) {
			printk(BIOS_ERR, "%s info size too large.\n",
			       fmap_name);
			goto fail;
		}
	} else if (info.header.tlv.type == VPD_TYPE_TERMINATOR ||
		   info.header.tlv.type == VPD_TYPE_IMPLICIT_TERMINATOR) {
		printk(BIOS_WARNING, "%s is uninitialized or empty.\n",
		       fmap_name);
		goto fail;
	}

	return;

fail:
	memset(rdev, 0, sizeof(*rdev));
}

static int init_vpd_rdevs_from_cbmem(void)
{
	if (!cbmem_possibly_online())
		return -1;

	struct vpd_cbmem *cbmem = cbmem_find(CBMEM_ID_VPD);
	if (!cbmem)
		return -1;

	rdev_chain_mem(&ro_vpd, cbmem->blob, cbmem->ro_size);
	rdev_chain_mem(&rw_vpd, cbmem->blob + cbmem->ro_size, cbmem->rw_size);

	return 0;
}

static void init_vpd_rdevs(void)
{
	static bool done = false;

	if (done)
		return;

	if (init_vpd_rdevs_from_cbmem() != 0) {
		init_vpd_rdev("RO_VPD", &ro_vpd);
		init_vpd_rdev("RW_VPD", &rw_vpd);
	}

	done = true;
}

static void cbmem_add_cros_vpd(int is_recovery)
{
	struct vpd_cbmem *cbmem;

	timestamp_add_now(TS_COPYVPD_START);

	init_vpd_rdevs();

	/* Return if no VPD at all */
	if (region_device_sz(&ro_vpd) == 0 && region_device_sz(&rw_vpd) == 0)
		return;

	size_t ro_size = region_device_sz(&ro_vpd);
	size_t rw_size = region_device_sz(&rw_vpd);

	cbmem = cbmem_add(CBMEM_ID_VPD, sizeof(*cbmem) + ro_size + rw_size);
	if (!cbmem) {
		printk(BIOS_ERR, "%s: Failed to allocate CBMEM (%zu+%zu).\n",
			__func__, ro_size, rw_size);
		return;
	}

	cbmem->magic = CROSVPD_CBMEM_MAGIC;
	cbmem->version = CROSVPD_CBMEM_VERSION;
	cbmem->ro_size = ro_size;
	cbmem->rw_size = rw_size;

	if (ro_size) {
		if (rdev_readat(&ro_vpd, cbmem->blob, 0, ro_size) != ro_size) {
			printk(BIOS_ERR, "Couldn't read RO VPD\n");
			cbmem->ro_size = ro_size = 0;
		}
		timestamp_add_now(TS_COPYVPD_RO_END);
	}

	if (rw_size) {
		if (rdev_readat(&rw_vpd, cbmem->blob + ro_size, 0, rw_size)
								 != rw_size) {
			printk(BIOS_ERR, "Couldn't read RW VPD\n");
			cbmem->rw_size = rw_size = 0;
		}
		timestamp_add_now(TS_COPYVPD_RW_END);
	}

	init_vpd_rdevs_from_cbmem();
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

static void vpd_find_in(struct region_device *rdev, struct vpd_gets_arg *arg)
{
	if (region_device_sz(rdev) == 0)
		return;

	uint32_t consumed = 0;
	void *mapping = rdev_mmap_full(rdev);
	while (vpd_decode_string(region_device_sz(rdev), mapping,
		&consumed, vpd_gets_callback, arg) == VPD_DECODE_OK) {
	/* Iterate until found or no more entries. */
	}
	rdev_munmap(rdev, mapping);
}

const void *vpd_find(const char *key, int *size, enum vpd_region region)
{
	struct vpd_gets_arg arg = {0};

	arg.key = (const uint8_t *)key;
	arg.key_len = strlen(key);

	init_vpd_rdevs();

	if (region == VPD_RW_THEN_RO)
		vpd_find_in(&rw_vpd, &arg);

	if (!arg.matched && (region == VPD_RO || region == VPD_RO_THEN_RW ||
			region == VPD_RW_THEN_RO))
		vpd_find_in(&ro_vpd, &arg);

	if (!arg.matched && (region == VPD_RW || region == VPD_RO_THEN_RW))
		vpd_find_in(&rw_vpd, &arg);

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

/*
 * Find value of integer type by vpd key.
 *
 * Expects to find a decimal string, trailing chars are ignored.
 * Returns true if the key is found and the value is not too long and
 * starts with a decimal digit. Leaves `val` untouched if unsuccessful.
 */
bool vpd_get_int(const char *const key, const enum vpd_region region, int *const val)
{
	char value[11];

	if (!vpd_gets(key, value, sizeof(value), region))
		return false;

	if (!isdigit(*value))
		return false;

	*val = (int)atol(value);
	return true;
}

CBMEM_CREATION_HOOK(cbmem_add_cros_vpd);
