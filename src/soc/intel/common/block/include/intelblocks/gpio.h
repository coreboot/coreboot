/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef _SOC_INTELBLOCKS_GPIO_H_
#define _SOC_INTELBLOCKS_GPIO_H_

#include <soc/gpio.h>
#include "gpio_defs.h"

/* GPIO community IOSF sideband VNNREQ/ACK handshake */
#define MISCCFG_GPVNNREQEN	(1 << 7)
/* GPIO community PGCB clock gating */
#define MISCCFG_GPPGCBDPCGEN	(1 << 6)
/* GPIO community IOSF sideband clock gating */
#define MISCCFG_GPSIDEDPCGEN	(1 << 5)
/* GPIO community RCOMP clock gating */
#define MISCCFG_GPRCOMPCDLCGEN	(1 << 4)
/* GPIO community RTC clock gating */
#define MISCCFG_GPRTCDLCGEN	(1 << 3)
/* GFX controller clock gating */
#define MISCCFG_GSXSLCGEN	(1 << 2)
/* GPIO community partition clock gating */
#define MISCCFG_GPDPCGEN	(1 << 1)
/* GPIO community local clock gating */
#define MISCCFG_GPDLCGEN	(1 << 0)

#ifndef __ACPI__
#include <types.h>
#include <device/device.h>

/*
 * GPIO numbers may not be contiguous and instead will have a different
 * starting pin number for each pad group.
 */
#define INTEL_GPP_BASE(first_of_community, start_of_group, end_of_group,\
			group_pad_base)					\
	{								\
		.first_pad = (start_of_group) - (first_of_community),	\
		.size = (end_of_group) - (start_of_group) + 1,		\
		.acpi_pad_base = (group_pad_base),			\
	}

/*
 * A pad base of -1 indicates that this group uses contiguous numbering
 * and a pad base should not be used for this group.
 */
#define PAD_BASE_NONE	-1

/* The common/default group numbering is contiguous */
#define INTEL_GPP(first_of_community, start_of_group, end_of_group)	\
	INTEL_GPP_BASE(first_of_community, start_of_group, end_of_group,\
		       PAD_BASE_NONE)

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

enum gpio_lock_action {
	GPIO_UNLOCK		 = 0x0,
	GPIO_LOCK_CONFIG	 = 0x1,
	GPIO_LOCK_TX		 = 0x2,
	GPIO_LOCK_FULL		 = GPIO_LOCK_CONFIG | GPIO_LOCK_TX,
};

struct pad_config {
	gpio_t		pad;/* offset of pad within community */
	uint32_t	pad_config[GPIO_NUM_PAD_CFG_REGS];/*
			Pad config data corresponding to DW0, DW1,.... */
	enum gpio_lock_action	lock_action; /* Pad lock configuration */
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
	/*
	 * This is the starting pin number for the pads in this group when
	 * they are used in ACPI.  This is only needed if the pins are not
	 * contiguous across groups, most groups will have this set to
	 * PAD_BASE_NONE and use contiguous numbering for ACPI.
	 */
	int		acpi_pad_base;
};

