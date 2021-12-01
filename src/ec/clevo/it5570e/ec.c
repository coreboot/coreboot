/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <cpu/x86/msr.h>
#include <device/device.h>
#include <device/mmio.h>
#include <device/pnp.h>
#include <ec/acpi/ec.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <soc/msr.h>
#include <superio/conf_mode.h>

#include "chip.h"
#include "commands.h"
#include "ec.h"

static void pnp_configure_smfi(void)
{
	if (!CONFIG_EC_CLEVO_IT5570E_MEM_BASE) {
		printk(BIOS_ERR, "EC: no LGMR base address configured. Check your config!\n");
		return;
	}

	/* Check for valid address (0xfeXXX000/0xffXXX000) */
	if ((CONFIG_EC_CLEVO_IT5570E_MEM_BASE & 0xfe000fff) != 0xfe000000) {
		printk(BIOS_ERR, "EC: LGMR base address 0x%08x invalid. Check your config!\n",
				 CONFIG_EC_CLEVO_IT5570E_MEM_BASE);
		return;
	}

	struct device dev = {
		.path.type       = DEVICE_PATH_PNP,
		.path.pnp.port   = 0x2e,
		.path.pnp.device = IT5570E_SMFI,
	};
	dev.ops->ops_pnp_mode = &pnp_conf_mode_870155_aa;

	/* Configure SMFI for LGMR */
	pnp_enter_conf_mode(&dev);
	pnp_set_logical_device(&dev);
	pnp_set_enable(&dev, 1);
	pnp_write_config(&dev, HLPCRAMBA_24,    CONFIG_EC_CLEVO_IT5570E_MEM_BASE >> 24 & 0x01);
	pnp_write_config(&dev, HLPCRAMBA_23_16, CONFIG_EC_CLEVO_IT5570E_MEM_BASE >> 16 & 0xff);
	pnp_write_config(&dev, HLPCRAMBA_15_12, CONFIG_EC_CLEVO_IT5570E_MEM_BASE >>  8 & 0xf0);
	pnp_exit_conf_mode(&dev);
}

static void ec_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	const ec_config_t *config = config_of(dev);
	printk(BIOS_DEBUG, "%s init.\n", dev->chip_ops->name);

	const char *const model = ec_read_model();
	const char *const version = ec_read_fw_version();
	printk(BIOS_DEBUG, "EC FW: model %s, version %s\n", model, version);

	pnp_configure_smfi();

	ec_set_ac_fan_always_on(
		get_uint_option("ac_fan_always_on", CONFIG(EC_CLEVO_IT5570E_AC_FAN_ALWAYS_ON)));

	ec_set_kbled_timeout(
		get_uint_option("kbled_timeout", CONFIG_EC_CLEVO_IT5570E_KBLED_TIMEOUT));

	ec_set_fn_win_swap(
		get_uint_option("fn_win_swap", CONFIG(EC_CLEVO_IT5570E_FN_WIN_SWAP)));

	ec_set_flexicharger(
		get_uint_option("flexicharger", CONFIG(EC_CLEVO_IT5570E_FLEXICHARGER)),
		get_uint_option("flexicharger_start", CONFIG_EC_CLEVO_IT5570E_FLEXICHG_START),
		get_uint_option("flexicharger_stop",  CONFIG_EC_CLEVO_IT5570E_FLEXICHG_STOP));

	ec_set_camera_boot_state(
		get_uint_option("camera_boot_state", CONFIG_EC_CLEVO_IT5570E_CAM_BOOT_STATE));

	ec_set_tp_toggle_mode(
		get_uint_option("tp_toggle_mode", CONFIG_EC_CLEVO_IT5570E_TP_TOGGLE_MODE));

	/*
	 * The vendor abuses the field PL2B (originally named PL1T) to set PL2 via PECI on
	 * battery-only. With AC attached, PL2B (PL1T) gets set as PL1 and PL2T as PL2, but
	 * both are never enabled (bit 15). Since PL1 is never enabled, Tau isn't either.
	 * Thus, set PL2T, TAUT to zero, so the EC doesn't write these non-effective values.
	 */
	const uint16_t power_unit = 1 << (msr_read(MSR_PKG_POWER_SKU_UNIT) & 0xf);
	write16p(ECRAM + PL2B, config->pl2_on_battery * power_unit);
	write16p(ECRAM + PL2T, 0);
	write16p(ECRAM + TAUT, 0);

	ec_set_aprd();

	pc_keyboard_init(NO_AUX_DEVICE);
}

static const char *ec_acpi_name(const struct device *dev)
{
	return "EC0";
}

static void ec_fill_ssdt_generator(const struct device *dev)
{
	ec_fan_curve_fill_ssdt(dev);
}

static struct device_operations ec_ops = {
	.init		= ec_init,
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
	.acpi_fill_ssdt	= ec_fill_ssdt_generator,
	.acpi_name	= ec_acpi_name,
};

static void enable_dev(struct device *dev)
{
	if (dev->path.type == DEVICE_PATH_GENERIC && dev->path.generic.id == 0)
		dev->ops = &ec_ops;
	else
		printk(BIOS_ERR, "EC: Unknown device. Check your devicetree!\n");
}

struct chip_operations ec_clevo_it5570e_ops = {
	CHIP_NAME("Clevo IT5570E EC")
	.enable_dev = enable_dev,
};
