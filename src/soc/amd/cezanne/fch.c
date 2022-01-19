/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/gpio.h>
#include <amdblocks/i2c.h>
#include <amdblocks/smi.h>
#include <assert.h>
#include <bootstate.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ids.h>
#include <fw_config.h>
#include <soc/amd_pci_int_defs.h>
#include <soc/iomap.h>
#include <soc/i2c.h>
#include <soc/platform_descriptors.h>
#include <soc/smi.h>
#include <soc/southbridge.h>
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
	{ PIRQ_HPET_L,	"HPET_L" },
	{ PIRQ_HPET_H,	"HPET_H" },
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
};

const struct irq_idx_name *sb_get_apic_reg_association(size_t *size)
{
	*size = ARRAY_SIZE(irq_association);
	return irq_association;
}

static void fch_clk_output_48Mhz(void)
{
	uint32_t ctrl = misc_read32(MISC_CLK_CNTL0);
	/* Enable BP_X48M0 Clock Output */
	ctrl |= BP_X48M0_OUTPUT_EN;
	/* Disable clock output in S0i3 */
	ctrl |= BP_X48M0_S0I3_DIS;
	misc_write32(MISC_CLK_CNTL0, ctrl);
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

static void fch_init_resets(void)
{
	pm_write16(PWR_RESET_CFG, pm_read16(PWR_RESET_CFG) | TOGGLE_ALL_PWR_GOOD);
}

/* Update gpp glk req config based on DXIO descriptors and enabled devices. */
static void gpp_dxio_update_clk_req_config(enum gpp_clk_req *gpp_clk_config,
					   size_t gpp_clk_config_num)
{
	const fsp_dxio_descriptor *dxio_descs = NULL;
	const fsp_ddi_descriptor *ddi_descs = NULL;
	size_t dxio_num = 0;
	size_t ddi_num = 0;

	mainboard_get_dxio_ddi_descriptors(&dxio_descs, &dxio_num, &ddi_descs, &ddi_num);
	if (dxio_descs == NULL) {
		printk(BIOS_WARNING,
			"No DXIO descriptors found, GPP clk req may not reflect enabled devices\n");
		return;
	}

	for (int i = 0; i < dxio_num; i++) {
		const fsp_dxio_descriptor *dxio_desc = &dxio_descs[i];

		/*  Only consider PCIe and unused engine types. */
		if (dxio_desc->engine_type != PCIE_ENGINE
			&& dxio_desc->engine_type != UNUSED_ENGINE)
			continue;
		enum cpm_clk_req dxio_clk_req = dxio_desc->clk_req;

		/* CLK_DISABLE means there's no corresponding clk req line in use */
		if (dxio_clk_req == CLK_DISABLE)
			continue;

		/*
		 * dxio_clk_req is only 4 bits so having CLK_ENABLE as a value for
		 * a descriptor should cause a compiler error. 0xF isn't a
		 * valid clk_req value according to AMD's internal code either.
		 * This is here to draw attention in case this code is ever used
		 * in a situation where this has changed.
		 */
		if (dxio_clk_req == (CLK_ENABLE & 0xF)) {
			printk(BIOS_WARNING,
				"CLK_ENABLE is an invalid clk_req value for PCIe device %d.%d, DXIO descriptor %d\n",
				dxio_desc->device_number, dxio_desc->function_number, i);
			continue;
		}

		/* cpm_clk_req 0 is CLK_DISABLE */
		int gpp_req_index = dxio_clk_req - CLK_REQ0;
		/* Ensure that our index is valid */
		if (gpp_req_index < 0 || gpp_req_index >= gpp_clk_config_num) {
			printk(BIOS_ERR, "Failed to convert DXIO clk req value %d to GPP clk req index for PCIe device %d.%d, DXIO descriptor %d, clk req settings may be incorrect\n",
				dxio_clk_req, dxio_desc->device_number,
				dxio_desc->function_number, i);
			continue;
		}

		const struct device *pci_device = pcidev_path_on_root(
			PCI_DEVFN(dxio_desc->device_number, dxio_desc->function_number));
		if (pci_device == NULL) {
			gpp_clk_config[gpp_req_index] = GPP_CLK_OFF;
			printk(BIOS_WARNING,
				"Cannot find PCIe device %d.%d, disabling GPP clk req %d, DXIO descriptor %d\n",
				dxio_desc->device_number, dxio_desc->function_number, i,
				gpp_req_index);
			continue;
		}

		/* PCIe devices haven't been fully set up yet, so directly read the vendor id
		 * and device id to determine if a device is physically present. If a device
		 * is not present then the id should be 0xffffffff. 0x00000000, 0xffff0000,
		 * and 0x0000ffff are there to account for any odd failure cases. */
		u32 id = pci_read_config32(pci_device, PCI_VENDOR_ID);
		bool enabled = pci_device->enabled && (id != 0xffffffff) &&
			(id != 0x00000000) && (id != 0x0000ffff) && (id != 0xffff0000);

		/* Inform of possible mismatches between devices and SoC gpp_clk_config. */
		if (!enabled && gpp_clk_config[gpp_req_index] != GPP_CLK_OFF) {
			gpp_clk_config[gpp_req_index] = GPP_CLK_OFF;
			printk(BIOS_INFO,
				"PCIe device %d.%d disabled, disabling GPP clk req %d, DXIO descriptor %d\n",
				dxio_desc->device_number, dxio_desc->function_number,
				gpp_req_index, i);
		} else if (enabled && gpp_clk_config[gpp_req_index] == GPP_CLK_OFF) {
			printk(BIOS_INFO,
				"PCIe device %d.%d enabled, GPP clk req is off, DXIO descriptor %d\n",
				dxio_desc->device_number, dxio_desc->function_number, i);
		}
	}
}

/* Configure the general purpose PCIe clock outputs according to the devicetree settings */
static void gpp_clk_setup(void)
{
	struct soc_amd_cezanne_config *cfg = config_of_soc();

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

	gpp_dxio_update_clk_req_config(&cfg->gpp_clk_config[0], GPP_CLK_OUTPUT_COUNT);
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

static void cgpll_clock_gate_init(void)
{
	uint32_t t;

	t = misc_read32(MISC_CLKGATEDCNTL);
	t |= ALINKCLK_GATEOFFEN;
	t |= BLINKCLK_GATEOFFEN;
	t |= XTAL_PAD_S3_TURNOFF_EN;
	t |= XTAL_PAD_S5_TURNOFF_EN;
	misc_write32(MISC_CLKGATEDCNTL, t);

	t = misc_read32(MISC_CGPLL_CONFIGURATION0);
	t |= USB_PHY_CMCLK_S3_DIS;
	t |= USB_PHY_CMCLK_S0I3_DIS;
	t |= USB_PHY_CMCLK_S5_DIS;
	misc_write32(MISC_CGPLL_CONFIGURATION0, t);

	t = pm_read32(PM_ISACONTROL);
	t |= ABCLKGATEEN;
	pm_write32(PM_ISACONTROL, t);
}

void fch_init(void *chip_info)
{
	fch_init_resets();
	i2c_soc_init();
	fch_init_acpi_ports();

	acpi_pm_gpe_add_events_print_events();
	gpio_add_events();

	gpp_clk_setup();
	fch_clk_output_48Mhz();
	cgpll_clock_gate_init();
}

void fch_final(void *chip_info)
{
}

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
