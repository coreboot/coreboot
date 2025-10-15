/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <assert.h>
#include <console/console.h>
#include <northbridge/intel/haswell/haswell.h>
#include <types.h>

#include "raminit_native.h"

uint32_t reg_frame_rev(void)
{
	/*
	 * Equivalent to MRC_CACHE_REVISION, but hidden via abstraction.
	 * The structures that get saved to flash are contained within
	 * this translation unit, so changes outside this file shouldn't
	 * require invalidating the cache.
	 */
	return 1;
}

struct register_save {
	uint16_t lower;
	uint16_t upper;
};

/** TODO: Haswell DDRIO aliases writes: 0x80 .. 0xff => 0x00 .. 0x7f **/
static const struct register_save ddrio_per_byte_list[] = {
	{0x0000, 0x003c}, /* 16 registers */
//	{0x0048, 0x0084}, /* 16 registers */	/** TODO: BDW support **/
	{0x0048, 0x004c}, /*  2 registers */
	{0x005c, 0x0078}, /*  8 registers */
};
#define DDRIO_PER_BYTE_REGISTER_COUNT	(16 + 2 + 8)

static const struct register_save ddrio_per_ch_list[] = {
	/* CKE */
	{0x1204, 0x1208}, /* 2 registers */
	{0x1214, 0x121c}, /* 3 registers */
	/* CMD North */
	{0x1404, 0x140c}, /* 3 registers */
	/* CLK */
	{0x1808, 0x1810}, /* 3 registers */
	/* CMD South */
	{0x1a04, 0x1a0c}, /* 3 registers */
	/* CTL */
	{0x1c14, 0x1c1c}, /* 3 registers */
};
#define DDRIO_PER_CH_REGISTER_COUNT	(2 + 3 * 5)

static const struct register_save ddrio_common_list[] = {
	{0x2000, 0x2008}, /* 3 registers */
	{0x3a14, 0x3a1c}, /* 3 registers */
	{0x3a24, 0x3a24}, /* 1 registers */
};

#define DDRIO_COMMON_REGISTER_COUNT	(3 + 3 + 1)

static const struct register_save mcmain_per_ch_list[] = {
	{0x4000, 0x4014}, /* 6 registers */
	{0x4024, 0x4028}, /* 2 registers */
	{0x40d0, 0x40d0}, /* 1 registers */
	{0x4220, 0x4224}, /* 2 registers */
	{0x4294, 0x4294}, /* 1 registers */
	{0x429c, 0x42a0}, /* 2 registers */
	{0x42ec, 0x42fc}, /* 5 registers */
	{0x4328, 0x4328}, /* 1 registers */
	{0x438c, 0x4390}, /* 2 registers */
};
#define MCMAIN_PER_CH_REGISTER_COUNT	(6 + 2 + 1 + 2 + 1 + 2 + 5 + 1 + 2)

static const struct register_save misc_common_list[] = {
	{0x5884, 0x5888}, /*  2 registers */
	{0x5890, 0x589c}, /*  4 registers */
	{0x58a4, 0x58a4}, /*  1 registers */
	{0x58d0, 0x58e4}, /*  6 registers */
	{0x5880, 0x5880}, /*  1 registers */
	{0x5000, 0x50dc}, /* 56 registers */
	{0x59b8, 0x59b8}  /*  1 registers */
};
#define MISC_COMMON_REGISTER_COUNT	(2 + 4 + 1 + 6 + 1 + 56 + 1)

struct save_params {
	bool is_initialised;

	/* Memory base frequency, either 100 or 133 MHz */
	uint8_t base_freq;

	/* Multiplier */
	uint32_t multiplier;

	/* Memory clock in MHz */
	uint32_t mem_clock_mhz;

	/* Memory clock in femtoseconds */
	uint32_t mem_clock_fs;

	/* Quadrature clock in picoseconds */
	uint16_t qclkps;

	/* Bitfield of supported CAS latencies */
	uint16_t cas_supported;

	/* CPUID value */
	uint32_t cpu;

	/* Cached CPU stepping value */
	uint8_t stepping;

	uint16_t vdd_mv;

	union dimm_flags_ddr3_st flags;

	/* Except for tCK, everything is stored in DCLKs */
	uint32_t tCK;
	uint32_t tAA;
	uint32_t tWR;
	uint32_t tRCD;
	uint32_t tRRD;
	uint32_t tRP;
	uint32_t tRAS;
	uint32_t tRC;
	uint32_t tRFC;
	uint32_t tWTR;
	uint32_t tRTP;
	uint32_t tFAW;
	uint32_t tCWL;
	uint32_t tCMD;

	uint32_t tREFI;
	uint32_t tXP;

	uint8_t lpddr_cke_rank_map[NUM_CHANNELS];

	struct raminit_dimm_info dimms[NUM_CHANNELS][NUM_SLOTS];

	uint8_t chanmap;

	uint32_t channel_size_mb[NUM_CHANNELS];

