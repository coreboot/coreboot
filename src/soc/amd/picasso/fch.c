/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootstate.h>
#include <cpu/amd/msr.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/reset.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/gpio.h>
#include <amdblocks/i2c.h>
#include <amdblocks/smi.h>
#include <soc/acpi.h>
#include <soc/cpu.h>
#include <soc/i2c.h>
#include <soc/iomap.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/amd_pci_int_defs.h>
#include <soc/pci_devs.h>
#include <types.h>
#include "chip.h"

/*
 * Table of APIC register index and associated IRQ name. Using IDX_XXX_NAME
 * provides a visible association with the index, therefore helping
 * maintainability of table. If a new index/name is defined in
 * amd_pci_int_defs.h, just add the pair at the end of this table.
 * Order is not important.
 */
const static struct irq_idx_name irq_association[] = {
	{ PIRQ_A,	"INTA#" },
	{ PIRQ_B,	"INTB#" },
	{ PIRQ_C,	"INTC#" },
	{ PIRQ_D,	"INTD#" },
	{ PIRQ_E,	"INTE#" },
	{ PIRQ_F,	"INTF#/GENINT2" },
	{ PIRQ_G,	"INTG#" },
	{ PIRQ_H,	"INTH#" },
	{ PIRQ_MISC,	"Misc" },
	{ PIRQ_MISC0,	"Misc0" },
	{ PIRQ_MISC1,	"Misc1" },
	{ PIRQ_MISC2,	"Misc2" },
	{ PIRQ_SIRQA,	"Ser IRQ INTA" },
	{ PIRQ_SIRQB,	"Ser IRQ INTB" },
	{ PIRQ_SIRQC,	"Ser IRQ INTC" },
	{ PIRQ_SIRQD,	"Ser IRQ INTD" },
	{ PIRQ_SCI,	"SCI" },
	{ PIRQ_SMBUS,	"SMBUS" },
	{ PIRQ_ASF,	"ASF" },
	{ PIRQ_PMON,	"PerMon" },
	{ PIRQ_SD,	"SD" },
	{ PIRQ_SDIO,	"SDIO" },
	{ PIRQ_CIR,	"CIR" },
	{ PIRQ_GPIOA,	"GPIOa" },
	{ PIRQ_GPIOB,	"GPIOb" },
	{ PIRQ_GPIOC,	"GPIOc" },
	{ PIRQ_SATA,	"SATA" },
	{ PIRQ_EMMC,	"eMMC" },
	{ PIRQ_GPP0,	"GPP0" },
	{ PIRQ_GPP1,	"GPP1" },
	{ PIRQ_GPP2,	"GPP2" },
	{ PIRQ_GPP3,	"GPP3" },
	{ PIRQ_GPIO,	"GPIO" },
	{ PIRQ_I2C0,	"I2C0" },
	{ PIRQ_I2C1,	"I2C1" },
	{ PIRQ_I2C2,	"I2C2" },
	{ PIRQ_I2C3,	"I2C3" },
	{ PIRQ_UART0,	"UART0" },
	{ PIRQ_UART1,	"UART1" },
	{ PIRQ_I2C4,	"I2C4" },
	{ PIRQ_I2C5,	"I2C5" },
	{ PIRQ_UART2,	"UART2" },
	{ PIRQ_UART3,	"UART3" },
};

const struct irq_idx_name *sb_get_apic_reg_association(size_t *size)
{
	*size = ARRAY_SIZE(irq_association);
	return irq_association;
}

static void fch_clk_output_48Mhz(void)
{
	u32 ctrl;
	const struct soc_amd_picasso_config *cfg = config_of_soc();

	ctrl = misc_read32(MISC_CLK_CNTL1);
	/* If used external clock source for I2S, disable the internal clock output */
	if (cfg->acp_i2s_use_external_48mhz_osc &&
			cfg->common_config.acp_config.acp_pin_cfg == I2S_PINS_I2S_TDM)
		ctrl &= ~BP_X48M0_OUTPUT_EN;
	else
		ctrl |= BP_X48M0_OUTPUT_EN;
	misc_write32(MISC_CLK_CNTL1, ctrl);
}

static void sb_rfmux_config_override(void)
{
	u8 port;
	const struct soc_amd_picasso_config *cfg;

	cfg = config_of_soc();

	for (port = 0; port < USB_PD_PORT_COUNT; port++) {
		if (cfg->usb_pd_config_override[port].rfmux_override_en) {
			write32p(USB_PD_PORT_CONTROL + PD_PORT_MUX_OFFSET(port),
				cfg->usb_pd_config_override[port].rfmux_config
				| USB_PD_RFMUX_OVERRIDE);
		}
	}
}

