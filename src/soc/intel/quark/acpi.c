/* SPDX-License-Identifier: GPL-2.0-only */

#include <console/console.h>
#include <device/pci_ops.h>
#include <soc/acpi.h>
#include <soc/ramstage.h>

void acpi_fill_fadt(acpi_fadt_t *fadt)
{
	struct device *dev = pcidev_on_root(PCI_DEVICE_NUMBER_QNC_LPC,
			PCI_FUNCTION_NUMBER_QNC_LPC);
	uint32_t gpe0_base = pci_read_config32(dev, R_QNC_LPC_GPE0BLK)
		& B_QNC_LPC_GPE0BLK_MASK;
	uint32_t pmbase = pci_read_config32(dev, R_QNC_LPC_PM1BLK)
		& B_QNC_LPC_PM1BLK_MASK;

	fadt->flags |= ACPI_FADT_PLATFORM_CLOCK;

	/* PM1 Status: ACPI 4.8.3.1.1 */
	fadt->pm1a_evt_blk = pmbase + R_QNC_PM1BLK_PM1S;
	fadt->pm1_evt_len = 2;

	fadt->x_pm1a_evt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_evt_blk.bit_width = fadt->pm1_evt_len * 8;
	fadt->x_pm1a_evt_blk.bit_offset = 0;
	fadt->x_pm1a_evt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_evt_blk.addrl = fadt->pm1a_evt_blk;
	fadt->x_pm1a_evt_blk.addrh = 0x0;

	/* PM1 Control: ACPI 4.8.3.2.1 */
	fadt->pm1a_cnt_blk = pmbase + R_QNC_PM1BLK_PM1C;
	fadt->pm1_cnt_len = 2;

	fadt->x_pm1a_cnt_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm1a_cnt_blk.bit_width = fadt->pm1_cnt_len * 8;
	fadt->x_pm1a_cnt_blk.bit_offset = 0;
	fadt->x_pm1a_cnt_blk.access_size = ACPI_ACCESS_SIZE_WORD_ACCESS;
	fadt->x_pm1a_cnt_blk.addrl = fadt->pm1a_cnt_blk;
	fadt->x_pm1a_cnt_blk.addrh = 0x0;

	/* PM Timer: ACPI 4.8.3.3 */
	fadt->pm_tmr_blk = pmbase + R_QNC_PM1BLK_PM1T;
	fadt->pm_tmr_len = 4;

	fadt->x_pm_tmr_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_pm_tmr_blk.bit_width = fadt->pm_tmr_len * 8;
	fadt->x_pm_tmr_blk.bit_offset = 0;
	fadt->x_pm_tmr_blk.access_size = ACPI_ACCESS_SIZE_DWORD_ACCESS;
	fadt->x_pm_tmr_blk.addrl = fadt->pm_tmr_blk;
	fadt->x_pm_tmr_blk.addrh = 0x0;

	/* General-Purpose Event 0 Registers: ACPI 4.8.4.1 */
	fadt->gpe0_blk = gpe0_base;
	fadt->gpe0_blk_len = 4 * 2;

	fadt->x_gpe0_blk.space_id = ACPI_ADDRESS_SPACE_IO;
	fadt->x_gpe0_blk.bit_width = fadt->gpe0_blk_len * 8;
	fadt->x_gpe0_blk.bit_offset = 0;
	fadt->x_gpe0_blk.access_size = ACPI_ACCESS_SIZE_BYTE_ACCESS;
	fadt->x_gpe0_blk.addrl = fadt->gpe0_blk;
	fadt->x_gpe0_blk.addrh = 0;

	/* Display the base registers */
	printk(BIOS_SPEW, "FADT:\n");
	printk(BIOS_SPEW, "  0x%08x: GPE0_BASE\n", gpe0_base);
	printk(BIOS_SPEW, "  0x%08x: PMBASE\n", pmbase);
	printk(BIOS_SPEW, "  0x%08x: RESET\n", fadt->reset_reg.addrl);

}

uint16_t get_pmbase(void)
{
	struct device *dev = pcidev_on_root(PCI_DEVICE_NUMBER_QNC_LPC,
		PCI_FUNCTION_NUMBER_QNC_LPC);
	return (uint16_t) pci_read_config32(dev, R_QNC_LPC_PM1BLK) & B_QNC_LPC_PM1BLK_MASK;
}
