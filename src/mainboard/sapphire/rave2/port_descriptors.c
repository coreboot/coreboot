/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <types.h>

/* TODO: Populate with actual DXIO/DDI descriptors for Rave2. */
void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	*dxio_descs = NULL;
	*dxio_num = 0;
	*ddi_descs = NULL;
	*ddi_num = 0;
}
