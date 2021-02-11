/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <console/console.h>
#include <cf9_reset.h>
#include <device/device.h>
#include <device/mmio.h>
#include <timestamp.h>
#include <cpu/x86/lapic.h>
#include <cbmem.h>
#include <commonlib/helpers.h>
#include <romstage_handoff.h>
#include <security/intel/txt/txt.h>
#include <security/intel/txt/txt_register.h>
#include <cpu/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/chip.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <string.h>

/* Copy SPD data for on-board memory */
void __weak copy_spd(struct pei_data *peid)
{
}

void __weak mb_late_romstage_setup(void)
{
}

/*
 * 0 = leave channel enabled
 * 1 = disable dimm 0 on channel
 * 2 = disable dimm 1 on channel
 * 3 = disable dimm 0+1 on channel
 */
static int make_channel_disabled_mask(const struct pei_data *pd, int ch)
{
	return (!pd->spd_addresses[ch + ch] << 0) | (!pd->spd_addresses[ch + ch + 1] << 1);
}

/* The romstage entry point for this platform is not mainboard-specific, hence the name */
void mainboard_romstage_entry(void)
{
	const struct device *gbe = pcidev_on_root(0x19, 0);

	const struct northbridge_intel_haswell_config *cfg = config_of_soc();

	int wake_from_s3;

	struct pei_data pei_data = {
		.pei_version		= PEI_VERSION,
		.mchbar			= CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar			= CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar			= CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar		= CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar		= CONFIG_FIXED_SMBUS_IO_BASE,
		.hpet_address		= HPET_ADDR,
		.rcba			= CONFIG_FIXED_RCBA_MMIO_BASE,
		.pmbase			= DEFAULT_PMBASE,
		.gpiobase		= DEFAULT_GPIOBASE,
		.temp_mmio_base		= 0xfed08000,
		.system_type		= get_pch_platform_type(),
		.tseg_size		= CONFIG_SMM_TSEG_SIZE,
		.ec_present		= cfg->ec_present,
		.gbe_enable		= gbe && gbe->enabled,
		.ddr_refresh_2x		= CONFIG(ENABLE_DDR_2X_REFRESH),
		.dq_pins_interleaved	= cfg->dq_pins_interleaved,
		.max_ddr3_freq		= 1600,
		.usb_xhci_on_resume	= cfg->usb_xhci_on_resume,
	};

	memcpy(pei_data.usb2_ports, mainboard_usb2_ports, sizeof(mainboard_usb2_ports));
	memcpy(pei_data.usb3_ports, mainboard_usb3_ports, sizeof(mainboard_usb3_ports));

	enable_lapic();

	wake_from_s3 = early_pch_init();

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization();
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	if (wake_from_s3) {
#if CONFIG(HAVE_ACPI_RESUME)
		printk(BIOS_DEBUG, "Resume from S3 detected.\n");
#else
		printk(BIOS_DEBUG, "Resume from S3 detected, but disabled.\n");
		wake_from_s3 = 0;
#endif
	}

	/* Prepare USB controller early in S3 resume */
	if (wake_from_s3)
		enable_usb_bar();

	post_code(0x3a);

	/* MRC has hardcoded assumptions of 2 meaning S3 wake. Normalize it here. */
	pei_data.boot_mode = wake_from_s3 ? 2 : 0;

	/* Obtain the SPD addresses from mainboard code */
	mb_get_spd_map(pei_data.spd_addresses);

	/* Calculate unimplemented DIMM slots for each channel */
	pei_data.dimm_channel0_disabled = make_channel_disabled_mask(&pei_data, 0);
	pei_data.dimm_channel1_disabled = make_channel_disabled_mask(&pei_data, 1);

	timestamp_add_now(TS_BEFORE_INITRAM);

	report_platform_info();

	if (CONFIG(INTEL_TXT))
		intel_txt_romstage_init();

	copy_spd(&pei_data);

	sdram_initialize(&pei_data);

	timestamp_add_now(TS_AFTER_INITRAM);

	if (CONFIG(INTEL_TXT)) {
		printk(BIOS_DEBUG, "Check TXT_ERROR register after MRC\n");

		intel_txt_log_acm_error(read32((void *)TXT_ERROR));

		intel_txt_log_spad();

		intel_txt_memory_has_secrets();

		txt_dump_regions();
	}

	post_code(0x3b);

	intel_early_me_status();

	if (!wake_from_s3) {
		cbmem_initialize_empty();
		/* Save data returned from MRC on non-S3 resumes. */
		save_mrc_data(&pei_data);
	} else if (cbmem_initialize()) {
	#if CONFIG(HAVE_ACPI_RESUME)
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	#endif
	}

	haswell_unhide_peg();

	setup_sdram_meminfo(&pei_data);

	romstage_handoff_init(wake_from_s3);

	mb_late_romstage_setup();

	post_code(0x3f);
}
