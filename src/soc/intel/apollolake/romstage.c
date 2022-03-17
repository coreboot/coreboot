/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <arch/romstage.h>
#include <device/pci_ops.h>
#include <arch/symbols.h>
#include <assert.h>
#include <cf9_reset.h>
#include <console/console.h>
#include <device/device.h>
#include <cpu/x86/pae.h>
#include <delay.h>
#include <device/pci_def.h>
#include <device/resource.h>
#include <fsp/api.h>
#include <fsp/util.h>
#include <intelblocks/cpulib.h>
#include <intelblocks/lpc_lib.h>
#include <intelblocks/msr.h>
#include <intelblocks/pmclib.h>
#include <intelblocks/systemagent.h>
#include <mrc_cache.h>
#include <soc/cpu.h>
#include <soc/iomap.h>
#include <soc/meminit.h>
#include <soc/pci_devs.h>
#include <soc/pm.h>
#include <soc/romstage.h>
#include <soc/systemagent.h>
#include <spi_flash.h>
#include <timer.h>
#include "chip.h"

static const uint8_t hob_variable_guid[16] = {
	0x7d, 0x14, 0x34, 0xa0, 0x0c, 0x69, 0x54, 0x41,
	0x8d, 0xe6, 0xc0, 0x44, 0x64, 0x1d, 0xe9, 0x42,
};

static uint32_t fsp_version;

/* High Performance Event Timer Configuration */
#define P2SB_HPTC				0x60
#define P2SB_HPTC_ADDRESS_ENABLE		(1 << 7)
/*
 * ADDRESS_SELECT            ENCODING_RANGE
 *      0                 0xFED0 0000 - 0xFED0 03FF
 *      1                 0xFED0 1000 - 0xFED0 13FF
 *      2                 0xFED0 2000 - 0xFED0 23FF
 *      3                 0xFED0 3000 - 0xFED0 33FF
 */
#define P2SB_HPTC_ADDRESS_SELECT_0		(0 << 0)
#define P2SB_HPTC_ADDRESS_SELECT_1		(1 << 0)
#define P2SB_HPTC_ADDRESS_SELECT_2		(2 << 0)
#define P2SB_HPTC_ADDRESS_SELECT_3		(3 << 0)

/*
 * Enables several BARs and devices which are needed for memory init
 * - MCH_BASE_ADDR is needed in order to talk to the memory controller
 * - HPET is enabled because FSP wants to store a pointer to global data in the
 *   HPET comparator register
 */
static void soc_early_romstage_init(void)
{
	static const struct sa_mmio_descriptor soc_fixed_pci_resources[] = {
		{ MCHBAR, MCH_BASE_ADDRESS, MCH_BASE_SIZE, "MCHBAR" },
	};

	/* Set Fixed MMIO address into PCI configuration space */
	sa_set_pci_bar(soc_fixed_pci_resources,
			ARRAY_SIZE(soc_fixed_pci_resources));

	/* Enable decoding for HPET. Needed for FSP global pointer storage */
	pci_write_config8(PCH_DEV_P2SB, P2SB_HPTC, P2SB_HPTC_ADDRESS_SELECT_0 |
						P2SB_HPTC_ADDRESS_ENABLE);
}

/*
 * Punit Initialization code. This all isn't documented, but
 * this is the recipe.
 */
