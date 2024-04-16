/* SPDX-License-Identifier: GPL-2.0-or-later */

/*
 * Configuration routines common to Nuvoton SIO chips,
 * except nct5104d, nct6687d, npcd378, wpcm450.
 */

#include <console/console.h>
#include <device/device.h>
#include <device/pnp.h>
#include <option.h>
#include <pc80/keyboard.h>
#include <superio/conf_mode.h>
#include "nuvoton.h"

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#define POWER_LOSS_CONTROL_SHIFT	5
#define POWER_LOSS_CONTROL_MASK		(3 << POWER_LOSS_CONTROL_SHIFT)

/* CR 0xe0 */
#define KBD_WAKEUP_PSOUT	BIT(6)
#define KBD_MS_SWAP		BIT(2)
#define KBXKEY			BIT(0)
/* CR 0xe4 */
/* Power RAM in S3; for reference only; this needs to be enabled in bootblock */
#define EN_3VSBSW		BIT(4)
#define KBD_WAKEUP_ANYKEY	BIT(3)

static void enable_ps2_mouse(struct device *dev, bool en)
{
	if (!CONFIG(HAVE_SHARED_PS2_PORT))
		return;

	pnp_enter_conf_mode(dev);
	u8 bit = KBD_MS_SWAP;
	/* pnp_set_logical_device() is not used because the bit is in another LD. */
	pnp_write_config(dev, 0x7, NCT677X_ACPI);
	pnp_unset_and_set_config(dev, 0xe0, bit, en ? bit : 0);
	pnp_exit_conf_mode(dev);
}

void nuvoton_common_init(struct device *dev)
{
	u8 byte, role, mask;

	if (!dev->enabled)
		return;

	switch (dev->path.pnp.device) {
	case NCT677X_KBC:
		/*
		 * If mainboard has both PS/2 keyboard and mouse ports, bootblock code
		 * should enable both and leave them enabled.
		 */
		role = PS2_PORT_ROLE_KEYBOARD;
		if (CONFIG(HAVE_SHARED_PS2_PORT)) {
			role = get_uint_option("ps2_port_role", PS2_PORT_ROLE_KEYBOARD);
			if (role > PS2_PORT_ROLE_AUTO)
				break; /* Invalid setting; abort */
		}
		if (role != PS2_PORT_ROLE_KEYBOARD) {
			enable_ps2_mouse(dev, true);
			if (role == PS2_PORT_ROLE_MOUSE)
				break; /* Leave the port swapped; we're done */

			/* This leaves PS2_PORT_ROLE_AUTO */
			if (pc_keyboard_init(PROBE_AUX_DEVICE)) {
				break; /* Mouse found; leave the port swapped */
			} else {
				printk(BIOS_INFO, "%s: Mouse not detected.\n", __func__);
			}
		}
		/* Set port as keyboard, and initialize keyboard if enabled in Kconfig */
		enable_ps2_mouse(dev, false);
		pc_keyboard_init(NO_AUX_DEVICE);
		break;
	case NCT677X_ACPI:
		pnp_enter_conf_mode(dev);
		pnp_set_logical_device(dev);

		/*
		 * Keyboard wakeup: any key
		 * NOTE: Bits related to keyboard wakeup are kept even in S5 (soft off),
		 *       and will allow keyboard to wake system even from this state.
		 *       Add SMM/ACPI code to turn it back off before entering S5.
		 */
		pnp_unset_and_set_config(dev, 0xe0, 0, KBD_WAKEUP_PSOUT | KBXKEY);

		byte = KBD_WAKEUP_ANYKEY;
		mask = 0;
		/*
		 * Set power state after power fail.
		 *
		 * Important: Make sure the definitions in mainboard/Kconfig around
		 * MAINBOARD_POWER_FAILURE_STATE and your board's cmos.layout match.
		 * And if MAINBOARD_POWER_FAILURE_STATE deviates from the chip's
		 * expectation (ie. 0=off, 1=on, 2=keep), this code must be adapted.
		 */
		if (CONFIG(HAVE_POWER_STATE_AFTER_FAILURE)) {
			unsigned int power_status = get_uint_option("power_on_after_fail",
				CONFIG_MAINBOARD_POWER_FAILURE_STATE);

			switch (power_status) {
			case MAINBOARD_POWER_KEEP:
				/*
				 * KEEP power state is software emulated using USER and
				 * a manually set power state.
				 */
				power_status = 3;
				__fallthrough;
			case MAINBOARD_POWER_OFF:
			case MAINBOARD_POWER_ON:
				byte |= (power_status << POWER_LOSS_CONTROL_SHIFT);
				mask = POWER_LOSS_CONTROL_MASK;
				break;
			}
			/* Log a "on" power state */
			pnp_unset_and_set_config(dev, 0xe6, BIT(4), 0);
		}

		pnp_unset_and_set_config(dev, 0xe4, mask, byte);
		pnp_exit_conf_mode(dev);
		break;
	}
}

struct device_operations nuvoton_common_ops = {
	.read_resources   = noop_read_resources,
	.set_resources    = pnp_set_resources,
	.enable_resources = pnp_enable_resources,
	.enable           = pnp_alt_enable,
	.init             = nuvoton_common_init,
	.ops_pnp_mode     = &pnp_conf_mode_8787_aa,
};
