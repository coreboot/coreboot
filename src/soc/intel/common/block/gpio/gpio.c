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
#include <assert.h>
#include <intelblocks/gpio.h>
#include <gpio.h>
#include <intelblocks/itss.h>
#include <intelblocks/pcr.h>
#include <soc/pm.h>
#include <types.h>

#define GPIO_DWx_SIZE(x)	(sizeof(uint32_t) * (x))
#define PAD_CFG_OFFSET(x, dw_num)	((x) + GPIO_DWx_SIZE(dw_num))
#define PAD_CFG0_OFFSET(x)	PAD_CFG_OFFSET(x, 0)
#define PAD_CFG1_OFFSET(x)	PAD_CFG_OFFSET(x, 1)
#define PAD_CFG2_OFFSET(x)	PAD_CFG_OFFSET(x, 2)
#define PAD_CFG3_OFFSET(x)	PAD_CFG_OFFSET(x, 3)

#define PAD_DW0_MASK	(PAD_CFG0_TX_STATE |				\
	PAD_CFG0_TX_DISABLE | PAD_CFG0_RX_DISABLE | PAD_CFG0_MODE_MASK |\
	PAD_CFG0_ROUTE_MASK | PAD_CFG0_RXTENCFG_MASK |			\
	PAD_CFG0_RXINV_MASK | PAD_CFG0_PREGFRXSEL |			\
	PAD_CFG0_TRIG_MASK | PAD_CFG0_RXRAW1_MASK |			\
	PAD_CFG0_RXPADSTSEL_MASK | PAD_CFG0_RESET_MASK)

#if IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_PADCFG_PADTOL)
#define PAD_DW1_MASK	(PAD_CFG1_IOSTERM_MASK |			\
			PAD_CFG1_PULL_MASK |				\
			PAD_CFG1_TOL_MASK |				\
			PAD_CFG1_IOSSTATE_MASK)
#else
#define PAD_DW1_MASK	(PAD_CFG1_IOSTERM_MASK |			\
			PAD_CFG1_PULL_MASK |				\
			PAD_CFG1_IOSSTATE_MASK)
#endif

#define PAD_DW2_MASK	(0)
#define PAD_DW3_MASK	(0)

#define MISCCFG_GPE0_DW0_SHIFT 8
#define MISCCFG_GPE0_DW0_MASK (0xf << MISCCFG_GPE0_DW0_SHIFT)
#define MISCCFG_GPE0_DW1_SHIFT 12
#define MISCCFG_GPE0_DW1_MASK (0xf << MISCCFG_GPE0_DW1_SHIFT)
#define MISCCFG_GPE0_DW2_SHIFT 16
#define MISCCFG_GPE0_DW2_MASK (0xf << MISCCFG_GPE0_DW2_SHIFT)

#define GPI_SMI_STS_OFFSET(comm, group) ((comm)->gpi_smi_sts_reg_0 +	\
				((group) * sizeof(uint32_t)))
#define GPI_SMI_EN_OFFSET(comm, group) ((comm)->gpi_smi_en_reg_0 +	\
				((group) * sizeof(uint32_t)))

static inline size_t relative_pad_in_comm(const struct pad_community *comm,
						gpio_t gpio)
{
	return gpio - comm->first_pad;
}

/* find the group within the community that the pad is a part of */
static inline size_t gpio_group_index(const struct pad_community *comm,
					unsigned int relative_pad)
{
	size_t i;

	assert(comm->groups != NULL);

	/* find the base pad number for this pad's group */
	for (i = 0; i < comm->num_groups; i++) {
		if (relative_pad >= comm->groups[i].first_pad &&
			relative_pad < comm->groups[i].first_pad +
			comm->groups[i].size) {
			return i;
		}
	}

	assert(0);

	return i;
}

static inline size_t gpio_group_index_scaled(const struct pad_community *comm,
					unsigned int relative_pad, size_t scale)
{
	return gpio_group_index(comm, relative_pad) * scale;
}

static inline size_t gpio_within_group(const struct pad_community *comm,
						unsigned int relative_pad)
{
	size_t i;

	i = gpio_group_index(comm, relative_pad);

	return relative_pad - comm->groups[i].first_pad;
}

static inline uint32_t gpio_bitmask_within_group(
					const struct pad_community *comm,
					unsigned int relative_pad)
{
	return 1U << gpio_within_group(comm, relative_pad);
}

