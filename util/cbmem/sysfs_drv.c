/* SPDX-License-Identifier: GPL-2.0-only */

#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(__linux__) || defined(__ANDROID__)
#include <linux/fs.h>
#include <sys/ioctl.h>
#endif

#include <commonlib/bsd/cbmem_id.h>
#include <commonlib/bsd/ipchksum.h>
#include <commonlib/coreboot_tables.h>
#include <commonlib/helpers.h>
#include <commonlib/timestamp_serialized.h>
#include <commonlib/tpm_log_serialized.h>

#include "cbmem_util.h"

#define CBMEM_SYSFS_BASE_DIR               "/sys/bus/coreboot/devices/"
#define CBMEM_SYSFS_ENTRY_DIR_NAME_PREFIX  "cbmem-"
#define CBMEM_SYSFS_ENTRY_DIR_NAME_SCN_FMT CBMEM_SYSFS_ENTRY_DIR_NAME_PREFIX "%8" SCNx32
#define CBMEM_SYSFS_ENTRY_DIR_NAME_PRI_FMT CBMEM_SYSFS_ENTRY_DIR_NAME_PREFIX "%08" PRIx32
#define CBMEM_SYSFS_ENTRY_DIR_PATH_PRI_FMT \
	CBMEM_SYSFS_BASE_DIR CBMEM_SYSFS_ENTRY_DIR_NAME_PRI_FMT

enum cbmem_sysfs_path_type {
	CBMEM_SYSFS_PATH_BASE,
	CBMEM_SYSFS_PATH_ADDRESS,
	CBMEM_SYSFS_PATH_MEM,
	CBMEM_SYSFS_PATH_SIZE,
};

/* Maximum filesystem path length. */
#define PATH_MAX_LEN 1024

/**
 * Create new path for CBMEM entry of a requested type. free() path_out after use.
 *
 * @param id CBMEM_ID_* value.
 * @param t path type.
 * @param path_out output pointer to created path.
 */
static void new_sysfs_path(uint32_t id, enum cbmem_sysfs_path_type t, char *path_out, size_t path_out_max_size)
{
	const char *const type2str[] = {
		[CBMEM_SYSFS_PATH_BASE] = "",
		[CBMEM_SYSFS_PATH_ADDRESS] = "/address",
		[CBMEM_SYSFS_PATH_MEM] = "/mem",
		[CBMEM_SYSFS_PATH_SIZE] = "/size"
	};

	if (t >= ARRAY_SIZE(type2str))
		die("Incorrect path type requested: %d\n", t);

	if (snprintf(path_out, path_out_max_size, CBMEM_SYSFS_ENTRY_DIR_PATH_PRI_FMT "%s", id,
		     type2str[t]) <= 0)
		die("Unable to create sysfs path string for coreboot table: %#" PRIx32
		    ". Path type: %d. Error: %s\n",
		    id, t, strerror(errno));
}

bool cbmem_sysfs_init(void)
{
	char path[PATH_MAX_LEN];

	for (enum cbmem_sysfs_path_type t = CBMEM_SYSFS_PATH_ADDRESS; t <= CBMEM_SYSFS_PATH_SIZE; ++t) {
		new_sysfs_path(CBMEM_ID_CBTABLE, t, path, sizeof(path));

		FILE *f = fopen(path, "rb");
		if (!f) {
			debug("Unable to open path %s for reading. Error: %s\n", path, strerror(errno));
			return false;
		}

		uint8_t test_byte = 0;
		if (fread(&test_byte, 1, 1, f) != 1) {
			debug("Unable to read data from %s. Error: %s\n", path, strerror(errno));
			return false;
		}

		fclose(f);
	}
	return true;
}

static bool cbmem_sysfs_probe_cbmem_entry(uint32_t id, uint64_t *addr_out, size_t *size_out)
{
	char path[PATH_MAX_LEN];

	new_sysfs_path(id, CBMEM_SYSFS_PATH_ADDRESS, path, sizeof(path));

	FILE *address_file = fopen(path, "rb");
	if (!address_file) {
		debug("Unable to access CBMEM entry id: %#" PRIx32
		      " address file at %s. Error: %s\n",
		      id, path, strerror(errno));
		return false;
	}

	if (fscanf(address_file, "%" SCNx64, addr_out) != 1) {
		debug("Read from %s failed.\n", path);
		fclose(address_file);
		return false;
	}
	fclose(address_file);

	new_sysfs_path(id,  CBMEM_SYSFS_PATH_SIZE, path, sizeof(path));

	FILE *size_file = fopen(path, "rb");
	if (!size_file) {
		debug("Unable to access CBMEM entry id: %#" PRIx32
		      " size file at %s. Error: %s\n",
		      id, path, strerror(errno));
		return false;
	}

	if (fscanf(size_file, "%zx", size_out) != 1) {
		debug("Read from %s failed.\n", path);
		fclose(size_file);
		*addr_out = 0;
		return false;
	}
	fclose(size_file);

	return true;
}

