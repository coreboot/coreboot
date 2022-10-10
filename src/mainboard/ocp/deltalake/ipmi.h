/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DELTALAKE_IPMI_H
#define DELTALAKE_IPMI_H

#include <types.h>

#define IPMI_OEM_GET_PCIE_CONFIG		0xf4
#define IPMI_OEM_GET_BOARD_ID			0x37


enum config_type {
	PCIE_CONFIG_UNKNOWN = 0x0,
	PCIE_CONFIG_A = 0x1,
	PCIE_CONFIG_B = 0x2,
	PCIE_CONFIG_C = 0x3,
	PCIE_CONFIG_D = 0x4,
};

enum cb_err ipmi_get_pcie_config(uint8_t *config);
enum cb_err ipmi_get_slot_id(uint8_t *slot_id);
void init_frb2_wdt(void);

#endif
