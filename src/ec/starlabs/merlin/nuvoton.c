/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <ec/acpi/ec.h>
#include <option.h>
#include <pc80/keyboard.h>

#include "ec.h"
#include "ecdefs.h"

uint16_t ec_get_version(void)
{
	return (ec_read(ECRAM_MAJOR_VERSION) << 8) | ec_read(ECRAM_MINOR_VERSION);
}

static uint8_t get_ec_value_from_option(const char *name,
					unsigned int fallback,
					const uint8_t *lut,
					size_t lut_size)
{
	unsigned int index = get_uint_option(name, fallback);
	if (index >= lut_size)
		index = fallback;
	return lut[index];
}

static uint16_t ec_get_chip_id(unsigned int port)
{
	return pnp_read_index(port, NUVOTON_CHIPID);
}

static void merlin_init(struct device *dev)
{
	if (!dev->enabled)
		return;

	/*
	 * The address/data IO port pair for the Nuvoton EC are configurable
	 * through the EC domain and are fixed by the EC's firmware blob. If
	 * the value(s) passed through the "dev" structure don't match the
	 * expected values then output severe warnings.
	 */
	if (dev->path.pnp.port != NUVOTON_FIXED_ADDR) {
		printk(BIOS_ERR, "NUVOTON: Incorrect ports defined in devicetree.cb.\n");
		printk(BIOS_ERR, "NUVOTON: Serious operational issues will arise.\n");
		return;
	}

	const uint16_t chip_id = ec_get_chip_id(dev->path.pnp.port);

	if (chip_id != NUVOTON_CHIPID_VAL) {
		printk(BIOS_ERR, "NUVOTON: Expected chip ID 0x%04x, but got 0x%04x instead.\n",
			NUVOTON_CHIPID_VAL, chip_id);
		return;
	}

	pc_keyboard_init(NO_AUX_DEVICE);

	/*
	 * Restore settings from CMOS into EC RAM:
	 *
	 * kbl_timeout
	 * fn_ctrl_swap
	 * max_charge
	 * fan_mode
	 * fn_lock_state
	 * trackpad_state
	 * kbl_brightness
	 * kbl_state
	 */

	/*
	 * Keyboard Backlight Timeout
	 *
	 * Setting:	kbl_timeout
	 *
	 * Values:	30 Seconds, 1 Minute, 3 Minutes, 5 Minutes, Never
	 * Default:	30 Seconds
	 *
	 */
	const uint8_t kbl_timeout[] = {
		SEC_30,
		MIN_1,
		MIN_3,
		MIN_5,
		NEVER
	};

	ec_write(ECRAM_KBL_TIMEOUT,
		get_ec_value_from_option("kbl_timeout",
					 0,
					 kbl_timeout,
					 ARRAY_SIZE(kbl_timeout)));

	/*
	 * Fn Ctrl Reverse
	 *
	 * Setting:	fn_ctrl_swap
	 *
	 * Values:	Enabled, Disabled
	 * Default:	Disabled
	 *
	 */
	const uint8_t fn_ctrl_swap[] = {
		FN_CTRL,
		CTRL_FN
	};

	ec_write(ECRAM_FN_CTRL_REVERSE,
		get_ec_value_from_option("fn_ctrl_swap",
					 1,
					 fn_ctrl_swap,
					 ARRAY_SIZE(fn_ctrl_swap)));

	/*
	 * Maximum Charge Level
	 *
	 * Setting:	max_charge
	 *
	 * Values:	60%, 80%, 100%
	 * Default:	100%
	 *
	 */
	const uint8_t max_charge[] = {
		CHARGE_100,
		CHARGE_80,
		CHARGE_60
	};

	if (CONFIG(EC_STARLABS_MAX_CHARGE))
		ec_write(ECRAM_MAX_CHARGE,
			get_ec_value_from_option("max_charge",
						 0,
						 max_charge,
						 ARRAY_SIZE(max_charge)));

	/*
	 * Fast Charge
	 *
	 * Setting:	fast_charge
	 *
	 * Values:	Normal, Fast
	 * Default:	Normal
	 *
	 */
	const uint8_t fast_charge[] = {
		CHARGE_RATE_NORMAL,
		CHARGE_RATE_FAST
	};

	if (CONFIG(EC_STARLABS_FAST_CHARGE))
		ec_write(ECRAM_FAST_CHARGE,
			get_ec_value_from_option("fast_charge",
						 0,
						 fast_charge,
						 ARRAY_SIZE(fast_charge)));

	/*
	 * Fan Mode
	 *
	 * Setting:	fan_mode
	 *
	 * Values:	Quiet, Normal, Aggressive
	 * Default:	Normal
	 *
	 */
	const uint8_t fan_mode[] = {
		FAN_NORMAL,
		FAN_AGGRESSIVE,
		FAN_QUIET
	};

	if (CONFIG(EC_STARLABS_FAN))
		ec_write(ECRAM_FAN_MODE,
			get_ec_value_from_option("fan_mode",
						 0,
						 fan_mode,
						 ARRAY_SIZE(fan_mode)));

	/*
	 * Function Lock
	 *
	 * Setting:	fn_lock_state
	 *
	 * Values:	Locked, Unlocked
	 * Default:	Locked
	 *
	 */
	const uint8_t fn_lock_state[] = {
		UNLOCKED,
		LOCKED
	};

	ec_write(ECRAM_FN_LOCK_STATE,
		get_ec_value_from_option("fn_lock_state",
					 1,
					 fn_lock_state,
					 ARRAY_SIZE(fn_lock_state)));

	/*
	 * Trackpad State
	 *
	 * Setting:	trackpad_state
	 *
	 * Values:	Enabled, Disabled
	 * Default:	Enabled
	 *
	 */
	const uint8_t trackpad_state[] = {
		TRACKPAD_ENABLED,
		TRACKPAD_DISABLED
	};

	ec_write(ECRAM_TRACKPAD_STATE,
		get_ec_value_from_option("trackpad_state",
					 0,
					 trackpad_state,
					 ARRAY_SIZE(trackpad_state)));

	/*
	 * Keyboard Backlight Brightness
	 *
	 * Setting:	kbl_brightness
	 *
	 * Values:	Off, Low, High / Off, On
	 * Default:	Low
	 *
	 */
	const uint8_t kbl_brightness[] = {
		KBL_ON,
		KBL_OFF,
		KBL_LOW,
		KBL_HIGH
	};

	if (CONFIG(EC_STARLABS_KBL_LEVELS))
		ec_write(ECRAM_KBL_BRIGHTNESS,
			get_ec_value_from_option("kbl_brightness",
						 2,
						 kbl_brightness,
						 ARRAY_SIZE(kbl_brightness)));
	else
		ec_write(ECRAM_KBL_BRIGHTNESS,
			get_ec_value_from_option("kbl_brightness",
						 0,
						 kbl_brightness,
						 ARRAY_SIZE(kbl_brightness)));

	/*
	 * Keyboard Backlight State
	 *
	 * Setting:	kbl_state
	 *
	 * Values:	Off, On
	 * Default:	On
	 *
	 */
	const uint8_t kbl_state[] = {
		KBL_DISABLED,
		KBL_ENABLED
	};

	ec_write(ECRAM_KBL_STATE,
		get_ec_value_from_option("kbl_state",
					 1,
					 kbl_state,
					 ARRAY_SIZE(kbl_state)));
}

