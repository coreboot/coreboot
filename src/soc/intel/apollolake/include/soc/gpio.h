/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2015 Intel Corp.
 * (Written by Alexandru Gagniuc <alexandrux.gagniuc@intel.com> for Intel Corp.)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_APOLLOLAKE_GPIO_H_
#define _SOC_APOLLOLAKE_GPIO_H_

#include <soc/gpio_defs.h>
/* __ACPI__ guard is needed to ignore below code in ACPI/ASL compilation */
#ifndef __ACPI__
#include <types.h>

typedef uint32_t gpio_t;

/*
 * Structure to represent GPI status for GPE and SMI. Use helper
 * functions for interrogating particular GPIs. Here the number of
 * array elements is total number of groups that can be present in all
 * the communities.
 */
struct gpi_status {
	uint32_t grp[NUM_GPI_STATUS_REGS];
};

/*
 * Clear GPI SMI status and fill in the structure representing enabled
 * and set status.
 */
void gpi_clear_get_smi_status(struct gpi_status *sts);

/* Return 1 if gpio is set in the gpi_status struct. Otherwise 0. */
int gpi_status_get(const struct gpi_status *sts, gpio_t gpi);

#define PAD_FUNC(value)		PAD_CFG0_MODE_##value
#define PAD_RESET(value)	PAD_CFG0_RESET_##value
#define PAD_PULL(value)		PAD_CFG1_PULL_##value
#define PAD_IOSSTATE(value)	PAD_CFG1_IOSSTATE_##value
#define PAD_IOSTERM(value)	PAD_CFG1_IOSTERM_##value
#define PAD_IRQ_CFG(route, trig, inv) \
				(PAD_CFG0_ROUTE_##route | \
				PAD_CFG0_TRIG_##trig | \
				(PAD_CFG0_RX_POL_##inv))

#define _PAD_CFG_STRUCT(__pad, __config0, __config1)	\
	{					\
		.pad = __pad,			\
		.config0 = __config0,		\
		.config1 = __config1,		\
	}

/* Native function configuration */
#define PAD_CFG_NF(pad, pull, rst, func) \
	PAD_CFG_NF_IOSSTATE(pad, pull, rst, func, TxLASTRxE)

/* Native function configuration for standby state */
#define PAD_CFG_NF_IOSSTATE(pad, pull, rst, func, iosstate) \
	PAD_CFG_NF_IOSSTATE_IOSTERM(pad,pull, rst, func, iosstate, SAME)

#define PAD_CFG_NF_IOSSTATE_IOSTERM(pad, pull, rst, func, iosstate, iosterm) \
	_PAD_CFG_STRUCT(pad, PAD_RESET(rst) | PAD_FUNC(func), PAD_PULL(pull) | \
		PAD_IOSSTATE(iosstate) | PAD_IOSTERM(iosterm))

/* General purpose output, no pullup/down. */
#define PAD_CFG_GPO(pad, val, rst)	\
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_RX_DISABLE | !!val, \
		PAD_PULL(NONE) | PAD_IOSSTATE(TxLASTRxE))

/* General purpose input */
#define PAD_CFG_GPI(pad, pull, rst) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE, \
		PAD_PULL(pull) | PAD_IOSSTATE(TxLASTRxE))

/* General purpose input. The following macro sets the
 * Host Software Pad Ownership to GPIO Driver mode.
 */
#define PAD_CFG_GPI_GPIO_DRIVER(pad, pull, rst) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE, \
		PAD_PULL(pull) | PAD_CFG1_GPIO_DRIVER | PAD_IOSSTATE(TxLASTRxE))

/* No Connect configuration for unused pad.
 * NC should be GPI with Term as PU20K, PD20K, NONE depending upon default Term
 */
#define PAD_NC(pad, pull)	PAD_CFG_GPI(pad, pull, DEEP)

/* General purpose input, routed to APIC */
#define PAD_CFG_GPI_APIC(pad, pull, rst, trig, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(IOAPIC, trig, inv), PAD_PULL(pull) | \
		PAD_IOSSTATE(TxLASTRxE))

/*
 * The following APIC macros assume the APIC will handle the filtering
 * on its own end. One just needs to pass an active high message into the
 * ITSS.
 */
#define PAD_CFG_GPI_APIC_LOW(pad, pull, rst) \
	PAD_CFG_GPI_APIC(pad, pull, rst, LEVEL, INVERT)

#define PAD_CFG_GPI_APIC_HIGH(pad, pull, rst) \
	PAD_CFG_GPI_APIC(pad, pull, rst, LEVEL, NONE)

/* General purpose input, routed to SMI */
#define PAD_CFG_GPI_SMI(pad, pull, rst, trig, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(SMI, trig, inv), PAD_PULL(pull) | \
		PAD_IOSSTATE(TxLASTRxE))

#define PAD_CFG_GPI_SMI_LOW(pad, pull, rst, trig) \
	PAD_CFG_GPI_SMI(pad, pull, rst, trig, INVERT)

#define PAD_CFG_GPI_SMI_HIGH(pad, pull, rst, trig) \
	PAD_CFG_GPI_SMI(pad, pull, rst, trig, NONE)

/* General purpose input, routed to SCI */
#define PAD_CFG_GPI_SCI(pad, pull, rst, trig, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(SCI, trig, inv), PAD_PULL(pull) | \
		PAD_IOSSTATE(TxLASTRxE))

#define PAD_CFG_GPI_SCI_LOW(pad, pull, rst, trig) \
	PAD_CFG_GPI_SCI(pad, pull, rst, trig, INVERT)

#define PAD_CFG_GPI_SCI_HIGH(pad, pull, rst, trig) \
	PAD_CFG_GPI_SCI(pad, pull, rst, trig, NONE)

/* General purpose input, routed to NMI */
#define PAD_CFG_GPI_NMI(pad, pull, rst, trig, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(NMI, trig, inv), PAD_PULL(pull) | \
		PAD_IOSSTATE(TxLASTRxE))

struct pad_config {
	uint32_t config0;
	uint16_t config1;
	uint16_t pad;
};

/*
 * Configuration for raw pads. Some pads are designated as only special function
 * pins, and don't have an associated GPIO number, so we need to expose the raw
 * pad configuration functionality.
 */
void gpio_configure_pad(const struct pad_config *cfg);
void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads);

/* Calculate GPIO DW0 address */
void *gpio_dwx_address(const uint16_t pad);

/* Get the port id of given pad */
uint8_t gpio_get_pad_portid(const uint16_t pad);

/*
 * Set the GPIO groups for the GPE blocks. The values from PMC register GPE_CFG
 * are passed which is then mapped to proper groups for MISCCFG. This basically
 * sets the MISCCFG register bits:
 *  dw0 = gpe0_route[11:8]. This is ACPI GPE0b.
 *  dw1 = gpe0_route[15:12]. This is ACPI GPE0c.
 *  dw2 = gpe0_route[19:16]. This is ACPI GPE0d.
 */
void gpio_route_gpe(uint8_t gpe0b, uint8_t gpe0c, uint8_t gpe0d);

#endif /* __ACPI__ */

#endif /* _SOC_APOLLOLAKE_GPIO_H_ */
