/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_pm.h>
#include <arch/io.h>
#include <assert.h>
#include <bootmode.h>
#include <device/mmio.h>
#include <device/pci.h>
#include <cbmem.h>
#include <cpu/x86/smm.h>
#include <console/console.h>
#include <halt.h>
#include <intelblocks/pmc_ipc.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/gpio.h>
#include <intelblocks/tco.h>
#include <option.h>
#include <security/vboot/vboot_common.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <stdint.h>
#include <string.h>
#include <timer.h>

#define PMC_IPC_BIOS_RST_COMPLETE		0xd0
#define PMC_IPC_BIOS_RST_SUBID_PCI_ENUM_DONE	0

static struct chipset_power_state power_state;

/* List of Minimum Assertion durations in microseconds */
enum min_assert_dur {
	MinAssertDur0s		= 0,
	MinAssertDur60us	= 60,
	MinAssertDur1ms		= 1000,
	MinAssertDur50ms	= 50000,
	MinAssertDur98ms	= 98000,
	MinAssertDur500ms	= 500000,
	MinAssertDur1s		= 1000000,
	MinAssertDur2s		= 2000000,
	MinAssertDur3s		= 3000000,
	MinAssertDur4s		= 4000000,
};

/* Signal Assertion duration values */
struct cfg_assert_dur {
	/* Minimum assertion duration of SLP_A signal */
	enum min_assert_dur slp_a;

	/* Minimum assertion duration of SLP_4 signal */
	enum min_assert_dur slp_s4;

	/* Minimum assertion duration of SLP_3 signal */
	enum min_assert_dur slp_s3;

	/* PCH PM Power Cycle duration */
	enum min_assert_dur pm_pwr_cyc_dur;
};

/* Default value of PchPmPwrCycDur */
#define PCH_PM_PWR_CYC_DUR	0

struct chipset_power_state *pmc_get_power_state(void)
{
	struct chipset_power_state *ptr = NULL;

	if (cbmem_possibly_online())
		ptr = acpi_get_pm_state();

	/* cbmem is online but ptr is not populated yet */
	if (ptr == NULL && !(ENV_RAMSTAGE || ENV_POSTCAR))
		return &power_state;

	return ptr;
}

static void migrate_power_state(int is_recovery)
{
	struct chipset_power_state *ps_cbmem;

	ps_cbmem = cbmem_add(CBMEM_ID_POWER_STATE, sizeof(*ps_cbmem));

	if (ps_cbmem == NULL) {
		printk(BIOS_DEBUG, "Not adding power state to cbmem!\n");
		return;
	}
	memcpy(ps_cbmem, &power_state, sizeof(*ps_cbmem));
}
CBMEM_CREATION_HOOK(migrate_power_state);

static void print_num_status_bits(int num_bits, uint32_t status,
				  const char *const bit_names[])
{
	int i;

	if (!status)
		return;

	for (i = num_bits - 1; i >= 0; i--) {
		if (status & (1 << i)) {
			if (bit_names[i])
				printk(BIOS_DEBUG, "%s ", bit_names[i]);
			else
				printk(BIOS_DEBUG, "BIT%d ", i);
		}
	}
}

__weak uint32_t soc_get_smi_status(uint32_t generic_sts)
{
	return generic_sts;
}

int acpi_get_sleep_type(void)
{
	struct chipset_power_state *ps;
	int prev_sleep_state = ACPI_S0;

	ps = pmc_get_power_state();
	if (ps)
		prev_sleep_state = ps->prev_sleep_state;

	return prev_sleep_state;
}

static uint32_t pmc_reset_smi_status(void)
{
	uint32_t smi_sts = inl(ACPI_BASE_ADDRESS + SMI_STS);
	outl(smi_sts, ACPI_BASE_ADDRESS + SMI_STS);

	return soc_get_smi_status(smi_sts);
}

static uint32_t print_smi_status(uint32_t smi_sts)
{
	size_t array_size;
	const char *const *smi_arr;

	if (!smi_sts)
		return 0;

	printk(BIOS_DEBUG, "SMI_STS: ");

	smi_arr = soc_smi_sts_array(&array_size);

	print_num_status_bits(array_size, smi_sts, smi_arr);
	printk(BIOS_DEBUG, "\n");

	return smi_sts;
}

