/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __PICASSO_PLATFORM_DESCRIPTORS_H__
#define __PICASSO_PLATFORM_DESCRIPTORS_H__

#include <types.h>
#include <platform_descriptors.h>
#include <FspsUpd.h>

/* Mainboard callback to obtain PCIe and DDI descriptors. */
void mainboard_get_pcie_ddi_descriptors(
		const picasso_fsp_pcie_descriptor **pcie_descs, size_t *pcie_num,
		const picasso_fsp_ddi_descriptor **ddi_descs, size_t *ddi_num);

#endif /* __PICASSO_PLATFORM_DESCRIPTORS_H__ */
