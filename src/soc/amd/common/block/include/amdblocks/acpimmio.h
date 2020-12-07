/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef AMD_BLOCK_ACPIMMIO_H
#define AMD_BLOCK_ACPIMMIO_H

#include <device/mmio.h>
#include <types.h>

/* IO index/data for accessing PMIO prior to enabling MMIO decode */
#define PM_INDEX			0xcd6
#define PM_DATA				0xcd7

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
extern uint8_t *MAYBE_CONST acpimmio_sm_pci;
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
extern uint8_t *MAYBE_CONST acpimmio_dpvga;
extern uint8_t *MAYBE_CONST acpimmio_gpio0;
extern uint8_t *MAYBE_CONST acpimmio_gpio1;
extern uint8_t *MAYBE_CONST acpimmio_gpio2;
extern uint8_t *MAYBE_CONST acpimmio_xhci_pm;
extern uint8_t *MAYBE_CONST acpimmio_acdc_tmr;
extern uint8_t *MAYBE_CONST acpimmio_aoac;

#undef MAYBE_CONST

/* For older discrete FCHs */
void enable_acpimmio_decode_pm24(void);

/* For newer integrated FCHs */
void enable_acpimmio_decode_pm04(void);

/* Access PM registers using IO cycles */
uint8_t pm_io_read8(uint8_t reg);
uint16_t pm_io_read16(uint8_t reg);
uint32_t pm_io_read32(uint8_t reg);
void pm_io_write8(uint8_t reg, uint8_t value);
void pm_io_write16(uint8_t reg, uint16_t value);
void pm_io_write32(uint8_t reg, uint32_t value);

static inline uint8_t sm_pci_read8(uint8_t reg)
{
	return read8(acpimmio_sm_pci + reg);
}

static inline uint16_t sm_pci_read16(uint8_t reg)
{
	return read16(acpimmio_sm_pci + reg);
}

static inline uint32_t sm_pci_read32(uint8_t reg)
{
	return read32(acpimmio_sm_pci + reg);
}

static inline void sm_pci_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_sm_pci + reg, value);
}

static inline void sm_pci_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_sm_pci + reg, value);
}

static inline void sm_pci_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_sm_pci + reg, value);
}

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

static inline uint16_t pm2_read16(uint8_t reg)
{
	return read16(acpimmio_pmio2 + reg);
}

static inline uint32_t pm2_read32(uint8_t reg)
{
	return read32(acpimmio_pmio2 + reg);
}

static inline void pm2_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_pmio2 + reg, value);
}

static inline void pm2_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_pmio2 + reg, value);
}

static inline void pm2_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_pmio2 + reg, value);
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

static inline uint16_t asf_read16(uint8_t reg)
{
	return read16(acpimmio_asf + reg);
}

static inline void asf_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_asf + reg, value);
}

static inline void asf_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_asf + reg, value);
}

static inline uint8_t smbus_read8(uint8_t reg)
{
	return read8(acpimmio_smbus + reg);
}

static inline uint16_t smbus_read16(uint8_t reg)
{
	return read16(acpimmio_smbus + reg);
}

static inline void smbus_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_smbus + reg, value);
}

static inline void smbus_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_smbus + reg, value);
}

static inline uint8_t iomux_read8(uint8_t reg)
{
	return read8(acpimmio_iomux + reg);
}

static inline uint16_t iomux_read16(uint8_t reg)
{
	return read16(acpimmio_iomux + reg);
}

static inline uint32_t iomux_read32(uint8_t reg)
{
	return read32(acpimmio_iomux + reg);
}

static inline void iomux_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_iomux + reg, value);
}

static inline void iomux_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_iomux + reg, value);
}

static inline void iomux_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_iomux + reg, value);
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

/* Old GPIO configuration registers */
static inline uint8_t gpio_100_read8(uint8_t reg)
{
	return read8(acpimmio_gpio_100 + reg);
}

static inline uint16_t gpio_100_read16(uint8_t reg)
{
	return read16(acpimmio_gpio_100 + reg);
}

static inline uint32_t gpio_100_read32(uint8_t reg)
{
	return read32(acpimmio_gpio_100 + reg);
}

static inline void gpio_100_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_gpio_100 + reg, value);
}

static inline void gpio_100_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_gpio_100 + reg, value);
}

static inline void gpio_100_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_gpio_100 + reg, value);
}

/* New GPIO banks configuration registers */

static inline void *gpio_ctrl_ptr(uint8_t gpio_num)
{
	return acpimmio_gpio0 + gpio_num * sizeof(uint32_t);
}

static inline uint32_t gpio_read32(uint8_t gpio_num)
{
	return read32(gpio_ctrl_ptr(gpio_num));
}

static inline void gpio_write32(uint8_t gpio_num, uint32_t value)
{
	write32(gpio_ctrl_ptr(gpio_num), value);
}

static inline void gpio_write32_rb(uint8_t gpio_num, uint32_t value)
{
	write32(gpio_ctrl_ptr(gpio_num), value);
	read32(gpio_ctrl_ptr(gpio_num));
}

/* GPIO bank 0 */
static inline uint8_t gpio0_read8(uint8_t reg)
{
	return read8(acpimmio_gpio0 + reg);
}

static inline uint16_t gpio0_read16(uint8_t reg)
{
	return read16(acpimmio_gpio0 + reg);
}

static inline uint32_t gpio0_read32(uint8_t reg)
{
	return read32(acpimmio_gpio0 + reg);
}

static inline void gpio0_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_gpio0 + reg, value);
}

static inline void gpio0_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_gpio0 + reg, value);
}

static inline void gpio0_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_gpio0 + reg, value);
}

/* GPIO bank 1 */
static inline uint8_t gpio1_read8(uint8_t reg)
{
	return read8(acpimmio_gpio1 + reg);
}

static inline uint16_t gpio1_read16(uint8_t reg)
{
	return read16(acpimmio_gpio1 + reg);
}

static inline uint32_t gpio1_read32(uint8_t reg)
{
	return read32(acpimmio_gpio1 + reg);
}

static inline void gpio1_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_gpio1 + reg, value);
}

static inline void gpio1_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_gpio1 + reg, value);
}

static inline void gpio1_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_gpio1 + reg, value);
}

/* GPIO bank 2 */
static inline uint8_t gpio2_read8(uint8_t reg)
{
	return read8(acpimmio_gpio2 + reg);
}

static inline uint16_t gpio2_read16(uint8_t reg)
{
	return read16(acpimmio_gpio2 + reg);
}

static inline uint32_t gpio2_read32(uint8_t reg)
{
	return read32(acpimmio_gpio2 + reg);
}

static inline void gpio2_write8(uint8_t reg, uint8_t value)
{
	write8(acpimmio_gpio2 + reg, value);
}

static inline void gpio2_write16(uint8_t reg, uint16_t value)
{
	write16(acpimmio_gpio2 + reg, value);
}

static inline void gpio2_write32(uint8_t reg, uint32_t value)
{
	write32(acpimmio_gpio2 + reg, value);
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
