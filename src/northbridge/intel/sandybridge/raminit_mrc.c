/* SPDX-License-Identifier: GPL-2.0-only */

#include <arch/hpet.h>
#include <console/console.h>
#include <console/usb.h>
#include <cf9_reset.h>
#include <string.h>
#include <device/device.h>
#include <device/dram/ddr3.h>
#include <device/pci_ops.h>
#include <arch/cpu.h>
#include <cbmem.h>
#include <cbfs.h>
#include <commonlib/bsd/ipchksum.h>
#include <cpu/intel/model_206ax/model_206ax.h>
#include <pc80/mc146818rtc.h>
#include <device/pci_def.h>
#include <lib.h>
#include <mrc_cache.h>
#include <spd.h>
#include <smbios.h>
#include <stddef.h>
#include <stdint.h>
#include <timestamp.h>
#include "raminit.h"
#include "pei_data.h"
#include "sandybridge.h"
#include "chip.h"
#include <security/vboot/vboot_common.h>
#include <southbridge/intel/bd82x6x/pch.h>
#include <memory_info.h>
#include <mode_switch.h>

/* Management Engine is in the southbridge */
#include <southbridge/intel/bd82x6x/me.h>

/*
 * MRC scrambler seed offsets should be reserved in
 * mainboard cmos.layout and not covered by checksum.
 */
#if CONFIG(USE_OPTION_TABLE)
#include "option_table.h"
#define CMOS_OFFSET_MRC_SEED     (CMOS_VSTART_mrc_scrambler_seed     >> 3)
#define CMOS_OFFSET_MRC_SEED_S3  (CMOS_VSTART_mrc_scrambler_seed_s3  >> 3)
#define CMOS_OFFSET_MRC_SEED_CHK (CMOS_VSTART_mrc_scrambler_seed_chk >> 3)
#else
#define CMOS_OFFSET_MRC_SEED     152
#define CMOS_OFFSET_MRC_SEED_S3  156
#define CMOS_OFFSET_MRC_SEED_CHK 160
#endif

#define MRC_CACHE_VERSION 0

/* Assembly functions: */
void mrc_wrapper(void *func_ptr, uint32_t arg1);
void __prot2lm_do_putchar(uint8_t byte);

static void save_mrc_data(struct pei_data *pei_data)
{
	u16 c1, c2, checksum;

	/* Save the MRC S3 restore data to cbmem */
	mrc_cache_stash_data(MRC_TRAINING_DATA, MRC_CACHE_VERSION,
			     (void *)(uintptr_t)pei_data->mrc_output_ptr,
			     pei_data->mrc_output_len);

	/* Save the MRC seed values to CMOS */
	cmos_write32(pei_data->scrambler_seed, CMOS_OFFSET_MRC_SEED);
	printk(BIOS_DEBUG, "Save scrambler seed    0x%08x to CMOS 0x%02x\n",
	       pei_data->scrambler_seed, CMOS_OFFSET_MRC_SEED);

	cmos_write32(pei_data->scrambler_seed_s3, CMOS_OFFSET_MRC_SEED_S3);
	printk(BIOS_DEBUG, "Save s3 scrambler seed 0x%08x to CMOS 0x%02x\n",
	       pei_data->scrambler_seed_s3, CMOS_OFFSET_MRC_SEED_S3);

	/* Save a simple checksum of the seed values */
	c1 = ipchksum((u8 *)&pei_data->scrambler_seed,    sizeof(u32));
	c2 = ipchksum((u8 *)&pei_data->scrambler_seed_s3, sizeof(u32));
	checksum = ipchksum_add(sizeof(u32), c1, c2);

	cmos_write((checksum >> 0) & 0xff, CMOS_OFFSET_MRC_SEED_CHK);
	cmos_write((checksum >> 8) & 0xff, CMOS_OFFSET_MRC_SEED_CHK + 1);
}