/*
 * Update supplied events in PM1_EN register. This does not disable any already
 * set events.
 */
void pmc_update_pm1_enable(u16 events)
{
	u16 pm1_en = pmc_read_pm1_enable();
	pm1_en |= events;
	pmc_enable_pm1(pm1_en);
}

/* Read events set in PM1_EN register. */
uint16_t pmc_read_pm1_enable(void)
{
	return inw(ACPI_BASE_ADDRESS + PM1_EN);
}

uint32_t pmc_clear_smi_status(void)
{
	uint32_t sts = pmc_reset_smi_status();

	return print_smi_status(sts);
}

uint32_t pmc_get_smi_en(void)
{
	return inl(ACPI_BASE_ADDRESS + SMI_EN);
}

void pmc_enable_smi(uint32_t mask)
{
	uint32_t smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	smi_en |= mask;
	outl(smi_en, ACPI_BASE_ADDRESS + SMI_EN);
}

void pmc_disable_smi(uint32_t mask)
{
	uint32_t smi_en = inl(ACPI_BASE_ADDRESS + SMI_EN);
	smi_en &= ~mask;
	outl(smi_en, ACPI_BASE_ADDRESS + SMI_EN);
}

/* PM1 */
void pmc_enable_pm1(uint16_t events)
{
	outw(events, ACPI_BASE_ADDRESS + PM1_EN);
}

uint32_t pmc_read_pm1_control(void)
{
	return inl(ACPI_BASE_ADDRESS + PM1_CNT);
}

void pmc_write_pm1_control(uint32_t pm1_cnt)
{
	outl(pm1_cnt, ACPI_BASE_ADDRESS + PM1_CNT);
}

void pmc_enable_pm1_control(uint32_t mask)
{
	uint32_t pm1_cnt = pmc_read_pm1_control();
	pm1_cnt |= mask;
	pmc_write_pm1_control(pm1_cnt);
}

void pmc_disable_pm1_control(uint32_t mask)
{
	uint32_t pm1_cnt = pmc_read_pm1_control();
	pm1_cnt &= ~mask;
	pmc_write_pm1_control(pm1_cnt);
}

static uint16_t reset_pm1_status(void)
{
	uint16_t pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	outw(pm1_sts, ACPI_BASE_ADDRESS + PM1_STS);
	return pm1_sts;
}

static uint16_t print_pm1_status(uint16_t pm1_sts)
{
	static const char *const pm1_sts_bits[] = {
		[0] = "TMROF",
		[5] = "GBL",
		[8] = "PWRBTN",
		[10] = "RTC",
		[11] = "PRBTNOR",
		[13] = "USB",
		[14] = "PCIEXPWAK",
		[15] = "WAK",
	};

	if (!pm1_sts)
		return 0;

	printk(BIOS_SPEW, "PM1_STS: ");
	print_num_status_bits(ARRAY_SIZE(pm1_sts_bits), pm1_sts, pm1_sts_bits);
	printk(BIOS_SPEW, "\n");

	return pm1_sts;
}

uint16_t pmc_clear_pm1_status(void)
{
	return print_pm1_status(reset_pm1_status());
}

/* TCO */

static uint32_t print_tco_status(uint32_t tco_sts)
{
	size_t array_size;
	const char *const *tco_arr;

	if (!tco_sts)
		return 0;

	printk(BIOS_DEBUG, "TCO_STS: ");

	tco_arr = soc_tco_sts_array(&array_size);

	print_num_status_bits(array_size, tco_sts, tco_arr);
	printk(BIOS_DEBUG, "\n");

	return tco_sts;
}

uint32_t pmc_clear_tco_status(void)
{
	return print_tco_status(tco_reset_status());
}

/* GPE */
static void pmc_enable_gpe(int gpe, uint32_t mask)
{
	uint32_t gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN(gpe));
	gpe0_en |= mask;
	outl(gpe0_en, ACPI_BASE_ADDRESS + GPE0_EN(gpe));
}

