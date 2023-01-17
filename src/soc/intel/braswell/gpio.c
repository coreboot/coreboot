/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <console/console.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <gpio.h>
#include <soc/pm.h>
#include <soc/smm.h>

#define GPIO_DEBUG

/* gpio map to pad number LUTs */

static const u8 gpncommunity_gpio_to_pad[GP_NORTH_COUNT] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8, 15,
	16, 17, 18, 19, 20, 21, 22, 23, 24, 25,
	26, 27, 30, 31, 32, 33, 34, 35, 36, 37,
	38, 39, 40, 41, 45, 46, 47, 48, 49, 50,
	51, 52, 53, 54, 55, 56, 60, 61, 62, 63,
	64, 65, 66, 67, 68, 69, 70, 71, 72 };

static const u8 gpsecommunity_gpio_to_pad[GP_SOUTHEAST_COUNT] = {
	 0,  1,  2,  3,  4,  5,  6,  7, 15, 16,
	17, 18, 19, 20, 21, 22, 23, 24, 25, 26,
	30, 31, 32, 33, 34, 35, 45, 46, 47, 48,
	49, 50, 51, 52, 60, 61, 62, 63, 64, 65,
	66, 67, 68, 69, 75, 76, 77, 78, 79, 80,
	81, 82, 83, 84, 85 };

static const u8 gpswcommunity_gpio_to_pad[GP_SOUTHWEST_COUNT] = {
	 0,  1,  2,  3,  4,  5,  6,  7, 15, 16,
	17, 18, 19, 20, 21, 22, 30, 31, 32, 33,
	34, 35, 36, 37, 45, 46, 47, 48, 49, 50,
	51, 52, 60, 61, 62, 63, 64, 65, 66, 67,
	75, 76, 77, 78, 79, 80, 81, 82, 90, 91,
	92, 93, 94, 95, 96, 97 };

static const u8 gpecommunity_gpio_to_pad[GP_EAST_COUNT] = {
	 0,  1,  2,  3,  4,  5,  6,  7,  8,  9,
	10, 11, 15, 16, 17, 18, 19, 20, 21, 22,
	23, 24, 25, 26 };

/* GPIO Community descriptions */
static const struct gpio_bank gpnorth_community = {
	.gpio_count  = GP_NORTH_COUNT,
	.gpio_to_pad = gpncommunity_gpio_to_pad,
	.pad_base    = COMMUNITY_GPNORTH_BASE,
	.has_gpe_en  = GPE_CAPABLE,
	.has_wake_en = 1,
};

static const struct gpio_bank gpsoutheast_community = {
	.gpio_count  = GP_SOUTHEAST_COUNT,
	.gpio_to_pad = gpsecommunity_gpio_to_pad,
	.pad_base    = COMMUNITY_GPSOUTHEAST_BASE,
	.has_gpe_en  = GPE_CAPABLE_NONE,
	.has_wake_en = 1,
};

static const struct gpio_bank gpsouthwest_community = {
	.gpio_count  = GP_SOUTHWEST_COUNT,
	.gpio_to_pad = gpswcommunity_gpio_to_pad,
	.pad_base    = COMMUNITY_GPSOUTHWEST_BASE,
	.has_gpe_en  = GPE_CAPABLE,
	.has_wake_en = 1,
};

static const struct gpio_bank gpeast_community = {
	.gpio_count  = GP_EAST_COUNT,
	.gpio_to_pad = gpecommunity_gpio_to_pad,
	.pad_base    = COMMUNITY_GPEAST_BASE,
	.has_gpe_en  = GPE_CAPABLE_NONE,
	.has_wake_en = 1,
};

static void setup_gpio_route(const struct soc_gpio_map *sw_gpios,
			     const struct soc_gpio_map *n_gpios)
{
	const struct soc_gpio_map *n_config;
	const struct soc_gpio_map *sw_config;
	uint32_t route_reg = 0;
	uint32_t int_selection = 0;
	uint32_t alt_gpio_smi = 0;
	uint32_t gpe0a_en = 0;
	int gpio = 0;
	int north_done = 0;
	int south_done = 0;

	for (sw_config = sw_gpios, n_config = n_gpios;
		(!north_done || !south_done); sw_config++, n_config++, gpio++) {

		/* When north config is done */
		if ((gpio > GP_NORTH_COUNT) || (n_config->pad_conf0 == GPIO_LIST_END))
			north_done = 1;

		/* When southwest config is done */
		if ((gpio > GP_SOUTHWEST_COUNT) || (sw_config->pad_conf0 == GPIO_LIST_END))
			south_done = 1;

		/* Route north gpios */
		if (!north_done) {
			 /* Int select from 8 to 15 */
			int_selection = ((n_config->pad_conf0 >> 28) & 0xf);

			if (n_config->gpe == SMI) {
				/* Set the corresponding bits (01) as per the interrupt line */
				route_reg |= (1 << ((int_selection - 8) * 2));

				/* Reset the higher bit */
				route_reg    &= ~(1 << ((int_selection - 8) * 2 + 1));
				alt_gpio_smi |=  (1 <<  (int_selection + 8));

			} else if (n_config->gpe == SCI) {
				/* Set the corresponding bits as per the interrupt line */
				route_reg |= (1 << (((int_selection - 8) * 2) + 1));

				/* Reset the bit */
				route_reg &= ~(1 << ((int_selection - 8) * 2));
				gpe0a_en  |=  (1 <<  (int_selection + 8));
			}
		}

		/* Route southwest gpios */
		if (!south_done) {
			 /* Int select from 8 to 15 */
			int_selection = ((sw_config->pad_conf0 >> 28) & 0xf);

			if (sw_config->gpe == SMI) {
				/* Set the corresponding bits (10) as per the interrupt line */
				route_reg    |=  (1 << (int_selection * 2));
				route_reg    &= ~(1 << (int_selection * 2 + 1));
				alt_gpio_smi |=  (1 << (int_selection + 16));

			} else if (sw_config->gpe == SCI) {
				/* Set the corresponding bits as per the interrupt line */
				route_reg |= (1 << ((int_selection * 2) + 1));

				/* Reset the bit */
				route_reg &= ~(1 << (int_selection * 2));
				gpe0a_en  |=  (1 << (int_selection + 16));
			}
		}
	}

	/* Enable gpe bits in GPE0A_EN_REG */
	outl(gpe0a_en, ACPI_BASE_ADDRESS + GPE0A_EN_REG);

#ifdef GPIO_DEBUG
	printk(BIOS_DEBUG, "gpio_rout = %x alt_gpio_smi = %x  gpe0a_en = %x\n",
	route_reg, alt_gpio_smi, gpe0a_en);
#endif
	/* Save as an SMM param */
	smm_southcluster_save_param(SMM_SAVE_PARAM_GPIO_ROUTE, route_reg);
}

