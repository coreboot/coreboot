/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/mmio.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/gpio.h>
#include <amdblocks/smi.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <smbios.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/soc_util.h>
#include <amdblocks/acpimmio.h>
#include <variant/ec.h>
#include <variant/thermal.h>
#include <commonlib/helpers.h>

#define METHOD_BACKLIGHT_ENABLE    "\\_SB.BKEN"
#define METHOD_BACKLIGHT_DISABLE   "\\_SB.BKDS"
#define METHOD_MAINBOARD_INI       "\\_SB.MINI"
#define METHOD_MAINBOARD_WAK       "\\_SB.MWAK"
#define METHOD_MAINBOARD_PTS       "\\_SB.MPTS"

/* The IRQ mapping in fch_irq_map ends up getting written to the indirect address space that is
   accessed via I/O ports 0xc00/0xc01. */

/*
 * This controls the device -> IRQ routing.
 *
 * Hardcoded IRQs:
 *  0: timer < soc/amd/common/acpi/lpc.asl
 *  1: i8042 <- ec/google/chromeec/acpi/superio.asl
 *  2: cascade
 *  8: rtc0 <- soc/amd/common/acpi/lpc.asl
 *  9: acpi <- soc/amd/common/acpi/lpc.asl
 * 12: i8042 <- ec/google/chromeec/acpi/superio.asl
 */
static const struct fch_irq_routing fch_irq_map[] = {
	{ PIRQ_A,	6,		PIRQ_NC },
	{ PIRQ_B,	13,		PIRQ_NC },
	{ PIRQ_C,	14,		PIRQ_NC },
	{ PIRQ_D,	15,		PIRQ_NC },
	{ PIRQ_E,	15,		PIRQ_NC },
	{ PIRQ_F,	14,		PIRQ_NC },
	{ PIRQ_G,	13,		PIRQ_NC },
	{ PIRQ_H,	6,		PIRQ_NC },

	{ PIRQ_SCI,	9,		9 },
	{ PIRQ_EMMC,	5,		5 },
	{ PIRQ_GPIO,	7,		7 },
	{ PIRQ_I2C2,	10,		10 },
	{ PIRQ_I2C3,	11,		11 },
	{ PIRQ_UART0,	4,		4 },
	{ PIRQ_UART1,	3,		3 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0x91,		0x00 },
	{ PIRQ_MISC1,	0x00,		0x00 },
	{ PIRQ_MISC2,	0x00,		0x00 },
};

const struct fch_irq_routing *mb_get_fch_irq_mapping(size_t *length)
{
	*length = ARRAY_SIZE(fch_irq_map);
	return fch_irq_map;
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

static void mainboard_devtree_update(void)
{
	variant_audio_update();
	variant_bluetooth_update();
	variant_touchscreen_update();
	variant_devtree_update();
}

static void mainboard_init(void *chip_info)
{
	int boardid;

	mainboard_ec_init();
	boardid = board_id();
	printk(BIOS_INFO, "Board ID: %d\n", boardid);

	mainboard_configure_gpios();

	/* Update DUT configuration */
	mainboard_devtree_update();
}

void mainboard_get_dxio_ddi_descriptors(const fsp_dxio_descriptor **dxio_descs,
					size_t *dxio_num,
					const fsp_ddi_descriptor **ddi_descs,
					size_t *ddi_num)
{
	variant_get_dxio_ddi_descriptors(dxio_descs, dxio_num, ddi_descs, ddi_num);
}

static void mainboard_write_blken(void)
{
	acpigen_write_method(METHOD_BACKLIGHT_ENABLE, 0);
	acpigen_soc_clear_tx_gpio(GPIO_85);
	acpigen_pop_len();
}

static void mainboard_write_blkdis(void)
{
	acpigen_write_method(METHOD_BACKLIGHT_DISABLE, 0);
	acpigen_soc_set_tx_gpio(GPIO_85);
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

static void mainboard_fill_ssdt(const struct device *dev)
{
	mainboard_write_blken();
	mainboard_write_blkdis();
	mainboard_write_mini();
	mainboard_write_mpts();
	mainboard_write_mwak();
}

/*************************************************
 * Dedicated mainboard function
 *************************************************/
static void mainboard_enable(struct device *dev)
{
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;
}

static void mainboard_final(void *chip_info)
{
	finalize_gpios(acpi_get_sleep_type());
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = mainboard_enable,
	.final = mainboard_final,
};

void __weak variant_devtree_update(void)
{
}

__weak const struct soc_amd_gpio *variant_override_gpio_table(size_t *size)
{
	/* Default weak implementation - No overrides. */
	*size = 0;
	return NULL;
}
