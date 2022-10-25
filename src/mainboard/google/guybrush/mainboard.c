/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/psp.h>
#include <baseboard/variants.h>
#include <console/console.h>
#include <device/device.h>
#include <drivers/i2c/tpm/chip.h>
#include <gpio.h>
#include <soc/acpi.h>
#include <variant/ec.h>
#include <string.h>

#define BACKLIGHT_GPIO			GPIO_129
#define WWAN_AUX_RST_GPIO		GPIO_18
#define METHOD_BACKLIGHT_ENABLE		"\\_SB.BKEN"
#define METHOD_BACKLIGHT_DISABLE	"\\_SB.BKDS"
#define METHOD_MAINBOARD_INI		"\\_SB.MINI"
#define METHOD_MAINBOARD_WAK		"\\_SB.MWAK"
#define METHOD_MAINBOARD_PTS		"\\_SB.MPTS"
#define METHOD_MAINBOARD_S0X		"\\_SB.MS0X"

/*
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 */
static uint8_t fch_pic_routing[FCH_IRQ_ROUTING_ENTRIES];
static uint8_t fch_apic_routing[FCH_IRQ_ROUTING_ENTRIES];

/*
 * This controls the device -> IRQ routing.
 *
 * Hardcoded IRQs:
 *  0: timer < soc/amd/common/acpi/lpc.asl
 *  1: i8042 - Keyboard
 *  2: cascade
 *  8: rtc0 <- soc/amd/common/acpi/lpc.asl
 *  9: acpi <- soc/amd/common/acpi/lpc.asl
 */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	12,		PIRQ_NC },
	{ PIRQ_B,	14,		PIRQ_NC },
	{ PIRQ_C,	15,		PIRQ_NC },
	{ PIRQ_D,	12,		PIRQ_NC },
	{ PIRQ_E,	14,		PIRQ_NC },
	{ PIRQ_F,	15,		PIRQ_NC },
	{ PIRQ_G,	12,		PIRQ_NC },
	{ PIRQ_H,	14,		PIRQ_NC },

	{ PIRQ_SCI,	ACPI_SCI_IRQ,	ACPI_SCI_IRQ },
	{ PIRQ_SD,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_SDIO,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_SATA,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_EMMC,	PIRQ_NC,	PIRQ_NC },
	{ PIRQ_GPIO,	11,		11 },
	{ PIRQ_I2C0,	10,		10 },
	{ PIRQ_I2C1,	 7,		 7 },
	{ PIRQ_I2C2,	 6,		 6 },
	{ PIRQ_I2C3,	 5,		 5 },
	{ PIRQ_UART0,	 4,		 4 },
	{ PIRQ_UART1,	 3,		 3 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
	{ PIRQ_HPET_L,	0x00,		0x00 },
	{ PIRQ_HPET_H,	0x00,		0x00 },
};

static void init_tables(void)
{
	const struct fch_irq_routing *entry;
	int i;

	memset(fch_pic_routing, PIRQ_NC, sizeof(fch_pic_routing));
	memset(fch_apic_routing, PIRQ_NC, sizeof(fch_apic_routing));

	for (i = 0; i < ARRAY_SIZE(fch_irq_map); i++) {
		entry = fch_irq_map + i;
		fch_pic_routing[entry->intr_index] = entry->pic_irq_num;
		fch_apic_routing[entry->intr_index] = entry->apic_irq_num;
	}
}

static void pirq_setup(void)
{
	intr_data_ptr = fch_apic_routing;
	picr_data_ptr = fch_pic_routing;
}

static void mainboard_configure_gpios(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	base_gpios = baseboard_gpio_table(&base_num_gpios);
	override_gpios = variant_override_gpio_table(&override_num_gpios);

	gpio_configure_pads_with_override(base_gpios, base_num_gpios, override_gpios,
					  override_num_gpios);
}

void __weak variant_devtree_update(void)
{
}

static void configure_psp_tpm_gpio(void)
{
	const struct device *cr50_dev = DEV_PTR(cr50);
	struct drivers_i2c_tpm_config *cfg = config_of(cr50_dev);

	psp_set_tpm_irq_gpio(cfg->irq_gpio.pins[0]);
}

static void mainboard_init(void *chip_info)
{
	mainboard_configure_gpios();
	mainboard_ec_init();
	variant_devtree_update();

	/* Run this after variant_devtree_update so the IRQ is correct. */
	configure_psp_tpm_gpio();
}

static void mainboard_write_blken(void)
{
	acpigen_write_method(METHOD_BACKLIGHT_ENABLE, 0);
	acpigen_soc_clear_tx_gpio(BACKLIGHT_GPIO);
	acpigen_pop_len();
}

static void mainboard_write_blkdis(void)
{
	acpigen_write_method(METHOD_BACKLIGHT_DISABLE, 0);
	acpigen_soc_set_tx_gpio(BACKLIGHT_GPIO);
	acpigen_pop_len();
}

static void mainboard_write_mini(void)
{
	acpigen_write_method(METHOD_MAINBOARD_INI, 0);
	acpigen_emit_namestring(METHOD_BACKLIGHT_ENABLE);
	acpigen_pop_len();
}

static void mainboard_write_mwak(void)
{
	acpigen_write_method(METHOD_MAINBOARD_WAK, 0);
	acpigen_emit_namestring(METHOD_BACKLIGHT_ENABLE);
	acpigen_pop_len();
}

static void mainboard_write_mpts(void)
{
	acpigen_write_method(METHOD_MAINBOARD_PTS, 0);
	acpigen_emit_namestring(METHOD_BACKLIGHT_DISABLE);
	acpigen_pop_len();
}

static void mainboard_assert_wwan_aux_reset(void)
{
	if (variant_has_pcie_wwan())
		acpigen_soc_clear_tx_gpio(WWAN_AUX_RST_GPIO);
}

static void mainboard_deassert_wwan_aux_reset(void)
{
	if (variant_has_pcie_wwan())
		acpigen_soc_set_tx_gpio(WWAN_AUX_RST_GPIO);
}

static void mainboard_write_ms0x(void)
{
	acpigen_write_method_serialized(METHOD_MAINBOARD_S0X, 1);
	/* S0ix Entry */
	acpigen_write_if_lequal_op_int(ARG0_OP, 1);
	mainboard_assert_wwan_aux_reset();
	/* S0ix Exit */
	acpigen_write_else();
	mainboard_deassert_wwan_aux_reset();
	acpigen_pop_len();
	acpigen_pop_len();
}

static void mainboard_fill_ssdt(const struct device *dev)
{
	mainboard_write_blken();
	mainboard_write_blkdis();
	mainboard_write_mini();
	mainboard_write_mpts();
	mainboard_write_mwak();
	mainboard_write_ms0x();
}

static void mainboard_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;

	init_tables();
	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();

	/* TODO: b/184678786 - Move into espi_config */
	/* Unmask eSPI IRQ 1 (Keyboard) */
	pm_write32(PM_ESPI_INTR_CTRL, PM_ESPI_DEV_INTR_MASK & ~(BIT(1)));
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
};
