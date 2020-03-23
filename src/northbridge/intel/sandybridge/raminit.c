/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#include <console/console.h>
#include <commonlib/region.h>
#include <bootmode.h>
#include <cf9_reset.h>
#include <string.h>
#include <arch/cpu.h>
#include <device/mmio.h>
#include <device/pci_ops.h>
#include <device/smbus_host.h>
#include <cbmem.h>
#include <timestamp.h>
#include <mrc_cache.h>
#include <southbridge/intel/bd82x6x/me.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <cpu/x86/msr.h>
#include <types.h>

#include "raminit_native.h"
#include "raminit_common.h"
#include "sandybridge.h"

/* FIXME: no ECC support */
/* FIXME: no support for 3-channel chipsets */

static void wait_txt_clear(void)
{
	struct cpuid_result cp = cpuid_ext(1, 0);

	/* Check if TXT is supported */
	if (!(cp.ecx & (1 << 6)))
		return;

	/* Some TXT public bit */
	if (!(read32((void *)0xfed30010) & 1))
		return;

	/* Wait for TXT clear */
	while (!(read8((void *)0xfed40000) & (1 << 7)))
		;
}

/* Disable a channel in ramctr_timing */
static void disable_channel(ramctr_timing *ctrl, int channel)
{
	ctrl->rankmap[channel] = 0;

	memset(&ctrl->rank_mirror[channel][0], 0, sizeof(ctrl->rank_mirror[0]));

	ctrl->channel_size_mb[channel] = 0;
	ctrl->cmd_stretch[channel]     = 0;
	ctrl->mad_dimm[channel]        = 0;
	memset(&ctrl->timings[channel][0],   0, sizeof(ctrl->timings[0]));
	memset(&ctrl->info.dimm[channel][0], 0, sizeof(ctrl->info.dimm[0]));
}

/* Fill cbmem with information for SMBIOS type 17 */
static void fill_smbios17(ramctr_timing *ctrl)
{
	int channel, slot;
	const u16 ddr_freq = (1000 << 8) / ctrl->tCK;

	FOR_ALL_CHANNELS for (slot = 0; slot < NUM_SLOTS; slot++) {
		enum cb_err ret = spd_add_smbios17(channel, slot, ddr_freq,
					&ctrl->info.dimm[channel][slot]);
		if (ret != CB_SUCCESS)
			printk(BIOS_ERR, "RAMINIT: Failed to add SMBIOS17\n");
	}
}

/* Return CRC16 match for all SPDs */
static int verify_crc16_spds_ddr3(spd_raw_data *spd, ramctr_timing *ctrl)
{
	int channel, slot, spd_slot;
	int match = 1;

	FOR_ALL_CHANNELS {
		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
			match &= ctrl->spd_crc[channel][slot] ==
				spd_ddr3_calc_unique_crc(spd[spd_slot], sizeof(spd_raw_data));
		}
	}
	return match;
}

void read_spd(spd_raw_data * spd, u8 addr, bool id_only)
{
	int j;
	if (id_only) {
		for (j = 117; j < 128; j++)
			(*spd)[j] = do_smbus_read_byte(SMBUS_IO_BASE, addr, j);
	} else {
		for (j = 0; j < 256; j++)
			(*spd)[j] = do_smbus_read_byte(SMBUS_IO_BASE, addr, j);
	}
}

