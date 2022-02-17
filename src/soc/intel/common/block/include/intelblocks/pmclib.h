/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_PMCLIB_H
#define SOC_INTEL_COMMON_BLOCK_PMCLIB_H

#include <device/pci_type.h>
#include <types.h>

#define PCH_PMC_EPOC			0x18EC

/**
 * enum pch_pmc_xtal - External crystal oscillator frequency.
 * @XTAL_24_MHZ: 24 MHz external crystal.
 * @XTAL_19_2_MHZ: 19.2 MHz external crystal.
 * @XTAL_38_4_MHZ: 38.4 MHz external crystal.
 * @XTAL_UNKNOWN_FREQ: Unsupported external crystal.
 */
enum pch_pmc_xtal {
	XTAL_24_MHZ,
	XTAL_19_2_MHZ,
	XTAL_38_4_MHZ,
	XTAL_UNKNOWN_FREQ = 0xf,
};

/*
 * pmc_get_xtal_freq() - Return &enum pch_pmc_xtal corresponding to
 * frequency of external oscillator.
 */
enum pch_pmc_xtal pmc_get_xtal_freq(void);

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
 * specific to the soc e.g. in SKL in handles deep S3 scenario.
 * Return ACPI_SX values to indicate the previous sleep state.
 */
int soc_prev_sleep_state(const struct chipset_power_state *ps,
					int prev_sleep_state);

/* PM1 */
void pmc_update_pm1_enable(uint16_t events);
uint16_t pmc_read_pm1_enable(void);

uint32_t pmc_read_pm1_control(void);
void pmc_write_pm1_control(uint32_t pm1_cnt);

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
const char *const *soc_tco_sts_array(size_t *a);

/* GPE */

/*
 * We have symmetrical pairs of GPE0_EN/STS registers for Standard(STD) and GPIO
 * events. STD events are specific to SoC and one of the GPE0_EN/STS pairs
 * handles the STD events. Other GPE0_EN/STS pairs are used for GPIO events
 * based on the GPE0_DWx mappings.
 *
 * STS registers are symmetrical to event enable registers.
 * In case of STD events, for GPE0_STS register if the corresponding _EN bit is
 * set in GPE0_EN, then when the STS bit gets set, the PMC will generate a Wake
 * Event. Once back in an S0 state (or if already in an S0 state when the event
 * occurs), the PMC will also generate an SCI if the SCI_EN bit is set,
 * or an SMI# if the SCI_EN bit is not set.
 *
 * GPIO GPE registers are symmetrical to STD GPE registers and reads/writes to
 * those register will result in the transaction being forwarded to the
 * corresponding GPIO community based on the GPIO_GPE_CFG.gpe0_dwX register
 * configuration.
 */
/* Enable a standard GPE. */
void pmc_enable_std_gpe(uint32_t mask);
/* Disable a standard GPE. */
void pmc_disable_std_gpe(uint32_t mask);
/* Disable all GPE's in STD and GPIO GPE registers. */
void pmc_disable_all_gpe(void);
/* Clear STD and GPIO GPE status registers. */
void pmc_clear_all_gpe_status(void);

/* Clear status bits in Power and Reset Status (PRSTS) register */
void pmc_clear_prsts(void);

/*
 * Enable or disable global reset. If global reset is enabled, hard reset and
 * soft reset will trigger global reset, where both host and TXE are reset.
 * This is cleared on cold boot, hard reset, soft reset and Sx.
 */
void pmc_global_reset_enable(bool enable);
/*
 * Disable global reset and lock the CF9 global reset register in accordance to PCH ME BWG
 * sections 4.4.1, 4.5.1 and 18.4 and the PCH datasheet(s) (Intel doc e.g. 332691-002EN,
 * 332996-002EN). Deviate from the BGW we don't depend on the Intel ME state because Intel
 * FPT (Flash Programming Tool) normally is not used with coreboot.
 *
 * Once the register is locked, it can't be changed. This lock is reset on cold boot, hard
 * reset, soft reset and Sx.
 */
