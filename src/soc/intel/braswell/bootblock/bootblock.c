/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/io.h>
#include <bootblock_common.h>
#include <build.h>
#include <console/console.h>
#include <device/pci_ops.h>
#include <fsp/util.h>
#include <gpio.h>
#include <pc80/mc146818rtc.h>
#include <soc/iomap.h>
#include <soc/iosf.h>
#include <soc/lpc.h>
#include <soc/msr.h>
#include <soc/pm.h>
#include <soc/spi.h>

asmlinkage void bootblock_c_entry(uint64_t base_timestamp)
{
	/* Call lib/bootblock.c main */
	bootblock_main_with_basetime(base_timestamp);
}

static void program_base_addresses(void)
{
	uint32_t reg;
	const uint32_t lpc_dev = PCI_DEV(0, LPC_DEV, LPC_FUNC);

	/* Memory Mapped IO registers. */
	reg = PMC_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PBASE, reg);
	reg = IO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IOBASE, reg);
	reg = ILB_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, IBASE, reg);
	reg = SPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, SBASE, reg);
	reg = MPHY_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, MPBASE, reg);
	reg = PUNIT_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, PUBASE, reg);
	reg = RCBA_BASE_ADDRESS | 1;
	pci_write_config32(lpc_dev, RCBA, reg);

	/* IO Port Registers. */
	reg = ACPI_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, ABASE, reg);
	reg = GPIO_BASE_ADDRESS | 2;
	pci_write_config32(lpc_dev, GBASE, reg);
}

static void tco_disable(void)
{
	uint32_t reg;

	reg = inl(ACPI_BASE_ADDRESS + TCO1_CNT);
	reg |= TCO_TMR_HALT;
	outl(reg, ACPI_BASE_ADDRESS + TCO1_CNT);
}

static void spi_init(void)
{
	void *scs = (void *)(SPI_BASE_ADDRESS + SCS);
	void *bcr = (void *)(SPI_BASE_ADDRESS + BCR);
	uint32_t reg;

	/* Disable generating SMI when setting WPD bit. */
	write32(scs, read32(scs) & ~SMIWPEN);
	/*
	 * Enable caching and prefetching in the SPI controller. Disable
	 * the SMM-only BIOS write and set WPD bit.
	 */
	reg = (read32(bcr) & ~SRC_MASK) | SRC_CACHE_PREFETCH | BCR_WPD;
	reg &= ~EISS;
	write32(bcr, reg);
}

static void soc_rtc_init(void)
{
	int rtc_failed = rtc_failure();

	if (rtc_failed) {
		printk(BIOS_ERR, "RTC Failure detected. Resetting date to %x/%x/%x%x\n",
			COREBOOT_BUILD_MONTH_BCD, COREBOOT_BUILD_DAY_BCD, 0x20,
			COREBOOT_BUILD_YEAR_BCD);
	}

	cmos_init(rtc_failed);
}

static void setup_mmconfig(void)
{
	uint32_t reg;

	/*
	 * Set up the MMCONF range. The register lives in the BUNIT. The IO variant of the
	 * config access needs to be used initially to properly configure as the IOSF access
	 * registers live in PCI config space.
	 */
	reg = 0;
	/* Clear the extended register. */
	pci_io_write_config32(IOSF_PCI_DEV, MCRX_REG, reg);
	reg = CONFIG_ECAM_MMCONF_BASE_ADDRESS | 1;
	pci_io_write_config32(IOSF_PCI_DEV, MDR_REG, reg);
	reg = IOSF_OPCODE(IOSF_OP_WRITE_BUNIT) | IOSF_PORT(IOSF_PORT_BUNIT) |
	      IOSF_REG(BUNIT_MMCONF_REG) | IOSF_BYTE_EN;
	pci_io_write_config32(IOSF_PCI_DEV, MCR_REG, reg);
}

void bootblock_soc_early_init(void)
{
	/* Allow memory-mapped PCI config access */
	setup_mmconfig();

	/* Early chipset initialization */
	program_base_addresses();
	tco_disable();
}
void bootblock_soc_init(void)
{
	report_fsp_output();

	/* Continue chipset initialization */
	soc_rtc_init();
	set_max_freq();
	spi_init();

	lpc_init();
}