static void pmc_disable_gpe(int gpe, uint32_t mask)
{
	uint32_t gpe0_en = inl(ACPI_BASE_ADDRESS + GPE0_EN(gpe));
	gpe0_en &= ~mask;
	outl(gpe0_en, ACPI_BASE_ADDRESS + GPE0_EN(gpe));
}

void pmc_enable_std_gpe(uint32_t mask)
{
	pmc_enable_gpe(GPE_STD, mask);
}

void pmc_disable_std_gpe(uint32_t mask)
{
	pmc_disable_gpe(GPE_STD, mask);
}

void pmc_disable_all_gpe(void)
{
	int i;
	for (i = 0; i < GPE0_REG_MAX; i++)
		pmc_disable_gpe(i, ~0);
}

/* Clear the gpio gpe0 status bits in ACPI registers */
static void pmc_clear_gpi_gpe_status(void)
{
	int i;

	for (i = 0; i < GPE0_REG_MAX; i++) {
		/* This is reserved GPE block and specific to chipset */
		if (i == GPE_STD)
			continue;
		uint32_t gpe_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(i));
		outl(gpe_sts, ACPI_BASE_ADDRESS + GPE0_STS(i));
	}
}

static uint32_t reset_std_gpe_status(void)
{
	uint32_t gpe_sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
	outl(gpe_sts, ACPI_BASE_ADDRESS + GPE0_STS(GPE_STD));
	return gpe_sts;
}

static uint32_t print_std_gpe_sts(uint32_t gpe_sts)
{
	size_t array_size;
	const char *const *sts_arr;

	if (!gpe_sts)
		return gpe_sts;

	printk(BIOS_DEBUG, "GPE0 STD STS: ");

	sts_arr = soc_std_gpe_sts_array(&array_size);
	print_num_status_bits(array_size, gpe_sts, sts_arr);
	printk(BIOS_DEBUG, "\n");

	return gpe_sts;
}

static void pmc_clear_std_gpe_status(void)
{
	print_std_gpe_sts(reset_std_gpe_status());
}

void pmc_clear_all_gpe_status(void)
{
	pmc_clear_std_gpe_status();
	pmc_clear_gpi_gpe_status();
}

__weak
void soc_clear_pm_registers(uintptr_t pmc_bar)
{
}

void pmc_clear_prsts(void)
{
	uint32_t prsts;
	uintptr_t pmc_bar;

	/* Read PMC base address from soc */
	pmc_bar = soc_read_pmc_base();

	prsts = read32p(pmc_bar + PRSTS);
	write32p(pmc_bar + PRSTS, prsts);

	soc_clear_pm_registers(pmc_bar);
}

__weak
int soc_prev_sleep_state(const struct chipset_power_state *ps,
			      int prev_sleep_state)
{
	return prev_sleep_state;
}

/*
 * Returns prev_sleep_state and also prints all power management registers.
 * Calls soc_prev_sleep_state which may be implemented by SOC.
 */
static int pmc_prev_sleep_state(const struct chipset_power_state *ps)
{
	/* Default to S0. */
	int prev_sleep_state = ACPI_S0;

	if (ps->pm1_sts & WAK_STS) {
		switch (acpi_sleep_from_pm1(ps->pm1_cnt)) {
		case ACPI_S3:
			if (CONFIG(HAVE_ACPI_RESUME))
				prev_sleep_state = ACPI_S3;
			break;
		case ACPI_S4:
			prev_sleep_state = ACPI_S4;
			break;
		case ACPI_S5:
			prev_sleep_state = ACPI_S5;
			break;
		}

		/* Clear SLP_TYP. */
		pmc_write_pm1_control(ps->pm1_cnt & ~(SLP_TYP));
	}
	return soc_prev_sleep_state(ps, prev_sleep_state);
}