static const struct pad_community *gpio_get_community(gpio_t pad)
{
	size_t gpio_communities;
	size_t i;
	const struct pad_community *comm;
	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++, comm++) {
		if (pad >= comm->first_pad && pad <= comm->last_pad)
			return comm;
	}
	printk(BIOS_ERR, "%s pad %d not found\n", __func__, pad);
	die("Invalid GPIO pad number\n");
	return NULL;
}

static void gpio_configure_owner(const struct pad_config *cfg,
				const struct pad_community *comm)
{
	uint16_t hostsw_reg;
	int pin;

	pin = relative_pad_in_comm(comm, cfg->pad);

	/* The 4th bit in pad_config 1 (RO) is used to indicate if the pad
	 * needs GPIO driver ownership.
	 */
	if (!(cfg->pad_config[1] & PAD_CFG1_GPIO_DRIVER))
		return;

	/* Based on the gpio pin number configure the corresponding bit in
	 * HOSTSW_OWN register. Value of 0x1 indicates GPIO Driver onwership.
	 */
	hostsw_reg = comm->host_own_reg_0;
	hostsw_reg += gpio_group_index_scaled(comm, pin, sizeof(uint32_t));
	pcr_or32(comm->port, hostsw_reg, gpio_bitmask_within_group(comm, pin));
}

static void gpi_enable_smi(const struct pad_config *cfg,
				const struct pad_community *comm)
{
	uint32_t value;
	uint16_t sts_reg;
	uint16_t en_reg;
	int group;
	int pin;

	if (((cfg->pad_config[0]) & PAD_CFG0_ROUTE_SMI) != PAD_CFG0_ROUTE_SMI)
		return;

	pin = relative_pad_in_comm(comm, cfg->pad);
	group = gpio_group_index(comm, pin);

	sts_reg = GPI_SMI_STS_OFFSET(comm, group);
	value = pcr_read32(comm->port, sts_reg);
	/* Write back 1 to reset the sts bits */
	pcr_write32(comm->port, sts_reg, value);

	/* Set enable bits */
	en_reg = GPI_SMI_EN_OFFSET(comm, group);
	pcr_or32(comm->port, en_reg, gpio_bitmask_within_group(comm, pin));
}

static void gpio_configure_itss(const struct pad_config *cfg, uint16_t port,
	uint16_t  pad_cfg_offset)
{
	/* No ITSS configuration in SMM. */
	if (ENV_SMM)
		return;

	if (!IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_ITSS_POL_CFG))
		return;

	int irq;

	/* Set up ITSS polarity if pad is routed to APIC.
	 *
	 * The ITSS takes only active high interrupt signals. Therefore,
	 * if the pad configuration indicates an inversion assume the
	 * intent is for the ITSS polarity. Before forwarding on the
	 * request to the APIC there's an inversion setting for how the
	 * signal is forwarded to the APIC. Honor the inversion setting
	 * in the GPIO pad configuration so that a hardware active low
	 * signal looks that way to the APIC (double inversion).
	 */
	if (!(cfg->pad_config[0] & PAD_CFG0_ROUTE_IOAPIC))
		return;

	irq = pcr_read32(port, PAD_CFG1_OFFSET(pad_cfg_offset));
	irq &= PAD_CFG1_IRQ_MASK;
	if (!irq) {
		printk(BIOS_ERR, "GPIO %u doesn't support APIC routing,\n",
			cfg->pad);
		return;
	}
	itss_set_irq_polarity(irq, !!(cfg->pad_config[0] &
			PAD_CFG0_RX_POL_INVERT));
}

/* Number of DWx config registers can be different for different SOCs */
static uint16_t pad_config_offset(const struct pad_community *comm, gpio_t pad)
{
	size_t offset;

	offset = relative_pad_in_comm(comm, pad);
	offset *= GPIO_DWx_SIZE(GPIO_NUM_PAD_CFG_REGS);
	return offset + comm->pad_cfg_base;
}

static uint32_t gpio_pad_reset_config_override(const struct pad_community *comm,
	uint32_t config_value)
{
	const struct reset_mapping *rst_map = comm->reset_map;
	int i;

	if (rst_map == NULL || comm->num_reset_vals == 0)
		return config_value;/* Logical reset values equal chipset
					values */
	for (i = 0; i < comm->num_reset_vals; i++, rst_map++) {
		if ((config_value & PAD_CFG0_RESET_MASK) == rst_map->logical) {
			config_value &= ~PAD_CFG0_RESET_MASK;
			config_value |= rst_map->chipset;
			return config_value;
		}
	}
	printk(BIOS_ERR, "%s: Logical to Chipset mapping not found\n",
			__func__);
	return config_value;
}

