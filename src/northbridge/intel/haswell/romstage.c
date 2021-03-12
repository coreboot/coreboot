/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/romstage.h>
#include <cbfs.h>
#include <console/console.h>
#include <cf9_reset.h>
#include <device/device.h>
#include <device/mmio.h>
#include <elog.h>
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
#include <southbridge/intel/common/pmclib.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <string.h>
#include <types.h>

/* Copy SPD data for on-board memory */
static void copy_spd(struct pei_data *pei_data, struct spd_info *spdi)
{
	if (!CONFIG(HAVE_SPD_IN_CBFS))
		return;

	printk(BIOS_DEBUG, "SPD index %d\n", spdi->spd_index);

	size_t spd_file_len;
	uint8_t *spd_file = cbfs_map("spd.bin", &spd_file_len);

	if (!spd_file)
		die("SPD data not found.");

	if (spd_file_len < ((spdi->spd_index + 1) * SPD_LEN)) {
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spdi->spd_index = 0;
	}

	if (spd_file_len < SPD_LEN)
		die("Missing SPD data.");

	/* MRC only uses index 0, but coreboot uses the other indices */
	memcpy(pei_data->spd_data[0], spd_file + (spdi->spd_index * SPD_LEN), SPD_LEN);

	for (size_t i = 1; i < ARRAY_SIZE(spdi->addresses); i++) {
		if (spdi->addresses[i] == SPD_MEMORY_DOWN)
			memcpy(pei_data->spd_data[i], pei_data->spd_data[0], SPD_LEN);
	}
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

	struct pei_data pei_data = {
		.pei_version		= PEI_VERSION,
		.mchbar			= CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar			= CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar			= CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar		= CONFIG_MMCONF_BASE_ADDRESS,
		.smbusbar		= CONFIG_FIXED_SMBUS_IO_BASE,
		.hpet_address		= CONFIG_HPET_ADDRESS,
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

	early_pch_init();

	const int s3resume = southbridge_detect_s3_resume();

	elog_boot_notify(s3resume);

	/* Perform some early chipset initialization required
	 * before RAM initialization can work
	 */
	haswell_early_initialization();
	printk(BIOS_DEBUG, "Back from haswell_early_initialization()\n");

	/* Prepare USB controller early in S3 resume */
	if (s3resume)
		enable_usb_bar();

	post_code(0x3a);

	/* MRC has hardcoded assumptions of 2 meaning S3 wake. Normalize it here. */
	pei_data.boot_mode = s3resume ? 2 : 0;

	/* Obtain the SPD addresses from mainboard code */
	struct spd_info spdi = {0};
	mb_get_spd_map(&spdi);

	for (size_t i = 0; i < ARRAY_SIZE(spdi.addresses); i++)
		pei_data.spd_addresses[i] = spdi.addresses[i];

	/* Calculate unimplemented DIMM slots for each channel */
	pei_data.dimm_channel0_disabled = make_channel_disabled_mask(&pei_data, 0);
	pei_data.dimm_channel1_disabled = make_channel_disabled_mask(&pei_data, 1);

	timestamp_add_now(TS_BEFORE_INITRAM);

	report_platform_info();

	if (CONFIG(INTEL_TXT))
		intel_txt_romstage_init();

	copy_spd(&pei_data, &spdi);

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

	int cbmem_was_initted = !cbmem_recovery(s3resume);
	if (s3resume && !cbmem_was_initted) {
		/* Failed S3 resume, reset to come up cleanly */
		printk(BIOS_CRIT, "Failed to recover CBMEM in S3 resume.\n");
		system_reset();
	}

	/* Save data returned from MRC on non-S3 resumes. */
	if (!s3resume)
		save_mrc_data(&pei_data);


	haswell_unhide_peg();

	setup_sdram_meminfo(&pei_data);

	romstage_handoff_init(s3resume);

	mb_late_romstage_setup();

	post_code(0x3f);
}
