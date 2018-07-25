/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corp.
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

#ifndef _SOC_INTELBLOCKS_GPIO_H_
#define _SOC_INTELBLOCKS_GPIO_H_

#include <soc/gpio.h>
#include "gpio_defs.h"

#ifndef __ACPI__
#include <types.h>

#define INTEL_GPP(first_of_community, start_of_group, end_of_group) \
	{                                               \
		.first_pad = (start_of_group) - (first_of_community), \
		.size = (end_of_group) - (start_of_group) + 1,        \
	}

/*
 * Following should be defined in soc/gpio.h
 * GPIO_MISCCFG - offset to GPIO MISCCFG Register
 *
 * GPIO_NUM_PAD_CFG_REGS - number of PAD config registers in the SOC
 *                         For SOCs that have DW0 and DW1, it should be 2
 * NUM_GPI_STATUS_REGS - total number of GPI status registers across all
 *                       GPIO communities in the SOC
 *
 * The register offsets specific to the soc communities should be provided in
 * struct pad_community table returned from soc_gpio_get_community
 */

typedef uint32_t gpio_t;

struct pad_config {
	int		pad;/* offset of pad within community */
	uint32_t	pad_config[GPIO_NUM_PAD_CFG_REGS];/*
			Pad config data corresponding to DW0, DW1,.... */
};

/*
 * Structure provides the logical to actual value for PADRSTCFG in DW0. Note
 * that the values are expected to be within the field placement of the register
 * itself. i.e. if the reset field is at 31:30 then the values within logical
 * and chipset should occupy 31:30.
 */
struct reset_mapping {
	uint32_t logical;
	uint32_t chipset;
};


/* Structure describes the groups within each community */
struct pad_group {
	int		first_pad; /* offset of first pad of the group relative
	to the community */
	unsigned int	size; /* Size of the group */
};

/* This structure will be used to describe a community or each group within a
 * community when multiple groups exist inside a community
 */
struct pad_community {
	const char	*name;
	const char	*acpi_path;
	size_t		num_gpi_regs;/* number of gpi registers in community */
	size_t		max_pads_per_group; /* number of pads in each group;
	Number of pads bit mapped in each GPI status/en and Host Own Reg */
	gpio_t		first_pad;   /* first pad in community */
	gpio_t		last_pad;    /* last pad in community */
	uint16_t	host_own_reg_0; /* offset to Host Ownership Reg 0 */
	uint16_t	gpi_smi_sts_reg_0; /* offset to GPI SMI EN Reg 0 */
	uint16_t	gpi_smi_en_reg_0; /* offset to GPI SMI STS Reg 0 */
	uint16_t	pad_cfg_base; /* offset to first PAD_GFG_DW0 Reg */
	uint8_t		gpi_status_offset;  /* specifies offset in struct
						gpi_status */
	uint8_t		port;	/* PCR Port ID */
	const struct reset_mapping	*reset_map; /* PADRSTCFG logical to
			chipset mapping */
	size_t		num_reset_vals;
	const struct pad_group	*groups;
	size_t		num_groups;
};

/*
 * Provides storage for all GPI status registers from all communities
 */
struct gpi_status {
	uint32_t grp[NUM_GPI_STATUS_REGS];
};

/*
 * Structure provides the pmc to gpio group mapping
 */
struct pmc_to_gpio_route {
	int	pmc;
	int	gpio;
};

/*
 * Returns the first community in the list. This will help to iterate
 * through the list. It also returns total number of gpio communities.
 * The soc layer provides a table describing available gpio communities.
 */
const struct pad_community *soc_gpio_get_community(size_t *num_communities);

/*
 * Clear GPI SMI status and fill in the structure representing enabled
 * and set status.
 */
void gpi_clear_get_smi_status(struct gpi_status *sts);

/* Return 1 if gpio is set in the sts.  Otherwise 0. */
int gpi_status_get(const struct gpi_status *sts, gpio_t gpi);

/*
 * Configuration for raw pads. Some pads are designated as only special function
 * pins, and don't have an associated GPIO number, so we need to expose the raw
 * pad configuration functionality.
 */
void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads);

/*
 * gpio_configure_pads_with_override accepts as input two GPIO tables:
 * 1. Base config
 * 2. Override config
 *
 * This function configures raw pads in base config and applies override in
 * override config if any. Thus, for every GPIO_x in base config, this function
 * looks up the GPIO in override config and if it is present there, then applies
 * the configuration from override config.
 */
void gpio_configure_pads_with_override(const struct pad_config *base_cfg,
					size_t base_num_pads,
					const struct pad_config *override_cfg,
					size_t override_num_pads);

/*
 * Calculate Address of DW0 register for given GPIO
 */
void *gpio_dwx_address(const gpio_t pad);

/*
 * Returns the pmc_gpe to gpio_gpe mapping table
 *
 */
const struct pmc_to_gpio_route *soc_pmc_gpio_routes(size_t *num);

/*
 * Set the GPIO groups for the GPE blocks. The values from PMC register GPE_CFG
 * are passed which is then mapped to proper groups for MISCCFG. This basically
 * sets the MISCCFG register bits:
 *  dw0 = gpe0_route[11:8]. This is ACPI GPE0b.
 *  dw1 = gpe0_route[15:12]. This is ACPI GPE0c.
 *  dw2 = gpe0_route[19:16]. This is ACPI GPE0d.
 */
void gpio_route_gpe(uint8_t gpe0b, uint8_t gpe0c, uint8_t gpe0d);

/*
 * Function returns PCR port ID for this pad
 */
uint8_t gpio_get_pad_portid(const gpio_t pad);

#endif
#endif /* _SOC_INTELBLOCKS_GPIO_H_ */
