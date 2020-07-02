/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef DELTALAKE_IPMI_H
#define DELTALAKE_IPMI_H

#include <stdint.h>

#define IPMI_NETFN_OEM				0x30
#define   IPMI_OEM_SET_PPIN			0x77
#define   IPMI_OEM_GET_PCIE_CONFIG		0xf4
#define   IPMI_OEM_GET_BOARD_ID			0x37
#define   IPMI_BMC_SET_POST_START		0x73
#define   IPMI_OEM_SET_BIOS_BOOT_ORDER		0x52
#define   IPMI_OEM_GET_BIOS_BOOT_ORDER		0x53

#define   CMOS_BIT  (1 << 1)
#define   VALID_BIT (1 << 7)
#define   CLEAR_CMOS_AND_VALID_BIT(x) ((x) &= ~(CMOS_BIT | VALID_BIT))
#define   SET_CMOS_AND_VALID_BIT(x)   ((x) |= (CMOS_BIT | VALID_BIT))
#define   IS_CMOS_AND_VALID_BIT(x)    ((x)&CMOS_BIT && (x)&VALID_BIT)

enum config_type {
	PCIE_CONFIG_UNKNOWN = 0x0,
	PCIE_CONFIG_A = 0x1,
	PCIE_CONFIG_B = 0x2,
	PCIE_CONFIG_C = 0x3,
	PCIE_CONFIG_D = 0x4,
};

struct ppin_req {
	uint32_t cpu0_lo;
	uint32_t cpu0_hi;
	uint32_t cpu1_lo;
	uint32_t cpu1_hi;
} __packed;

struct boot_order {
	uint8_t boot_mode;
	uint8_t boot_dev0;
	uint8_t boot_dev1;
	uint8_t boot_dev2;
	uint8_t boot_dev3;
	uint8_t boot_dev4;
} __packed;

enum cb_err ipmi_set_ppin(struct ppin_req *req);
enum cb_err ipmi_get_pcie_config(uint8_t *config);
enum cb_err ipmi_get_slot_id(uint8_t *slot_id);
enum cb_err ipmi_set_post_start(const int port);
void init_frb2_wdt(void);
enum cb_err ipmi_set_cmos_clear(void);
#endif