static bool punit_init(void)
{
	uint32_t reg;
	uint32_t data;
	struct stopwatch sw;

	/* Thermal throttle activation offset */
	configure_tcc_thermal_target();

	/*
	 * Software Core Disable Mask (P_CR_CORE_DISABLE_MASK_0_0_0_MCHBAR).
	 * Enable all cores here.
	 */
	MCHBAR32(CORE_DISABLE_MASK) = 0x0;

	/* P-Unit bring up */
	reg = MCHBAR32(BIOS_RESET_CPL);
	if (reg == 0xffffffff) {
		/* P-unit not found */
		printk(BIOS_DEBUG, "Punit MMIO not available\n");
		return false;
	}
	/* Set Punit interrupt pin IPIN offset 3D */
	pci_write_config8(SA_DEV_PUNIT, PCI_INTERRUPT_PIN, 0x2);

	/* Set PUINT IRQ to 24 and INTPIN LOCK */
	MCHBAR32(PUNIT_THERMAL_DEVICE_IRQ) =
			PUINT_THERMAL_DEVICE_IRQ_VEC_NUMBER |
			PUINT_THERMAL_DEVICE_IRQ_LOCK;

	if (!CONFIG(SOC_INTEL_GEMINILAKE)) {
		data = MCHBAR32(0x7818);
		data &= 0xFFFFE01F;
		data |= 0x20 | 0x200;
		MCHBAR32(0x7818) = data;
	}

	/* Stage0 BIOS Reset Complete (RST_CPL) */
	enable_bios_reset_cpl();

	/*
	 * Poll for bit 8 to check if PCODE has completed its action
	 * in response to BIOS Reset complete.
	 * We wait here till 1 ms for the bit to get set.
	 */
	stopwatch_init_msecs_expire(&sw, 1);
	while (!(MCHBAR32(BIOS_RESET_CPL) & PCODE_INIT_DONE)) {
		if (stopwatch_expired(&sw)) {
			printk(BIOS_DEBUG, "PCODE Init Done Failure\n");
			return false;
		}
		udelay(100);
	}

	return true;
}

void set_max_freq(void)
{
	if (cpu_get_burst_mode_state() == BURST_MODE_UNAVAILABLE) {
		/* Burst Mode has been factory configured as disabled
		 * and is not available in this physical processor
		 * package.
		 */
		printk(BIOS_DEBUG, "Burst Mode is factory disabled\n");
		return;
	}

	/* Enable burst mode */
	cpu_burst_mode(true);

	/* Enable speed step. */
	cpu_set_eist(true);

	/* Set P-State ratio */
	cpu_set_p_state_to_turbo_ratio();
}

void mainboard_romstage_entry(void)
{
	bool s3wake;
	size_t var_size;
	struct chipset_power_state *ps = pmc_get_power_state();
	const void *new_var_data;

	soc_early_romstage_init();
	report_platform_info();

	s3wake = pmc_fill_power_state(ps) == ACPI_S3;
	fsp_memory_init(s3wake);

	if (punit_init())
		set_max_freq();
	else
		printk(BIOS_DEBUG, "Punit failed to initialize properly\n");

	/* Stash variable MRC data and let cache system update it later */
	new_var_data = fsp_find_extension_hob_by_guid(hob_variable_guid,
							&var_size);
	if (new_var_data)
		mrc_cache_stash_data(MRC_VARIABLE_DATA,
				fsp_version, new_var_data,
				var_size);
	else
		printk(BIOS_ERR, "Failed to determine variable data\n");

	mainboard_save_dimm_info();
}

static void fill_console_params(FSPM_UPD *mupd)
{
	if (CONFIG(CONSOLE_SERIAL)) {
		if (CONFIG(INTEL_LPSS_UART_FOR_CONSOLE)) {
			mupd->FspmConfig.SerialDebugPortDevice =
					CONFIG_UART_FOR_CONSOLE;
			/* use MMIO port type */
			mupd->FspmConfig.SerialDebugPortType = 2;
			/* use 4 byte register stride */
			mupd->FspmConfig.SerialDebugPortStrideSize = 2;
			/* used only for port type set to external */
			mupd->FspmConfig.SerialDebugPortAddress = 0;
		} else if (CONFIG(DRIVERS_UART_8250IO)) {
			/* use external UART for debug */
			mupd->FspmConfig.SerialDebugPortDevice = 3;
			/* use I/O port type */
			mupd->FspmConfig.SerialDebugPortType = 1;
			/* use 1 byte register stride */
			mupd->FspmConfig.SerialDebugPortStrideSize = 0;
			/* used only for port type set to external */
			mupd->FspmConfig.SerialDebugPortAddress =
					CONFIG_TTYS0_BASE;
		}
	} else {
		mupd->FspmConfig.SerialDebugPortType = 0;
	}
}

