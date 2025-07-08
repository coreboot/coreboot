/* SPDX-License-Identifier: GPL-2.0-only */

#include <amdblocks/acpi.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/gpio.h>
#include <amdblocks/smi.h>
#include <cpu/x86/smm.h>
#include <soc/amd_pci_int_defs.h>
#include <soc/smi.h>
#include <soc/southbridge.h>

/*
 * Table of APIC register index and associated IRQ name. Using IDX_XXX_NAME
 * provides a visible association with the index, therefore helping
 * maintainability of table. If a new index/name is defined in
 * amd_pci_int_defs.h, just add the pair at the end of this table.
 * Order is not important.
 */
static const struct irq_idx_name irq_association[] = {
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
	{ PIRQ_HDA,	"HDA" },
	{ PIRQ_GBE0,	"GBE0" },
	{ PIRQ_GBE1,	"GBE1" },
	{ PIRQ_PMON,	"PerMon" },
	{ PIRQ_SD,	"SD" },
	{ PIRQ_SDIO,	"SDIO" },
	{ PIRQ_CIR,	"CIR" },
	{ PIRQ_GPIOA,	"GPIOa" },
	{ PIRQ_GPIOB,	"GPIOb" },
	{ PIRQ_GPIOC,	"GPIOc" },
	{ PIRQ_USB_EMU,	"USB_EMU" },
	{ PIRQ_USB_DR0,	"USB_DR0" },
	{ PIRQ_USB_DR1,	"USB_DR1" },
	{ PIRQ_XHCI0,	"XHCI0" },
	{ PIRQ_SSIC,	"USB_SSIC" },
	{ PIRQ_SATA,	"SATA" },
	{ PIRQ_UFS,	"UFS" },
	{ PIRQ_GPP0,	"GPP0" },
	{ PIRQ_GPP1,	"GPP1" },
	{ PIRQ_GPP2,	"GPP2" },
	{ PIRQ_GPP3,	"GPP3" },
	{ PIRQ_GSCI,	"GEvent SCI" },
	{ PIRQ_GSMI,	"GEvent SMI" },
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

static void fch_init_acpi_ports(void)
{
	uint32_t reg32;
	uint8_t reg8;

	if (CONFIG(HAVE_SMI_HANDLER)) {
		/* Configure and enable APMC SMI Command Port */
		pm_write16(PM_ACPI_SMI_CMD, APM_CNT);
		configure_smi(SMITYPE_SMI_CMD_PORT, SMI_MODE_SMI);

		/* SMI on SlpTyp requires sending SMI before completion
		  response of the I/O write. */
		reg32 = pm_read32(PM_PCI_CTRL);
		reg32 |= FORCE_SLPSTATE_RETRY;
		/* Ths bit should be cleared in case of SlpType SMI trapping */
		reg32 &= ~FORCE_STPCLK_RETRY;
		pm_write32(PM_PCI_CTRL, reg32);

		/* Disable SlpTyp feature */
		reg8 = pm_read8(PM_RST_CTRL1);
		reg8 &= ~SLPTYPE_CONTROL_EN;
		pm_write8(PM_RST_CTRL1, reg8);

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

static void fch_clk_output_48Mhz(void)
{
	uint32_t ctrl = misc_read32(MISC_CLK_CNTL0);
	/* Enable BP_X48M0 Clock Output */
	ctrl |= BP_X48M0_OUTPUT_EN;
	misc_write32(MISC_CLK_CNTL0, ctrl);
}

static void fch_pci_int_vw_mode(void)
{
	pm_write32(0xa8, 0x80ffcef8);
}

void fch_init(void *chip_info)
{
	fch_init_acpi_ports();
	fch_clk_output_48Mhz();

	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();
	fch_pci_int_vw_mode();

	fch_enable_ioapic_decode();
}
