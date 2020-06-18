/* SPDX-License-Identifier: GPL-2.0-only */

#include <types.h>
#include <arch/io.h>
#include <amdblocks/acpimmio.h>

#if ENV_X86
#include <amdblocks/acpimmio_map.h>
#endif

#if ENV_X86 && CONSTANT_ACPIMMIO_BASE_ADDRESS
#define DECLARE_ACPIMMIO(ptr, bank) \
	uint8_t *const ptr = (void *)(uintptr_t)ACPIMMIO_BASE(bank)
#else
#define DECLARE_ACPIMMIO(ptr, bank) uint8_t *ptr
#endif

DECLARE_ACPIMMIO(acpimmio_aoac, AOAC);
DECLARE_ACPIMMIO(acpimmio_iomux, IOMUX);
DECLARE_ACPIMMIO(acpimmio_gpio0, GPIO0);
DECLARE_ACPIMMIO(acpimmio_misc, MISC);

#if ENV_X86
DECLARE_ACPIMMIO(acpimmio_gpio_100, GPIO_100);
DECLARE_ACPIMMIO(acpimmio_smi, SMI);
DECLARE_ACPIMMIO(acpimmio_pmio, PMIO);
DECLARE_ACPIMMIO(acpimmio_pmio2, PMIO2);
DECLARE_ACPIMMIO(acpimmio_biosram, BIOSRAM);
DECLARE_ACPIMMIO(acpimmio_cmosram, CMOSRAM);
DECLARE_ACPIMMIO(acpimmio_cmos, CMOS);
DECLARE_ACPIMMIO(acpimmio_acpi, ACPI);
DECLARE_ACPIMMIO(acpimmio_asf, ASF);
DECLARE_ACPIMMIO(acpimmio_smbus, SMBUS);
DECLARE_ACPIMMIO(acpimmio_wdt, WDT);
DECLARE_ACPIMMIO(acpimmio_hpet, HPET);
DECLARE_ACPIMMIO(acpimmio_remote_gpio, REMOTE_GPIO);
DECLARE_ACPIMMIO(acpimmio_dpvga, DPVGA);
DECLARE_ACPIMMIO(acpimmio_xhci_pm, XHCIPM);
DECLARE_ACPIMMIO(acpimmio_acdc_tmr, ACDCTMR);
#endif

#undef DECLARE_ACPIMMIO

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

void fch_enable_cf9_io(void)
{
	pm_write32(PM_DECODE_EN, pm_read32(PM_DECODE_EN) | CF9_IO_EN);
}

void fch_enable_legacy_io(void)
{
	pm_write32(PM_DECODE_EN, pm_read32(PM_DECODE_EN) | LEGACY_IO_EN);
}

void fch_disable_legacy_dma_io(void)
{
	pm_write32(PM_DECODE_EN, pm_read32(PM_DECODE_EN) &
		~(LEGACY_DMA_IO_EN | LEGACY_DMA_IO_80_EN));
}

void fch_io_enable_legacy_io(void)
{
	pm_io_write32(PM_DECODE_EN, pm_io_read32(PM_DECODE_EN) | LEGACY_IO_EN);
}

void fch_enable_ioapic_decode(void)
{
	pm_write32(PM_DECODE_EN, pm_read32(PM_DECODE_EN) | FCH_IOAPIC_EN);
}

void fch_configure_hpet(void)
{
	uint32_t reg = pm_read32(PM_DECODE_EN);
	reg |=  HPET_EN | HPET_MSI_EN;
	reg &= ~HPET_WIDTH_SEL; /* 32 bit HPET */
	pm_write32(PM_DECODE_EN, reg);
}

void fch_disable_kb_rst(void)
{
	pm_write8(PM_RST_CTRL1, pm_read8(PM_RST_CTRL1) & ~KBRSTEN);
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
