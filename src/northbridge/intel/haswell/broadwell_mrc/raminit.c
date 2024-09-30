/* SPDX-License-Identifier: GPL-2.0-only */

#include <assert.h>
#include <console/console.h>
#include <console/streams.h>
#include <console/usb.h>
#include <string.h>
#include <cbmem.h>
#include <cbfs.h>
#include <cf9_reset.h>
#include <device/dram/ddr3.h>
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
#include <static.h>
#include <security/vboot/vboot_common.h>
#include <commonlib/region.h>
#include <southbridge/intel/lynxpoint/me.h>
#include <southbridge/intel/lynxpoint/pch.h>
#include <timestamp.h>
#include <types.h>

#include "pei_data.h"

static void save_mrc_data(struct pei_data *pei_data)
{
	printk(BIOS_DEBUG, "MRC data at %p %d bytes\n", pei_data->data_to_save,
	       pei_data->data_to_save_size);

	if (pei_data->data_to_save != NULL && pei_data->data_to_save_size > 0)
		mrc_cache_stash_data(MRC_TRAINING_DATA, 0,
					pei_data->data_to_save,
					pei_data->data_to_save_size);
}

static const char *const ecc_decoder[] = {
	"inactive",
	"active on IO",
	"disabled on IO",
	"active",
};

/*
 * Dump in the log memory controller configuration as read from the memory
 * controller registers.
 */
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

typedef int ABI_X86(*pei_wrapper_entry_t)(struct pei_data *pei_data);

static void ABI_X86 send_to_console(unsigned char b)
{
	console_tx_byte(b);
}

/*
 * Find PEI executable in coreboot filesystem and execute it.
 */
