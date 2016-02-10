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
 */

#ifndef _SOC_APOLLOLAKE_GPIO_H_
#define _SOC_APOLLOLAKE_GPIO_H_

#include <types.h>
#include <soc/gpio_defs.h>

#define PAD_FUNC(value)		PAD_CFG0_MODE_##value
#define PAD_RESET(value)	PAD_CFG0_RESET_##value
#define PAD_PULL(value)		PAD_CFG1_PULL_##value
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
	_PAD_CFG_STRUCT(pad, PAD_RESET(rst) | PAD_FUNC(func), PAD_PULL(pull))

/* General purpose output, no pullup/down. */
#define PAD_CFG_GPO(pad, val, rst)	\
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_RX_DISABLE | !!val, \
		PAD_PULL(NONE))

/* General purpose input */
#define PAD_CFG_GPI(pad, pull, rst) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE, \
		PAD_PULL(pull))

/* General purpose input, routed to APIC */
#define PAD_CFG_GPI_APIC(pad, pull, rst) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(IOAPIC, LEVEL, NONE), PAD_PULL(pull))

/* General purpose input, routed to SMI */
#define PAD_CFG_GPI_SMI(pad, pull, rst, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(SMI, LEVEL, inv), PAD_PULL(pull))

/* General purpose input, routed to SCI */
#define PAD_CFG_GPI_SCI(pad, pull, rst, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(SCI, LEVEL, inv), PAD_PULL(pull))

/* General purpose input, routed to NMI */
#define PAD_CFG_GPI_NMI(pad, pull, rst, inv) \
	_PAD_CFG_STRUCT(pad,		\
		PAD_FUNC(GPIO) | PAD_RESET(rst) | PAD_CFG0_TX_DISABLE | \
		PAD_IRQ_CFG(NMI, LEVEL, inv), PAD_PULL(pull))

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

#endif /* _SOC_APOLLOLAKE_GPIO_H_ */