static const int mask[4] = {
	PAD_DW0_MASK, PAD_DW1_MASK, PAD_DW2_MASK, PAD_DW3_MASK
};

static void gpio_configure_pad(const struct pad_config *cfg)
{
	const struct pad_community *comm = gpio_get_community(cfg->pad);
	uint16_t config_offset;
	uint32_t pad_conf, soc_pad_conf;
	int i;

	config_offset = pad_config_offset(comm, cfg->pad);
	for (i = 0; i < GPIO_NUM_PAD_CFG_REGS; i++) {
		pad_conf = pcr_read32(comm->port,
			PAD_CFG_OFFSET(config_offset, i));

		soc_pad_conf = cfg->pad_config[i];
		if (i == 0)
			soc_pad_conf = gpio_pad_reset_config_override(comm,
				soc_pad_conf);
		soc_pad_conf &= mask[i];
		soc_pad_conf |= pad_conf & ~mask[i];

		if (IS_ENABLED(CONFIG_DEBUG_SOC_COMMON_BLOCK_GPIO))
			printk(BIOS_DEBUG,
			"gpio_padcfg [0x%02x, %02zd] DW%d [0x%08x : 0x%08x"
			" : 0x%08x]\n",
			comm->port, relative_pad_in_comm(comm, cfg->pad), i,
			pad_conf,/* old value */
			cfg->pad_config[i],/* value passed from gpio table */
			soc_pad_conf);/*new value*/
		pcr_write32(comm->port, PAD_CFG_OFFSET(config_offset, i),
			soc_pad_conf);
	}
	gpio_configure_itss(cfg, comm->port, config_offset);
	gpio_configure_owner(cfg, comm);
	gpi_enable_smi(cfg, comm);
}

void gpio_configure_pads(const struct pad_config *cfg, size_t num_pads)
{
	size_t i;

	for (i = 0; i < num_pads; i++)
		gpio_configure_pad(cfg + i);
}

void *gpio_dwx_address(const gpio_t pad)
{
	/* Calculate Address of DW0 register for given GPIO
	 * pad - GPIO number
	 * returns - address of GPIO
	 */
	const struct pad_community *comm = gpio_get_community(pad);
	uint16_t config_offset;

	config_offset = pad_config_offset(comm, pad);
	return pcr_reg_address(comm->port, config_offset);
}

uint8_t gpio_get_pad_portid(const gpio_t pad)
{
	/* Get the port id of given pad
	 * pad - GPIO number
	 * returns - given pad port id
	 */
	const struct pad_community *comm = gpio_get_community(pad);
	return comm->port;
}

void gpio_input_pulldown(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, DN_20K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input_pullup(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, UP_20K, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_input(gpio_t gpio)
{
	struct pad_config cfg = PAD_CFG_GPI(gpio, NONE, DEEP);
	gpio_configure_pad(&cfg);
}

void gpio_output(gpio_t gpio, int value)
{
	struct pad_config cfg = PAD_CFG_GPO(gpio, value, DEEP);
	gpio_configure_pad(&cfg);
}

int gpio_get(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset;
	uint32_t reg;

	config_offset = pad_config_offset(comm, gpio_num);
	reg = pcr_read32(comm->port, config_offset);

	return !!(reg & PAD_CFG0_RX_STATE);
}

void gpio_set(gpio_t gpio_num, int value)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	uint16_t config_offset;

	config_offset = pad_config_offset(comm, gpio_num);
	pcr_rmw32(comm->port, config_offset,
		~PAD_CFG0_TX_STATE, (!!value & PAD_CFG0_TX_STATE));
}

uint16_t gpio_acpi_pin(gpio_t gpio_num)
{
	if (!IS_ENABLED(CONFIG_SOC_INTEL_COMMON_BLOCK_GPIO_MULTI_ACPI_DEVICES))
		return gpio_num;

	return relative_pad_in_comm(gpio_get_community(gpio_num), gpio_num);
}

static void print_gpi_status(const struct gpi_status *sts)
{
	int i;
	int group;
	int index;
	int bit_set;
	int num_groups;
	int abs_bit;
	size_t gpio_communities;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++) {
		num_groups = comm->num_gpi_regs;
		index = comm->gpi_status_offset;
		for (group = 0; group < num_groups; group++, index++) {
			for (bit_set = comm->max_pads_per_group - 1;
				bit_set >= 0; bit_set--) {
				if (!(sts->grp[index] & (1 << bit_set)))
					continue;

				abs_bit = bit_set;
				abs_bit += group * comm->max_pads_per_group;
				printk(BIOS_DEBUG, "%s %d\n", comm->name,
								abs_bit);
			}
		}
		comm++;
	}
}