	/* DIMMs per channel */
	uint8_t dpc[NUM_CHANNELS];

	uint8_t rankmap[NUM_CHANNELS];

	/* Whether a rank is mirrored or not (only rank 1 of each DIMM can be) */
	uint8_t rank_mirrored[NUM_CHANNELS];

	/*
	 * FIXME: LPDDR support is incomplete. The largest chunks are missing,
	 * but some LPDDR-specific variations in algorithms have been handled.
	 * LPDDR-specific functions have stubs which will halt upon execution.
	 */
	bool lpddr;

	uint8_t lanes;

	/* FIXME: ECC support missing */
	bool is_ecc;
};

struct register_save_frame {
	uint32_t ddrio_per_byte[NUM_CHANNELS][NUM_LANES][DDRIO_PER_BYTE_REGISTER_COUNT];
	uint32_t ddrio_per_ch[NUM_CHANNELS][DDRIO_PER_CH_REGISTER_COUNT];
	uint32_t ddrio_common[DDRIO_COMMON_REGISTER_COUNT];
	uint32_t mcmain_per_ch[NUM_CHANNELS][MCMAIN_PER_CH_REGISTER_COUNT];
	uint32_t misc_common[MISC_COMMON_REGISTER_COUNT];
	struct save_params params;
};

struct register_save_frame *reg_frame_ptr(void)
{
	/* The chonky register save frame struct, used for fast boot and S3 resume */
	static struct register_save_frame register_frame = { 0 };
	return &register_frame;
}

size_t reg_frame_size(void)
{
	return sizeof(struct register_save_frame);
}

void reg_frame_get_spd_data(const uint8_t *out_spd_data[NUM_CHANNELS][NUM_SLOTS])
{
	const struct save_params *params = &reg_frame_ptr()->params;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++) {
			out_spd_data[channel][slot] = params->dimms[channel][slot].raw_spd;
		}
	}
}

typedef void (*reg_func_t)(const uint16_t offset, uint32_t *const value);

static void save_value(const uint16_t offset, uint32_t *const value)
{
	*value = mchbar_read32(offset);
}

static void restore_value(const uint16_t offset, uint32_t *const value)
{
	mchbar_write32(offset, *value);
}

static void save_restore(
	uint32_t *reg_frame,
	const uint16_t g_offset,
	const struct register_save *reg_save_list,
	const size_t reg_save_length,
	reg_func_t handle_reg)
{
	for (size_t i = 0; i < reg_save_length; i++) {
		const struct register_save *entry = &reg_save_list[i];
		for (uint16_t offset = entry->lower; offset <= entry->upper; offset += 4) {
			handle_reg(offset + g_offset, reg_frame++);
		}
	}
}

static void save_restore_all(struct register_save_frame *reg_frame, reg_func_t handle_reg)
{
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		for (uint8_t byte = 0; byte < NUM_LANES; byte++) {
			const uint16_t g_offset = _DDRIO_C_R_B(0, channel, 0, byte);
			save_restore(
				reg_frame->ddrio_per_byte[channel][byte],
				g_offset,
				ddrio_per_byte_list,
				ARRAY_SIZE(ddrio_per_byte_list),
				handle_reg);
		}
	}
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		const uint16_t g_offset = _DDRIO_C_R_B(0, channel, 0, 0);
		save_restore(
			reg_frame->ddrio_per_ch[channel],
			g_offset,
			ddrio_per_ch_list,
			ARRAY_SIZE(ddrio_per_ch_list),
			handle_reg);
	}
	save_restore(
		reg_frame->ddrio_common,
		0,
		ddrio_common_list,
		ARRAY_SIZE(ddrio_common_list),
		handle_reg);

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		const uint16_t g_offset = _MCMAIN_C(0, channel);
		save_restore(
			reg_frame->mcmain_per_ch[channel],
			g_offset,
			mcmain_per_ch_list,
			ARRAY_SIZE(mcmain_per_ch_list),
			handle_reg);
	}
	save_restore(
		reg_frame->misc_common,
		0,
		misc_common_list,
		ARRAY_SIZE(misc_common_list),
		handle_reg);
}

enum raminit_status save_training_values(struct sysinfo *ctrl)
{
	save_restore_all(reg_frame_ptr(), save_value);
	return RAMINIT_STATUS_SUCCESS;
}

enum raminit_status restore_training_values(struct sysinfo *ctrl)
{
	save_restore_all(reg_frame_ptr(), restore_value);
	return RAMINIT_STATUS_SUCCESS;
}

enum raminit_status save_non_training(struct sysinfo *ctrl)
{
	struct register_save_frame *reg_frame = reg_frame_ptr();
	struct save_params *params = &reg_frame->params;

	params->is_initialised = true;

