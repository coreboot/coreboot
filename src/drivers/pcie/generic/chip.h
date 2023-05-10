/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PCIE_GENERIC_H_
#define _PCIE_GENERIC_H_

#include <types.h>

struct drivers_pcie_generic_config {
	const char *name;
	unsigned int wake_gpe;
	unsigned int wake_deepest;
	/* When set to true, this will add a _DSD which contains a single
	   property, `DmaProperty`, set to 1, under the ACPI Device. */
	bool add_acpi_dma_property;

};

#endif /* _PCIE_GENERIC_H_ */
