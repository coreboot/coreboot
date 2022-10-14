/* SPDX-License-Identifier: GPL-2.0-only */

#include <b64_decode.h>
#include <cbmem.h>
#include <console/console.h>
#include <stdlib.h>
#include <string.h>

#include <vendorcode/google/chromeos/chromeos.h>
#include <drivers/vpd/vpd.h>

/*
 * This file provides functions looking in the VPD for WiFi calibration data,
 * and if found, copying the calibration blobs into CBMEM.
 *
 * Per interface calibration data is stored in the VPD in opaque blobs. The
 * keys of the blobs follow one of two possible patterns:
 * "wifi_base64_calibration<N>" or "wifi_calibration<N>", where <N> is the
 * interface number.
 *
 * This function accommodates up to 4 interfaces. All calibration blobs found
 * in the VPD are packed into a single CBMEM entry as describe by the
 * structures below:
 */

/* This structure describes a single calibration data blob */
struct calibration_blob {
	uint32_t blob_size;  /* Total size. rounded up to fall on a 4 byte
				   boundary. */
	uint32_t key_size;   /* Size of the name of this entry, \0 included. */
	uint32_t value_size; /* Size of the value of this entry */
	/* Zero terminated name(key) goes here, immediately followed by value */
};

/*
 * This is the structure of the CBMEM entry containing WiFi calibration blobs.
 * It starts with the total size (header size included) followed by an
 * arbitrary number of concatenated 4 byte aligned calibration blobs.
 */
struct calibration_entry {
	uint32_t size;
	struct calibration_blob entries[0];  /* A varialble size container. */
};


#define MAX_WIFI_INTERFACE_COUNT 4

/*
 * Structure of the cache to keep information about calibration blobs present
 * in the VPD, one cache entry per VPD blob.
 *
 * Maintaing the cache allows to scan the VPD once, determine the CBMEM entry
 * memory requirements, then allocate as much room as necessary and fill it
 * up.
 */
struct vpd_blob_cache_t {
	/* The longest name template must fit with an extra character. */
	char key_name[40];
	void  *value_pointer;
	unsigned blob_size;
	unsigned key_size;
	unsigned value_size;
};

static const char * const templates[] = {
	"wifi_base64_calibrationX",
	"wifi_calibrationX"
};

/*
 * Scan the VPD for WiFi calibration data, checking for all possible key names
 * and caching discovered blobs.
 *
 * Return the sum of sizes of all blobs, as stored in CBMEM.
 */
static size_t fill_up_entries_cache(struct vpd_blob_cache_t *cache,
				    size_t max_entries, size_t *filled_entries)
{
	int i;
	int cbmem_entry_size = 0;
	size_t used_entries = 0;


	for (i = 0;
	     (i < ARRAY_SIZE(templates)) && (used_entries < max_entries);
	     i++) {
		int j;
		const int index_location = strlen(templates[i]) - 1;
		const int key_length = index_location + 2;

		if (key_length > sizeof(cache->key_name))
			continue;

		for (j = 0; j < MAX_WIFI_INTERFACE_COUNT; j++) {
			const void *payload;
			void *decoded_payload;
			int payload_size;
			size_t decoded_size;

			strcpy(cache->key_name, templates[i]);
			cache->key_name[index_location] = j + '0';

			payload = vpd_find(cache->key_name, &payload_size, VPD_RO_THEN_RW);
			if (!payload)
				continue;

			decoded_size = B64_DECODED_SIZE(payload_size);
			decoded_payload = malloc(decoded_size);
			if (!decoded_payload) {
				printk(BIOS_ERR,
				       "%s: failed allocating %zd bytes\n",
				       __func__, decoded_size);
				continue;
			}

			decoded_size = b64_decode(payload, payload_size,
						  decoded_payload);
			if (!decoded_size) {
				free(decoded_payload);
				printk(BIOS_ERR, "%s: failed decoding %s\n",
				       __func__, cache->key_name);
				continue;
			}

			cache->value_pointer = decoded_payload;
			cache->key_size = key_length;
			cache->value_size = decoded_size;
			cache->blob_size =
				ALIGN_UP(sizeof(struct calibration_blob) +
				      cache->key_size +
				      cache->value_size, 4);
			cbmem_entry_size += cache->blob_size;

			used_entries++;
			if (used_entries == max_entries)
				break;

			cache++;
		}
	}

	*filled_entries = used_entries;
	return cbmem_entry_size;
}

void cbmem_add_vpd_calibration_data(void)
{
	size_t cbmem_entry_size, filled_entries;
	struct calibration_entry *cbmem_entry;
	struct calibration_blob *cal_blob;
	int i;
	/*
	 * Allocate one more cache entry than max required, to make sure that
	 * the last entry can be identified by the key size of zero.
	 */
	struct vpd_blob_cache_t vpd_blob_cache[ARRAY_SIZE(templates) *
					       MAX_WIFI_INTERFACE_COUNT];

	cbmem_entry_size = fill_up_entries_cache(vpd_blob_cache,
						 ARRAY_SIZE(vpd_blob_cache),
						 &filled_entries);

	if (!cbmem_entry_size)
		return; /* No calibration data found in the VPD. */

	cbmem_entry_size += sizeof(struct calibration_entry);
	cbmem_entry = cbmem_add(CBMEM_ID_WIFI_CALIBRATION, cbmem_entry_size);
	if (!cbmem_entry) {
		printk(BIOS_ERR, "%s: no room in cbmem to add %zd bytes\n",
		       __func__, cbmem_entry_size);
		return;
	}

	cbmem_entry->size = cbmem_entry_size;

	/* Copy cached data into the CBMEM entry. */
	cal_blob = cbmem_entry->entries;

	for (i = 0; i < filled_entries; i++) {
		/* Use this as a pointer to the current cache entry. */
		struct vpd_blob_cache_t *cache = vpd_blob_cache + i;
		char *pointer;

		cal_blob->blob_size = cache->blob_size;
		cal_blob->key_size = cache->key_size;
		cal_blob->value_size = cache->value_size;

		/* copy the key */
		pointer = (char *)(cal_blob + 1);
		memcpy(pointer, cache->key_name, cache->key_size);

		/* and the value */
		pointer += cache->key_size;
		memcpy(pointer, cache->value_pointer, cache->value_size);
		free(cache->value_pointer);

		printk(BIOS_INFO, "%s: added %s to CBMEM\n",
		       __func__, cache->key_name);

		cal_blob = (struct calibration_blob *)
			((char *)cal_blob + cal_blob->blob_size);
	}
}
