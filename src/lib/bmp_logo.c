/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootsplash.h>
#include <cbfs.h>
#include <cbmem.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

static const struct cbmem_entry *logo_entry;

/* Mapping of different bootsplash logo name based on bootsplash type */
static const char *bootsplash_list[BOOTSPLASH_MAX_NUM] = {
	[BOOTSPLASH_LOW_BATTERY] = "low_battery.bmp",
	[BOOTSPLASH_CENTER] = "logo.bmp",
	[BOOTSPLASH_FOOTER] = "footer_logo.bmp"
};

/*
 * Return the appropriate logo filename based on the bootsplash type.
 * This will be the default filename from 'bootsplash_list' or the
 * custom one if it was overridden for BOOTSPLASH_OEM.
 */
static const char *bmp_get_logo_filename(enum bootsplash_type type)
{
	/* Override `BOOTSPLASH_CENTER` logo name if required */
	if ((type == BOOTSPLASH_CENTER) && CONFIG(HAVE_CUSTOM_BMP_LOGO))
		return bmp_logo_filename();

	return bootsplash_list[type];
}

void *bmp_load_logo_by_type(enum bootsplash_type type, size_t *logo_size)
{
	void *logo_buffer;

	/* CBMEM is locked for S3 resume path. */
	if (acpi_is_wakeup_s3())
		return NULL;

	logo_entry = cbmem_entry_add(CBMEM_ID_BMP_LOGO, 1 * MiB);
	if (!logo_entry)
		return NULL;

	logo_buffer = cbmem_entry_start(logo_entry);
	if (!logo_buffer)
		return NULL;

	*logo_size = cbfs_load(bmp_get_logo_filename(type), logo_buffer, 1 * MiB);
	if (*logo_size == 0)
		return NULL;

	return logo_buffer;
}

void *bmp_load_logo(size_t *logo_size)
{
	enum bootsplash_type type = BOOTSPLASH_CENTER;

	if (platform_is_low_battery_shutdown_needed())
		type = BOOTSPLASH_LOW_BATTERY;

	return bmp_load_logo_by_type(type, logo_size);
}

void bmp_release_logo(void)
{
	if (logo_entry)
		cbmem_entry_remove(logo_entry);
	logo_entry = NULL;
}