static void dram_find_spds_ddr3(spd_raw_data *spd, ramctr_timing *ctrl)
{
	int dimms = 0, ch_dimms;
	int channel, slot, spd_slot;
	dimm_info *dimm = &ctrl->info;

	memset (ctrl->rankmap, 0, sizeof(ctrl->rankmap));

	ctrl->extended_temperature_range = 1;
	ctrl->auto_self_refresh = 1;

	FOR_ALL_CHANNELS {
		ctrl->channel_size_mb[channel] = 0;

		ch_dimms = 0;
		/* Count dimms on channel */
		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
			printk(BIOS_DEBUG, "SPD probe channel%d, slot%d\n", channel, slot);

			spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			if (dimm->dimm[channel][slot].dram_type == SPD_MEMORY_TYPE_SDRAM_DDR3)
				ch_dimms++;
		}

		for (slot = 0; slot < NUM_SLOTS; slot++) {
			spd_slot = 2 * channel + slot;
			printk(BIOS_DEBUG, "SPD probe channel%d, slot%d\n", channel, slot);

			/* Search for XMP profile */
			spd_xmp_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot],
					DDR3_XMP_PROFILE_1);

			if (dimm->dimm[channel][slot].dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3) {
				printram("No valid XMP profile found.\n");
				spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);

			} else if (ch_dimms > dimm->dimm[channel][slot].dimms_per_channel) {
				printram(
				"XMP profile supports %u DIMMs, but %u DIMMs are installed.\n",
					dimm->dimm[channel][slot].dimms_per_channel, ch_dimms);

				if (CONFIG(NATIVE_RAMINIT_IGNORE_XMP_MAX_DIMMS))
					printk(BIOS_WARNING,
						"XMP maximum DIMMs will be ignored.\n");
				else
					spd_decode_ddr3(&dimm->dimm[channel][slot],
							spd[spd_slot]);

			} else if (dimm->dimm[channel][slot].voltage != 1500) {
				/* TODO: Support DDR3 voltages other than 1500mV */
				printram("XMP profile's requested %u mV is unsupported.\n",
						 dimm->dimm[channel][slot].voltage);
				spd_decode_ddr3(&dimm->dimm[channel][slot], spd[spd_slot]);
			}

			/* Fill in CRC16 for MRC cache */
			ctrl->spd_crc[channel][slot] =
				spd_ddr3_calc_unique_crc(spd[spd_slot], sizeof(spd_raw_data));

			if (dimm->dimm[channel][slot].dram_type != SPD_MEMORY_TYPE_SDRAM_DDR3) {
				/* Mark DIMM as invalid */
				dimm->dimm[channel][slot].ranks   = 0;
				dimm->dimm[channel][slot].size_mb = 0;
				continue;
			}

			dram_print_spd_ddr3(&dimm->dimm[channel][slot]);
			dimms++;
			ctrl->rank_mirror[channel][slot * 2] = 0;
			ctrl->rank_mirror[channel][slot * 2 + 1] =
				dimm->dimm[channel][slot].flags.pins_mirrored;

			ctrl->channel_size_mb[channel] += dimm->dimm[channel][slot].size_mb;

			ctrl->auto_self_refresh &= dimm->dimm[channel][slot].flags.asr;

			ctrl->extended_temperature_range &=
				dimm->dimm[channel][slot].flags.ext_temp_refresh;

			ctrl->rankmap[channel] |=
				((1 << dimm->dimm[channel][slot].ranks) - 1) << (2 * slot);

			printk(BIOS_DEBUG, "channel[%d] rankmap = 0x%x\n", channel,
				ctrl->rankmap[channel]);
		}
		if ((ctrl->rankmap[channel] & 0x03) && (ctrl->rankmap[channel] & 0x0c)
				&& dimm->dimm[channel][0].reference_card <= 5
				&& dimm->dimm[channel][1].reference_card <= 5) {

			const int ref_card_offset_table[6][6] = {
				{ 0, 0, 0, 0, 2, 2 },
				{ 0, 0, 0, 0, 2, 2 },
				{ 0, 0, 0, 0, 2, 2 },
				{ 0, 0, 0, 0, 1, 1 },
				{ 2, 2, 2, 1, 0, 0 },
				{ 2, 2, 2, 1, 0, 0 },
			};
			ctrl->ref_card_offset[channel] = ref_card_offset_table
					[dimm->dimm[channel][0].reference_card]
					[dimm->dimm[channel][1].reference_card];
		} else {
			ctrl->ref_card_offset[channel] = 0;
		}
	}

	if (!dimms)
		die("No DIMMs were found");
}

static void save_timings(ramctr_timing *ctrl)
{
	/* Save the MRC S3 restore data to cbmem */
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION, ctrl, sizeof(*ctrl));
}

