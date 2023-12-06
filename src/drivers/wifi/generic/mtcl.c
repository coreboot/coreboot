/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpigen.h>
#include <cbfs.h>
#include <mtcl.h>
#include <stdint.h>
#include <string.h>

#define WIFI_MTCL_CBFS_DEFAULT_FILENAME "wifi_mtcl.bin"
#define MAX_VERSION 2
#define MAX_SUPPORT_STATE 2
#define COUNTRY_LIST_SIZE 6
#define NAME_SIZE 4
#define MTCL_NAME "MTCL"

/*
 * Represent the structured MTCL data.
 * This struct is used to cast from an array of uint8_t in order to help
 * understand the semantic purpose of individual bytes. This struct is used in
 * order to verify that the bytes included match the known MTCL data format.
 * This struct is explicitly __packed because it is explicitly cast to from an
 * array of uint8_t.
 */
struct wifi_mtcl {
	uint8_t name[NAME_SIZE];
	uint8_t revision;
	uint8_t support_6ghz;
	uint8_t country_list_6ghz[COUNTRY_LIST_SIZE];
	uint8_t support_5p9ghz;
	uint8_t country_list_5p9ghz[COUNTRY_LIST_SIZE];
} __packed;

void write_mtcl_aml(uint8_t *bytes, size_t count);
int validate_mtcl(uint8_t *mtcl_bytes, size_t count);

/*
 * Generate ACPI AML code for MTCL method.
 * This function takes as input an array of bytes that correspond to the value
 * map to be passed as a package, as well as the count of bytes to be written.
 *
 * AML code generate would look like:
 * Method(MTCL, 0, Serialized)
 * {
 *   Name (LIST, Package()
 *   {
 *     // data table
 *   })
 *   Return (LIST)
 * }
 */
void write_mtcl_aml(uint8_t *bytes, size_t count)
{
	/* Method (MTCL, 0, Serialized) */
	acpigen_write_method_serialized("MTCL", 0x0);

	/* Name (LIST */
	acpigen_write_name("LIST");

	/* Package () */
	acpigen_write_package(count);

	/* Write the provided bytes. */
	for (int i = 0; i < count; ++i)
		acpigen_write_byte(bytes[i]);

	acpigen_write_package_end(); /* Package */

	/* Return MTCL */
	acpigen_write_return_namestr("LIST");
	acpigen_write_method_end();  /* Method MTCL */
}

/*
 * Validate the WiFi MTCL data that is passed in from CBFS.
 *
 * Data is expected in the format:
 *   [Revision,
 *     6GHz Support,
 *     6GHz Country List,
 *     5.9GHz Support,
 *     5.9GHz Country List]
 *
 * The revision is expected to be "2".
 *
 * 6GHz support is a byte with the following states:
 *   - 0 - 6GHz operation disabled
 *   - 1 - 6GHz operation dictated by the country list and Operating System
 *   - 2 - 6GHz operation dictated by the Operating System
 *
 * 6GHz Country List is a set of 6 bytes that represent a bitmask of countries
 * in which 6GHz operation is enabled.
 *
 * 5.9GHz Support is a byte with the following known states:
 *   - 0 - 5.9GHz operation disabled
 *   - 1 - 5.9GHz operation dictated by the country list and Operating System
 *   - 2 - 5.9GHz operation dictated by the Operating System
 *
 * 5.9GHz Country List is a set of 6 bytes that represent a bitmask of countries
 * in which 5.9GHz operation is enabled
 *
 * Validation:
 *   - Verify that there are MTCL_SIZE bytes.
 *   - Verify that the name is MTCL_NAME.
 *   - Verify that the version is less than or equal to MAX_MTCL_VERSION.
 *   - Verify that the support bytes are less than or equal to the
 *      MAX_SUPPORT_STATE.
 *
 * Returns 0 for a valid file, -1 for an invalid file.
 */
int validate_mtcl(uint8_t *mtcl_bytes, size_t count)
{
	if (!mtcl_bytes) {
		printk(BIOS_ERR, "Failed to get the %s file size!\n",
			  WIFI_MTCL_CBFS_DEFAULT_FILENAME);
		return -1;
	}

	if (count != sizeof(struct wifi_mtcl)) {
		printk(BIOS_ERR, "Size of file read was: %zu, expected: %zu\n",
			  count, sizeof(struct wifi_mtcl));
		return -1;
	}

	struct wifi_mtcl *mtcl = (struct wifi_mtcl *)mtcl_bytes;

	if (strncmp(((char *)mtcl->name), MTCL_NAME, NAME_SIZE)) {
		printk(BIOS_ERR, "MTCL string not present but expected\n");
		return -1;
	}

	if (mtcl->revision > MAX_VERSION) {
		printk(BIOS_ERR, "MTCL version too high\n");
		return -1;
	}

	if (mtcl->support_6ghz > MAX_SUPPORT_STATE) {
		printk(BIOS_ERR, "MTCL 6GHz support state too high\n");
		return -1;
	}

	if (mtcl->support_5p9ghz > MAX_SUPPORT_STATE) {
		printk(BIOS_ERR, "MTCL 5.9GHz support state too high\n");
		return -1;
	}

	return 0;
}

/*
 * Retrieve WiFi MTCL data from CBFS, decode it, validate it and write it to
 * AML.
 *
 * Returns the number of bytes read.
 */
void write_mtcl_function(void)
{
	size_t mtcl_bin_len;
	uint8_t *mtcl_bin;

	mtcl_bin = cbfs_map(WIFI_MTCL_CBFS_DEFAULT_FILENAME, &mtcl_bin_len);

	if (validate_mtcl(mtcl_bin, mtcl_bin_len) == 0)
		write_mtcl_aml(mtcl_bin, mtcl_bin_len);

	cbfs_unmap(mtcl_bin);
}
