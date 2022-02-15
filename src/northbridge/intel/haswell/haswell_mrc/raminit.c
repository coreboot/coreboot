/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <console/console.h>
#include <console/usb.h>
#include <string.h>
#include <cbmem.h>
#include <cbfs.h>
#include <cf9_reset.h>
#include <ip_checksum.h>
#include <memory_info.h>
#include <mrc_cache.h>
#include <device/device.h>
#include <device/pci_def.h>
#include <device/pci_ops.h>
#include <device/dram/ddr3.h>
#include <northbridge/intel/haswell/chip.h>
#include <northbridge/intel/haswell/haswell.h>
#include <northbridge/intel/haswell/raminit.h>
#include <smbios.h>
#include <spd.h>
#include <security/vboot/vboot_common.h>
#include <commonlib/region.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <timestamp.h>
#include <types.h>

#include "pei_data.h"

#define MRC_CACHE_VERSION 1

static void save_mrc_data(struct pei_data *pei_data)
{
	/* Save the MRC S3 restore data to cbmem */
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION, pei_data->mrc_output,
			     pei_data->mrc_output_len);
}

static void prepare_mrc_cache(struct pei_data *pei_data)
{
	size_t mrc_size;

	/* Preset just in case there is an error */
	pei_data->mrc_input = NULL;
	pei_data->mrc_input_len = 0;

	pei_data->mrc_input =
		mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
					    MRC_CACHE_VERSION,
					    &mrc_size);
	if (!pei_data->mrc_input)
		/* Error message printed in find_current_mrc_cache */
		return;

	pei_data->mrc_input_len = mrc_size;

	printk(BIOS_DEBUG, "%s: at %p, size %zx\n", __func__,
	       pei_data->mrc_input, mrc_size);
}

static const char *const ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active",
};

/* Print out the memory controller configuration, as per the values in its registers. */
static void report_memory_config(void)
{
	int i;

	const u32 addr_decoder_common = mchbar_read32(MAD_CHNL);

	printk(BIOS_DEBUG, "memcfg DDR3 clock %d MHz\n",
	       (mchbar_read32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100);

	printk(BIOS_DEBUG, "memcfg channel assignment: A: %d, B % d, C % d\n",
	       (addr_decoder_common >> 0) & 3,
	       (addr_decoder_common >> 2) & 3,
	       (addr_decoder_common >> 4) & 3);

	for (i = 0; i < NUM_CHANNELS; i++) {
		const u32 ch_conf = mchbar_read32(MAD_DIMM(i));

		printk(BIOS_DEBUG, "memcfg channel[%d] config (%8.8x):\n", i, ch_conf);
		printk(BIOS_DEBUG, "   ECC %s\n", ecc_decoder[(ch_conf >> 24) & 3]);
		printk(BIOS_DEBUG, "   enhanced interleave mode %s\n",
		       ((ch_conf >> 22) & 1) ? "on" : "off");

		printk(BIOS_DEBUG, "   rank interleave %s\n",
		       ((ch_conf >> 21) & 1) ? "on" : "off");

		printk(BIOS_DEBUG, "   DIMMA %d MB width %s %s rank%s\n",
		       ((ch_conf >> 0) & 0xff) * 256,
		       ((ch_conf >> 19) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 17) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? "" : ", selected");

		printk(BIOS_DEBUG, "   DIMMB %d MB width %s %s rank%s\n",
		       ((ch_conf >> 8) & 0xff) * 256,
		       ((ch_conf >> 20) & 1) ? "x16" : "x8 or x32",
		       ((ch_conf >> 18) & 1) ? "dual" : "single",
		       ((ch_conf >> 16) & 1) ? ", selected" : "");
	}
}

/**
 * Find PEI executable in coreboot filesystem and execute it.
 *
 * @param pei_data: configuration data for UEFI PEI reference code
 */
static void sdram_initialize(struct pei_data *pei_data)
{
	int (*entry)(struct pei_data *pei_data) __attribute__((regparm(1)));

	printk(BIOS_DEBUG, "Starting UEFI PEI System Agent\n");

	/*
	 * Always pass in mrc_cache data.  The driver will determine
	 * whether to use the data or not.
	 */
	prepare_mrc_cache(pei_data);

	/* If MRC data is not found, we cannot continue S3 resume */
	if (pei_data->boot_mode == 2 && !pei_data->mrc_input) {
		post_code(POST_RESUME_FAILURE);
		printk(BIOS_DEBUG, "Giving up in %s: No MRC data\n", __func__);
		system_reset();
	}

	/* Pass console handler in pei_data */
	pei_data->tx_byte = do_putchar;

	/*
	 * Locate and call UEFI System Agent binary. The binary needs to be at a fixed offset
	 * in the flash and can therefore only reside in the COREBOOT fmap region. We don't care
	 * about leaking the mapping.
	 */
	entry = cbfs_ro_map("mrc.bin", NULL);
	if (entry) {
		int rv = entry(pei_data);

		/* The mrc.bin reconfigures USB, so usbdebug needs to be reinitialized */
		if (CONFIG(USBDEBUG_IN_PRE_RAM))
			usbdebug_hw_init(true);

		if (rv) {
			switch (rv) {
			case -1:
				printk(BIOS_ERR, "PEI version mismatch.\n");
				break;
			case -2:
				printk(BIOS_ERR, "Invalid memory frequency.\n");
				break;
			default:
				printk(BIOS_ERR, "MRC returned %x.\n", rv);
			}
			die_with_post_code(POST_INVALID_VENDOR_BINARY,
					   "Nonzero MRC return value.\n");
		}
	} else {
		die("UEFI PEI System Agent not found.\n");
	}

	/* Print the MRC version after executing the UEFI PEI stage */
	u32 version = mchbar_read32(MRC_REVISION);
	printk(BIOS_DEBUG, "MRC Version %u.%u.%u Build %u\n",
		(version >> 24) & 0xff, (version >> 16) & 0xff,
		(version >>  8) & 0xff, (version >>  0) & 0xff);

	/*
	 * MRC may return zero even when raminit did not complete successfully.
	 * Ensure the mc_init_done_ack bit is set before continuing. Otherwise,
	 * attempting to access memory will lock up the system.
	 */
	if (!(mchbar_read32(MC_INIT_STATE_G) & (1 << 5))) {
		printk(BIOS_EMERG, "Memory controller did not acknowledge raminit.\n");
		die("MRC raminit failed\n");
	}

	report_memory_config();
}

static uint8_t nb_get_ecc_type(const uint32_t capid0_a)
{
	return capid0_a & CAPID_ECCDIS ? MEMORY_ARRAY_ECC_NONE : MEMORY_ARRAY_ECC_SINGLE_BIT;
}

static uint16_t nb_slots_per_channel(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_DDPCD) + 1;
}

