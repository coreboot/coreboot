/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BROADWELL_PCH_H_
#define _BROADWELL_PCH_H_

#include <acpi/acpi.h>
#include <types.h>

/* Haswell ULT Pch (LynxPoint-LP) */
#define PCH_LPT_LP_SAMPLE	0x9c41
#define PCH_LPT_LP_PREMIUM	0x9c43
#define PCH_LPT_LP_MAINSTREAM	0x9c45
#define PCH_LPT_LP_VALUE	0x9c47

/* Broadwell PCH (WildatPoint) */
#define PCH_WPT_HSW_U_SAMPLE	0x9cc1
#define PCH_WPT_BDW_U_SAMPLE	0x9cc2
#define PCH_WPT_BDW_U_PREMIUM	0x9cc3
#define PCH_WPT_BDW_U_BASE	0x9cc5
#define PCH_WPT_BDW_Y_SAMPLE	0x9cc6
#define PCH_WPT_BDW_Y_PREMIUM	0x9cc7
#define PCH_WPT_BDW_Y_BASE	0x9cc9
#define PCH_WPT_BDW_H		0x9ccb

/* Power Management Control and Status */
#define PCH_PCS			0x84
#define  PCH_PCS_PS_D3HOT	3

enum pch_acpi_device {
	PCH_ACPI_SDMA = 0,
	PCH_ACPI_I2C0,
	PCH_ACPI_I2C1,
	PCH_ACPI_GSPI0,
	PCH_ACPI_GSPI1,
	PCH_ACPI_UART0,
	PCH_ACPI_UART1,
	PCH_ACPI_SDIO,
	PCH_ACPI_ADSP,
	NUM_PCH_ACPI_DEVICES,
};

struct pch_acpi_device_state {
	bool enable;
	uint32_t bar0;
	uint32_t bar1;
};

u8 pch_revision(void);
u16 pch_type(void);
int pch_is_wpt(void);
int pch_is_wpt_ulx(void);
u32 pch_read_soft_strap(int id);
void pch_disable_devfn(struct device *dev);

struct pch_acpi_device_state *get_acpi_device_state(enum pch_acpi_device dev_index);
void acpi_create_serialio_ssdt(acpi_header_t *ssdt);

void broadwell_pch_finalize(void);

#endif