static void prepare_mrc_cache(struct pei_data *pei_data)
{
	u16 c1, c2, checksum, seed_checksum;
	size_t mrc_size;

	/* Preset just in case there is an error */
	pei_data->mrc_input_ptr = 0;
	pei_data->mrc_input_len = 0;

	/* Read scrambler seeds from CMOS */
	pei_data->scrambler_seed = cmos_read32(CMOS_OFFSET_MRC_SEED);
	printk(BIOS_DEBUG, "Read scrambler seed    0x%08x from CMOS 0x%02x\n",
	       pei_data->scrambler_seed, CMOS_OFFSET_MRC_SEED);

	pei_data->scrambler_seed_s3 = cmos_read32(CMOS_OFFSET_MRC_SEED_S3);
	printk(BIOS_DEBUG, "Read S3 scrambler seed 0x%08x from CMOS 0x%02x\n",
	       pei_data->scrambler_seed_s3, CMOS_OFFSET_MRC_SEED_S3);

	/* Compute seed checksum and compare */
	c1 = ipchksum((u8 *)&pei_data->scrambler_seed,    sizeof(u32));
	c2 = ipchksum((u8 *)&pei_data->scrambler_seed_s3, sizeof(u32));
	checksum = ipchksum_add(sizeof(u32), c1, c2);

	seed_checksum  = cmos_read(CMOS_OFFSET_MRC_SEED_CHK);
	seed_checksum |= cmos_read(CMOS_OFFSET_MRC_SEED_CHK + 1) << 8;

	if (checksum != seed_checksum) {
		printk(BIOS_ERR, "%s: invalid seed checksum\n", __func__);
		pei_data->scrambler_seed = 0;
		pei_data->scrambler_seed_s3 = 0;
		return;
	}

	pei_data->mrc_input_ptr = (uintptr_t)mrc_cache_current_mmap_leak(MRC_TRAINING_DATA,
							  MRC_CACHE_VERSION,
							  &mrc_size);
	if (!pei_data->mrc_input_ptr) {
		/* Error message printed in find_current_mrc_cache */
		return;
	}

	pei_data->mrc_input_len = mrc_size;

	printk(BIOS_DEBUG, "%s: at 0x%x, size %zx\n", __func__,
	       pei_data->mrc_input_ptr, mrc_size);
}

/**
 * Find PEI executable in coreboot filesystem and execute it.
 *
 * @param pei_data: configuration data for UEFI PEI reference code
 */
static void sdram_initialize(struct pei_data *pei_data)
{
	int (*entry)(struct pei_data *pei_data);

	/* Wait for ME to be ready */
	intel_early_me_init();
	intel_early_me_uma_size();

	printk(BIOS_DEBUG, "Starting UEFI PEI System Agent\n");

	/*
	 * Always pass in mrc_cache data.  The driver will determine
	 * whether to use the data or not.
	 */
	prepare_mrc_cache(pei_data);

	/* If MRC data is not found we cannot continue S3 resume. */
	if (pei_data->boot_mode == 2 && !pei_data->mrc_input_ptr) {
		printk(BIOS_DEBUG, "Giving up in %s: No MRC data\n", __func__);
		system_reset();
	}

	/*
	 * Pass console handler in pei_data. On x86_64 provide a wrapper around
	 * do_putchar that switches to long mode before calling do_putchar.
	 */
	if (ENV_X86_64)
		pei_data->tx_byte_ptr = (uintptr_t)__prot2lm_do_putchar;
	else
		pei_data->tx_byte_ptr = (uintptr_t)do_putchar;

	/* Locate and call UEFI System Agent binary. */
	entry = cbfs_map("mrc.bin", NULL);
	if (entry) {
		int rv;
		rv = protected_mode_call_2arg(mrc_wrapper, (uintptr_t)entry, (uintptr_t)pei_data);
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
			die_with_post_code(POSTCODE_INVALID_VENDOR_BINARY,
					   "Nonzero MRC return value.\n");
		}
	} else {
		die("UEFI PEI System Agent not found.\n");
	}

	/* mrc.bin reconfigures USB, so reinit it to have debug */
	if (CONFIG(USBDEBUG_IN_PRE_RAM))
		usbdebug_hw_init(true);

	/* Print the MRC version after executing the UEFI PEI stage */
	u32 version = mchbar_read32(MRC_REVISION);
	printk(BIOS_DEBUG, "MRC Version %u.%u.%u Build %u\n",
		(version >> 24) & 0xff, (version >> 16) & 0xff,
		(version >>  8) & 0xff, (version >>  0) & 0xff);

	/*
	 * Send ME init done for SandyBridge here.
	 * This is done inside the SystemAgent binary on IvyBridge.
	 */
	if (BASE_REV_SNB == (pci_read_config16(PCI_CPU_DEVICE, PCI_DEVICE_ID) & BASE_REV_MASK))
		intel_early_me_init_done(ME_INIT_STATUS_SUCCESS);
	else
		intel_early_me_status();

	report_memory_config();
}

