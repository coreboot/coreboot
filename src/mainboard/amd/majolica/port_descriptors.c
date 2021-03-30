/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/platform_descriptors.h>
#include <types.h>

static const fsp_dxio_descriptor majolica_czn_dxio_descriptors[] = {
};

static const fsp_ddi_descriptor majolica_czn_ddi_descriptors[] = {
};

void mainboard_get_dxio_ddi_descriptors(
		const fsp_dxio_descriptor **dxio_descs, size_t *dxio_num,
		const fsp_ddi_descriptor **ddi_descs, size_t *ddi_num)
{
	*dxio_descs = majolica_czn_dxio_descriptors;
	*dxio_num = ARRAY_SIZE(majolica_czn_dxio_descriptors);
	*ddi_descs = majolica_czn_ddi_descriptors;
	*ddi_num = ARRAY_SIZE(majolica_czn_ddi_descriptors);
}