static uint16_t nb_number_of_channels(const uint32_t capid0_a)
{
	return !(capid0_a & CAPID_PDCD) + 1;
}

static uint32_t nb_max_chan_capacity_mib(const uint32_t capid0_a)
{
	uint32_t ddrsz;

	/* Values from documentation, which assume two DIMMs per channel */
	switch (CAPID_DDRSZ(capid0_a)) {
	case 1:
		ddrsz = 8192;
		break;
	case 2:
		ddrsz = 2048;
		break;
	case 3:
		ddrsz = 512;
		break;
	default:
		ddrsz = 16384;
		break;
	}

	/* Account for the maximum number of DIMMs per channel */
	return (ddrsz / 2) * nb_slots_per_channel(capid0_a);
}

static void setup_sdram_meminfo(struct pei_data *pei_data)
{
	struct memory_info *mem_info;
	struct dimm_info *dimm;
	int ch, d_num;
	int dimm_cnt = 0;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	if (!mem_info)
		die("Failed to add memory info to CBMEM.\n");

	memset(mem_info, 0, sizeof(struct memory_info));

	const u32 ddr_freq_mhz = (mchbar_read32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100;

	for (ch = 0; ch < NUM_CHANNELS; ch++) {
		const u32 ch_conf = mchbar_read32(MAD_DIMM(ch));
		/* DIMMs A/B */
		for (d_num = 0; d_num < NUM_SLOTS; d_num++) {
			const u32 dimm_size = ((ch_conf >> (d_num * 8)) & 0xff) * 256;
			if (dimm_size) {
				const int index = ch * NUM_SLOTS + d_num;
				dimm = &mem_info->dimm[dimm_cnt];
				dimm->dimm_size = dimm_size;
				dimm->ddr_type = MEMORY_TYPE_DDR3;
				dimm->ddr_frequency = ddr_freq_mhz * 2; /* In MT/s */
				dimm->rank_per_dimm = 1 + ((ch_conf >> (17 + d_num)) & 1);
				dimm->channel_num = ch;
				dimm->dimm_num = d_num;
				dimm->bank_locator = ch * 2;
				memcpy(dimm->serial,
					&pei_data->spd_data[index][SPD_DIMM_SERIAL_NUM],
					SPD_DIMM_SERIAL_LEN);
				memcpy(dimm->module_part_number,
					&pei_data->spd_data[index][SPD_DIMM_PART_NUM],
					SPD_DIMM_PART_LEN);
				dimm->mod_id =
					(pei_data->spd_data[index][SPD_DIMM_MOD_ID2] << 8) |
					(pei_data->spd_data[index][SPD_DIMM_MOD_ID1] & 0xff);
				dimm->mod_type = DDR3_SPD_SODIMM;
				dimm->bus_width = MEMORY_BUS_WIDTH_64;
				dimm_cnt++;
			}
		}
	}
	mem_info->dimm_cnt = dimm_cnt;

	const uint32_t capid0_a = pci_read_config32(HOST_BRIDGE, CAPID0_A);

	const uint16_t channels = nb_number_of_channels(capid0_a);

	mem_info->ecc_type = nb_get_ecc_type(capid0_a);
	mem_info->max_capacity_mib = channels * nb_max_chan_capacity_mib(capid0_a);
	mem_info->number_of_devices = channels * nb_slots_per_channel(capid0_a);
}

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

static enum pei_usb2_port_location map_to_pei_usb2_location(const enum usb2_port_location loc)
{
	static const enum pei_usb2_port_location map[] = {
		[USB_PORT_SKIP]		= PEI_USB_PORT_SKIP,
		[USB_PORT_BACK_PANEL]	= PEI_USB_PORT_BACK_PANEL,
		[USB_PORT_FRONT_PANEL]	= PEI_USB_PORT_FRONT_PANEL,
		[USB_PORT_DOCK]		= PEI_USB_PORT_DOCK,
		[USB_PORT_MINI_PCIE]	= PEI_USB_PORT_MINI_PCIE,
		[USB_PORT_FLEX]		= PEI_USB_PORT_FLEX,
		[USB_PORT_INTERNAL]	= PEI_USB_PORT_INTERNAL,
	};
	return loc >= ARRAY_SIZE(map) ? PEI_USB_PORT_SKIP : map[loc];
}

static uint8_t map_to_pei_oc_pin(const uint8_t oc_pin)
{
	return oc_pin >= USB_OC_PIN_SKIP ? PEI_USB_OC_PIN_SKIP : oc_pin;
}

void perform_raminit(const int s3resume)
{
	const struct device *gbe = pcidev_on_root(0x19, 0);

	const struct northbridge_intel_haswell_config *cfg = config_of_soc();

	struct pei_data pei_data = {
		.pei_version		= PEI_VERSION,
		.mchbar			= CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar			= CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar			= CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar		= CONFIG_ECAM_MMCONF_BASE_ADDRESS,
		.smbusbar		= CONFIG_FIXED_SMBUS_IO_BASE,
		.hpet_address		= HPET_BASE_ADDRESS,
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

	for (size_t i = 0; i < ARRAY_SIZE(mainboard_usb2_ports); i++) {
		/* If a port is not enabled, skip it */
		if (!mainboard_usb2_ports[i].enable) {
			pei_data.usb2_ports[i].over_current_pin	= PEI_USB_OC_PIN_SKIP;
			pei_data.usb2_ports[i].location		= PEI_USB_PORT_SKIP;
			continue;
		}
		const enum usb2_port_location loc = mainboard_usb2_ports[i].location;
		const uint8_t oc_pin = mainboard_usb2_ports[i].oc_pin;
		pei_data.usb2_ports[i].length		= mainboard_usb2_ports[i].length;
		pei_data.usb2_ports[i].enable		= mainboard_usb2_ports[i].enable;
		pei_data.usb2_ports[i].over_current_pin	= map_to_pei_oc_pin(oc_pin);
		pei_data.usb2_ports[i].location		= map_to_pei_usb2_location(loc);
	}

	for (size_t i = 0; i < ARRAY_SIZE(mainboard_usb3_ports); i++) {
		const uint8_t oc_pin = mainboard_usb3_ports[i].oc_pin;
		pei_data.usb3_ports[i].enable		= mainboard_usb3_ports[i].enable;
		pei_data.usb3_ports[i].over_current_pin	= map_to_pei_oc_pin(oc_pin);
	}

	/* MRC has hardcoded assumptions of 2 meaning S3 wake. Normalize it here. */
	pei_data.boot_mode = s3resume ? 2 : 0;

	/* Obtain the SPD addresses from mainboard code */
	struct spd_info spdi = {0};
	mb_get_spd_map(&spdi);

	/* MRC expects left-aligned SMBus addresses, and 0xff for memory-down */
	for (size_t i = 0; i < ARRAY_SIZE(spdi.addresses); i++) {
		const uint8_t addr = spdi.addresses[i];
		pei_data.spd_addresses[i] = addr == SPD_MEMORY_DOWN ? 0xff : addr << 1;
	}

	/* Calculate unimplemented DIMM slots for each channel */
	pei_data.dimm_channel0_disabled = make_channel_disabled_mask(&pei_data, 0);
	pei_data.dimm_channel1_disabled = make_channel_disabled_mask(&pei_data, 1);

	timestamp_add_now(TS_INITRAM_START);

	copy_spd(&pei_data, &spdi);

	sdram_initialize(&pei_data);

	timestamp_add_now(TS_INITRAM_END);

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

	setup_sdram_meminfo(&pei_data);
}