void pmc_fill_pm_reg_info(struct chipset_power_state *ps)
{
	int i;

	memset(ps, 0, sizeof(*ps));

	ps->pm1_sts = inw(ACPI_BASE_ADDRESS + PM1_STS);
	ps->pm1_en = inw(ACPI_BASE_ADDRESS + PM1_EN);
	ps->pm1_cnt = pmc_read_pm1_control();

	printk(BIOS_DEBUG, "pm1_sts: %04x pm1_en: %04x pm1_cnt: %08x\n",
	       ps->pm1_sts, ps->pm1_en, ps->pm1_cnt);

	for (i = 0; i < GPE0_REG_MAX; i++) {
		ps->gpe0_sts[i] = inl(ACPI_BASE_ADDRESS + GPE0_STS(i));
		ps->gpe0_en[i] = inl(ACPI_BASE_ADDRESS + GPE0_EN(i));
		printk(BIOS_DEBUG, "gpe0_sts[%d]: %08x gpe0_en[%d]: %08x\n",
		       i, ps->gpe0_sts[i], i, ps->gpe0_en[i]);
	}

	soc_fill_power_state(ps);
}

/* Reads and prints ACPI specific PM registers */
int pmc_fill_power_state(struct chipset_power_state *ps)
{
	pmc_fill_pm_reg_info(ps);

	ps->prev_sleep_state = pmc_prev_sleep_state(ps);
	printk(BIOS_DEBUG, "prev_sleep_state %d\n", ps->prev_sleep_state);

	return ps->prev_sleep_state;
}

#if CONFIG(PMC_GLOBAL_RESET_ENABLE_LOCK)
void pmc_global_reset_disable_and_lock(void)
{
	uint32_t *etr = soc_pmc_etr_addr();
	uint32_t reg;

	reg = read32(etr);
	reg = (reg & ~CF9_GLB_RST) | CF9_LOCK;
	write32(etr, reg);
}

void pmc_global_reset_enable(bool enable)
{
	uint32_t *etr = soc_pmc_etr_addr();
	uint32_t reg;

	reg = read32(etr);
	reg = enable ? reg | CF9_GLB_RST : reg & ~CF9_GLB_RST;
	write32(etr, reg);
}
#endif // CONFIG_PMC_GLOBAL_RESET_ENABLE_LOCK

int platform_is_resuming(void)
{
	if (!(inw(ACPI_BASE_ADDRESS + PM1_STS) & WAK_STS))
		return 0;

	return acpi_sleep_from_pm1(pmc_read_pm1_control()) == ACPI_S3;
}

/* Read and clear GPE status (defined in acpi/acpi.h) */
int acpi_get_gpe(int gpe)
{
	int bank;
	uint32_t mask, sts;
	struct stopwatch sw;
	int rc = 0;

	if (gpe < 0 || gpe > GPE_MAX)
		return -1;

	bank = gpe / 32;
	mask = 1 << (gpe % 32);

	/* Wait up to 1ms for GPE status to clear */
	stopwatch_init_msecs_expire(&sw, 1);
	do {
		if (stopwatch_expired(&sw))
			return rc;

		sts = inl(ACPI_BASE_ADDRESS + GPE0_STS(bank));
		if (sts & mask) {
			outl(mask, ACPI_BASE_ADDRESS + GPE0_STS(bank));
			rc = 1;
		}
	} while (sts & mask);

	return rc;
}

/*
 * The PM1 control is set to S5 when vboot requests a reboot because the power
 * state code above may not have collected its data yet. Therefore, set it to
 * S5 when vboot requests a reboot. That's necessary if vboot fails in the
 * resume path and requests a reboot. This prevents a reboot loop where the
 * error is continually hit on the failing vboot resume path.
 */
void vboot_platform_prepare_reboot(void)
{
	uint32_t pm1_cnt;
	pm1_cnt = (pmc_read_pm1_control() & ~(SLP_TYP)) |
		(SLP_TYP_S5 << SLP_TYP_SHIFT);
	pmc_write_pm1_control(pm1_cnt);
}

