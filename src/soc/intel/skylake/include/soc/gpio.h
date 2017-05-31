/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 Google Inc.
 * Copyright (C) 2015 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _SOC_GPIO_H_
#define _SOC_GPIO_H_

#include <soc/gpio_defs.h>

#define GPIO_DWx_SIZE(x)	(sizeof(uint32_t) * (x))

#define CROS_GPIO_DEVICE_NAME	"INT344B:00"

#ifndef __ACPI__
#include <stdint.h>
#include <stddef.h>

typedef uint32_t gpio_t;

/* Structure to represent GPI status for GPE and SMI. Use helper
 * functions for interrogating particular GPIs. */
struct gpi_status {
	uint32_t grp[GPIO_NUM_GROUPS];
};

/*
 * Clear GPI SMI status and fill in the structure representing enabled
 * and set status.
 */
void gpi_clear_get_smi_status(struct gpi_status *sts);

/* Return 1 if gpio is set in the gpi_status struct. Otherwise 0. */
int gpi_status_get(const struct gpi_status *sts, gpio_t gpi);

/*
 * Set the GPIO groups for the GPE blocks. The gpe0_route is interpreted
 * as the packed configuration for GPE0_DW[2:0]:
 *  dw0 = gpe0_route[3:0]
 *  dw1 = gpe0_route[7:4]
 *  dw2 = gpe0_route[11:8].
 */
void gpio_route_gpe(uint16_t gpe0_route);

/* Configure the pads according to the pad_config array. */
struct pad_config;
void gpio_configure_pads(const struct pad_config *cfgs, size_t num);

#define PAD_FIELD_VAL(field_, val_) \
	(((val_) & field_ ## _MASK) << field_ ## _SHIFT)

#define PAD_FIELD(field_, setting_) \
	PAD_FIELD_VAL(field_, field_ ## _ ## setting_)

/*
 * This encodes all the fields found within the dw0 register for each
 * pad. It directly follows the register specification:
 *   rst - reset type when pad configuration is reset
 *   rxst - native function routing: raw buffer or internal buffer
 *   rxraw1 - drive fixed '1' for Rx buffer
 *   rxev - event filtering for pad value: level, edge, drive '0'
 *   rxgf - glitch filter enable
 *   rxinv - invert the internal pad state
 *   gpiioapic - route to IOxAPIC
 *   gpisci -  route for SCI
 *   gpismi - route for SMI
 *   gpinmi - route for NMI
 *   mode -  GPIO vs native function
 *   rxdis - disable Rx buffer
 *   txdis - disable Tx buffer
 */
#define _DW0_VALS(rst, rxst, rxraw1, rxev, rxgf, rxinv, gpiioapic, gpisci, \
			gpismi, gpinmi, mode, rxdis, txdis) \
	(PAD_FIELD(PADRSTCFG, rst) | \
	 PAD_FIELD(RXPADSTSEL, rxst) | \
	 PAD_FIELD(RXRAW1, rxraw1) | \
	 PAD_FIELD(RXEVCFG, rxev) | \
	 PAD_FIELD(PREGFRXSEL, rxgf) | \
	 PAD_FIELD(RXINV, rxinv) | \
	 PAD_FIELD(GPIROUTIOXAPIC, gpiioapic) | \
	 PAD_FIELD(GPIROUTSCI, gpisci) | \
	 PAD_FIELD(GPIROUTSMI, gpismi) | \
	 PAD_FIELD(GPIROUTNMI, gpinmi) | \
	 PAD_FIELD(PMODE, mode) | \
	 PAD_FIELD(GPIORXDIS, rxdis) | \
	 PAD_FIELD(GPIOTXDIS, txdis))

#define _PAD_CFG_ATTRS(pad_, term_, dw0_, attrs_)			\
	{								\
		.pad = pad_,						\
		.attrs = PAD_FIELD(PAD_TERM,  term_) | attrs_,		\
		.dw0 = dw0_,						\
	}

/* Default to ACPI owned. Ownership only matters for GPI pads. */
#define _PAD_CFG(pad_, term_, dw0_) \
	_PAD_CFG_ATTRS(pad_, term_, dw0_, PAD_FIELD(HOSTSW, ACPI))

/* Native Function - No Rx buffer manipulation */
#define PAD_CFG_NF(pad_, term_, rst_, func_) \
	_PAD_CFG(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, NO, NO, NO, NO, NO, func_, NO, NO))

/* Native 1.8V tolerant pad, only applies to some pads like I2C/I2S. */
#define PAD_CFG_NF_1V8(pad_, term_, rst_, func_) \
	_PAD_CFG_ATTRS(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, NO, \
		NO, NO, NO, NO, func_, NO, NO), PAD_FIELD(PAD_TOL, 1V8))

