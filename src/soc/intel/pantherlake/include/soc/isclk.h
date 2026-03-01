/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_PANTHERLAKE_ISCLK_H_
#define _SOC_PANTHERLAKE_ISCLK_H_

#include <types.h>

/* Disable PCIe clock source; clock_number: 0-based */
void soc_disable_pcie_clock_out(size_t clock_number);

/* Enable PCIe clock source; clock_number: 0-based */
void soc_enable_pcie_clock_out(size_t clock_number);

#endif /* _SOC_PANTHERLAKE_ISCLK_H_ */
