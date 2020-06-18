/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_ACPIMMIO_H
#define AMD_BLOCK_ACPIMMIO_H

#include <device/mmio.h>
#include <types.h>

/* IO index/data for accessing PMIO prior to enabling MMIO decode */
#define PM_INDEX			0xcd6
#define PM_DATA				0xcd7

/*
 * Power management registers: 0xfed80300 or index/data at IO 0xcd6/cd7. Valid for Mullins and
 * newer SoCs, but not for the generations with separate FCH or Kabini.
 */
#define PM_DECODE_EN			0x00
#define   HPET_MSI_EN			(1 << 29)
#define   HPET_WIDTH_SEL		(1 << 28) /* 0=32bit, 1=64bit */
#define   SMBUS_ASF_IO_BASE_SHIFT	8
#define   SMBUS_ASF_IO_BASE_MASK	(0xff << SMBUS_ASF_IO_BASE_SHIFT)
#define   HPET_EN			(1 << 6) /* decode HPET MMIO at 0xfed00000 */
#define   FCH_IOAPIC_EN			(1 << 5)
#define   SMBUS_ASF_IO_EN		(1 << 4)
#define   LEGACY_DMA_IO_80_EN		(1 << 3) /* pass 0x80-0x83 to legacy DMA IO range */
#define   LEGACY_DMA_IO_EN		(1 << 2)
#define   CF9_IO_EN			(1 << 1)
#define   LEGACY_IO_EN			(1 << 0)
#define PM_ESPI_INTR_CTRL		0x40
#define   PM_ESPI_DEV_INTR_MASK		0x00FFFFFF
#define PM_RST_CTRL1			0xbe
#define   SLPTYPE_CONTROL_EN		(1 << 5)
#define   KBRSTEN			(1 << 4)
#define PM_RST_STATUS			0xc0

/*
 * Earlier devices enable the ACPIMMIO bank decodes in PMx24. All discrete FCHs
 * and the Kabini SoC fall into this category. Kabini's successor, Mullins, uses
 * this newer method of enable in PMx04.
 */

#define ACPIMMIO_DECODE_REGISTER_24	0x24
#define   PM_24_ACPIMMIO_DECODE_EN	BIT(0)

#define ACPIMMIO_DECODE_REGISTER_04	0x04
#define   PM_04_BIOSRAM_DECODE_EN	BIT(0)
#define   PM_04_ACPIMMIO_DECODE_EN	BIT(1)

/* For x86 base is constant, while PSP does mapping runtime. */
#define CONSTANT_ACPIMMIO_BASE_ADDRESS	ENV_X86

#if CONSTANT_ACPIMMIO_BASE_ADDRESS
#define MAYBE_CONST const
#else
#define MAYBE_CONST
#endif

extern uint8_t *MAYBE_CONST acpimmio_gpio_100;
extern uint8_t *MAYBE_CONST acpimmio_smi;
extern uint8_t *MAYBE_CONST acpimmio_pmio;
extern uint8_t *MAYBE_CONST acpimmio_pmio2;
extern uint8_t *MAYBE_CONST acpimmio_biosram;
extern uint8_t *MAYBE_CONST acpimmio_cmosram;
extern uint8_t *MAYBE_CONST acpimmio_cmos;
extern uint8_t *MAYBE_CONST acpimmio_acpi;
extern uint8_t *MAYBE_CONST acpimmio_asf;
extern uint8_t *MAYBE_CONST acpimmio_smbus;
extern uint8_t *MAYBE_CONST acpimmio_wdt;
extern uint8_t *MAYBE_CONST acpimmio_hpet;
extern uint8_t *MAYBE_CONST acpimmio_iomux;
extern uint8_t *MAYBE_CONST acpimmio_misc;
extern uint8_t *MAYBE_CONST acpimmio_remote_gpio;
extern uint8_t *MAYBE_CONST acpimmio_dpvga;
extern uint8_t *MAYBE_CONST acpimmio_gpio0;
extern uint8_t *MAYBE_CONST acpimmio_xhci_pm;
extern uint8_t *MAYBE_CONST acpimmio_acdc_tmr;
extern uint8_t *MAYBE_CONST acpimmio_aoac;

#undef MAYBE_CONST

/* For older discrete FCHs */
void enable_acpimmio_decode_pm24(void);