void gpi_clear_get_smi_status(struct gpi_status *sts)
{
	int i;
	int group;
	int index;
	uint32_t sts_value;
	uint32_t en_value;
	size_t gpio_communities;
	int num_groups;
	const struct pad_community *comm;

	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++) {
		num_groups = comm->num_gpi_regs;
		index = comm->gpi_status_offset;
		for (group = 0; group < num_groups; group++, index++) {
			sts_value = pcr_read32(comm->port,
					GPI_SMI_STS_OFFSET(comm, group));
			en_value = pcr_read32(comm->port,
					GPI_SMI_EN_OFFSET(comm, group));
			sts->grp[index] = sts_value & en_value;
			/* Clear the set status bits. */
			pcr_write32(comm->port, GPI_SMI_STS_OFFSET(comm,
				group), sts->grp[index]);
		}
		comm++;
	}

	if (IS_ENABLED(CONFIG_DEBUG_SMI))
		print_gpi_status(sts);

}

int gpi_status_get(const struct gpi_status *sts, gpio_t pad)
{
	uint8_t sts_index;
	const struct pad_community *comm = gpio_get_community(pad);

	pad = relative_pad_in_comm(comm, pad);
	sts_index = comm->gpi_status_offset;
	sts_index += gpio_group_index(comm, pad);

	return !!(sts->grp[sts_index] & gpio_bitmask_within_group(comm, pad));
}

static int gpio_route_pmc_gpio_gpe(int pmc_gpe_num)
{
	size_t num_routes;
	const struct pmc_to_gpio_route *routes;
	int i;

	routes = soc_pmc_gpio_routes(&num_routes);
	assert (routes != NULL);
	for (i = 0; i < num_routes; i++, routes++) {
		if (pmc_gpe_num == routes->pmc)
			return routes->gpio;
	}
	return -1;
}

void gpio_route_gpe(uint8_t gpe0b, uint8_t gpe0c, uint8_t gpe0d)
{
	int i;
	uint32_t misccfg_mask;
	uint32_t misccfg_value;
	int ret;
	size_t gpio_communities;
	const struct pad_community *comm;

	/* Get the group here for community specific MISCCFG register.
	 * If any of these returns -1 then there is some error in devicetree
	 * where the group is probably hardcoded and does not comply with the
	 * PMC group defines. So we return from here and MISCFG is set to
	 * default.
	 */
	ret = gpio_route_pmc_gpio_gpe(gpe0b);
	if (ret == -1)
		return;
	gpe0b = ret;

	ret = gpio_route_pmc_gpio_gpe(gpe0c);
	if (ret == -1)
		return;
	gpe0c = ret;

	ret = gpio_route_pmc_gpio_gpe(gpe0d);
	if (ret == -1)
		return;
	gpe0d = ret;

	misccfg_value = gpe0b << MISCCFG_GPE0_DW0_SHIFT;
	misccfg_value |= gpe0c << MISCCFG_GPE0_DW1_SHIFT;
	misccfg_value |= gpe0d << MISCCFG_GPE0_DW2_SHIFT;

	/* Program GPIO_MISCCFG */
	misccfg_mask = ~(MISCCFG_GPE0_DW2_MASK |
			MISCCFG_GPE0_DW1_MASK |
			MISCCFG_GPE0_DW0_MASK);

	if (IS_ENABLED(CONFIG_DEBUG_SOC_COMMON_BLOCK_GPIO))
		printk(BIOS_DEBUG, "misccfg_mask:%x misccfg_value:%x\n",
			misccfg_mask, misccfg_value);
	comm = soc_gpio_get_community(&gpio_communities);
	for (i = 0; i < gpio_communities; i++, comm++)
		pcr_rmw32(comm->port, GPIO_MISCCFG,
				misccfg_mask, misccfg_value);
}

const char *gpio_acpi_path(gpio_t gpio_num)
{
	const struct pad_community *comm = gpio_get_community(gpio_num);
	return comm->acpi_path;
}