void poweroff(void)
{
	pmc_enable_pm1_control(SLP_EN | (SLP_TYP_S5 << SLP_TYP_SHIFT));

	/*
	 * Setting SLP_TYP_S5 in PM1 triggers SLP_SMI, which is handled by SMM
	 * to transition to S5 state. If halt is called in SMM, then it prevents
	 * the SMI handler from being triggered and system never enters S5.
	 */
	if (!ENV_SMM)
		halt();
}

void pmc_gpe_init(void)
{
	uint32_t gpio_cfg = 0;
	uint32_t gpio_cfg_reg;
	uint8_t dw0, dw1, dw2;

	/* Read PMC base address from soc. This is implemented in soc */
	uintptr_t pmc_bar = soc_read_pmc_base();

	/*
	 * Get the dwX values for pmc gpe settings.
	 */
	soc_get_gpi_gpe_configs(&dw0, &dw1, &dw2);

	const uint32_t gpio_cfg_mask =
	    (GPE0_DWX_MASK << GPE0_DW_SHIFT(0)) |
	    (GPE0_DWX_MASK << GPE0_DW_SHIFT(1)) |
	    (GPE0_DWX_MASK << GPE0_DW_SHIFT(2));

	/* Making sure that bad values don't bleed into the other fields */
	dw0 &= GPE0_DWX_MASK;
	dw1 &= GPE0_DWX_MASK;
	dw2 &= GPE0_DWX_MASK;

	/*
	 * Route the GPIOs to the GPE0 block. Determine that all values
	 * are different, and if they aren't use the reset values.
	 */
	if (dw0 == dw1 || dw1 == dw2) {
		printk(BIOS_INFO, "PMC: Using default GPE route.\n");
		gpio_cfg = read32p(pmc_bar + GPIO_GPE_CFG);

		dw0 = (gpio_cfg >> GPE0_DW_SHIFT(0)) & GPE0_DWX_MASK;
		dw1 = (gpio_cfg >> GPE0_DW_SHIFT(1)) & GPE0_DWX_MASK;
		dw2 = (gpio_cfg >> GPE0_DW_SHIFT(2)) & GPE0_DWX_MASK;
	} else {
		gpio_cfg |= (uint32_t) dw0 << GPE0_DW_SHIFT(0);
		gpio_cfg |= (uint32_t) dw1 << GPE0_DW_SHIFT(1);
		gpio_cfg |= (uint32_t) dw2 << GPE0_DW_SHIFT(2);
	}

	gpio_cfg_reg = read32p(pmc_bar + GPIO_GPE_CFG) & ~gpio_cfg_mask;
	gpio_cfg_reg |= gpio_cfg & gpio_cfg_mask;

	write32p(pmc_bar + GPIO_GPE_CFG, gpio_cfg_reg);

	/* Set the routes in the GPIO communities as well. */
	gpio_route_gpe(dw0, dw1, dw2);
}

#if ENV_RAMSTAGE
static void pmc_clear_pmcon_sts_mmio(void)
{
	uint8_t *addr = pmc_mmio_regs();

	clrbits32((addr + GEN_PMCON_A), MS4V);
}

static void pmc_clear_pmcon_sts_pci(void)
{
	struct device *dev = pcidev_path_on_root(PCH_DEVFN_PMC);
	if (!dev)
		return;

	pci_and_config32(dev, GEN_PMCON_A, ~MS4V);
}

/*
 * Clear PMC GEN_PMCON_A register status bits:
 * SUS_PWR_FLR, GBL_RST_STS, HOST_RST_STS, PWR_FLR bits
 * while retaining MS4V write-1-to-clear bit
 */
void pmc_clear_pmcon_sts(void)
{
	/*
	 * Accessing PMC GEN_PMCON_A register differs between different Intel chipsets.
	 * Typically, there are two possible ways to perform GEN_PMCON_A register programming
	 * (like `pmc_clear_pmcon_sts()`) as:
	 * 1. Using PCI configuration space when GEN_PMCON_A is a PCI configuration register.
	 * 2. Using MMIO access when GEN_PMCON_A is a memory mapped register.
	 */
	if (CONFIG(SOC_INTEL_MEM_MAPPED_PM_CONFIGURATION))
		pmc_clear_pmcon_sts_mmio();
	else
		pmc_clear_pmcon_sts_pci();
}
#endif

