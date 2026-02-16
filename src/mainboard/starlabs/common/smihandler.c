/* SPDX-License-Identifier: GPL-2.0-only */

#include <acpi/acpi_gnvs.h>
#include <commonlib/helpers.h>
#include <cpu/x86/smm.h>
#include <ec/starlabs/merlin/ec.h>
#if CONFIG(SOC_INTEL_COMMON_BLOCK_FAST_SPI)
#include <cpu/intel/msr.h>
#include <cpu/x86/msr.h>
#include <device/mmio.h>
#include <intelblocks/fast_spi.h>
#endif
#include <option.h>
#include <soc/nvs.h>
#include <starlabs/efi_option_smi.h>
#include <types.h>

#if CONFIG(SOC_INTEL_COMMON_BLOCK_FAST_SPI)
static void set_insmm_sts(const bool enable_writes)
{
	msr_t msr = {
		.lo = read32p(0xfed30880),
		.hi = 0,
	};

	if (enable_writes)
		msr.lo |= 1;
	else
		msr.lo &= ~1;

	wrmsr(MSR_SPCL_CHIPSET_USAGE, msr);
}
#endif

static bool chipset_disable_wp(void)
{
#if CONFIG(SOC_INTEL_COMMON_BLOCK_FAST_SPI)
	const bool wp_enabled = !fast_spi_wpd_status();

	if (wp_enabled) {
		set_insmm_sts(true);
		/*
		 * As per BWG, clearing "SPI_BIOS_CONTROL_SYNC_SS"
		 * bit is a must prior setting SPI_BIOS_CONTROL_WPD" bit
		 * to avoid 3-strike error.
		 */
		fast_spi_clear_sync_smi_status();
		fast_spi_disable_wp();
	}

	return wp_enabled;
#endif

	return false;
}

static void chipset_enable_wp(void)
{
#if CONFIG(SOC_INTEL_COMMON_BLOCK_FAST_SPI)
	fast_spi_enable_wp();
	set_insmm_sts(false);
#endif
}

static struct starlabs_dnvs_efiopt *get_starlabs_dnvs_efiopt(void)
{
	if (!gnvs)
		return NULL;

	const size_t gnvs_size = ALIGN_UP(sizeof(struct global_nvs), sizeof(uint64_t));

	uint8_t *base = (uint8_t *)gnvs;
	base += gnvs_size;
	return (struct starlabs_dnvs_efiopt *)base;
}

struct starlabs_efiopt_entry {
	const char *name;
	enum starlabs_efiopt_id id;
	uint32_t fallback;
};

static const struct starlabs_efiopt_entry *find_efiopt(enum starlabs_efiopt_id id)
{
	static const struct starlabs_efiopt_entry opts[] = {
		{
			.name = "fn_lock_state",
			.id = STARLABS_EFIOPT_ID_FN_LOCK_STATE,
			.fallback = LOCKED,
		},
		{
			.name = "trackpad_state",
			.id = STARLABS_EFIOPT_ID_TRACKPAD_STATE,
			.fallback = TRACKPAD_ENABLED,
		},
		{
			.name = "kbl_brightness",
			.id = STARLABS_EFIOPT_ID_KBL_BRIGHTNESS,
			.fallback = CONFIG(EC_STARLABS_KBL_LEVELS) ? KBL_LOW : KBL_ON,
		},
		{
			.name = "kbl_state",
			.id = STARLABS_EFIOPT_ID_KBL_STATE,
			.fallback = KBL_ENABLED,
		},
	};

	for (size_t i = 0; i < ARRAY_SIZE(opts); i++) {
		if (opts[i].id == id)
			return &opts[i];
	}

	return NULL;
}

static enum cb_err normalize_value(enum starlabs_efiopt_id id, uint32_t *value)
{
	if (!value)
		return CB_ERR_ARG;

	switch (id) {
	case STARLABS_EFIOPT_ID_FN_LOCK_STATE:
		if (*value == UNLOCKED || *value == LOCKED)
			return CB_SUCCESS;
		return CB_ERR_ARG;
	case STARLABS_EFIOPT_ID_TRACKPAD_STATE:
		/* Normalize "re-enabled" to "enabled". */
		if (*value == 0x11)
			*value = TRACKPAD_ENABLED;
		if (*value == TRACKPAD_ENABLED || *value == TRACKPAD_DISABLED)
			return CB_SUCCESS;
		return CB_ERR_ARG;
	case STARLABS_EFIOPT_ID_KBL_BRIGHTNESS:
		if (*value == KBL_ON || *value == KBL_OFF || *value == KBL_LOW ||
		    *value == KBL_HIGH)
			return CB_SUCCESS;
		return CB_ERR_ARG;
	case STARLABS_EFIOPT_ID_KBL_STATE:
		if (*value == KBL_DISABLED || *value == KBL_ENABLED)
			return CB_SUCCESS;
		return CB_ERR_ARG;
	default:
		return CB_ERR_ARG;
	}
}

int mainboard_smi_apmc(u8 data)
{
	if (data != STARLABS_APMC_CMD_EFI_OPTION)
		return 0;

	struct starlabs_dnvs_efiopt *dnvs = get_starlabs_dnvs_efiopt();
	if (!dnvs)
		return 0;

	const enum starlabs_efiopt_cmd cmd = dnvs->cmd;
	const enum starlabs_efiopt_id id = dnvs->id;
	const struct starlabs_efiopt_entry *opt = find_efiopt(id);

	if (!opt) {
		dnvs->status = CB_ERR_ARG;
		return 1;
	}

	switch (cmd) {
	case STARLABS_EFIOPT_CMD_GET:
		dnvs->value = get_uint_option(opt->name, opt->fallback);
		dnvs->status = CB_SUCCESS;
		break;
	case STARLABS_EFIOPT_CMD_SET: {
		uint32_t value = dnvs->value;
		dnvs->status = normalize_value(id, &value);
		if (dnvs->status != CB_SUCCESS)
			break;

		const bool wp_enabled = chipset_disable_wp();
		dnvs->status = set_uint_option(opt->name, value);
		if (wp_enabled)
			chipset_enable_wp();
		break;
	}
	default:
		dnvs->status = CB_ERR_ARG;
		break;
	}

	return 1;
}
