/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_LOCKDOWN_H_
#define _SOC_LOCKDOWN_H_

void sata_lockdown_config(int chipset_lockdown);

void spi_lockdown_config(int chipset_lockdown);
#endif