static void check_full_retrain(const FSPM_UPD *mupd)
{
	struct chipset_power_state *ps;

	if (mupd->FspmArchUpd.BootMode != FSP_BOOT_WITH_FULL_CONFIGURATION)
		return;

	ps = pmc_get_power_state();

	if (ps->gen_pmcon1 & WARM_RESET_STS) {
		printk(BIOS_INFO, "Full retrain unsupported on warm reboot.\n");
		full_reset();
	}
}

static void soc_gpu_init_params(FSPM_UPD *mupd)
{
	enum {
		GPU_PRIMARY_IGD = 0,
		GPU_PRIMARY_PCI = 1,
	};
	/* Select primary GPU device */
	if (CONFIG(ONBOARD_VGA_IS_PRIMARY) && is_devfn_enabled(SA_DEVFN_IGD))
		mupd->FspmConfig.PrimaryVideoAdaptor = GPU_PRIMARY_IGD;
	else
		mupd->FspmConfig.PrimaryVideoAdaptor = GPU_PRIMARY_PCI;
}

static void soc_memory_init_params(FSPM_UPD *mupd)
{
#if CONFIG(SOC_INTEL_GEMINILAKE)
	/* Only for GLK */
	FSP_M_CONFIG *m_cfg = &mupd->FspmConfig;

	m_cfg->PrmrrSize = get_valid_prmrr_size();

	/*
	 * CpuMemoryTest in FSP tests 0 to 1M of the RAM after MRC init.
	 * With PAGING_IN_CACHE_AS_RAM enabled for GLK, there was no page
	 * table entry for this range which caused a page fault. Since this
	 * test is anyway not exhaustive, skipping the memory test in FSP.
	 */
	m_cfg->SkipMemoryTestUpd = 1;

	/*
	 * PCIe power sequence can be done from within FSP when provided
	 * with the GPIOs used for PERST to FSP. Since this is done in
	 * coreboot, skipping the PCIe power sequence done by FSP.
	 */
	m_cfg->SkipPciePowerSequence = 1;
#endif
}

static void parse_devicetree_setting(FSPM_UPD *m_upd)
{
#if CONFIG(SOC_INTEL_GEMINILAKE)
	m_upd->FspmConfig.TraceHubEn = is_devfn_enabled(PCH_DEVFN_NPK);
#else
	m_upd->FspmConfig.NpkEn = is_devfn_enabled(PCH_DEVFN_NPK);
#endif
}

void platform_fsp_memory_init_params_cb(FSPM_UPD *mupd, uint32_t version)
{
	check_full_retrain(mupd);

	fill_console_params(mupd);
	soc_gpu_init_params(mupd);

	if (CONFIG(SOC_INTEL_GEMINILAKE))
		soc_memory_init_params(mupd);

	mainboard_memory_init_params(mupd);

	parse_devicetree_setting(mupd);

	/* Do NOT let FSP do any GPIO pad configuration */
	mupd->FspmConfig.PreMemGpioTablePtr = (uintptr_t) NULL;

	mupd->FspmConfig.SkipCseRbp = CONFIG(SKIP_CSE_RBP);

	/*
	 * Converged Security Engine (CSE) has secure storage functionality.
	 * HECI2 device can be used to access that functionality. However, part
	 * of S3 resume flow involves resetting HECI2 which takes 136ms. Since
	 * coreboot does not use secure storage functionality, instruct FSP to
	 * skip HECI2 reset.
	 */
	mupd->FspmConfig.EnableS3Heci2 = 0;

	/*
	 * Apollolake splits MRC cache into two parts: constant and variable.
	 * The constant part is not expected to change often and variable is.
	 * Currently variable part consists of parameters that change on cold
	 * boots such as scrambler seed and some memory controller registers.
	 * Scrambler seed is vital for S3 resume case because attempt to use
	 * wrong/missing key renders DRAM contents useless.
	 */

	mupd->FspmConfig.VariableNvsBufferPtr =
		mrc_cache_current_mmap_leak(MRC_VARIABLE_DATA, version,
					    NULL);

	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	fsp_version = version;

}

__weak
void mainboard_memory_init_params(FSPM_UPD *mupd)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}

__weak
void mainboard_save_dimm_info(void)
{
	printk(BIOS_DEBUG, "WEAK: %s/%s called\n", __FILE__, __func__);
}