/* Unused PINS will be controlled by GPIO controller (PMODE = GPIO) and
   GPIO TX/RX will be disabled. */
#define PAD_CFG_NC(pad_) \
	_PAD_CFG(pad_, NONE, \
	_DW0_VALS(DEEP, RAW, NO, LEVEL, NO, NO, NO, NO, NO, NO, GPIO, YES, YES))

/* General purpose output with termination. */
#define PAD_CFG_TERM_GPO(pad_, val_, term_, rst_) \
	_PAD_CFG(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, NO, NO, NO, NO, NO, GPIO, YES, NO) \
		| PAD_FIELD_VAL(GPIOTXSTATE, val_))

/* General purpose output. By default no termination. */
#define PAD_CFG_GPO(pad_, val_, rst_) \
	PAD_CFG_TERM_GPO(pad_, val_, NONE, rst_)

/* General purpose input with no special IRQ routing. */
#define PAD_CFG_GPI(pad_, term_, rst_) \
	_PAD_CFG_ATTRS(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, NO, NO, NO, NO, NO, GPIO, NO, YES),\
	PAD_FIELD(HOSTSW, GPIO))

/* General purpose input passed through to GPIO interrupt */
#define PAD_CFG_GPI_INT(pad_, term_, rst_, trig_) \
	_PAD_CFG_ATTRS(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, trig_, NO, NO, NO, NO, NO, NO, GPIO, NO, YES),\
	PAD_FIELD(HOSTSW, GPIO))

/* General purpose input passed through to IOxAPIC. Assume APIC logic can
 * handle polarity/edge/level constraints. */
#define PAD_CFG_GPI_APIC(pad_, term_, rst_) \
	_PAD_CFG(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, NO, YES, NO, NO, NO, GPIO, NO, YES))

/* General purpose input passed through to IOxAPIC as inverted input. */
#define PAD_CFG_GPI_APIC_INVERT(pad_, term_, rst_) \
	_PAD_CFG(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, YES, YES, NO, NO, NO, GPIO, NO, \
		  YES))

/* General purpose input passed through to IOxAPIC. This assumes edge
 * triggered events. */
#define PAD_CFG_GPI_APIC_EDGE(pad_, term_, rst_) \
	_PAD_CFG(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, EDGE, NO, NO, YES, NO, NO, NO, GPIO, NO, YES))

/* General purpose input routed to SCI. This assumes edge triggered events. */
#define PAD_CFG_GPI_ACPI_SCI(pad_, term_, rst_, inv_) \
	_PAD_CFG_ATTRS(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, EDGE, NO, inv_, \
		NO, YES, NO, NO, GPIO, NO, YES), PAD_FIELD(HOSTSW, ACPI))

#define PAD_CFG_GPI_ACPI_SCI_LEVEL(pad_, term_, rst_, inv_) \
	_PAD_CFG_ATTRS(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, LEVEL, NO, inv_, \
		NO, YES, NO, NO, GPIO, NO, YES), PAD_FIELD(HOSTSW, ACPI))

/* General purpose input routed to SMI. This assumes edge triggered events. */
#define PAD_CFG_GPI_ACPI_SMI(pad_, term_, rst_, inv_) \
	_PAD_CFG_ATTRS(pad_, term_, \
	_DW0_VALS(rst_, RAW, NO, EDGE, NO, inv_, \
		NO, NO, YES, NO, GPIO, NO, YES), PAD_FIELD(HOSTSW, ACPI))

/*
 * The 'attrs' field carries the termination in bits 13:10 and tolerance in bit
 * 25 to match up with thd DW1 pad configuration register. Additionally, other
 * attributes can be applied such as the ones below. Bit allocation matters.
 */
#define HOSTSW_SHIFT		0
#define HOSTSW_MASK		1
#define HOSTSW_ACPI		HOSTSW_OWN_ACPI
#define HOSTSW_GPIO		HOSTSW_OWN_GPIO

struct pad_config {
	uint16_t pad;
	uint32_t attrs;
	uint32_t dw0;
};

#endif /* __ACPI__ */
#endif