/* A range of consecutive virtual-wire entries in a community */
struct vw_entries {
	gpio_t first_pad;
	gpio_t last_pad;
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
	uint16_t	gpi_int_sts_reg_0; /* offset to GPI Int STS Reg 0 */
	uint16_t	gpi_int_en_reg_0; /* offset to GPI Int Enable Reg 0 */
	uint16_t	gpi_smi_sts_reg_0; /* offset to GPI SMI STS Reg 0 */
	uint16_t	gpi_smi_en_reg_0; /* offset to GPI SMI EN Reg 0 */
	uint16_t	gpi_gpe_sts_reg_0; /* offset to GPI GPE STS Reg 0 */
	uint16_t	gpi_gpe_en_reg_0; /* offset to GPI GPE EN Reg 0 */
	uint16_t	gpi_nmi_sts_reg_0; /* offset to GPI NMI STS Reg 0 */
	uint16_t	gpi_nmi_en_reg_0; /* offset to GPI NMI EN Reg 0 */
	uint16_t	pad_cfg_base; /* offset to first PAD_GFG_DW0 Reg */
	uint16_t	pad_cfg_lock_offset; /* offset to first PADCFGLOCK Reg */
	uint8_t		gpi_status_offset;  /* specifies offset in struct
						gpi_status */
	uint8_t		port;	/* PCR Port ID */
	uint8_t		cpu_port; /* CPU Port ID */
	const struct reset_mapping	*reset_map; /* PADRSTCFG logical to
			chipset mapping */
	size_t		num_reset_vals;
	const struct pad_group	*groups;
	size_t		num_groups;
	unsigned int	vw_base;
	/*
	 * Note: The entries must be in the same order here as the order in
	 * which they map to VW indexes (beginning with VW base)
	 */
	const struct vw_entries	*vw_entries;
	size_t		num_vw_entries;
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
 * the configuration from override config. GPIOs that are only specified in the
 * override, but not in the base configuration, will be ignored.
 */
void gpio_configure_pads_with_override(const struct pad_config *base_cfg,
					size_t base_num_pads,
					const struct pad_config *override_cfg,
					size_t override_num_pads);

/*
 * Calculate Address of DW0 register for given GPIO
 */
void *gpio_dwx_address(const gpio_t pad);

struct gpio_lock_config {
	gpio_t			pad;
	enum gpio_lock_action	lock_action;
};

/*
 * Lock a GPIO's configuration.
 *
 * The caller may specify if they wish to only lock the pad configuration, only
 * the TX state, or both.  When the configuration is locked, the following
 * registers become Read-Only and software writes to these registers have no
 * effect.
 *
 *	Pad Configuration registers,
 *	GPI_NMI_EN,
 *	GPI_SMI_EN,
 *	GPI_GPE_EN
 *
 * Note that this is only effective if the pad is owned by the host and this
 * function may only be called in SMM.
 *
 * @param pad: GPIO pad number
 * @param lock_action: Which register to lock.
 * @return 0 if successful,
 * 1 - unsuccessful
 * 2 - powered down
 * 3 - multi-cast mixed
 * -1 - sideband message failed or other error
 */
int gpio_lock_pad(const gpio_t pad, enum gpio_lock_action lock_action);

/*
 * gpio_lock_pads() can be used to lock an array of gpio pads, avoiding
 * the p2sb_unhide() and p2sb_hide() calls between each gpio lock that would
 * occur if gpio_lock_pad() were used to lock each pad in the list.
 *
 * @param pad_list: array of gpio_lock_config structures, one for each gpio to lock
 * @param count: number of gpio_lock_config structs in the pad_list array
 * @return 0 if successful,
 * 1 - unsuccessful
 * 2 - powered down
 * 3 - multi-cast mixed
 * -1 - sideband message failed or other error
 */
int gpio_lock_pads(const struct gpio_lock_config *pad_list, const size_t count);

/*
 * Returns an array of gpio_lock_config entries that the SoC
 * deems security risks that should be locked down.
 */
const struct gpio_lock_config *soc_gpio_lock_config(size_t *num);

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

/*
 * Function to patch GPIO settings for SoC specifically
 *  cfg = pad config contains pad number and reg value.
 *  dw_reg = pad config dword number.
 *  reg_val = the reg value need to be patched.
 * Returns gpio setting patched for SoC specifically
 */
uint32_t soc_gpio_pad_config_fixup(const struct pad_config *cfg,
						int dw_reg, uint32_t reg_val);

/*
 * Function to reset/clear the GPI Interrupt Enable & Status registers for
 * all GPIO pad communities.
 */
void gpi_clear_int_cfg(void);

/* The function performs GPIO Power Management programming. */
void gpio_pm_configure(const uint8_t *misccfg_pm_values, size_t num);

/*
 * Set gpio ops of the device to gpio block ops.
 * Shall be called by all SoCs that use intelblocks/gpio.
 */
void block_gpio_enable(struct device *dev);

/*
 * Returns true if any GPIO that uses the specified IRQ is also programmed to
 * route IRQs to IOAPIC.
 */
bool gpio_routes_ioapic_irq(unsigned int irq);

size_t gpio_get_index_in_group(gpio_t pad);

/*
 * Returns true and stuffs out params for virtual-wire index and bit position
 * for the given GPIO, otherwise false if there is no VW index for the pad.
 */
bool gpio_get_vw_info(gpio_t pad, unsigned int *vw_index, unsigned int *vw_bit);

/* Returns PCR port ID for this pad for the CPU; will be 0 if not available */
unsigned int gpio_get_pad_cpu_portid(gpio_t pad);

/* Return the gpio pad number based table */
struct pad_config *new_padbased_table(void);

/* Must pass the table with pad number based */
void gpio_padbased_override(struct pad_config *padbased_table,
					const struct pad_config *override_cfg,
					size_t override_num_pads);

/*
 * Must pass the table with pad number based, will skip configures the unmapped
 * pins by check pad and DW0 are 0.
 */
void gpio_configure_pads_with_padbased(struct pad_config *padbased_table);

#endif
#endif /* _SOC_INTELBLOCKS_GPIO_H_ */