static void init_dram_ddr3(int min_tck, int s3resume, const u32 cpuid)
{
	int me_uma_size, cbmem_was_inited, fast_boot, err;
	ramctr_timing ctrl;
	spd_raw_data spds[4];
	struct region_device rdev;
	ramctr_timing *ctrl_cached = NULL;

	MCHBAR32(SAPMCTL) |= 1;

	/* Wait for ME to be ready */
	intel_early_me_init();
	me_uma_size = intel_early_me_uma_size();

	printk(BIOS_DEBUG, "Starting native Platform init\n");

	wait_txt_clear();

	wrmsr(0x000002e6, (msr_t) { .lo = 0, .hi = 0 });

	const u32 sskpd = MCHBAR32(SSKPD);	// !!! = 0x00000000
	if ((pci_read_config16(SOUTHBRIDGE, 0xa2) & 0xa0) == 0x20 && sskpd && !s3resume) {
		MCHBAR32(SSKPD) = 0;
		/* Need reset */
		system_reset();
	}

	early_pch_init_native();
	early_init_dmi();
	early_thermal_init();

	/* Try to find timings in MRC cache */
	err = mrc_cache_get_current(MRC_TRAINING_DATA, MRC_CACHE_VERSION, &rdev);

	if (!err && !(region_device_sz(&rdev) < sizeof(ctrl)))
		ctrl_cached = rdev_mmap_full(&rdev);

	/* Before reusing training data, assert that the CPU has not been replaced */
	if (ctrl_cached && cpuid != ctrl_cached->cpu) {

		/* It is not really worrying on a cold boot, but fatal when resuming from S3 */
		printk(s3resume ? BIOS_ALERT : BIOS_NOTICE,
				"CPUID %x differs from stored CPUID %x, CPU was replaced!\n",
				cpuid, ctrl_cached->cpu);

		/* Invalidate the stored data, it likely does not apply to the current CPU */
		ctrl_cached = NULL;
	}

	if (s3resume && !ctrl_cached) {
		/* S3 resume is impossible, reset to come up cleanly */
		system_reset();
	}

	/* Verify MRC cache for fast boot */
	if (!s3resume && ctrl_cached) {
		/* Load SPD unique information data. */
		memset(spds, 0, sizeof(spds));
		mainboard_get_spd(spds, 1);

		/* check SPD CRC16 to make sure the DIMMs haven't been replaced */
		fast_boot = verify_crc16_spds_ddr3(spds, ctrl_cached);
		if (!fast_boot)
			printk(BIOS_DEBUG, "Stored timings CRC16 mismatch.\n");
	} else {
		fast_boot = s3resume;
	}

	if (fast_boot) {
		printk(BIOS_DEBUG, "Trying stored timings.\n");
		memcpy(&ctrl, ctrl_cached, sizeof(ctrl));

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
		if (err) {
			if (s3resume) {
				/* Failed S3 resume, reset to come up cleanly */
				system_reset();
			}
			/* No need to erase bad MRC cache here, it gets overwritten on a
			   successful boot */
			printk(BIOS_ERR, "Stored timings are invalid !\n");
			fast_boot = 0;
		}
	}
	if (!fast_boot) {
		/* Reset internal state */
		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.tCK = min_tck;

		/* Get architecture */
		ctrl.cpu = cpuid;

		/* Get DDR3 SPD data */
		memset(spds, 0, sizeof(spds));
		mainboard_get_spd(spds, 0);
		dram_find_spds_ddr3(spds, &ctrl);

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err) {
		/* Fallback: disable failing channel */
		printk(BIOS_ERR, "RAM training failed, trying fallback.\n");
		printram("Disable failing channel.\n");

		/* Reset internal state */
		memset(&ctrl, 0, sizeof(ctrl));
		ctrl.tCK = min_tck;

		/* Get architecture */
		ctrl.cpu = cpuid;

		/* Reset DDR3 frequency */
		dram_find_spds_ddr3(spds, &ctrl);

		/* Disable failing channel */
		disable_channel(&ctrl, GET_ERR_CHANNEL(err));

		err = try_init_dram_ddr3(&ctrl, fast_boot, s3resume, me_uma_size);
	}

	if (err)
		die("raminit failed");

	/* FIXME: should be hardware revision-dependent. The register only exists on IVB. */
	MCHBAR32(CHANNEL_HASH) = 0x00a030ce;

	set_scrambling_seed(&ctrl);

	set_normal_operation(&ctrl);

	final_registers(&ctrl);

	/* Zone config */
	dram_zones(&ctrl, 0);

	intel_early_me_status();
	intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);
	intel_early_me_status();

	report_memory_config();

	cbmem_was_inited = !cbmem_recovery(s3resume);
	if (!fast_boot)
		save_timings(&ctrl);
	if (s3resume && !cbmem_was_inited) {
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	}

	if (!s3resume)
		fill_smbios17(&ctrl);
}

void perform_raminit(int s3resume)
{
	post_code(0x3a);

	timestamp_add_now(TS_BEFORE_INITRAM);

	init_dram_ddr3(get_mem_min_tck(), s3resume, cpu_get_cpuid());
}
