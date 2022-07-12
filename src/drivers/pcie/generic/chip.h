/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _PCIE_GENERIC_H_
#define _PCIE_GENERIC_H_

#include <types.h>

struct drivers_pcie_generic_config {
	const char *name;
	unsigned int wake_gpe;
	unsigned int wake_deepest;
};

#endif /* _PCIE_GENERIC_H_ */
