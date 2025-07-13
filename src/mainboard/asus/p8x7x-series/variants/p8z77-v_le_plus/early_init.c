/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootblock_common.h>
#include <cf9_reset.h>
#include <device/pnp_ops.h>
#include <device/pci_ops.h>
#include <device/pci.h>
#include <gpio.h>
#include <superio/nuvoton/common/nuvoton.h>
#include <superio/nuvoton/nct6779d/nct6779d.h>
#include <southbridge/intel/common/rcba.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <console/console.h>
#include <option.h>

#define GLOBAL_DEV PNP_DEV(0x2e, 0)
#define SERIAL_DEV PNP_DEV(0x2e, NCT6779D_SP1)
#define ACPI_DEV   PNP_DEV(0x2e, NCT6779D_ACPI)
#define GPIO0_DEV  PNP_DEV(0x2e, NCT6779D_WDT1_GPIO01_V)

enum pciex16_3_bandwidth {
	PCIEX16_3_X2 = 0,
	PCIEX16_3_X4 = 1
};

void mainboard_late_rcba_config(void)
{
	/* This is called after raminit. We turn off DRAM_LED here. */
	nuvoton_pnp_enter_conf_state(GPIO0_DEV);
	pnp_set_logical_device(GPIO0_DEV);
	pnp_write_config(GPIO0_DEV, 0xe1, 0x80);
	nuvoton_pnp_exit_conf_state(GPIO0_DEV);

	DIR_ROUTE(D31IR, PIRQA, PIRQD, PIRQC, PIRQA);
	DIR_ROUTE(D29IR, PIRQH, PIRQD, PIRQA, PIRQC);
	DIR_ROUTE(D27IR, PIRQG, PIRQB, PIRQC, PIRQD);
	DIR_ROUTE(D26IR, PIRQH, PIRQF, PIRQC, PIRQD);
	DIR_ROUTE(D25IR, PIRQE, PIRQF, PIRQG, PIRQH);
	DIR_ROUTE(D22IR, PIRQF, PIRQD, PIRQC, PIRQB);
}

void bootblock_mainboard_init(void)
{
	int gp46 = 0;

	/*
	 * PCHSTRP9[PCIEPCS1] soft strap (reflected here) tells us how PCIe lanes 1-4 are
	 * configured.
	 *
	 * GPIO46 controls one ASM1440 switch that routes PCH PCIe lanes 3 & 4 as follows:
	 * 0 = PCIEX1_1 and PCIEX1_2 respectively (pulled low on board)
	 * 1 = PCIEX16_3
	 */
	u8 pciepcs1 = RCBA32(RPC) & 0x3;
	/*
	unsigned int pciex16_bandwidth = get_uint_option("pciex16_3_bandwidth", PCIEX16_3_X2);
	u8 new_pciepcs1 = (pciex16_bandwidth == PCIEX16_3_X4) ? 3 : 1;

	if (pciepcs1 != new_pciepcs1) {
		printk(BIOS_INFO, "Updating PCHSTRP9[PCIEPCS1] to 0x%x... ", new_pciepcs1);
		if (write_pchstrp9(new_pciepcs1)) {
			printk(BIOS_INFO, "done.\n");
			full_reset();
		} else {
		*/
			/* Strap update failed.
			 * If halting, blink PCH GPIO8 for power LED so that this condition
			 * can be indicated by DRAM_LED lit and power LED blinking.
			 * If not, need a way to let user know to restore
			 * the flash descriptor with flashrom. System may be unstable.
			 */
			/*printk(BIOS_ERR, "PCH soft strap update failed!\n");
		}
	}
	*/

	switch (pciepcs1) {
	case 1: /* 1 x2, 3/4 x1 */
		break;
	case 3: /* 1 x4 */
		gp46 = 1;
		break;
	default:
		printk(BIOS_ERR, "PCH PCIe lane configuration 0x%x is invalid!\n", pciepcs1);
	}

	/* Match GPIO to soft strap. */
	gpio_set(46, gp46);
}

void bootblock_mainboard_early_init(void)
{
	nuvoton_pnp_enter_conf_state(GLOBAL_DEV);

	/* Select SIO pin states */
	pnp_write_config(GLOBAL_DEV, 0x1a, 0x02);
	pnp_write_config(GLOBAL_DEV, 0x27, 0x10);
	pnp_write_config(GLOBAL_DEV, 0x2a, 0x48);
	pnp_write_config(GLOBAL_DEV, 0x2c, 0x00);

	/* Power RAM in S3 */
	pnp_set_logical_device(ACPI_DEV);
	pnp_write_config(ACPI_DEV, 0xe4, 0x10);

	/* Turn on DRAM_LED. If raminit dies, this would remain on and we know
	 * we have a problem. We turn it off after raminit. */
	pnp_set_logical_device(GPIO0_DEV);
	pnp_write_config(GPIO0_DEV, 0x30, 0x02);
	pnp_write_config(GPIO0_DEV, 0xe0, 0x7f);
	pnp_write_config(GPIO0_DEV, 0xe1, 0x00);

	nuvoton_pnp_exit_conf_state(GLOBAL_DEV);

	/* Enable UART */
	nuvoton_enable_serial(SERIAL_DEV, CONFIG_TTYS0_BASE);

	/* Put PCIe root port 6 into subtractive decode. This should allow a POST card in
	 * the PCI slot to receive POST codes via a connected ASM1083 bridge.
	 *
	 * TODO: Also needs to set up early PCI bridge.
	 */
	if (CONFIG(POST_DEVICE_PCI_PCIE)) {
		pci_or_config32(PCI_DEV(0, 30, 0), 0x4c, 1 << 28); /* pci_bridge[BPC] |= SDE */
		pci_write_config32(PCH_PCIE_DEV(5), 0xec, 1); /* pcie_rp6[PECR3] |= SDE */
	}
}