static void setup_gpios(const struct soc_gpio_map *gpios, const struct gpio_bank *community)
{
	const struct soc_gpio_map *config;
	int gpio = 0;
	u32 reg, family, internal_pad_num;
	u32 mmio_addr, int_selection;
	u32 gpio_wake0 = 0;
	u32 gpio_wake1 = 0;
	u32 gpio_int_mask = 0;

	if (!gpios)
		return;

	for (config = gpios; config->pad_conf0 != GPIO_LIST_END; config++, gpio++) {
		if (gpio > community->gpio_count)
			break;

		/* Pad configuration registers */
		family = community->gpio_to_pad[gpio] / MAX_FAMILY_PAD_GPIO_NO;
		internal_pad_num = community->gpio_to_pad[gpio] % MAX_FAMILY_PAD_GPIO_NO;

		/*
		 * Calculate the MMIO Address for GPIO pin control register pointed by index.
		 * REG = IOBASE + COMMUNITY_BASE + 0x4400 + (0x400 * FAMILY_NUM) + (8 * PAD_NUM)
		 */
		mmio_addr = FAMILY_PAD_REGS_OFF + (FAMILY_PAD_REGS_SIZE * family) +
				(GPIO_REGS_SIZE * internal_pad_num);

		reg = community->pad_base + mmio_addr;

		/* Get int selection value */
		int_selection = ((config->pad_conf0 >> 28) & 0xf);

		/* Get int mask register value */
		gpio_int_mask |= (config->int_mask << int_selection);

		/* Wake capable programming, some communities have 2 wake regs */
		if (gpio > 31)
			gpio_wake1 |= config->wake_mask << (gpio % 32);
		else
			gpio_wake0 |= config->wake_mask << gpio;

		if (!config->skip_config) {
#ifdef GPIO_DEBUG
			printk(BIOS_DEBUG,
				"Write Pad: Base(%x) - conf0 = %x conf1= %x gpio #- %d pad # = %d\n",
				reg, config->pad_conf0, config->pad_conf1,
				community->gpio_to_pad[gpio], gpio);
#endif
			/* Write pad configurations to conf0 and conf1 register */
			write32((void *)(reg + PAD_CONF0_REG), config->pad_conf0);
			write32((void *)(reg + PAD_CONF1_REG), config->pad_conf1);
		}
	}

#ifdef GPIO_DEBUG
	printk(BIOS_DEBUG, "gpio_wake_mask0 = %x gpio_wake_mask1 = %x gpio_int_mask = %x\n",
		gpio_wake0, gpio_wake1, gpio_int_mask);
#endif

	/* Wake */
	write32((void *)(community->pad_base + GPIO_WAKE_MASK_REG0), gpio_wake0);

	/* Wake mask config for communities with 2 regs */
	if (community->gpio_count > 32)
		write32((void *)(community->pad_base + GPIO_WAKE_MASK_REG1), gpio_wake1);

	/* Interrupt */
	write32((void *)(community->pad_base + GPIO_INTERRUPT_MASK), gpio_int_mask);
}

void setup_soc_gpios(struct soc_gpio_config *config, u8 enable_xdp_tap)
{
	if (config) {

		/*
		 * Write the default value 0xffffff to the SW write_access_policy_interrupt_reg
		 * to allow the SW interrupt mask register to be set
		 */
		write32((void *)(COMMUNITY_GPSOUTHWEST_BASE + 0x108), 0xffffffff);

		printk(BIOS_DEBUG, "north\n");
		setup_gpios(config->north, &gpnorth_community);

		printk(BIOS_DEBUG, "southwest\n");
		setup_gpios(config->southwest, &gpsouthwest_community);

		printk(BIOS_DEBUG, "southeast\n");
		setup_gpios(config->southeast, &gpsoutheast_community);

		printk(BIOS_DEBUG, "east\n");
		setup_gpios(config->east, &gpeast_community);

		printk(BIOS_DEBUG, "Routing SW and N gpios\n");
		setup_gpio_route(config->southwest, config->north);
	}

	/*
	 * Set on die termination feature with pull up value
	 * and drive the pad high for TAP_TDO and TAP_TMS
	 */
	if (!enable_xdp_tap)
		printk(BIOS_DEBUG, "Tri-state TDO and TMS\n");
}

__weak struct soc_gpio_config *mainboard_get_gpios(void)
{
	printk(BIOS_DEBUG, "Default/empty GPIO config\n");
	return NULL;
}
