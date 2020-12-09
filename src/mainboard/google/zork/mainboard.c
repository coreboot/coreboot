/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <string.h>
#include <console/console.h>
#include <cbfs.h>
#include <device/device.h>
#include <device/mmio.h>
#include <acpi/acpi.h>
#include <acpi/acpigen.h>
#include <acpi/acpi_gnvs.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/gpio_banks.h>
#include <amdblocks/smi.h>
#include <baseboard/variants.h>
#include <boardid.h>
#include <gpio.h>
#include <smbios.h>
#include <soc/cpu.h>
#include <soc/gpio.h>
#include <soc/nvs.h>
#include <soc/pci_devs.h>
#include <soc/platform_descriptors.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/soc_util.h>
#include <amdblocks/acpimmio.h>
#include <variant/ec.h>
#include <variant/thermal.h>
#include <vendorcode/google/chromeos/chromeos.h>
#include <commonlib/helpers.h>
#include <bootstate.h>

#define METHOD_BACKLIGHT_ENABLE    "\\_SB.BKEN"
#define METHOD_BACKLIGHT_DISABLE   "\\_SB.BKDS"
#define METHOD_MAINBOARD_INI       "\\_SB.MINI"
#define METHOD_MAINBOARD_WAK       "\\_SB.MWAK"
#define METHOD_MAINBOARD_PTS       "\\_SB.MPTS"

/***********************************************************
 * These arrays set up the FCH PCI_INTR registers 0xC00/0xC01.
 * This table is responsible for physically routing the PIC and
 * IOAPIC IRQs to the different PCI devices on the system.  It
 * is read and written via registers 0xC00/0xC01 as an
 * Index/Data pair.  These values are chipset and mainboard
 * dependent and should be updated accordingly.
 */
static uint8_t fch_pic_routing[0x80];
static uint8_t fch_apic_routing[0x80];

_Static_assert(sizeof(fch_pic_routing) == sizeof(fch_apic_routing),
	"PIC and APIC FCH interrupt tables must be the same size");

/*
 * This table doesn't actually perform any routing. It only populates the
 * PCI_INTERRUPT_LINE register on the PCI device with the PIC value specified
 * in fch_apic_routing. The linux kernel only looks at this field as a backup
 * if ACPI routing fails to describe the PCI routing correctly. The linux kernel
 * also uses the APIC by default, so the value coded into the registers will be
 * wrong.
 *
 * This table is also confusing because PCI Interrupt routing happens at the
 * device/slot level, not the function level.
 */
static const struct pirq_struct mainboard_pirq_data[] = {
	{ PCIE_GPP_0_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE_GPP_1_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } }, // Bridge 1 - Wifi
	{ PCIE_GPP_2_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } }, // Bridge 2 - SD
	{ PCIE_GPP_3_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE_GPP_4_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE_GPP_5_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE_GPP_6_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } }, // Bridge 6 - NVME
	{ PCIE_GPP_A_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ PCIE_GPP_B_DEVFN,	{ PIRQ_A, PIRQ_B, PIRQ_C, PIRQ_D } },
	{ SMBUS_DEVFN,	{ PIRQ_SMBUS, PIRQ_NC, PIRQ_NC, PIRQ_NC } },
};

/*
 * This controls the device -> IRQ routing.
 * The PIC values are limited to 0,1, 3 - 12, 14, 15.
 */
static const struct fch_irq_routing {
	uint8_t intr_index;
	uint8_t pic_irq_num;
	uint8_t apic_irq_num;
} fch_pirq[] = {
	{ PIRQ_A,	6,		16 },
	{ PIRQ_B,	6,		17 },
	{ PIRQ_C,	14,		18 },
	{ PIRQ_D,	15,		19 },
	{ PIRQ_SCI,	9,		9 },
	{ PIRQ_EMMC,	5,		5 },
	{ PIRQ_GPIO,	7,		7 },
	{ PIRQ_I2C2,	10,		10 },
	{ PIRQ_I2C3,	11,		11 },
	{ PIRQ_UART0,	4,		4 },
	{ PIRQ_UART1,	3,		3 },

	/* The MISC registers are not interrupt numbers */
	{ PIRQ_MISC,	0xfa,		0x00 },
	{ PIRQ_MISC0,	0xf1,		0x00 },
	{ PIRQ_MISC1,	0x00,		0x00 },
	{ PIRQ_MISC2,	0x00,		0x00 },
};

static void init_tables(void)
{
	const struct fch_irq_routing *entry;
	int i;

	memset(fch_pic_routing, PIRQ_NC, sizeof(fch_pic_routing));
	memset(fch_apic_routing, PIRQ_NC, sizeof(fch_apic_routing));

	for (i = 0; i < ARRAY_SIZE(fch_pirq); i++) {
		entry = fch_pirq + i;
		fch_pic_routing[entry->intr_index] = entry->pic_irq_num;
		fch_apic_routing[entry->intr_index] = entry->apic_irq_num;
	}
}

/* PIRQ Setup */
static void pirq_setup(void)
{
	init_tables();

	pirq_data_ptr = mainboard_pirq_data;
	pirq_data_size = ARRAY_SIZE(mainboard_pirq_data);
	intr_data_ptr = fch_apic_routing;
	picr_data_ptr = fch_pic_routing;
}

static void mainboard_configure_gpios(void)
{
	size_t base_num_gpios, override_num_gpios;
	const struct soc_amd_gpio *base_gpios, *override_gpios;

	base_gpios = variant_base_gpio_table(&base_num_gpios);
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
static void zork_enable(struct device *dev)
{
	printk(BIOS_INFO, "Mainboard " CONFIG_MAINBOARD_PART_NUMBER " Enable.\n");

	/* Initialize the PIRQ data structures for consumption */
	pirq_setup();

	dev->ops->acpi_inject_dsdt = chromeos_dsdt_generator;
	dev->ops->acpi_fill_ssdt = mainboard_fill_ssdt;

}

static void mainboard_final(void *chip_info)
{
	struct global_nvs *gnvs;

	gnvs = acpi_get_gnvs();

	if (gnvs) {
		gnvs->tmps = CTL_TDP_SENSOR_ID;
		gnvs->tcrt = CRITICAL_TEMPERATURE;
		gnvs->tpsv = PASSIVE_TEMPERATURE;
	}

	finalize_gpios(acpi_get_sleep_type());
}

struct chip_operations mainboard_ops = {
	.init = mainboard_init,
	.enable_dev = zork_enable,
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