void pmc_global_reset_disable_and_lock(void);

/* Returns the power state structure */
struct chipset_power_state *pmc_get_power_state(void);

/*
 * Reads, fills(chipset_power_state) and prints ACPI specific PM registers. This
 * function does not check the previous sleep state.
 */
void pmc_fill_pm_reg_info(struct chipset_power_state *ps);

/*
 * Reads and prints ACPI specific PM registers which are common across
 * chipsets. Returns the previous sleep state which is one of ACPI_SX
 * values. Additionally, it also disables all GPEs after GPE_EN
 * registers are read.
 */
int pmc_fill_power_state(struct chipset_power_state *ps);

/*
 * Sets the gpe routing table by properly programming the GPE_CFG
 * and the MISCCFG registers. This function calls soc specific
 * soc_get_gpi_gpe_configs which reads the devicetree info
 * and populates the dw variables and also returns the bit offset
 * in GPIO_CFG register which is assigned to ACPI register.
 */
void pmc_gpe_init(void);

/* Clear PMC GEN_PMCON_A register status bits */
void pmc_clear_pmcon_sts(void);

/* Power Management Utility Functions. */

/* Returns PMC base address */
uintptr_t soc_read_pmc_base(void);

/* Returns pointer to the ETR register */
uint32_t *soc_pmc_etr_addr(void);

/*
 * This function returns array of string which represents
 * names for the SMI status register bits. Size of the array is
 * returned as an output parameter.
 */
const char *const *soc_smi_sts_array(size_t *a);

/*
 * This function returns array of string which represents
 * names for the STD GPE status register bits.
 * Size of the array is returned as an output parameter.
 */
const char *const *soc_std_gpe_sts_array(size_t *a);

/*
 * This function gets the gpe0 dwX values from devicetree
 * for pmc_gpe_init which will use those to set the GPE_CFG
 * register.
 */
void soc_get_gpi_gpe_configs(uint8_t *dw0, uint8_t *dw1, uint8_t *dw2);

/*
 * Reads soc specific power management critical registers, fills
 * chipset_power_state structure variable and prints.
 */
void soc_fill_power_state(struct chipset_power_state *ps);

/*
 * Which state do we want to goto after g3 (power restored)?
 * 0 == S5 Soft Off
 * 1 == S0 Full On
 * 2 == Keep Previous State
 * Keep in sync with `config MAINBOARD_POWER_FAILURE_STATE`.
 */
enum {
	MAINBOARD_POWER_STATE_OFF,
	MAINBOARD_POWER_STATE_ON,
	MAINBOARD_POWER_STATE_PREVIOUS,
};

/*
 * Implemented by SoC code to set PMC register to know which state
 * system should go into after power is reapplied.
 */
void pmc_soc_set_afterg3_en(bool on);
/*
 * Configure power state to go into when power is reapplied.
 *
 * To be called by SoC code once during boot and will be called by
 * the "sleep" SMI handler when going into S5.
 *
 * `target_on` signifies that we are currently powering on, so that
 * MAINBOARD_POWER_STATE_PREVIOUS can be handled accordingly.
 */
void pmc_set_power_failure_state(bool target_on);

/*
 * This function ensures that the duration programmed in the PchPmPwrCycDur will never be
 * smaller than the SLP_Sx assertion widths.
 * If the pm_pwr_cyc_dur is less than any of the SLP_Sx assertion widths then it returns the
 * default value PCH_PM_PWR_CYC_DUR.
 */
uint8_t get_pm_pwr_cyc_dur(uint8_t slp_s4_min_assert, uint8_t slp_s3_min_assert,
					uint8_t slp_a_min_assert, uint8_t pm_pwr_cyc_dur);

/* API to set ACPI mode */
void pmc_set_acpi_mode(void);

#endif /* SOC_INTEL_COMMON_BLOCK_PMCLIB_H */