	params->base_freq     = ctrl->base_freq;
	params->multiplier    = ctrl->multiplier;
	params->mem_clock_mhz = ctrl->mem_clock_mhz;
	params->mem_clock_fs  = ctrl->mem_clock_fs;
	params->qclkps        = ctrl->qclkps;
	params->cas_supported = ctrl->cas_supported;
	params->cpu           = ctrl->cpu;
	params->stepping      = ctrl->stepping;
	params->vdd_mv        = ctrl->vdd_mv;
	params->flags         = ctrl->flags;

	params->tCK       = ctrl->tCK;
	params->tAA       = ctrl->tAA;
	params->tWR       = ctrl->tWR;
	params->tRCD      = ctrl->tRCD;
	params->tRRD      = ctrl->tRRD;
	params->tRP       = ctrl->tRP;
	params->tRAS      = ctrl->tRAS;
	params->tRC       = ctrl->tRC;
	params->tRFC      = ctrl->tRFC;
	params->tWTR      = ctrl->tWTR;
	params->tRTP      = ctrl->tRTP;
	params->tFAW      = ctrl->tFAW;
	params->tCWL      = ctrl->tCWL;
	params->tCMD      = ctrl->tCMD;
	params->tREFI     = ctrl->tREFI;
	params->tXP       = ctrl->tXP;

	params->chanmap = ctrl->chanmap;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		params->lpddr_cke_rank_map[channel] = ctrl->lpddr_cke_rank_map[channel];
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++)
			params->dimms[channel][slot] = ctrl->dimms[channel][slot];
	}
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		params->dpc[channel] = ctrl->dpc[channel];
		params->rankmap[channel] = ctrl->rankmap[channel];
		params->rank_mirrored[channel] = ctrl->rank_mirrored[channel];
		params->channel_size_mb[channel] = ctrl->channel_size_mb[channel];
	}
	params->lpddr = ctrl->lpddr;
	params->lanes = ctrl->lanes;
	params->is_ecc = ctrl->is_ecc;
	return RAMINIT_STATUS_SUCCESS;
}

#define RAMINIT_COMPARE(_s1, _s2) \
	((sizeof(_s1) == sizeof(_s2)) && !memcmp(_s1, _s2, sizeof(_s1)))

enum raminit_status restore_non_training(struct sysinfo *ctrl)
{
	struct register_save_frame *reg_frame = reg_frame_ptr();
	struct save_params *params = &reg_frame->params;

	if (!params->is_initialised) {
		printk(BIOS_WARNING, "Cannot fast boot: saved data is invalid\n");
		return RAMINIT_STATUS_INVALID_CACHE;
	}

	if (!RAMINIT_COMPARE(ctrl->dimms, params->dimms)) {
		printk(BIOS_WARNING, "Cannot fast boot: DIMMs have changed\n");
		return RAMINIT_STATUS_INVALID_CACHE;
	}

	if (ctrl->cpu != params->cpu) {
		printk(BIOS_WARNING, "Cannot fast boot: CPU has changed\n");
		return RAMINIT_STATUS_INVALID_CACHE;
	}

	ctrl->base_freq     = params->base_freq;
	ctrl->multiplier    = params->multiplier;
	ctrl->mem_clock_mhz = params->mem_clock_mhz;
	ctrl->mem_clock_fs  = params->mem_clock_fs;
	ctrl->qclkps        = params->qclkps;
	ctrl->cas_supported = params->cas_supported;
	ctrl->cpu           = params->cpu;
	ctrl->stepping      = params->stepping;
	ctrl->vdd_mv        = params->vdd_mv;
	ctrl->flags         = params->flags;

	ctrl->tCK       = params->tCK;
	ctrl->tAA       = params->tAA;
	ctrl->tWR       = params->tWR;
	ctrl->tRCD      = params->tRCD;
	ctrl->tRRD      = params->tRRD;
	ctrl->tRP       = params->tRP;
	ctrl->tRAS      = params->tRAS;
	ctrl->tRC       = params->tRC;
	ctrl->tRFC      = params->tRFC;
	ctrl->tWTR      = params->tWTR;
	ctrl->tRTP      = params->tRTP;
	ctrl->tFAW      = params->tFAW;
	ctrl->tCWL      = params->tCWL;
	ctrl->tCMD      = params->tCMD;
	ctrl->tREFI     = params->tREFI;
	ctrl->tXP       = params->tXP;

	ctrl->chanmap = params->chanmap;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		ctrl->lpddr_cke_rank_map[channel] = params->lpddr_cke_rank_map[channel];
		for (uint8_t slot = 0; slot < NUM_SLOTS; slot++)
			ctrl->dimms[channel][slot] = params->dimms[channel][slot];
	}
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		ctrl->dpc[channel] = params->dpc[channel];
		ctrl->rankmap[channel] = params->rankmap[channel];
		ctrl->rank_mirrored[channel] = params->rank_mirrored[channel];
		ctrl->channel_size_mb[channel] = params->channel_size_mb[channel];
	}
	ctrl->lpddr = params->lpddr;
	ctrl->lanes = params->lanes;
	ctrl->is_ecc = params->is_ecc;
	return RAMINIT_STATUS_SUCCESS;
}