static void sdram_initialize(struct pei_data *pei_data)
{
	size_t mrc_size;
	pei_wrapper_entry_t entry;
	int ret;

	/* Assume boot device is memory mapped. */
	assert(CONFIG(BOOT_DEVICE_MEMORY_MAPPED));

	pei_data->saved_data =
		mrc_cache_current_mmap_leak(MRC_TRAINING_DATA, 0,
					    &mrc_size);
	if (pei_data->saved_data) {
		/* MRC cache found */
		pei_data->saved_data_size = mrc_size;
	} else if (pei_data->boot_mode == ACPI_S3) {
		/* Waking from S3 and no cache. */
		printk(BIOS_DEBUG,
		       "No MRC cache found in S3 resume path.\n");
		post_code(POSTCODE_RESUME_FAILURE);
		system_reset();
	} else {
		printk(BIOS_DEBUG, "No MRC cache found.\n");
	}

	/*
	 * Do not use saved pei data.  Can be set by mainboard romstage
	 * to force a full train of memory on every boot.
	 */
	if (pei_data->disable_saved_data) {
		printk(BIOS_DEBUG, "Disabling PEI saved data by request\n");
		pei_data->saved_data = NULL;
		pei_data->saved_data_size = 0;
	}

	/* We don't care about leaking the mapping */
	entry = cbfs_ro_map("mrc.bin", NULL);
	if (entry == NULL)
		die("mrc.bin not found!");

	printk(BIOS_DEBUG, "Starting Memory Reference Code\n");

	ret = entry(pei_data);
	if (ret < 0)
		die("pei_data version mismatch\n");

	/* Print the MRC version after executing the UEFI PEI stage. */
	u32 version = mchbar_read32(MRC_REVISION);
	printk(BIOS_DEBUG, "MRC Version %u.%u.%u Build %u\n",
		(version >> 24) & 0xff, (version >> 16) & 0xff,
		(version >>  8) & 0xff, (version >>  0) & 0xff);

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
	unsigned int dimm_cnt = 0;

	struct memory_info *mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(*mem_info));
	if (!mem_info)
		die("Failed to add memory info to CBMEM.\n");

	memset(mem_info, 0, sizeof(struct memory_info));

	const u32 ddr_frequency = (mchbar_read32(MC_BIOS_DATA) * 13333 * 2 + 50) / 100;

	for (unsigned int ch = 0; ch < NUM_CHANNELS; ch++) {
		const u32 ch_conf = mchbar_read32(MAD_DIMM(ch));
		for (unsigned int slot = 0; slot < NUM_SLOTS; slot++) {
			const u32 dimm_size = ((ch_conf >> (slot * 8)) & 0xff) * 256;
			if (dimm_size) {
				struct dimm_info *dimm = &mem_info->dimm[dimm_cnt];
				dimm->dimm_size = dimm_size;
				dimm->ddr_type = MEMORY_TYPE_DDR3;
				dimm->ddr_frequency = ddr_frequency;
				dimm->rank_per_dimm = 1 + ((ch_conf >> (17 + slot)) & 1);
				dimm->channel_num = ch;
				dimm->dimm_num = slot;
				dimm->bank_locator = ch * 2;
				memcpy(dimm->serial,
					&pei_data->spd_data[ch][slot][SPD_DDR3_SERIAL_NUM],
					SPD_DDR3_SERIAL_LEN);
				memcpy(dimm->module_part_number,
					&pei_data->spd_data[ch][slot][SPD_DDR3_PART_NUM],
					SPD_DDR3_PART_LEN);
				dimm->mod_id =
					(pei_data->spd_data[ch][slot][SPD_DDR3_MOD_ID2] << 8) |
					(pei_data->spd_data[ch][slot][SPD_DDR3_MOD_ID1] & 0xff);
				dimm->mod_type = SPD_DDR3_DIMM_TYPE_SO_DIMM;
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

#include <device/smbus_host.h>

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

	if (spd_file_len < ((spdi->spd_index + 1) * SPD_SIZE_MAX_DDR3)) {
		printk(BIOS_ERR, "SPD index override to 0 - old hardware?\n");
		spdi->spd_index = 0;
	}

	if (spd_file_len < SPD_SIZE_MAX_DDR3)
		die("Missing SPD data.");

	/* MRC only uses index 0, but coreboot uses the other indices */
	memcpy(pei_data->spd_data[0], spd_file + (spdi->spd_index * SPD_SIZE_MAX_DDR3),
		SPD_SIZE_MAX_DDR3);

	for (size_t i = 1; i < ARRAY_SIZE(spdi->addresses); i++) {
		if (spdi->addresses[i] == SPD_MEMORY_DOWN)
			memcpy(pei_data->spd_data[i], pei_data->spd_data[0], SPD_SIZE_MAX_DDR3);
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
	/* TODO: USB_PORT_NGFF_DEVICE_DOWN (not used by any board, though) */
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

static bool early_init_native(int s3resume)
{
	printk(BIOS_DEBUG, "Starting native platform initialisation\n");

	intel_early_me_init();
	/** TODO: CPU replacement check must be skipped in warm boots and S3 resumes **/
	const bool cpu_replaced = !s3resume && intel_early_me_cpu_replacement_check();

	early_pch_init_native(s3resume);

	if (!CONFIG(INTEL_LYNXPOINT_LP))
		dmi_early_init();

	return cpu_replaced;
}

void perform_raminit(const int s3resume)
{
	const struct northbridge_intel_haswell_config *cfg = config_of_soc();

	struct pei_data pei_data = {
		.pei_version		= PEI_VERSION,
		.board_type		= (enum board_type)get_pch_platform_type(),
		.usbdebug		= CONFIG(USBDEBUG),
		.pciexbar		= CONFIG_ECAM_MMCONF_BASE_ADDRESS,
		.smbusbar		= CONFIG_FIXED_SMBUS_IO_BASE,
		.ehcibar		= CONFIG_EHCI_BAR,
		.xhcibar		= 0xd7000000,
		.gttbar			= 0xe0000000,
		.pmbase			= DEFAULT_PMBASE,
		.gpiobase		= DEFAULT_GPIOBASE,
		.tseg_size		= CONFIG_SMM_TSEG_SIZE,
		.temp_mmio_base		= 0xfed08000,
		.ec_present		= cfg->ec_present,
		.dq_pins_interleaved	= cfg->dq_pins_interleaved,
		.tx_byte		= send_to_console,
		.ddr_refresh_2x		= CONFIG(ENABLE_DDR_2X_REFRESH),
		.rcba			= CONFIG_FIXED_RCBA_MMIO_BASE,	/* Might be unused */
	};

	for (size_t i = 0; i < ARRAY_SIZE(mainboard_usb2_ports); i++) {
		/* If a port is not enabled, skip it */
		if (!mainboard_usb2_ports[i].enable) {
			pei_data.usb2_ports[i].oc_pin	= PEI_USB_OC_PIN_SKIP;
			pei_data.usb2_ports[i].location	= PEI_USB_PORT_SKIP;
			continue;
		}
		const enum usb2_port_location loc = mainboard_usb2_ports[i].location;
		const uint8_t oc_pin = mainboard_usb2_ports[i].oc_pin;
		pei_data.usb2_ports[i].length	= mainboard_usb2_ports[i].length;
		pei_data.usb2_ports[i].enable	= mainboard_usb2_ports[i].enable;
		pei_data.usb2_ports[i].oc_pin	= map_to_pei_oc_pin(oc_pin);
		pei_data.usb2_ports[i].location	= map_to_pei_usb2_location(loc);
	}

	for (size_t i = 0; i < ARRAY_SIZE(mainboard_usb3_ports); i++) {
		const uint8_t oc_pin = mainboard_usb3_ports[i].oc_pin;
		pei_data.usb3_ports[i].enable	= mainboard_usb3_ports[i].enable;
		pei_data.usb3_ports[i].oc_pin	= map_to_pei_oc_pin(oc_pin);
	}

	/* Broadwell MRC uses ACPI values for boot_mode */
	pei_data.boot_mode = s3resume ? ACPI_S3 : ACPI_S0;

	struct spd_info spdi = {0};
	get_spd_info(&spdi, cfg);

	/*
	 * Read the SPDs over SMBus in coreboot code so that the data can be used to
	 * populate meminfo. MRC returns some data, but it seems to be incomplete.
	 */
	for (size_t i = 0; i < ARRAY_SIZE(spdi.addresses); i++) {
		const uint8_t addr = spdi.addresses[i];
		pei_data.spd_addresses[i] = addr == SPD_MEMORY_DOWN ? 0xff : addr << 1;
		if (addr == SPD_MEMORY_DOWN)
			continue;

		if (i2c_eeprom_read(addr, 0, 256, pei_data.spd_data[i / 2][i % 2]) != 256) {
			printk(BIOS_ERR, "0x%02x failed to read\n", addr);
			memset(pei_data.spd_data[i / 2][i % 2], 0, 256);
		}
	}

	/* Calculate unimplemented DIMM slots for each channel */
	pei_data.dimm_channel0_disabled = make_channel_disabled_mask(&pei_data, 0);
	pei_data.dimm_channel1_disabled = make_channel_disabled_mask(&pei_data, 1);

	for (size_t i = 0; i < ARRAY_SIZE(spdi.addresses); i++)
		pei_data.spd_addresses[i] = 0;

	if (early_init_native(s3resume))
		pei_data.disable_saved_data = true;

	timestamp_add_now(TS_INITRAM_START);

	copy_spd(&pei_data, &spdi);

	sdram_initialize(&pei_data);

	timestamp_add_now(TS_INITRAM_END);

	if (intel_early_me_uma_size() > 0) {
		/*
		 * The 'other' success value is to report loss of memory
		 * consistency to ME if warm boot was downgraded to cold.
		 * However, we can't tell if MRC downgraded the bootmode.
		 */
		intel_early_me_init_done(ME_INIT_STATUS_SUCCESS_OTHER);
	}

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
