/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <arch/io.h>
#include <amdblocks/acpimmio_map.h>
#include <amdblocks/acpimmio.h>

uint8_t *const acpimmio_sm_pci	= ACPIMMIO_BASE(SM_PCI);
uint8_t *const acpimmio_gpio_100 = ACPIMMIO_BASE(GPIO_100);
uint8_t *const acpimmio_smi	= ACPIMMIO_BASE(SMI);
uint8_t *const acpimmio_pmio	= ACPIMMIO_BASE(PMIO);
uint8_t *const acpimmio_pmio2	= ACPIMMIO_BASE(PMIO2);
uint8_t *const acpimmio_biosram	= ACPIMMIO_BASE(BIOSRAM);
uint8_t *const acpimmio_cmosram	= ACPIMMIO_BASE(CMOSRAM);
uint8_t *const acpimmio_cmos	= ACPIMMIO_BASE(CMOS);
uint8_t *const acpimmio_acpi	= ACPIMMIO_BASE(ACPI);
uint8_t *const acpimmio_asf	= ACPIMMIO_BASE(ASF);
uint8_t *const acpimmio_smbus	= ACPIMMIO_BASE(SMBUS);
uint8_t *const acpimmio_wdt	= ACPIMMIO_BASE(WDT);
uint8_t *const acpimmio_hpet	= ACPIMMIO_BASE(HPET);
uint8_t *const acpimmio_iomux	= ACPIMMIO_BASE(IOMUX);
uint8_t *const acpimmio_misc	= ACPIMMIO_BASE(MISC);
uint8_t *const acpimmio_dpvga	= ACPIMMIO_BASE(DPVGA);
uint8_t *const acpimmio_gpio0	= ACPIMMIO_BASE(GPIO0);
uint8_t *const acpimmio_gpio1	= ACPIMMIO_BASE(GPIO1);
uint8_t *const acpimmio_gpio2	= ACPIMMIO_BASE(GPIO2);
uint8_t *const acpimmio_xhci_pm	= ACPIMMIO_BASE(XHCIPM);
uint8_t *const acpimmio_acdc_tmr	= ACPIMMIO_BASE(ACDCTMR);
uint8_t *const acpimmio_aoac	= ACPIMMIO_BASE(AOAC);

void enable_acpimmio_decode_pm24(void)
{
	uint32_t dw;

	dw = pm_io_read32(ACPIMMIO_DECODE_REGISTER_24);
	dw |= PM_24_ACPIMMIO_DECODE_EN;
	pm_io_write32(ACPIMMIO_DECODE_REGISTER_24, dw);
}

void enable_acpimmio_decode_pm04(void)
{
	uint32_t dw;

	dw = pm_io_read32(ACPIMMIO_DECODE_REGISTER_04);
	dw |= PM_04_ACPIMMIO_DECODE_EN;
	pm_io_write32(ACPIMMIO_DECODE_REGISTER_04, dw);
}

/* PM registers are accessed a byte at a time via CD6/CD7 */
uint8_t pm_io_read8(uint8_t reg)
{
	outb(reg, PM_INDEX);
	return inb(PM_DATA);
}

uint16_t pm_io_read16(uint8_t reg)
{
	return (pm_io_read8(reg + sizeof(uint8_t)) << 8) | pm_io_read8(reg);
}

uint32_t pm_io_read32(uint8_t reg)
{
	return (pm_io_read16(reg + sizeof(uint16_t)) << 16) | pm_io_read16(reg);
}

void pm_io_write8(uint8_t reg, uint8_t value)
{
	outb(reg, PM_INDEX);
	outb(value, PM_DATA);
}

void pm_io_write16(uint8_t reg, uint16_t value)
{
	pm_io_write8(reg, value & 0xff);
	value >>= 8;
	pm_io_write8(reg + sizeof(uint8_t), value & 0xff);
}

void pm_io_write32(uint8_t reg, uint32_t value)
{
	pm_io_write16(reg, value & 0xffff);
	value >>= 16;
	pm_io_write16(reg + sizeof(uint16_t), value & 0xffff);
}