void pmc_set_power_failure_state(const bool target_on)
{
	const unsigned int state = get_uint_option("power_on_after_fail",
					 CONFIG_MAINBOARD_POWER_FAILURE_STATE);

	/*
	 * On the shutdown path (target_on == false), we only need to
	 * update the register for MAINBOARD_POWER_STATE_PREVIOUS. For
	 * all other cases, we don't write the register to avoid clob-
	 * bering the value set on the boot path. This is necessary,
	 * for instance, when we can't access the option backend in SMM.
	 */

	switch (state) {
	case MAINBOARD_POWER_STATE_OFF:
		if (!target_on)
			break;
		printk(BIOS_INFO, "Set power off after power failure.\n");
		pmc_soc_set_afterg3_en(false);
		break;
	case MAINBOARD_POWER_STATE_ON:
		if (!target_on)
			break;
		printk(BIOS_INFO, "Set power on after power failure.\n");
		pmc_soc_set_afterg3_en(true);
		break;
	case MAINBOARD_POWER_STATE_PREVIOUS:
		printk(BIOS_INFO, "Keep power state after power failure.\n");
		pmc_soc_set_afterg3_en(target_on);
		break;
	default:
		printk(BIOS_WARNING, "Unknown power-failure state: %d\n", state);
		break;
	}
}

/* This function returns the highest assertion duration of the SLP_Sx assertion widths */
static enum min_assert_dur get_high_assert_width(const struct cfg_assert_dur *cfg_assert_dur)
{
	enum min_assert_dur max_assert_dur = cfg_assert_dur->slp_s4;

	if (max_assert_dur < cfg_assert_dur->slp_s3)
		max_assert_dur = cfg_assert_dur->slp_s3;

	if (max_assert_dur < cfg_assert_dur->slp_a)
		max_assert_dur = cfg_assert_dur->slp_a;

	return max_assert_dur;
}

/* This function converts assertion durations from register-encoded to microseconds */
static void get_min_assert_dur(uint8_t slp_s4_min_assert, uint8_t slp_s3_min_assert,
		uint8_t slp_a_min_assert, uint8_t pm_pwr_cyc_dur,
		struct cfg_assert_dur *cfg_assert_dur)
{
	/*
	 * Ensure slp_x_dur_list[] elements in the devicetree config are in sync with
	 * FSP encoded values.
	 */

	/* slp_s4_assert_dur_list : 1s, 1s(default), 2s, 3s, 4s */
	const enum min_assert_dur slp_s4_assert_dur_list[] = {
		MinAssertDur1s, MinAssertDur1s, MinAssertDur2s, MinAssertDur3s, MinAssertDur4s
	};

	/* slp_s3_assert_dur_list: 50ms, 60us, 1ms, 50ms (Default), 2s */
	const enum min_assert_dur slp_s3_assert_dur_list[] = {
		MinAssertDur50ms, MinAssertDur60us, MinAssertDur1ms, MinAssertDur50ms,
										MinAssertDur2s
	};

	/* slp_a_assert_dur_list: 2s, 0s, 4s, 98ms, 2s(Default) */
	const enum min_assert_dur slp_a_assert_dur_list[] = {
		MinAssertDur2s, MinAssertDur0s, MinAssertDur4s, MinAssertDur98ms, MinAssertDur2s
	};

	/* pm_pwr_cyc_dur_list: 4s(Default), 1s, 2s, 3s, 4s */
	const enum min_assert_dur pm_pwr_cyc_dur_list[] = {
		MinAssertDur4s, MinAssertDur1s, MinAssertDur2s, MinAssertDur3s, MinAssertDur4s
	};

	/* Get signal assertion width */
	if (slp_s4_min_assert < ARRAY_SIZE(slp_s4_assert_dur_list))
		cfg_assert_dur->slp_s4 = slp_s4_assert_dur_list[slp_s4_min_assert];

	if (slp_s3_min_assert < ARRAY_SIZE(slp_s3_assert_dur_list))
		cfg_assert_dur->slp_s3 = slp_s3_assert_dur_list[slp_s3_min_assert];