/*
 * These are the location and structure of MRC_VAR data in CAR.
 * The CAR region looks like this:
 * +------------------+ -> DCACHE_RAM_BASE
 * |                  |
 * |                  |
 * |  COREBOOT STACK  |
 * |                  |
 * |                  |
 * +------------------+ -> DCACHE_RAM_BASE + DCACHE_RAM_SIZE
 * |                  |
 * |  MRC HEAP        |
 * |  size = 0x5000   |
 * |                  |
 * +------------------+
 * |                  |
 * |  MRC VAR         |
 * |  size = 0x4000   |
 * |                  |
 * +------------------+ -> DACHE_RAM_BASE + DACHE_RAM_SIZE
 *                               + DCACHE_RAM_MRC_VAR_SIZE
 */
#define DCACHE_RAM_MRC_VAR_BASE	 (CONFIG_DCACHE_RAM_BASE + CONFIG_DCACHE_RAM_SIZE \
				+ CONFIG_DCACHE_RAM_MRC_VAR_SIZE - 0x4000)

struct mrc_var_data {
	u32 acpi_timer_flag;
	u32 pool_used;
	u32 pool_base;
	u32 tx_byte;
	u32 reserved[4];
} __packed;

static bool do_pcie_init(void)
{
	if (IS_IVY_CPU(cpu_get_cpuid())) {
		return is_devfn_enabled(PCI_DEVFN(1, 0));
	} else {
		return false;
	}
}

static void devicetree_fill_pei_data(struct pei_data *pei_data)
{
	const struct northbridge_intel_sandybridge_config *cfg = config_of_soc();

	switch (cfg->max_mem_clock_mhz) {
	/* MRC only supports fixed numbers of frequencies */
	default:
		printk(BIOS_WARNING, "RAMINIT: Limiting DDR3 clock to 800 Mhz\n");
		__fallthrough;
	case 400:
		pei_data->max_ddr3_freq = 800;
		break;
	case 533:
		pei_data->max_ddr3_freq = 1066;
		break;
	case 666:
		pei_data->max_ddr3_freq = 1333;
		break;
	case 800:
		pei_data->max_ddr3_freq = 1600;
		break;
	}

	/*
	 * SPD addresses are listed in devicetree as actual addresses,
	 * and for MRC need to be shifted left so bit 0 is always zero.
	 */
	if (!CONFIG(HAVE_SPD_IN_CBFS)) {
		for (unsigned int i = 0; i < ARRAY_SIZE(cfg->spd_addresses); i++) {
			pei_data->spd_addresses[i] = cfg->spd_addresses[i] << 1;
		}
	}
	memcpy(pei_data->ts_addresses,  cfg->ts_addresses,  sizeof(pei_data->ts_addresses));
}

static void spd_fill_pei_data(struct pei_data *pei_data)
{
	struct spd_info spdi = {0};
	unsigned int i, have_memory_down = 0;

	mb_get_spd_map(&spdi);

	for (i = 0; i < ARRAY_SIZE(spdi.addresses); i++) {
		if (spdi.addresses[i] == SPD_MEMORY_DOWN) {
			pei_data->spd_addresses[i] = 0;
			have_memory_down = 1;
		} else {
			/* MRC expects left-aligned SMBus addresses. */
			pei_data->spd_addresses[i] = spdi.addresses[i] << 1;
		}
	}
	/* Copy SPD data from CBFS for on-board memory */
	if (have_memory_down) {
		printk(BIOS_DEBUG, "SPD index %d\n", spdi.spd_index);

		size_t spd_file_len;
		uint8_t *spd_file = cbfs_map("spd.bin", &spd_file_len);

		if (!spd_file)
			die("SPD data %s!", "not found");

		if (spd_file_len < ((spdi.spd_index + 1) * SPD_SIZE_MAX_DDR3))
			die("SPD data %s!", "incomplete");

		/* MRC only uses index 0... */
		memcpy(pei_data->spd_data[0], spd_file + (spdi.spd_index * SPD_SIZE_MAX_DDR3), SPD_SIZE_MAX_DDR3);

		/* but coreboot uses the other indices */
		for (i = 1; i < ARRAY_SIZE(spdi.addresses); i++) {
			if (spdi.addresses[i] == SPD_MEMORY_DOWN)
				memcpy(pei_data->spd_data[i], pei_data->spd_data[0], SPD_SIZE_MAX_DDR3);
		}
	}
}

