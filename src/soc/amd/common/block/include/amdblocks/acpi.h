/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_ACPI_H
#define AMD_BLOCK_ACPI_H

#include <acpi/acpi.h>
#include <amdblocks/gpio.h>
#include <types.h>

/* ACPI MMIO registers 0xfed80800 */
#define MMIO_ACPI_PM1_EVT_BLK		0x00
#define  MMIO_ACPI_PM1_STS		0x00
#define  MMIO_ACPI_PM1_EN		0x02
#define MMIO_ACPI_PM1_CNT_BLK		0x04
	  /* sleep types defined in include/acpi/acpi.h */
#define   ACPI_PM1_CNT_SCIEN		BIT(0)
#define MMIO_ACPI_PM_TMR_BLK		0x08
#define MMIO_ACPI_CPU_CONTROL		0x0c
#define MMIO_ACPI_GPE0_BLK		0x14
#define  MMIO_ACPI_GPE0_STS		0x14
#define  MMIO_ACPI_GPE0_EN		0x18

/* Structure to maintain standard ACPI register state for reporting purposes. */
struct acpi_pm_gpe_state {
	uint16_t pm1_sts;
	uint16_t pm1_en;
	uint32_t gpe0_sts;
	uint32_t gpe0_en;
	uint16_t previous_sx_state;
	uint16_t aligning_field;
};

/* Fill object with the ACPI PM and GPE state. */
void acpi_fill_pm_gpe_state(struct acpi_pm_gpe_state *state);
/* Save events to eventlog log and also print information on console. */
void acpi_pm_gpe_add_events_print_events(void);
/* Clear PM and GPE status registers. */
void acpi_clear_pm_gpe_status(void);

void fill_fadt_extended_pm_regs(acpi_fadt_t *fadt);

/*
 * If a system reset is about to be requested, modify the PM1 register so it
 * will never be misinterpreted as an S3 resume.
 */
void set_pm1cnt_s5(void);
void acpi_enable_sci(void);
void acpi_disable_sci(void);

struct chipset_power_state {
	struct acpi_pm_gpe_state gpe_state;
	struct gpio_wake_state gpio_state;
};

unsigned long southbridge_write_acpi_tables(const struct device *device, unsigned long current,
					    struct acpi_rsdp *rsdp);

unsigned long acpi_fill_madt_irqoverride(unsigned long current);
void acpi_fill_root_complex_tom(const struct device *device);

uintptr_t add_agesa_fsp_acpi_table(guid_t guid, const char *name, acpi_rsdp_t *rsdp,
				   uintptr_t current);

void acpi_log_events(const struct chipset_power_state *ps);
unsigned long acpi_fill_ivrs(acpi_ivrs_t *ivrs, unsigned long current);

#endif /* AMD_BLOCK_ACPI_H */
