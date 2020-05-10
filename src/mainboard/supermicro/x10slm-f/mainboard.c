/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <console/console.h>
#include <device/device.h>
#include <option.h>
#include <stdint.h>
#include <types.h>

/*
 * Hiding the AST2400 might be desirable to reduce attack surface.
 *
 * The PCIe root port that the AST2400 is on is disabled, but the
 * AST2400 itself likely remains in an enabled state.
 *
 * The AST2400 is also attached to the LPC. That interface does not get
 * disabled.
 */
static void hide_ast2400(void)
{
	struct device *dev = pcidev_on_root(0x1c, 0);
	if (!dev)
		return;

	/*
	 * Marking this device as disabled means that the southbridge code
	 * will properly disable the root port when it configures it later.
	 */
	dev->enabled = 0;
	printk(BIOS_INFO, "The AST2400 is now set to be hidden.\n");
}

static void mainboard_enable(struct device *dev)
{
	u8 hide = 0;

	if (get_option(&hide, "hide_ast2400") == CB_SUCCESS && hide)
		hide_ast2400();
}

struct chip_operations mainboard_ops = {
	CHIP_NAME("X10SLM+-F")
	.enable_dev = mainboard_enable,
};
