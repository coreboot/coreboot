/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <device/pci_ops.h>
#include <device/pnp_ops.h>
#include <device/pnp_def.h>
#include <option.h>
#include <northbridge/intel/i945/i945.h>
#include <southbridge/intel/i82801gx/i82801gx.h>
#include <superio/winbond/common/winbond.h>
#include <superio/winbond/w83627thg/w83627thg.h>

/* Override the default lpc decode ranges */
void mainboard_lpc_decode(void)
{
	int lpt_en = 0;
	u8 val;

	if (get_option(&val, "lpt") == CB_SUCCESS && val)
		lpt_en = LPT_LPC_EN; /* enable LPT */

	pci_update_config16(PCI_DEV(0, 0x1f, 0), LPC_EN, ~LPT_LPC_EN, lpt_en);
}

/* This box has two superios, so enabling serial becomes slightly excessive.
 * We disable a lot of stuff to make sure that there are no conflicts between
 * the two. Also set up the GPIOs from the beginning. This is the "no schematic
 * but safe anyways" method.
 */
void bootblock_mainboard_early_init(void)
{
	pnp_devfn_t dev;

	dev = PNP_DEV(0x2e, W83627THG_SP1);
	pnp_enter_conf_state(dev);

	pnp_write_config(dev, 0x24, 0xc6); /* PNPCSV */

	pnp_write_config(dev, 0x29, 0x43); /* GPIO settings */
	pnp_write_config(dev, 0x2a, 0x40); /* GPIO settings */

	dev = PNP_DEV(0x2e, W83627THG_SP1);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3f8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 4);
	pnp_set_enable(dev, 1);

	dev = PNP_DEV(0x2e, W83627THG_SP2);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x2f8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 3);
	pnp_set_enable(dev, 1);

	dev = PNP_DEV(0x2e, W83627THG_KBC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x60);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x64);
	pnp_set_enable(dev, 1);

	dev = PNP_DEV(0x2e, W83627THG_GAME_MIDI_GPIO1);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_write_config(dev, PNP_IDX_MSC5, 0xff); /* invert all GPIOs */
	pnp_set_enable(dev, 1);

	dev = PNP_DEV(0x2e, W83627THG_GPIO2);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 1); /* Just enable it */

	dev = PNP_DEV(0x2e, W83627THG_GPIO3);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_write_config(dev, PNP_IDX_MSC0, 0xfb); /* GPIO bit 2 is output */
	pnp_write_config(dev, PNP_IDX_MSC1, 0x00); /* GPIO bit 2 is 0 */
	/* Enable GPIO3+4. pnp_set_enable is not sufficient */
	pnp_write_config(dev, PNP_IDX_EN, 0x03);

	dev = PNP_DEV(0x2e, W83627THG_FDC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	dev = PNP_DEV(0x2e, W83627THG_PP);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	/* Enable HWM */
	dev = PNP_DEV(0x2e, W83627THG_HWM);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0xa00);
	pnp_set_enable(dev, 1);

	pnp_exit_conf_state(dev);

	dev = PNP_DEV(0x4e, W83627THG_SP1);
	pnp_enter_conf_state(dev);

	pnp_set_logical_device(dev); /* Set COM3 to sane non-conflicting values */
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x3e8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 11);
	pnp_set_enable(dev, 1);

	dev = PNP_DEV(0x4e, W83627THG_SP2);
	pnp_set_logical_device(dev); /* Set COM4 to sane non-conflicting values */
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x2e8);
	pnp_set_irq(dev, PNP_IDX_IRQ0, 10);
	pnp_set_enable(dev, 1);

	dev = PNP_DEV(0x4e, W83627THG_FDC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	dev = PNP_DEV(0x4e, W83627THG_PP);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);

	dev = PNP_DEV(0x4e, W83627THG_KBC);
	pnp_set_logical_device(dev);
	pnp_set_enable(dev, 0);
	pnp_set_iobase(dev, PNP_IDX_IO0, 0x00);
	pnp_set_iobase(dev, PNP_IDX_IO1, 0x00);

	pnp_exit_conf_state(dev);
}

void mainboard_late_rcba_config(void)
{
	/* Device 1f interrupt pin register */
	RCBA32(D31IP) = 0x00042210;
	/* Device 1d interrupt pin register */
	RCBA32(D28IP) = 0x00214321;

	/* dev irq route register */
	RCBA16(D31IR) = 0x0132;
	RCBA16(D30IR) = 0x3241;
	RCBA16(D29IR) = 0x0237;
	RCBA16(D28IR) = 0x3210;
	RCBA16(D27IR) = 0x3210;

	/* Enable PCIe Root Port Clock Gate */
}