static void fetch_cbmem_entry(const uint32_t id, const size_t size, uint8_t **buf_out)
{
	char path[PATH_MAX_LEN];

	new_sysfs_path(id, CBMEM_SYSFS_PATH_MEM, path, sizeof(path));

	FILE *mem_file = fopen(path, "rb");
	if (!mem_file)
		die("Unable to open mem file for CBMEM entry id: %#" PRIx32
		    " at %s. Error: %s\n",
		    id, path, strerror(errno));

	*buf_out = malloc(size);
	if (!buf_out)
		die("Unable to allocate memory for CBMEM entry id: %#" PRIx32
		    " of size: %zuB.\n",
		    id, size);

	if (fread(*buf_out, 1, size, mem_file) != size)
		die("Unable to correctly read memory of CBMEM entry id: %#" PRIx32
		    " at %s. Error: %s\n",
		    id, path, strerror(errno));

	fclose(mem_file);
}

bool cbmem_sysfs_get_cbmem_entry(uint32_t id, uint8_t **buf_out, size_t *size_out, uint64_t *addr_out)
{
	uint64_t addr;
	size_t size;

	if (!cbmem_sysfs_probe_cbmem_entry(id, &addr, &size)) {
		debug("CBMEM entry id: %#" PRIx32 " not found.\n", id);
		return false;
	}

	fetch_cbmem_entry(id, size, buf_out);
	if (size_out)
		*size_out = size;
	if (addr_out)
		*addr_out = addr;
	return true;
}

bool cbmem_sysfs_write_cbmem_entry(uint32_t id, uint8_t *buf, size_t buf_size)
{
	char path[PATH_MAX_LEN];
	uint64_t addr;
	size_t size;

	if (!cbmem_sysfs_probe_cbmem_entry(id, &addr, &size)) {
		debug("Unable to find CBMEM entry id: %#" PRIx32 "\n", id);
		return false;
	}

	if (buf_size > size)
		die("Attempting to write %zu bytes to CBMEM entry id: %#" PRIx32
		    " of %zu bytes. Operation not possible.\n",
		    buf_size, id, size);

	new_sysfs_path(id, CBMEM_SYSFS_PATH_MEM, path, sizeof(path));

	FILE *mem_file = fopen(path, "rb+");
	if (!mem_file)
		die("Unable to open mem file for CBMEM entry id: %#" PRIx32
		    " at %s. Error: %s\n",
		    id, path, strerror(errno));

	if (fwrite(buf, 1, buf_size, mem_file) != buf_size)
		die("Unable to correctly write memory of CBMEM entry id: %#" PRIx32
		    " at %s. Error: %s\n",
		    id, path, strerror(errno));

	fclose(mem_file);

	return true;
}

void cbmem_sysfs_foreach_cbmem_entry(cbmem_iterator_callback cb, void *data, bool with_contents)
{
	DIR *dir;
	struct dirent *entry;

	if ((dir = opendir(CBMEM_SYSFS_BASE_DIR)) == NULL)
		die("Unable to open directory containing CBMEM entries. Path: %s. Error: %s\n",
		    CBMEM_SYSFS_BASE_DIR, strerror(errno));

	while ((entry = readdir(dir)) != NULL) {
		debug("Checking path: %s%s\n", CBMEM_SYSFS_BASE_DIR, entry->d_name);
		/* Check directory name prefix */
		if (strncmp(entry->d_name, CBMEM_SYSFS_ENTRY_DIR_NAME_PREFIX,
			    strlen(CBMEM_SYSFS_ENTRY_DIR_NAME_PREFIX)) != 0)
			continue;

		uint32_t id = 0;
		/* Extract CBMEM entry id from the directory name.
		   If it fails, then directory is not a correct entry. */
		if (sscanf(entry->d_name, CBMEM_SYSFS_ENTRY_DIR_NAME_SCN_FMT, &id) != 1)
			continue;

		uint64_t addr = 0;
		uint64_t size = 0;
		uint8_t *buf;

		/* If entry was not found or previously matched directory
		   does not contain necessary files, then omit the entry. */
		if (!cbmem_sysfs_probe_cbmem_entry(id, &addr, &size))
			continue;

		if (with_contents)
			fetch_cbmem_entry(id, size, &buf);

		debug("Invoking callback on %s%s\n", CBMEM_SYSFS_BASE_DIR, entry->d_name);

		const bool res = cb(id, addr, buf, size, data);

		if (with_contents)
			free(buf);

		/* Finish iteration if callback requested it. */
		if (res)
			break;
	}

	closedir(dir);
}