static void disable_p2p(void)
{
	/* Disable PCI-to-PCI bridge early to prevent probing by MRC */
	const struct device *const p2p = pcidev_on_root(0x1e, 0);
	if (p2p && p2p->enabled)
		return;

	RCBA32(FD) |= PCH_DISABLE_P2P;
}

static void setup_sdram_meminfo(struct pei_data *pei_data);

void perform_raminit(int s3resume)
{
	const struct northbridge_intel_sandybridge_config *cfg = config_of_soc();
	struct pei_data pei_data = {
		.pei_version = PEI_VERSION,
		.mchbar = CONFIG_FIXED_MCHBAR_MMIO_BASE,
		.dmibar = CONFIG_FIXED_DMIBAR_MMIO_BASE,
		.epbar = CONFIG_FIXED_EPBAR_MMIO_BASE,
		.pciexbar = CONFIG_ECAM_MMCONF_BASE_ADDRESS,
		.smbusbar = CONFIG_FIXED_SMBUS_IO_BASE,
		.wdbbar = 0x4000000,
		.wdbsize = 0x1000,
		.hpet_address = HPET_BASE_ADDRESS,
		.rcba = (uintptr_t)DEFAULT_RCBA,
		.pmbase = DEFAULT_PMBASE,
		.gpiobase = DEFAULT_GPIOBASE,
		.thermalbase = 0xfed08000,
		.tseg_size = CONFIG_SMM_TSEG_SIZE,
		.system_type = !(get_platform_type() == PLATFORM_MOBILE),
		.pcie_init = do_pcie_init(),
		.gbe_enable = is_devfn_enabled(PCI_DEVFN(0x19, 0)),
		.boot_mode = s3resume ? BOOT_PATH_RESUME : BOOT_PATH_NORMAL,
		.ec_present     = cfg->ec_present,
		.ddr3lv_support = cfg->ddr3lv_support,
		.nmode          = cfg->nmode,
		.ddr_refresh_rate_config  = cfg->ddr_refresh_rate_config,
		.usb3.mode                = cfg->usb3.mode,
		/* .usb3.hs_port_switch_mask = native config->xhci_switchable_ports */
		.usb3.preboot_support     = cfg->usb3.preboot_support,
		.usb3.xhci_streams        = cfg->usb3.xhci_streams,
	};

	struct mrc_var_data *mrc_var;

	/* Prepare USB controller early in S3 resume */
	if (s3resume)
		enable_usb_bar();

	southbridge_fill_pei_data(&pei_data);
	devicetree_fill_pei_data(&pei_data);
	if (CONFIG(HAVE_SPD_IN_CBFS))
		spd_fill_pei_data(&pei_data);
	mainboard_fill_pei_data(&pei_data);

	post_code(0x3a);

	/* Fill after mainboard_fill_pei_data as it might provide spd_data */
	pei_data.dimm_channel0_disabled =
		(!pei_data.spd_addresses[0] && !pei_data.spd_data[0][0]) +
		(!pei_data.spd_addresses[1] && !pei_data.spd_data[1][0]) * 2;

	pei_data.dimm_channel1_disabled =
		(!pei_data.spd_addresses[2] && !pei_data.spd_data[2][0]) +
		(!pei_data.spd_addresses[3] && !pei_data.spd_data[3][0]) * 2;

	disable_p2p();

	timestamp_add_now(TS_INITRAM_START);
	sdram_initialize(&pei_data);
	timestamp_add_now(TS_INITRAM_END);

	/* Sanity check mrc_var location by verifying a known field */
	mrc_var = (void *)DCACHE_RAM_MRC_VAR_BASE;
	if (mrc_var->tx_byte == pei_data.tx_byte_ptr) {
		printk(BIOS_DEBUG, "MRC_VAR pool occupied [%08x,%08x]\n",
		       mrc_var->pool_base, mrc_var->pool_base + mrc_var->pool_used);

	} else {
		printk(BIOS_ERR, "Could not parse MRC_VAR data\n");
		hexdump(mrc_var, sizeof(*mrc_var));
	}

	const int cbmem_was_initted = !cbmem_recovery(s3resume);
	if (!s3resume)
		save_mrc_data(&pei_data);

	if (s3resume && !cbmem_was_initted) {
		/* Failed S3 resume, reset to come up cleanly */
		system_reset();
	}
	setup_sdram_meminfo(&pei_data);
}

