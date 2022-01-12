/* SPDX-License-Identifier: GPL-2.0-only */

#include <device/mmio.h>
#include <bootstate.h>
#include <cpu/x86/smm.h>
#include <device/device.h>
#include <device/pci.h>
#include <device/pci_ops.h>
#include <acpi/acpi_gnvs.h>
#include <amdblocks/amd_pci_util.h>
#include <amdblocks/aoac.h>
#include <amdblocks/acpimmio.h>
#include <amdblocks/acpi.h>
#include <amdblocks/smi.h>
#include <soc/southbridge.h>
#include <soc/smi.h>
#include <soc/amd_pci_int_defs.h>
#include <soc/pci_devs.h>
#include <agesa_headers.h>
#include <soc/acpi.h>
#include <soc/aoac_defs.h>
#include <soc/nvs.h>
#include <types.h>

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
	{ PIRQ_F,	"INTF#" },
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
	{ PIRQ_HDA,	"HDA" },
	{ PIRQ_FC,	"FC" },
	{ PIRQ_PMON,	"PerMon" },
	{ PIRQ_SD,	"SD" },
	{ PIRQ_SDIO,	"SDIOt" },
	{ PIRQ_EHCI,	"EHCI" },
	{ PIRQ_XHCI,	"XHCI" },
	{ PIRQ_SATA,	"SATA" },
	{ PIRQ_GPIO,	"GPIO" },
	{ PIRQ_I2C0,	"I2C0" },
	{ PIRQ_I2C1,	"I2C1" },
	{ PIRQ_I2C2,	"I2C2" },
	{ PIRQ_I2C3,	"I2C3" },
	{ PIRQ_UART0,	"UART0" },
	{ PIRQ_UART1,	"UART1" },
};

const struct irq_idx_name *sb_get_apic_reg_association(size_t *size)
{
	*size = ARRAY_SIZE(irq_association);
	return irq_association;
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
	/* CpuControl is in \_SB.CP00, 6 bytes */
	pm_write16(PM_CPU_CTRL, ACPI_CPU_CONTROL);

	if (CONFIG(HAVE_SMI_HANDLER)) {
		/* APMC - SMI Command Port */
		pm_write16(PM_ACPI_SMI_CMD, APM_CNT);
		configure_smi(SMITYPE_SMI_CMD_PORT, SMI_MODE_SMI);

		/* SMI on SlpTyp requires sending SMI before completion
		 * response of the I/O write.  The BKDG also specifies
		 * clearing ForceStpClkRetry for SMI trapping.
		 */
		reg = pm_read32(PM_PCI_CTRL);
		reg |= FORCE_SLPSTATE_RETRY;
		reg &= ~FORCE_STPCLK_RETRY;
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

void fch_init(void *chip_info)
{
	fch_init_acpi_ports();
}

static void set_sb_aoac(struct aoac_devs *aoac)
{
	const struct device *sd, *sata;

	aoac->ic0e = is_aoac_device_enabled(FCH_AOAC_DEV_I2C0);
	aoac->ic1e = is_aoac_device_enabled(FCH_AOAC_DEV_I2C1);
	aoac->ic2e = is_aoac_device_enabled(FCH_AOAC_DEV_I2C2);
	aoac->ic3e = is_aoac_device_enabled(FCH_AOAC_DEV_I2C3);
	aoac->ut0e = is_aoac_device_enabled(FCH_AOAC_DEV_UART0);
	aoac->ut1e = is_aoac_device_enabled(FCH_AOAC_DEV_UART1);
	aoac->ehce = is_aoac_device_enabled(FCH_AOAC_DEV_USB2);
	aoac->xhce = is_aoac_device_enabled(FCH_AOAC_DEV_USB3);

	/* Rely on these being in sync with devicetree */
	sd = pcidev_path_on_root(SD_DEVFN);
	aoac->sd_e = sd && sd->enabled ? 1 : 0;
	sata = pcidev_path_on_root(SATA_DEVFN);
	aoac->st_e = sata && sata->enabled ? 1 : 0;
	aoac->espi = 1;
}

static void set_sb_gnvs(struct global_nvs *gnvs)
{
	uintptr_t amdfw_rom;
	uintptr_t xhci_fw;
	uintptr_t fwaddr;
	size_t fwsize;

	amdfw_rom = 0x20000 - (0x80000 << CONFIG_AMD_FWM_POSITION_INDEX);
	xhci_fw = read32((void *)(amdfw_rom + XHCI_FW_SIG_OFFSET));

	fwaddr = 2 + read16((void *)(xhci_fw + XHCI_FW_ADDR_OFFSET
			+ XHCI_FW_BOOTRAM_SIZE));
	fwsize = read16((void *)(xhci_fw + XHCI_FW_SIZE_OFFSET
			+ XHCI_FW_BOOTRAM_SIZE));
	gnvs->fw00 = 0;
	gnvs->fw01 = ((32 * KiB) << 16) + 0;
	gnvs->fw02 = fwaddr + XHCI_FW_BOOTRAM_SIZE;
	gnvs->fw03 = fwsize << 16;

	/* TODO: This might break if the OS decides to re-allocate the PCI BARs. */
	gnvs->eh10 = pci_read_config32(SOC_EHCI1_DEV, PCI_BASE_ADDRESS_0)
			& ~PCI_BASE_ADDRESS_MEM_ATTR_MASK;
}

void fch_final(void *chip_info)
{
	/* TODO: The AOAC states and EHCI/XHCI addresses should be moved out of GNVS */
	struct global_nvs *gnvs = acpi_get_gnvs();
	if (gnvs) {
		set_sb_aoac(&gnvs->aoac);
		set_sb_gnvs(gnvs);
	}
}

/*
 * Update the PCI devices with a valid IRQ number
 * that is set in the mainboard PCI_IRQ structures.
 */
static void set_pci_irqs(void *unused)
{
	/* Write PCI_INTR regs 0xC00/0xC01 */
	write_pci_int_table();

	/* Write IRQs for all devicetree enabled devices */
	write_pci_cfg_irqs();
}

/*
 * Hook this function into the PCI state machine
 * on entry into BS_DEV_ENABLE.
 */
BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_ENTRY, set_pci_irqs, NULL);