static void fch_init_acpi_ports(void)
{
	u32 reg;

	/* We use some of these ports in SMM regardless of whether or not
	 * ACPI tables are generated. Enable these ports indiscriminately.
	 */

	pm_write16(PM_EVT_BLK, ACPI_PM_EVT_BLK);
	pm_write16(PM1_CNT_BLK, ACPI_PM1_CNT_BLK);
	pm_write16(PM_TMR_BLK, ACPI_PM_TMR_BLK);
	pm_write16(PM_GPE0_BLK, ACPI_GPE0_BLK);

	if (CONFIG(HAVE_SMI_HANDLER)) {
		/* APMC - SMI Command Port */
		pm_write16(PM_ACPI_SMI_CMD, APM_CNT);
		configure_smi(SMITYPE_SMI_CMD_PORT, SMI_MODE_SMI);

		/* SMI on SlpTyp requires sending SMI before completion
		   response of the I/O write. */
		reg = pm_read32(PM_PCI_CTRL);
		reg |= FORCE_SLPSTATE_RETRY;
		pm_write32(PM_PCI_CTRL, reg);

		/* Disable SlpTyp feature */
		reg = pm_read8(PM_RST_CTRL1);
		reg &= ~SLPTYPE_CONTROL_EN;
		pm_write8(PM_RST_CTRL1, reg);

		configure_smi(SMITYPE_SLP_TYP, SMI_MODE_SMI);
	} else {
		pm_write16(PM_ACPI_SMI_CMD, 0);
	}

	/* Decode ACPI registers and enable standard features */
	pm_write8(PM_ACPI_CONF, PM_ACPI_DECODE_STD |
				PM_ACPI_GLOBAL_EN |
				PM_ACPI_RTC_EN_EN |
				PM_ACPI_TIMER_EN_EN);
}

/*
 * A-Link to AHB bridge, part of the AMBA fabric. These are internal clocks
 * and unneeded for Raven/Picasso so gate them to save power.
 */
static void al2ahb_clock_gate(void)
{
	uint8_t al2ahb_val;
	uintptr_t al2ahb_base = ALINK_AHB_ADDRESS;

	al2ahb_val = read8p(al2ahb_base + AL2AHB_CONTROL_CLK_OFFSET);
	al2ahb_val |= AL2AHB_CLK_GATE_EN;
	write8p(al2ahb_base + AL2AHB_CONTROL_CLK_OFFSET, al2ahb_val);
	al2ahb_val = read8p(al2ahb_base + AL2AHB_CONTROL_HCLK_OFFSET);
	al2ahb_val |= AL2AHB_HCLK_GATE_EN;
	write8p(al2ahb_base + AL2AHB_CONTROL_HCLK_OFFSET, al2ahb_val);
}

/* configure the general purpose PCIe clock outputs according to the devicetree settings */
static void gpp_clk_setup(void)
{
	const struct soc_amd_picasso_config *cfg = config_of_soc();

	/* look-up table to be able to iterate over the PCIe clock output settings */
	const uint8_t gpp_clk_shift_lut[GPP_CLK_OUTPUT_COUNT] = {
		GPP_CLK0_REQ_SHIFT,
		GPP_CLK1_REQ_SHIFT,
		GPP_CLK2_REQ_SHIFT,
		GPP_CLK3_REQ_SHIFT,
		GPP_CLK4_REQ_SHIFT,
		GPP_CLK5_REQ_SHIFT,
		GPP_CLK6_REQ_SHIFT,
	};

	uint32_t gpp_clk_ctl = misc_read32(GPP_CLK_CNTRL);

	for (int i = 0; i < GPP_CLK_OUTPUT_COUNT; i++) {
		gpp_clk_ctl &= ~GPP_CLK_REQ_MASK(gpp_clk_shift_lut[i]);
		/*
		 * The remapping of values is done so that the default of the enum used for the
		 * devicetree settings is the clock being enabled, so that a missing devicetree
		 * configuration for this will result in an always active clock and not an
		 * inactive PCIe clock output.
		 */
		switch (cfg->gpp_clk_config[i]) {
		case GPP_CLK_REQ:
			gpp_clk_ctl |= GPP_CLK_REQ_EXT(gpp_clk_shift_lut[i]);
			break;
		case GPP_CLK_OFF:
			gpp_clk_ctl |= GPP_CLK_REQ_OFF(gpp_clk_shift_lut[i]);
			break;
		case GPP_CLK_ON:
		default:
			gpp_clk_ctl |= GPP_CLK_REQ_ON(gpp_clk_shift_lut[i]);
		}
	}

	misc_write32(GPP_CLK_CNTRL, gpp_clk_ctl);
}

void fch_init(void *chip_info)
{
	i2c_soc_init();
	fch_init_acpi_ports();

	acpi_pm_gpe_add_events_print_events();
	gpio_add_events();

	al2ahb_clock_gate();

	gpp_clk_setup();

	fch_clk_output_48Mhz();

	sb_rfmux_config_override();
}

void fch_final(void *chip_info)
{
}

/*
 * Update the PCI devices with a valid IRQ number
 * that is set in the mainboard PCI_IRQ structures.
 */
static void set_pci_irqs(void *unused)
{
	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();

	/* pirq_data is consumed by `write_pci_cfg_irqs` */
	populate_pirq_data();

	/* Write IRQs for all devicetree enabled devices */
	write_pci_cfg_irqs();
}

/*
 * Hook this function into the PCI state machine
 * on entry into BS_DEV_ENABLE.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, set_pci_irqs, NULL);