static void setup_sdram_meminfo(struct pei_data *pei_data)
{
	u32 addr_decode_ch[2];
	struct memory_info *mem_info;
	struct dimm_info *dimm;
	int dimm_size;
	int i;
	int dimm_cnt = 0;

	mem_info = cbmem_add(CBMEM_ID_MEMINFO, sizeof(struct memory_info));
	memset(mem_info, 0, sizeof(struct memory_info));

	addr_decode_ch[0] = mchbar_read32(MAD_DIMM_CH0);
	addr_decode_ch[1] = mchbar_read32(MAD_DIMM_CH1);

	const int refclk = mchbar_read32(MC_BIOS_REQ) & 0x100 ? 100 : 133;
	const int ddr_frequency = (mchbar_read32(MC_BIOS_DATA) * refclk * 100 * 2 + 50) / 100;

	for (i = 0; i < ARRAY_SIZE(addr_decode_ch); i++) {
		u32 ch_conf = addr_decode_ch[i];

		/* DIMM-A */
		dimm_size = ((ch_conf >> 0) & 0xff) * 256;
		if (dimm_size) {
			dimm = &mem_info->dimm[dimm_cnt];
			dimm->dimm_size = dimm_size;
			dimm->ddr_type = MEMORY_TYPE_DDR3;
			dimm->ddr_frequency = ddr_frequency;
			dimm->rank_per_dimm = 1 + ((ch_conf >> 17) & 1);
			dimm->channel_num = i;
			dimm->dimm_num = 0;
			dimm->bank_locator = i * 2;
			memcpy(dimm->serial,				/* bytes 122-125 */
				&pei_data->spd_data[0][SPD_DDR3_SERIAL_NUM],
				sizeof(uint8_t) * SPD_DDR3_SERIAL_LEN);
			memcpy(dimm->module_part_number,		/* bytes 128-145 */
				&pei_data->spd_data[0][SPD_DDR3_PART_NUM],
				sizeof(uint8_t) * SPD_DDR3_PART_LEN);
			dimm->mod_id =					/* bytes 117/118 */
				(pei_data->spd_data[0][SPD_DDR3_MOD_ID2] << 8) |
				(pei_data->spd_data[0][SPD_DDR3_MOD_ID1] & 0xFF);
			dimm->mod_type = SPD_DDR3_DIMM_TYPE_SO_DIMM;
			dimm->bus_width = MEMORY_BUS_WIDTH_64;
			dimm_cnt++;
		}
		/* DIMM-B */
		dimm_size = ((ch_conf >> 8) & 0xff) * 256;
		if (dimm_size) {
			dimm = &mem_info->dimm[dimm_cnt];
			dimm->dimm_size = dimm_size;
			dimm->ddr_type = MEMORY_TYPE_DDR3;
			dimm->ddr_frequency = ddr_frequency;
			dimm->rank_per_dimm =  1 + ((ch_conf >> 18) & 1);
			dimm->channel_num = i;
			dimm->dimm_num = 1;
			dimm->bank_locator = i * 2;
			memcpy(dimm->serial,				/* bytes 122-125 */
				&pei_data->spd_data[0][SPD_DDR3_SERIAL_NUM],
				sizeof(uint8_t) * SPD_DDR3_SERIAL_LEN);
			memcpy(dimm->module_part_number,		/* bytes 128-145 */
				&pei_data->spd_data[0][SPD_DDR3_PART_NUM],
				sizeof(uint8_t) * SPD_DDR3_PART_LEN);
			dimm->mod_id =					/* bytes 117/118 */
				(pei_data->spd_data[0][SPD_DDR3_MOD_ID2] << 8) |
				(pei_data->spd_data[0][SPD_DDR3_MOD_ID1] & 0xFF);
			dimm->mod_type = SPD_DDR3_DIMM_TYPE_SO_DIMM;
			dimm->bus_width = MEMORY_BUS_WIDTH_64;
			dimm_cnt++;
		}
	}
	mem_info->dimm_cnt = dimm_cnt;
}
