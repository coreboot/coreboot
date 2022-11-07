/* SPDX-License-Identifier: GPL-2.0-only */

#include <bootstate.h>
#include <intelblocks/cse.h>
#include <intelblocks/spi.h>
#include <console/console.h>
#include <soc/me.h>
#include <stdint.h>

/* Host Firmware Status Register 2 */
union me_hfsts2 {
	uint32_t data;
	struct {
		uint32_t nftp_load_failure	: 1;
		uint32_t icc_prog_status	: 2;
		uint32_t invoke_mebx		: 1;
		uint32_t cpu_replaced		: 1;
		uint32_t rsvd0			: 1;
		uint32_t mfs_failure		: 1;
		uint32_t warm_reset_rqst	: 1;
		uint32_t cpu_replaced_valid	: 1;
		uint32_t low_power_state	: 1;
		uint32_t me_power_gate		: 1;
		uint32_t ipu_needed		: 1;
		uint32_t forced_safe_boot	: 1;
		uint32_t rsvd1			: 1;
		uint32_t cse_to_be_disabled	: 1;
		uint32_t listener_change	: 1;
		uint32_t status_data		: 8;
		uint32_t current_pmevent	: 4;
		uint32_t phase			: 4;
	} __packed fields;
};

/* Host Firmware Status Register 4 */
union me_hfsts4 {
	uint32_t data;
	struct {
		uint32_t rsvd0			: 9;
		uint32_t enforcement_flow	: 1;
		uint32_t sx_resume_type		: 1;
		uint32_t rsvd1			: 1;
		uint32_t tpms_disconnected	: 1;
		uint32_t rvsd2			: 1;
		uint32_t fwsts_valid		: 1;
		uint32_t boot_guard_self_test	: 1;
		uint32_t rsvd3			: 16;
	} __packed fields;
};

/* Host Firmware Status Register 5 */
union me_hfsts5 {
	uint32_t data;
	struct {
		uint32_t acm_active		: 1;
		uint32_t valid			: 1;
		uint32_t result_code_source	: 1;
		uint32_t error_status_code	: 5;
		uint32_t acm_done_sts		: 1;
		uint32_t timeout_count		: 7;
		uint32_t scrtm_indicator	: 1;
		uint32_t rsvd0			: 14;
		uint32_t start_enforcement	: 1;
	} __packed fields;
};

/* Host Firmware Status Register 6 */
union me_hfsts6 {
	uint32_t data;
	struct {
		uint32_t force_boot_guard_acm	: 1;
		uint32_t cpu_debug_disable	: 1;
		uint32_t bsp_init_disable	: 1;
		uint32_t protect_bios_env	: 1;
		uint32_t rsvd0			: 2;
		uint32_t error_enforce_policy	: 2;
		uint32_t measured_boot		: 1;
		uint32_t verified_boot		: 1;
		uint32_t rsvd1			: 11;
		uint32_t manuf_lock		: 1;
		uint32_t key_manifest_id	: 4;
		uint32_t boot_policy_status	: 1;
		uint32_t error			: 1;
		uint32_t boot_guard_disable	: 1;
		uint32_t fpf_disable		: 1;
		uint32_t fpf_soc_lock		: 1;
		uint32_t txt_support		: 1;
	} __packed fields;
};

static void log_me_ro_write_protection_info(bool mfg_mode)
{
	bool cse_ro_wp_en = is_spi_wp_cse_ro_en();

	printk(BIOS_DEBUG, "ME: WP for RO is enabled        : %s\n",
			cse_ro_wp_en ? "YES" : "NO");

	if (cse_ro_wp_en) {
		uint32_t base, limit;
		spi_get_wp_cse_ro_range(&base, &limit);
		printk(BIOS_DEBUG, "ME: RO write protection scope - Start=0x%X, End=0x%X\n",
				base, limit);
	}

	/*
	 * If manufacturing mode is disabled, but CSE RO is not write protected,
	 * log error.
	 */
	if (!mfg_mode && !cse_ro_wp_en)
		printk(BIOS_ERR, "ME: Write protection for CSE RO is not enabled\n");
}

static bool is_manuf_mode(union me_hfsts1 hfsts1, union me_hfsts6 hfsts6)
{
	/*
	 * ME manufacturing mode is disabled if the descriptor is locked, fuses
	 * are programmed and manufacturing variables are locked.
	 */
	return !((hfsts1.fields.mfg_mode == 0) &&
		 (hfsts6.fields.fpf_soc_lock == 1) &&
		 (hfsts6.fields.manuf_lock == 1));
}