static struct device_operations ops = {
	.init		= merlin_init,
	.read_resources	= noop_read_resources,
	.set_resources	= noop_set_resources,
};

static struct pnp_info pnp_dev_info[] = {
	/* System Wake-Up Control (SWUC) */
	{ NULL,	NUVOTON_MSWC,	PNP_IO0 | PNP_IRQ0,		0xfff0,		},
	/* KBC / Mouse Interface */
	{ NULL, NUVOTON_KBCM,	PNP_IRQ0,					},
	/* KBC / Keyboard Interface */
	{ NULL, NUVOTON_KBCK,	PNP_IO0 | PNP_IO1 | PNP_IRQ0,	0x07ff,	0x07ff,	},
	/* Shared Memory / Flash Interface (SMFI) */
	{ NULL, NUVOTON_SHM,	PNP_IO0 | PNP_IRQ0,		0xfff0,		},
	/* Power Management I/F Channel 1 (PMC1) */
	{ NULL, NUVOTON_PM1,	PNP_IO0 | PNP_IO1 | PNP_IRQ0,	0x07ff,	0x07ff,	},
	/* Power Management I/F Channel 2 (PMC2) */
	{ NULL, NUVOTON_PM2,	PNP_IO0 | PNP_IO1 | PNP_IO2 | PNP_IRQ0,	0x07fc,
				0x07fc, 0xfff0,					},
	/* Power Management I/F Channel 3 (PMC3) */
	{ NULL, NUVOTON_PM3,	PNP_IO0 | PNP_IO1 | PNP_IRQ0,	0x07ff,	0x07ff,	},
	/* Extended Shared Memory (ESHM) */
	{ NULL, NUVOTON_ESHM							},
	/* Power Management I/F Channel 4 (PMC4) */
	{ NULL, NUVOTON_PM4,	PNP_IO0 | PNP_IO1 | PNP_IRQ0,	0x07ff,	0x07ff,	},
};

static void enable_dev(struct device *dev)
{
	pnp_enable_devices(dev, &ops, ARRAY_SIZE(pnp_dev_info), pnp_dev_info);
}

struct chip_operations ec_starlabs_merlin_ops = {
	CHIP_NAME("NUVOTON EC")
	.enable_dev = enable_dev
};
