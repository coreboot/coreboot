/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi_gnvs.h>
#include <soc/nvs.h>
#include <stdint.h>

size_t gnvs_size_of_array(void)
{
	return sizeof(struct global_nvs);
}

uint32_t *gnvs_cbmc_ptr(struct global_nvs *gnvs)
{
	return &gnvs->cbmc;
}

/* Some <soc/nvs.h> have no chromeos entry. */
void *gnvs_chromeos_ptr(struct global_nvs *gnvs)
{
#if CONFIG(BOARD_EMULATION_QEMU_X86_Q35)
	return NULL;
#elif CONFIG(MAINBOARD_HAS_CHROMEOS)
	return &gnvs->chromeos;
#else
	return NULL;
#endif
}