	if (slp_a_min_assert < ARRAY_SIZE(slp_a_assert_dur_list))
		cfg_assert_dur->slp_a = slp_a_assert_dur_list[slp_a_min_assert];

	if (pm_pwr_cyc_dur < ARRAY_SIZE(pm_pwr_cyc_dur_list))
		cfg_assert_dur->pm_pwr_cyc_dur = pm_pwr_cyc_dur_list[pm_pwr_cyc_dur];
}

/*
 * This function ensures that the duration programmed in the PchPmPwrCycDur will never be
 * smaller than the SLP_Sx assertion widths.
 * If the pm_pwr_cyc_dur is less than any of the SLP_Sx assertion widths then it returns the
 * default value PCH_PM_PWR_CYC_DUR.
 */
uint8_t get_pm_pwr_cyc_dur(uint8_t slp_s4_min_assert, uint8_t slp_s3_min_assert,
		uint8_t slp_a_min_assert, uint8_t pm_pwr_cyc_dur)
{
	/* Set default values for the minimum assertion duration */
	struct cfg_assert_dur cfg_assert_dur = {
		.slp_a		= MinAssertDur2s,
		.slp_s4		= MinAssertDur1s,
		.slp_s3		= MinAssertDur50ms,
		.pm_pwr_cyc_dur	= MinAssertDur4s
	};

	enum min_assert_dur high_assert_width;

	/* Convert assertion durations from register-encoded to microseconds */
	get_min_assert_dur(slp_s4_min_assert, slp_s3_min_assert, slp_a_min_assert,
		pm_pwr_cyc_dur,	&cfg_assert_dur);

	/* Get the highest assertion duration among PCH EDS specified signals for pwr_cyc_dur */
	high_assert_width = get_high_assert_width(&cfg_assert_dur);

	if (cfg_assert_dur.pm_pwr_cyc_dur >= high_assert_width)
		return pm_pwr_cyc_dur;

	printk(BIOS_DEBUG,
			"Set PmPwrCycDur to 4s as configured PmPwrCycDur (%d) violates PCH EDS "
			"spec\n", pm_pwr_cyc_dur);

	return PCH_PM_PWR_CYC_DUR;
}

void pmc_set_acpi_mode(void)
{
	if (!CONFIG(NO_SMM) && !acpi_is_wakeup_s3()) {
		apm_control(APM_CNT_ACPI_DISABLE);
	}
}

enum pch_pmc_xtal pmc_get_xtal_freq(void)
{
	if (!CONFIG(SOC_INTEL_COMMON_BLOCK_PMC_EPOC))
		dead_code();

	uint32_t xtal_freq = 0;
	const uint32_t epoc = read32p(soc_read_pmc_base() + PCH_PMC_EPOC);

	/* XTAL frequency in bits 21, 20, 17 */
	xtal_freq |= !!(epoc & (1 << 21)) << 2;
	xtal_freq |= !!(epoc & (1 << 20)) << 1;
	xtal_freq |= !!(epoc & (1 << 17)) << 0;
	switch (xtal_freq) {
	case 0:
		return XTAL_24_MHZ;
	case 1:
		return XTAL_19_2_MHZ;
	case 2:
		return XTAL_38_4_MHZ;
	default:
		printk(BIOS_ERR, "Unknown EPOC XTAL frequency setting %u\n", xtal_freq);
		return XTAL_UNKNOWN_FREQ;
	}
}

void pmc_send_pci_enum_done(void)
{
	struct pmc_ipc_buffer req = { 0 };
	struct pmc_ipc_buffer rsp;
	uint32_t cmd;

	cmd = pmc_make_ipc_cmd(PMC_IPC_BIOS_RST_COMPLETE,
			 PMC_IPC_BIOS_RST_SUBID_PCI_ENUM_DONE, 0);
	if (pmc_send_ipc_cmd(cmd, &req, &rsp) != CB_SUCCESS)
		printk(BIOS_ERR, "PMC: Failed sending PCI Enumeration Done Command\n");
}
