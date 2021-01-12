/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <console/console.h>
#include <device/device.h>
#include <device/pci.h>
#include <reg_script.h>

#include <soc/iosf.h>
#include <soc/device_nvs.h>
#include <soc/ramstage.h>

static const struct reg_script scc_start_dll[] = {
	/* Configure master DLL. */
	REG_IOSF_WRITE(IOSF_PORT_SCORE, 0x4964, 0x00078000),
	/* Configure Swing,FSM for Master DLL */
	REG_IOSF_WRITE(IOSF_PORT_SCORE, 0x4970, 0x00000133),
	/* Run+Local Reset on Master DLL */
	REG_IOSF_WRITE(IOSF_PORT_SCORE, 0x4970, 0x00001933),
	REG_SCRIPT_END,
};

static const struct reg_script scc_after_dll[] = {
	/* Configure Write Path */
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x4954, ~0x7fff, 0x35ad),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x4958, ~0x7fff, 0x35ad),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x495c, ~0x7fff, 0x35ad),
	/* Configure Read Path */
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x43e4, ~0x7fff, 0x35ad),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x4324, ~0x7fff, 0x35ad),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x42b4, ~0x7fff, 0x35ad),
	/* eMMC 4.5 TX and RX DLL */
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x49a4, ~0x1f001f, 0xa000d),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x49a8, ~0x1f001f, 0xd000d),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x49ac, ~0x1f001f, 0xd000d),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x49b0, ~0x1f001f, 0xd000d),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x49b4, ~0x1f001f, 0xd000d),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x49b8, ~0x1, 0x0),
	/* cfio_regs_mmc1_ELECTRICAL.nslew/pslew  */
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x48c0, ~0x3c, 0x0),
	REG_IOSF_RMW(IOSF_PORT_SCORE, 0x48c4, ~0x3c, 0x0),
	/*
	 * iosf2ocp_private.GENREGRW1.cr_clock_enable_clk_ocp = 01
	 * iosf2ocp_private.GENREGRW1.cr_clock_enable_clk_xin = 01
	 */
	REG_IOSF_RMW(IOSF_PORT_SCC, 0x600, ~0xf, 0x5),
	/* Enable IOSF Snoop */
	REG_IOSF_OR(IOSF_PORT_SCC, 0x00, (1 << 7)),
	/* SDIO 3V Support. */
	REG_IOSF_RMW(IOSF_PORT_SCC, 0x600, ~0x30, 0x30),
	REG_SCRIPT_END,
};

void baytrail_init_scc(void)
{
	uint32_t dll_values;

	printk(BIOS_DEBUG, "Initializing sideband SCC registers.\n");

	/* Common Sideband Initialization for SCC */
	reg_script_run(scc_start_dll);

	/* Override Slave Path - populate DLL settings. */
	dll_values = iosf_score_read(0x496c) & 0x7ffff;
	dll_values |= iosf_score_read(0x4950) & ~0xfffff;
	iosf_score_write(0x4950, dll_values | (1 << 19));

	reg_script_run(scc_after_dll);
}

void scc_enable_acpi_mode(struct device *dev, int iosf_reg, int nvs_index)
{
	struct reg_script ops[] = {
		/* Disable PCI interrupt, enable Memory and Bus Master */
		REG_PCI_OR16(PCI_COMMAND,
			     PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER | PCI_COMMAND_INT_DISABLE),
		/* Enable ACPI mode */
		REG_IOSF_OR(IOSF_PORT_SCC, iosf_reg,
			    SCC_CTL_PCI_CFG_DIS | SCC_CTL_ACPI_INT_EN),
		REG_SCRIPT_END
	};
	struct resource *bar;
	struct device_nvs *dev_nvs = acpi_get_device_nvs();

	/* Save BAR0 and BAR1 to ACPI NVS */
	bar = find_resource(dev, PCI_BASE_ADDRESS_0);
	if (bar)
		dev_nvs->scc_bar0[nvs_index] = (u32)bar->base;

	bar = find_resource(dev, PCI_BASE_ADDRESS_1);
	if (bar)
		dev_nvs->scc_bar1[nvs_index] = (u32)bar->base;

	/* Device is enabled in ACPI mode */
	dev_nvs->scc_en[nvs_index] = 1;

	/* Put device in ACPI mode */
	reg_script_run_on_dev(dev, ops);
}
