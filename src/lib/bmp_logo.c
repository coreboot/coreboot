/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi.h>
#include <bootsplash.h>
#include <cbfs.h>
#include <cbmem.h>
#include <stdint.h>
#include <vendorcode/google/chromeos/chromeos.h>

static const struct cbmem_entry *logo_entry;

#if !CONFIG(HAVE_CUSTOM_BMP_LOGO)
const char *bmp_logo_filename(void)
{
	return "logo.bmp";
}
#endif

void bmp_load_logo(uint32_t *logo_ptr, uint32_t *logo_size)
{
	void *logo_buffer;

	/* CBMEM is locked for S3 resume path. */
	if (acpi_is_wakeup_s3())
		return;

	logo_entry = cbmem_entry_add(CBMEM_ID_FSP_LOGO, 1 * MiB);
	if (!logo_entry)
		return;

	logo_buffer = cbmem_entry_start(logo_entry);
	if (!logo_buffer)
		return;

	*logo_size = cbfs_load(bmp_logo_filename(), logo_buffer, 1 * MiB);
	if (*logo_size)
		*logo_ptr = (uintptr_t)logo_buffer;
}

void bmp_release_logo(void)
{
	if (logo_entry)
		cbmem_entry_remove(logo_entry);
	logo_entry = NULL;
}
