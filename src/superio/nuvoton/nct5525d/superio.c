/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <superio/conf_mode.h>

#if CONFIG(HAVE_ACPI_TABLES)
#include <superio/common/ssdt.h>
#include <acpi/acpi.h>
#endif

#include "chip.h"
#include "nct5525d.h"

/* CR1A: pin 39 function */
#define MFS_1A_PIN39_CIRRX	(1 << 7)
/* CR1B: pin 39/40 and 46/48 function */
#define MFS_1B_PIN39_40_TSI	(1 << 2)
#define MFS_1B_PIN46_48_MASK	(0x3 << 0)
/* CR1C: pin 39 and pins 57-64 function */
#define MFS_1C_PIN39_IRRX	(1 << 7)
#define MFS_1C_UARTC_SHIFT	2
/* CR22 */
#define PWR_DOWN_AUXFANOUT2_PP	(1 << 2)
/* CR24 */
#define GLOBAL_24_SYSFANOUT_PP	(1 << 6)
#define GLOBAL_24_CPUFANOUT_PP	(1 << 5)
#define GLOBAL_24_BEEP_EN	(1 << 1)
/* CR2A */
#define GLOBAL_2A_AUXFAN2	(1 << 1)
/* CR2C: pins 14-22 function */
#define UART_OPT_UARTA_SHIFT	0

static bool check_chip_id(struct device *dev)
{
	const u16 id = pnp_read_config(dev, NCT5525D_CR_CHIP_ID_HI) << 8 |
		       pnp_read_config(dev, NCT5525D_CR_CHIP_ID_LO);

	if ((id & NCT5525D_CHIP_ID_MASK) != NCT5525D_CHIP_ID) {
		printk(BIOS_ERR, "NCT5525D: unexpected chip ID 0x%04x at 0x%x\n",
		       id, dev->path.pnp.port);
		return false;
	}

	printk(BIOS_DEBUG, "NCT5525D: chip ID 0x%04x at 0x%x\n", id, dev->path.pnp.port);
	return true;
}

static void configure_global(struct device *dev)
{
	const struct superio_nuvoton_nct5525d_config *conf = dev->chip_info;

	pnp_enter_conf_mode(dev);

	if (!check_chip_id(dev) || !conf)
		goto out;

	/* Pin mux: route pins 46/48 to GP63/GP64 */
	if (conf->pins_46_48 == NCT5525D_PINS_46_48_GPIO) {
		pnp_unset_and_set_config(dev, NCT5525D_CR_GLOBAL_2A, GLOBAL_2A_AUXFAN2, 0);
		pnp_unset_and_set_config(dev, NCT5525D_CR_MFS_1B, MFS_1B_PIN46_48_MASK, 0);
	}

	/* Pin mux: route pins 39/40 to TSIC/TSID */
	if (conf->tsi_pins) {
		/* IRRX and CIRRX take precedence over TSIC on pin 39 */
		pnp_unset_and_set_config(dev, NCT5525D_CR_MFS_1A, MFS_1A_PIN39_CIRRX, 0);
		pnp_unset_and_set_config(dev, NCT5525D_CR_MFS_1C, MFS_1C_PIN39_IRRX, 0);
		pnp_unset_and_set_config(dev, NCT5525D_CR_MFS_1B, 0, MFS_1B_PIN39_40_TSI);
	}

out:
	pnp_exit_conf_mode(dev);
}

/*
 * pnp_enable_devices() also allocates the LDNs missing from the devicetree,
 * and those default to enabled. Leave them untouched: several settings on
 * this chip, like Deep S5, are battery-backed.
 */
static bool in_devicetree(const struct device *dev)
{
	return dev->chip_ops != NULL;
}

static void nct5525d_enable(struct device *dev)
{
	if (in_devicetree(dev))
		pnp_alt_enable(dev);
}

static void nct5525d_set_resources(struct device *dev)
{
	if (in_devicetree(dev))
		pnp_set_resources(dev);
}

static void nct5525d_enable_resources(struct device *dev)
{
	if (in_devicetree(dev))
		pnp_enable_resources(dev);
}

#if CONFIG(HAVE_ACPI_TABLES)
static const char *nct5525d_acpi_hid(const struct device *dev)
{
	if ((dev->path.type != DEVICE_PATH_PNP) || (dev->path.pnp.port == 0))
		return NULL;

	if (dev->path.pnp.device == NCT5525D_HWM_FPLED)
		return ACPI_HID_PNP;

	return NULL;
}

static void nct5525d_fill_ssdt(const struct device *dev)
{
	if (in_devicetree(dev) && nct5525d_acpi_hid(dev))
		superio_common_fill_ssdt_generator(dev);
}
#endif

static struct device_operations ops = {
	.read_resources   = pnp_read_resources,
	.set_resources    = nct5525d_set_resources,
	.enable_resources = nct5525d_enable_resources,
	.enable           = nct5525d_enable,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
#if CONFIG(HAVE_ACPI_TABLES)
	.acpi_fill_ssdt   = nct5525d_fill_ssdt,
	.acpi_name        = superio_common_ldn_acpi_name,
	.acpi_hid         = nct5525d_acpi_hid,
#endif
};

static struct pnp_info pnp_dev_info[] = {
	{ NULL, NCT5525D_SP1, PNP_IO0 | PNP_IRQ0, 0x0ff8, },
	{ NULL, NCT5525D_KBC, PNP_IO0 | PNP_IO1 | PNP_IRQ0 | PNP_IRQ1,
		0x0fff, 0x0fff, },
	/* Hardware monitor: 8 bytes, SMBus master: 32 bytes */
	{ NULL, NCT5525D_HWM_FPLED, PNP_IO0 | PNP_IO1 | PNP_IRQ0,
		0x0ff8, 0x0fe0, },
	/* CR30 bit 0 enables Deep S5, a battery-backed setting */
	{ NULL, NCT5525D_DS5},
};

static void enable_dev(struct device *dev)
{
	/* The chip responds only on 0x2e or 0x4e, chosen by a strap */
	static bool configured[2];
	const bool alt_port = dev->path.pnp.port == 0x4e;

	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);

	if (dev->path.type == DEVICE_PATH_PNP && dev->ops && !configured[alt_port]) {
		configured[alt_port] = true;
		configure_global(dev);
	}
}

struct chip_operations superio_nuvoton_nct5525d_ops = {
	.name = "NUVOTON NCT5525D Super I/O",
	.enable_dev = enable_dev,
};
