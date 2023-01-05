/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_BLOCK_ACPI_H
#define SOC_INTEL_COMMON_BLOCK_ACPI_H

#include <acpi/acpi.h>
#include <device/device.h>
#include <intelblocks/cpulib.h>
#include <soc/pm.h>
#include <stdint.h>

/* CPU Types */
enum core_type {
	CPUID_RESERVED_1 = 0x10,
	CPUID_CORE_TYPE_INTEL_ATOM = 0x20,
	CPUID_RESERVED_2 = 0x30,
	CPUID_CORE_TYPE_INTEL_CORE = 0x40,
	CPUID_UNKNOWN = 0xff,
};

/* Generates ACPI code to define _CPC control method */
void acpigen_write_CPPC_hybrid_method(int core_id);

/* Forward declare the power state struct here */
struct chipset_power_state;

/* Forward  declare the global nvs structure here */
struct global_nvs;

/* Return ACPI name for this device */
const char *soc_acpi_name(const struct device *dev);

/* Read the scis from soc specific register. Returns int scis value */
uint32_t soc_read_sci_irq_select(void);

/* Write the scis from soc specific register. */
void soc_write_sci_irq_select(uint32_t scis);

/* _CST MWAIT resource used by cstate_map. */
#define MWAIT_RES(state, sub_state)                         \
	{                                                   \
		.addrl = (((state) << 4) | (sub_state)),    \
		.space_id = ACPI_ADDRESS_SPACE_FIXED,       \
		.bit_width = ACPI_FFIXEDHW_VENDOR_INTEL,    \
		.bit_offset = ACPI_FFIXEDHW_CLASS_MWAIT,    \
		.access_size = ACPI_FFIXEDHW_FLAG_HW_COORD, \
	}

/*
 * get_cstate_map returns a table of processor specific acpi_cstate_t entries
 * and number of entries in the table
 */
const acpi_cstate_t *soc_get_cstate_map(size_t *num_entries);

/*
 * get_tstate_map returns a table of processor specific acpi_tstate_t entries
 * and number of entries in the table
 */
acpi_tstate_t *soc_get_tss_table(int *entries);

/*
 * Chipset specific quirks for the wake enable bits.
 */
void acpi_fill_soc_wake(uint32_t *pm1_en, uint32_t *gpe0_en,
			const struct chipset_power_state *ps);

/* Chipset specific settings for filling up dmar table */
unsigned long sa_write_acpi_tables(const struct device *dev,
				   unsigned long current,
				   struct acpi_rsdp *rsdp);

/* Return the polarity flag for SCI IRQ */
int soc_madt_sci_irq_polarity(int sci);

/* Generate P-state tables */
void generate_p_state_entries(int core, int cores_per_package);

/* Generate T-state tables */
void generate_t_state_entries(int core, int cores_per_package);

/*
 * soc specific power states generation. We need this to be defined by soc
 * as the state generations varies in chipsets e.g. APL generates T and P
 * states while SKL generates  * P state only depending on a devicetree config
 */
void soc_power_states_generation(int core_id, int cores_per_package);

/*
 * Common function to calculate the power ratio for power state generation
 */
int common_calculate_power_ratio(int tdp, int p1_ratio, int ratio);


/*
 * Return the number of table entries and takes a pointer to an array of ioapic bases.
 */
size_t soc_get_ioapic_info(const uintptr_t *ioapic_bases[]);

struct soc_pmc_lpm {
	unsigned int num_substates;
	unsigned int num_req_regs;
	unsigned int lpm_ipc_offset;
	unsigned int req_reg_stride;
	uint8_t lpm_enable_mask;
};

/* Generate an Intel Power Engine ACPI device */
void generate_acpi_power_engine(void);

/* Generate an Intel Power Engine ACPI device that supports exposing LPM
   substate requirements */
void generate_acpi_power_engine_with_lpm(const struct soc_pmc_lpm *lpm);

/* Fill SSDT for SGX status, EPC base and length */
void sgx_fill_ssdt(void);

/*
 * This function returns the CPU type (big or small) of the CPU that it is executing
 * on. It is designed to be called after MP initialization. If the SoC selects
 * SOC_INTEL_COMMON_BLOCK_ACPI_CPU_HYBRID, then this function must be implemented,
 * and will be called from set_cpu_type().
 */
enum core_type get_soc_cpu_type(void);

/* Check if CPU supports Nominal frequency or not */
bool soc_is_nominal_freq_supported(void);

/* Min sleep state per device*/
struct min_sleep_state {
	uint8_t pci_dev;
	enum acpi_device_sleep_states min_sleep_state;
};

/*
 * This SOC callback returns an array that maps devices to their min sleep state.
 * Example:
 *
 * static struct min_sleep_state min_pci_sleep_states[] = {
 *	{ SA_DEVFN_ROOT,	ACPI_DEVICE_SLEEP_D3 },
 *	{ SA_DEVFN_CPU_PCIE1_0,	ACPI_DEVICE_SLEEP_D3 },
 *	{ SA_DEVFN_IGD,		ACPI_DEVICE_SLEEP_D3 },
 *	...
 * };
 *
 * const struct pad_config *variant_early_gpio_table(size_t *num)
 * {
 *	*num = ARRAY_SIZE(early_gpio_table);
 *	return early_gpio_table;
 * }
 *
 */
struct min_sleep_state *soc_get_min_sleep_state_array(size_t *size);

#endif				/* _SOC_INTEL_COMMON_BLOCK_ACPI_H_ */