static void dump_me_status(void *unused)
{
	union me_hfsts1 hfsts1;
	union me_hfsts2 hfsts2;
	union me_hfsts3 hfsts3;
	union me_hfsts4 hfsts4;
	union me_hfsts5 hfsts5;
	union me_hfsts6 hfsts6;
	bool manuf_mode;

	if (!is_cse_enabled())
		return;

	hfsts1.data = me_read_config32(PCI_ME_HFSTS1);
	hfsts2.data = me_read_config32(PCI_ME_HFSTS2);
	hfsts3.data = me_read_config32(PCI_ME_HFSTS3);
	hfsts4.data = me_read_config32(PCI_ME_HFSTS4);
	hfsts5.data = me_read_config32(PCI_ME_HFSTS5);
	hfsts6.data = me_read_config32(PCI_ME_HFSTS6);

	printk(BIOS_DEBUG, "ME: HFSTS1                      : 0x%08X\n", hfsts1.data);
	printk(BIOS_DEBUG, "ME: HFSTS2                      : 0x%08X\n", hfsts2.data);
	printk(BIOS_DEBUG, "ME: HFSTS3                      : 0x%08X\n", hfsts3.data);
	printk(BIOS_DEBUG, "ME: HFSTS4                      : 0x%08X\n", hfsts4.data);
	printk(BIOS_DEBUG, "ME: HFSTS5                      : 0x%08X\n", hfsts5.data);
	printk(BIOS_DEBUG, "ME: HFSTS6                      : 0x%08X\n", hfsts6.data);

	manuf_mode = is_manuf_mode(hfsts1, hfsts6);
	printk(BIOS_DEBUG, "ME: Manufacturing Mode          : %s\n",
	       manuf_mode ? "YES" : "NO");
	/*
	 * The SPI Protection Mode bit reflects SPI descriptor
	 * locked(0) or unlocked(1).
	 */
	printk(BIOS_DEBUG, "ME: SPI Protection Mode Enabled : %s\n",
		hfsts1.fields.mfg_mode ? "NO" : "YES");
	printk(BIOS_DEBUG, "ME: FPFs Committed              : %s\n",
		hfsts6.fields.fpf_soc_lock ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Manufacturing Vars Locked   : %s\n",
		hfsts6.fields.manuf_lock ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: FW Partition Table          : %s\n",
		hfsts1.fields.fpt_bad ? "BAD" : "OK");
	printk(BIOS_DEBUG, "ME: Bringup Loader Failure      : %s\n",
		hfsts1.fields.ft_bup_ld_flr ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Firmware Init Complete      : %s\n",
		hfsts1.fields.fw_init_complete ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Boot Options Present        : %s\n",
		hfsts1.fields.boot_options_present ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Update In Progress          : %s\n",
		hfsts1.fields.update_in_progress ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: D0i3 Support                : %s\n",
		hfsts1.fields.d0i3_support_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Low Power State Enabled     : %s\n",
		hfsts2.fields.low_power_state ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Replaced                : %s\n",
		hfsts2.fields.cpu_replaced  ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Replacement Valid       : %s\n",
		hfsts2.fields.cpu_replaced_valid ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: Current Working State       : %u\n",
		hfsts1.fields.working_state);
	printk(BIOS_DEBUG, "ME: Current Operation State     : %u\n",
		hfsts1.fields.operation_state);
	printk(BIOS_DEBUG, "ME: Current Operation Mode      : %u\n",
		hfsts1.fields.operation_mode);
	printk(BIOS_DEBUG, "ME: Error Code                  : %u\n",
		hfsts1.fields.error_code);
	printk(BIOS_DEBUG, "ME: Enhanced Debug Mode         : %s\n",
		hfsts1.fields.invoke_enhance_dbg_mode ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: CPU Debug Disabled          : %s\n",
		hfsts6.fields.cpu_debug_disable ? "YES" : "NO");
	printk(BIOS_DEBUG, "ME: TXT Support                 : %s\n",
		hfsts6.fields.txt_support ? "YES" : "NO");

	if (CONFIG(SOC_INTEL_CSE_LITE_SKU))
		log_me_ro_write_protection_info(manuf_mode);
}

BOOT_STATE_INIT_ENTRY(BS_DEV_ENABLE, BS_ON_EXIT, print_me_fw_version, NULL);
BOOT_STATE_INIT_ENTRY(BS_OS_RESUME_CHECK, BS_ON_EXIT, dump_me_status, NULL);