/* For newer integrated FCHs */
void enable_acpimmio_decode_pm04(void);
void fch_enable_cf9_io(void);
void fch_enable_legacy_io(void);
void fch_disable_legacy_dma_io(void);
void fch_io_enable_legacy_io(void);
void fch_enable_ioapic_decode(void);
void fch_configure_hpet(void);
void fch_disable_kb_rst(void);

/* Access PM registers using IO cycles */
uint8_t pm_io_read8(uint8_t reg);
uint16_t pm_io_read16(uint8_t reg);
uint32_t pm_io_read32(uint8_t reg);
void pm_io_write8(uint8_t reg, uint8_t value);
void pm_io_write16(uint8_t reg, uint16_t value);
void pm_io_write32(uint8_t reg, uint32_t value);

/* Print source of last reset */
void fch_print_pmxc0_status(void);

static inline uint8_t smi_read8(uint8_t reg)
{
	return read8(acpimmio_smi + reg);
}

static inline uint16_t smi_read16(uint8_t reg)
{
	return read16(acpimmio_smi + reg);
}

static inline uint32_t smi_read32(uint8_t reg)
{
	return read32(acpimmio_smi + reg);
}

static inline void smi_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_smi + reg, value);
}

static inline void smi_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_smi + reg, value);
}

static inline void smi_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_smi + reg, value);
}

static inline uint8_t pm_read8(uint8_t reg)
{
	return read8(acpimmio_pmio + reg);
}

static inline uint16_t pm_read16(uint8_t reg)
{
	return read16(acpimmio_pmio + reg);
}

static inline uint32_t pm_read32(uint8_t reg)
{
	return read32(acpimmio_pmio + reg);
}

static inline void pm_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_pmio + reg, value);
}

static inline void pm_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_pmio + reg, value);
}

static inline void pm_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_pmio + reg, value);
}

static inline uint8_t pm2_read8(uint8_t reg)
{
	return read8(acpimmio_pmio2 + reg);
}

static inline void pm2_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_pmio2 + reg, value);
}

static inline uint8_t acpi_read8(uint8_t reg)
{
	return read8(acpimmio_acpi + reg);
}

static inline uint16_t acpi_read16(uint8_t reg)
{
	return read16(acpimmio_acpi + reg);
}

static inline uint32_t acpi_read32(uint8_t reg)
{
	return read32(acpimmio_acpi + reg);
}

static inline void acpi_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_acpi + reg, value);
}

static inline void acpi_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_acpi + reg, value);
}

static inline void acpi_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_acpi + reg, value);
}

static inline uint8_t asf_read8(uint8_t reg)
{
	return read8(acpimmio_asf + reg);
}

static inline void asf_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_asf + reg, value);
}

static inline uint8_t smbus_read8(uint8_t reg)
{
	return read8(acpimmio_smbus + reg);
}

static inline void smbus_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_smbus + reg, value);
}

static inline uint8_t misc_read8(uint8_t reg)
{
	return read8(acpimmio_misc + reg);
}

static inline uint16_t misc_read16(uint8_t reg)
{
	return read16(acpimmio_misc + reg);
}

static inline uint32_t misc_read32(uint8_t reg)
{
	return read32(acpimmio_misc + reg);
}

static inline void misc_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_misc + reg, value);
}

static inline void misc_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_misc + reg, value);
}

static inline void misc_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_misc + reg, value);
}

static inline uint8_t xhci_pm_read8(uint8_t reg)
{
	return read8(acpimmio_xhci_pm + reg);
}

static inline uint16_t xhci_pm_read16(uint8_t reg)
{
	return read16(acpimmio_xhci_pm + reg);
}

static inline uint32_t xhci_pm_read32(uint8_t reg)
{
	return read32(acpimmio_xhci_pm + reg);
}

static inline void xhci_pm_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_xhci_pm + reg, value);
}

static inline void xhci_pm_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_xhci_pm + reg, value);
}

static inline void xhci_pm_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_xhci_pm + reg, value);
}

static inline uint8_t aoac_read8(uint8_t reg)
{
	return read8(acpimmio_aoac + reg);
}

static inline void aoac_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_aoac + reg, value);
}

#endif /* AMD_BLOCK_ACPIMMIO_H */
