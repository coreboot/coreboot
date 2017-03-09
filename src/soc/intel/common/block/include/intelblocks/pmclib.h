/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef SOC_INTEL_COMMON_BLOCK_PMCLIB_H
#define SOC_INTEL_COMMON_BLOCK_PMCLIB_H

#include <stdint.h>

/* Forward declare the power state struct here */
struct chipset_power_state;

/*
 * This is implemented as weak function in common pmc lib.
 * Clears all power management related registers as the boot
 * flow is past the point of needing to maintain the values.
 */
void soc_clear_pm_registers(uintptr_t pmc_bar);

/*
 * This is implemented as weak function in common pmc lib and deals with any
 * soc specific quarks. Returns SMI status bits.
 */
uint32_t soc_get_smi_status(uint32_t generic_sts);

/*
 * This function is specific to soc and is defined as weak in common
 * pmclib file. SOC code can implement it for any special condition
 * specific to the soc e.g. in SKL in handles deep S3 scenerio.
 * Return ACPI_SX values to indicate the previous sleep state.
 */
int soc_prev_sleep_state(const struct chipset_power_state *ps,
					int prev_sleep_state);
/*
 * Function to print, clear, and return SMI status bits in SMI_STS
 * register. This function internally calls pmc_reset_smi_status with
 * extra functionality of printing the set smi_sts bits.
 */
uint32_t pmc_clear_smi_status(void);

/*
 * Function to return the 32 bit value of SMI control and enable register
 * used to check which smi's are enabled.
 */
uint32_t pmc_get_smi_en(void);

/* Enable SMI event in SMI control and enable register */
void pmc_enable_smi(uint32_t mask);

/* Disable SMI event */
void pmc_disable_smi(uint32_t mask);

/* Enable events in PM1 control register */
void pmc_enable_pm1_control(uint32_t mask);

/* Disable events in PM1 control register*/
void pmc_disable_pm1_control(uint32_t mask);

/* Set the PM1 register to events in PM1_STS_EN register */
void pmc_enable_pm1(uint16_t events);

/* Print, clear, and return PM1 status */
uint16_t pmc_clear_pm1_status(void);

/* TCO */

/* Print, clear, and return TCO status */
uint32_t pmc_clear_tco_status(void);

/*
 * This function returns array of string which represents
 * names for the TCO status bits. Size of the array is
 * returned as an output parameter.
 */
const char * const *soc_tco_sts_array(size_t *a);

/*
 * Resets the tco status registers. This function clears the tco_sts register
 * and returns the sts and enable bits set.
 */
uint32_t soc_reset_tco_status(void);


/* GPE */

/*
 * We have gpe0a_en/sts, gpe0b_en/sts, gpe0c_en/sts and gpe0d_en/sts
 * registers. gpe0a_en is symmetrical to the general purpose event
 * 0a status register and have all the enable bits for
 * gpe's. Other gpe registers gpe0b_en, gpe0c_en and
 * gpe0d_en are symmetrical to general purpose event status
 * registers and reads/writes to those register will result in
 * the transaction being forwarded to the corresponding GPIO
 * community based on the GPIO_GPE_CFG.gpe0_dw1, GPIO_GPE_CFG.gpe0_dw2
 * and GPIO_GPE_CFG.gpe0_dw3 register configuration.
 *
 * STS registers are symmetrical to event enable registers.
 * For gpe0a_sts register if the corresponding _EN bit is set in gpe0a_en,
 * then when the STS bit get set, the PMC will generate a Wake Event.
 * Once back in an S0 state (or if already in an S0 state when the event
 * occurs), the PMC will also generate an SCI if the SCI_EN bit is set,
 * or an SMI# if the SCI_EN bit is not set. Other gpe registers gpe0b_sts,
 * gpe0c_sts and gpe0d_sts are symmetrical to general purpose event enable
 * registers and reads/writes to those register will result in
 * the transaction being forwarded to the corresponding GPIO
 * community based on the GPIO_GPE_CFG.gpe0_dw1, GPIO_GPE_CFG.gpe0_dw2 and
 * GPIO_GPE_CFG.gpe0_dw3 register configuration.
 */
/* Enable a standard GPE in gpe0_en register */
void pmc_enable_gpe(uint32_t mask);
/* Disable a standard GPE in gpe0a_en register */
void pmc_disable_gpe(uint32_t mask);
/* Disable all GPE's in gpe0a_en register */
void pmc_disable_all_gpe(void);
/* Clear all GPE status and return "standard" GPE event status */
uint32_t pmc_clear_gpe_status(void);
/* Clear status bits in Power and Reset Status (PRSTS) register */
void pmc_clear_status(void);
/* Clear the gpio gpe0 status bits in ACPI registers */
void pmc_clear_gpi_gpe_sts(void);

/*
 * Enable or disable global reset. If global reset is enabled, hard reset and
 * soft reset will trigger global reset, where both host and TXE are reset.
 * This is cleared on cold boot, hard reset, soft reset and Sx.
 */
void pmc_global_reset_enable(bool enable);
/*
 * If possible, lock 0xcf9. Once the register is locked, it can't be changed.
 * This lock is reset on cold boot, hard reset, soft reset and Sx.
 */
void pmc_global_reset_lock(void);

/* Rewrite the gpe0 registers in cbmem to proper values as per routing table */
void pmc_fixup_power_state(void);

/*
 * Reads and prints ACPI specific PM registers which are common across
 * chipsets. Returns the previous sleep state which is one of ACPI_SX values.
 */
int pmc_fill_power_state(struct chipset_power_state *ps);

/*
 * Sets the gpe routing table by properly programming the GPE_CFG
 * and the MISCCFG registers. This function calls soc specific
 * soc_get_gpe_configs which reads the devicetree info
 * and populates the dw variables and also returns the bit offset
 * in GPIO_CFG register which is assigned to ACPI register.
 */
void pmc_gpe_init(void);

/* Power Management Utility Functions. */

/* Returns PMC base address */
uintptr_t soc_read_pmc_base(void);

/*
 * This function returns array of string which represents
 * names for the SMI status register bits. Size of the array is
 * returned as an output parameter.
 */
const char * const *soc_smi_sts_array(size_t *a);

/*
 * This function returns array of string which represents
 * names for the General purpose Event status register bits.
 * Size of the array is returned as an output parameter.
 */
const char * const *soc_gpe_sts_array(size_t *a);

/*
 * This function gets the gpe0 dwX values from devicetree
 * for pmc_gpe_init which will use those to set the GPE_CFG
 * register.
 */
void soc_get_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2);

/*
 * Reads soc specific power management crtitical registers, fills
 * chipset_power_state structure variable and prints.
 */
void soc_fill_power_state(struct chipset_power_state *ps);

#endif /* SOC_INTEL_COMMON_BLOCK_PMCLIB_H */
