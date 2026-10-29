/* SPDX-License-Identifier: GPL-2.0-only */

#include <commonlib/rational.h>
#include <console/console.h>
#include <device/mmio.h>
#include <types.h>
#include <string.h>
#include <edid.h>
#include <soc/display/edp_reg.h>
#include <soc/display/edp_aux.h>
#include <soc/display/edp_link_train.h>
#include <soc/display/mdssreg.h>

#define DP_TU_FP_EDGE        0x40000
#define DRM_FIXED_POINT      32
#define DRM_FIXED_ONE        (1ULL << DRM_FIXED_POINT)
#define DRM_FIXED_EPSILON    1LL
#define DRM_FIXED_ALMOST_ONE (DRM_FIXED_ONE - DRM_FIXED_EPSILON)

static inline s64 div64_s64(s64 dividend, s64 divisor)
{
	return dividend / divisor;
}

static inline uint32_t drm_fixp_msbset(s64 a)
{
	uint32_t shift, sign = (a >> 63) & 1;

	for (shift = 62; shift > 0; --shift)
		if (((a >> shift) & 1) != sign)
			return shift;

	return 0;
}

static inline s64 drm_fixp_div(s64 a, s64 b)
{
	uint32_t shift = 62 - drm_fixp_msbset(a);
	s64 result;

	a <<= shift;

	if (shift < DRM_FIXED_POINT)
		b >>= (DRM_FIXED_POINT - shift);

	result = div64_s64(a, b);

	if (shift > DRM_FIXED_POINT)
		return result >> (shift - DRM_FIXED_POINT);

	return result;
}

static inline int drm_fixp2int(s64 a)
{
	return ((s64)a) >> DRM_FIXED_POINT;
}

static inline int drm_fixp2int_ceil(s64 a)
{
	if (a >= 0)
		return drm_fixp2int(a + DRM_FIXED_ALMOST_ONE);
	else
		return drm_fixp2int(a - DRM_FIXED_ALMOST_ONE);
}

static inline u64 div64_u64_rem(u64 dividend, u64 divisor, u64 *remainder)
{
	*remainder = dividend % divisor;
	return dividend / divisor;
}

static inline s64 drm_fixp_mul(s64 a, s64 b)
{
	uint32_t shift = drm_fixp_msbset(a) + drm_fixp_msbset(b);
	s64 result;

	if (shift > 61) {
		shift = shift - 61;
		a >>= (shift >> 1) + (shift & 1);
		b >>= shift >> 1;
	} else
		shift = 0;

	result = a * b;

	if (shift > DRM_FIXED_POINT)
		return result << (shift - DRM_FIXED_POINT);

	if (shift < DRM_FIXED_POINT)
		return result >> (DRM_FIXED_POINT - shift);

	return result;
}

static inline s64 drm_fixp_from_fraction(s64 a, s64 b)
{
	s64 res;
	bool a_neg = a < 0;
	bool b_neg = b < 0;
	u64 a_abs = a_neg ? -a : a;
	u64 b_abs = b_neg ? -b : b;
	u64 rem;

	/* determine integer part */
	u64 res_abs = div64_u64_rem(a_abs, b_abs, &rem);

	/* determine fractional part */
	{
		u32 i = DRM_FIXED_POINT;

		do {
			rem <<= 1;
			res_abs <<= 1;
			if (rem >= b_abs) {
				res_abs |= 1;
				rem -= b_abs;
			}
		} while (--i != 0);
	}

	/* round up LSB */
	{
		u64 summand = (rem << 1) >= b_abs;

		res_abs += summand;
	}

	res = (s64)res_abs;
	if (a_neg ^ b_neg)
		res = -res;
	return res;
}

static int _tu_param_compare(s64 a, s64 b)
{
	u32 a_int, a_frac, a_sign;
	u32 b_int, b_frac, b_sign;
	s64 a_temp, b_temp, minus_1;

	if (a == b)
		return 0;

	minus_1 = drm_fixp_from_fraction(-1, 1);

	a_int = (a >> 32) & 0x7FFFFFFF;
	a_frac = a & 0xFFFFFFFF;
	a_sign = (a >> 32) & 0x80000000 ? 1 : 0;

	b_int = (b >> 32) & 0x7FFFFFFF;
	b_frac = b & 0xFFFFFFFF;
	b_sign = (b >> 32) & 0x80000000 ? 1 : 0;

	if (a_sign > b_sign)
		return 2;
	else if (b_sign > a_sign)
		return 1;

	if (!a_sign && !b_sign) { /* positive */
		if (a > b)
			return 1;
		else
			return 2;
	} else { /* negative */
		a_temp = drm_fixp_mul(a, minus_1);
		b_temp = drm_fixp_mul(b, minus_1);

		if (a_temp > b_temp)
			return 2;
		else
			return 1;
	}
	printk(BIOS_INFO, "%x, %x, %x\n", b_int, b_frac, b_sign);
	printk(BIOS_INFO, "%x, %x, %x\n", a_int, a_frac, a_sign);
}

static s64 fixp_subtract(s64 a, s64 b)
{
	s64 minus_1 = drm_fixp_from_fraction(-1, 1);

	if (a >= b)
		return a - b;

	return drm_fixp_mul(b - a, minus_1);
}

static inline int fixp2int_ceil(s64 a)
{
	return a ? drm_fixp2int_ceil(a) : 0;
}

static void _tu_valid_boundary_calc(struct tu_algo_data *tu)
{
	s64 temp1_fp, temp2_fp;
	int temp, temp1, temp2;
	int compare_result_1, compare_result_2, compare_result_3;

	temp1_fp = drm_fixp_from_fraction(tu->tu_size, 1);
	temp2_fp = drm_fixp_mul(tu->ratio_fp, temp1_fp);

	tu->new_valid_boundary_link = fixp2int_ceil(temp2_fp);

	temp = (tu->i_upper_boundary_count * tu->new_valid_boundary_link +
		tu->i_lower_boundary_count * (tu->new_valid_boundary_link - 1));
	temp1 = tu->i_upper_boundary_count + tu->i_lower_boundary_count;
	tu->average_valid2_fp = drm_fixp_from_fraction(temp, temp1);

	temp1_fp = drm_fixp_from_fraction(tu->bpp, 8);
	temp2_fp = tu->lwidth_fp;
	temp1_fp = drm_fixp_mul(temp2_fp, temp1_fp);
	temp2_fp = drm_fixp_div(temp1_fp, tu->average_valid2_fp);
	tu->n_tus = drm_fixp2int(temp2_fp);

	if ((uint32_t)temp2_fp > 0xFFFFF000)
		tu->n_tus += 1;

	temp1_fp = drm_fixp_from_fraction(tu->n_tus, 1);
	temp2_fp = drm_fixp_mul(temp1_fp, tu->average_valid2_fp);
	temp1_fp = drm_fixp_from_fraction(tu->n_symbols, 1);
	temp2_fp = temp1_fp - temp2_fp;
	temp1_fp = drm_fixp_from_fraction(tu->nlanes, 1);
	temp2_fp = drm_fixp_div(temp2_fp, temp1_fp);
	tu->n_remainder_symbols_per_lane_fp = temp2_fp;

	temp1_fp = drm_fixp_from_fraction(tu->tu_size, 1);
	tu->last_partial_tu_fp = drm_fixp_div(tu->n_remainder_symbols_per_lane_fp, temp1_fp);

	if (tu->n_remainder_symbols_per_lane_fp != 0)
		tu->remainder_symbols_exist = 1;
	else
		tu->remainder_symbols_exist = 0;

	temp1_fp = drm_fixp_from_fraction(tu->n_tus, tu->nlanes);
	tu->n_tus_per_lane = drm_fixp2int(temp1_fp);

	tu->paired_tus = (int)((tu->n_tus_per_lane) / temp1);

	tu->remainder_tus = tu->n_tus_per_lane - tu->paired_tus * temp1;

	if ((tu->remainder_tus - tu->i_upper_boundary_count) > 0) {
		tu->remainder_tus_upper = tu->i_upper_boundary_count;
		tu->remainder_tus_lower = tu->remainder_tus - tu->i_upper_boundary_count;
	} else {
		tu->remainder_tus_upper = tu->remainder_tus;
		tu->remainder_tus_lower = 0;
	}

	temp = tu->paired_tus *
		       (tu->i_upper_boundary_count * tu->new_valid_boundary_link +
			tu->i_lower_boundary_count * (tu->new_valid_boundary_link - 1)) +
	       (tu->remainder_tus_upper * tu->new_valid_boundary_link) +
	       (tu->remainder_tus_lower * (tu->new_valid_boundary_link - 1));
	tu->total_valid_fp = drm_fixp_from_fraction(temp, 1);

	if (tu->remainder_symbols_exist) {
		temp1_fp = tu->total_valid_fp + tu->n_remainder_symbols_per_lane_fp;
		temp2_fp = drm_fixp_from_fraction(tu->n_tus_per_lane, 1);
		temp2_fp = temp2_fp + tu->last_partial_tu_fp;
		temp1_fp = drm_fixp_div(temp1_fp, temp2_fp);
	} else {
		temp2_fp = drm_fixp_from_fraction(tu->n_tus_per_lane, 1);
		temp1_fp = drm_fixp_div(tu->total_valid_fp, temp2_fp);
	}
	tu->effective_valid_fp = temp1_fp;

	temp1_fp = drm_fixp_from_fraction(tu->tu_size, 1);
	temp2_fp = drm_fixp_mul(tu->ratio_fp, temp1_fp);
	tu->n_n_err_fp = fixp_subtract(tu->effective_valid_fp, temp2_fp);

	temp1_fp = drm_fixp_from_fraction(tu->tu_size, 1);
	temp2_fp = drm_fixp_mul(tu->ratio_fp, temp1_fp);
	tu->n_err_fp = fixp_subtract(tu->average_valid2_fp, temp2_fp);

	tu->even_distribution = tu->n_tus % tu->nlanes == 0 ? 1 : 0;

	temp1_fp = drm_fixp_from_fraction(tu->bpp, 8);
	temp2_fp = tu->lwidth_fp;
	temp1_fp = drm_fixp_mul(temp2_fp, temp1_fp);
	temp2_fp = drm_fixp_div(temp1_fp, tu->average_valid2_fp);
	tu->n_tus_incl_last_incomplete_tu = fixp2int_ceil(temp2_fp);

	temp1_fp = drm_fixp_from_fraction(tu->tu_size, 1);
	temp2_fp = drm_fixp_mul(tu->original_ratio_fp, temp1_fp);
	temp1_fp = fixp_subtract(tu->average_valid2_fp, temp2_fp);
	temp2_fp = drm_fixp_from_fraction(tu->n_tus_incl_last_incomplete_tu, 1);
	temp1_fp = drm_fixp_mul(temp2_fp, temp1_fp);
	temp1 = fixp2int_ceil(temp1_fp);

	if ((uint32_t)temp1_fp < DP_TU_FP_EDGE)
		temp1 = drm_fixp2int(temp1_fp);

	temp = tu->i_upper_boundary_count * tu->nlanes;
	temp1_fp = drm_fixp_from_fraction(tu->tu_size, 1);
	temp2_fp = drm_fixp_mul(tu->original_ratio_fp, temp1_fp);
	temp1_fp = drm_fixp_from_fraction(tu->new_valid_boundary_link, 1);
	temp2_fp = fixp_subtract(temp1_fp, temp2_fp);
	temp1_fp = drm_fixp_from_fraction(temp, 1);
	temp2_fp = drm_fixp_mul(temp1_fp, temp2_fp);
	temp2 = fixp2int_ceil(temp2_fp);

	if ((uint32_t)temp2_fp < DP_TU_FP_EDGE)
		temp2 = drm_fixp2int(temp2_fp);

	tu->extra_required_bytes_new_tmp = temp1 + temp2;

	temp = tu->extra_required_bytes_new_tmp * 8;
	temp1_fp = drm_fixp_div(temp, tu->bpp);
	tu->extra_pclk_cycles_tmp = fixp2int_ceil(temp1_fp);

	temp1_fp = drm_fixp_from_fraction(tu->extra_pclk_cycles_tmp, 1);
	temp2_fp = drm_fixp_mul(temp1_fp, tu->lclk_fp);

	temp1_fp = drm_fixp_div(temp2_fp, tu->pclk_fp);
	temp = fixp2int_ceil(temp1_fp);

	if ((uint32_t)temp1_fp < DP_TU_FP_EDGE)
		temp = drm_fixp2int(temp1_fp);

	tu->extra_pclk_cycles_in_link_clk_tmp = temp;
	tu->filler_size_tmp = tu->tu_size - tu->new_valid_boundary_link;

	tu->lower_filler_size_tmp = tu->filler_size_tmp + 1;

	tu->delay_start_link_tmp = tu->extra_pclk_cycles_in_link_clk_tmp +
				   tu->lower_filler_size_tmp + tu->extra_buffer_margin;

	temp1_fp = drm_fixp_from_fraction(tu->delay_start_link_tmp, 1);
	tu->delay_start_time_fp = drm_fixp_div(temp1_fp, tu->lclk_fp);

	if (tu->rb2) {
		temp1_fp = drm_fixp_mul(tu->delay_start_time_fp, tu->lclk_fp);
		tu->delay_start_link_lclk = fixp2int_ceil(temp1_fp);

		if (tu->remainder_tus > tu->i_upper_boundary_count) {
			temp = (tu->remainder_tus - tu->i_upper_boundary_count) *
			       (tu->new_valid_boundary_link - 1);
			temp += (tu->i_upper_boundary_count * tu->new_valid_boundary_link);
			temp *= tu->nlanes;
		} else {
			temp = tu->nlanes * tu->remainder_tus * tu->new_valid_boundary_link;
		}

		temp1 = tu->i_lower_boundary_count * (tu->new_valid_boundary_link - 1);
		temp1 += tu->i_upper_boundary_count * tu->new_valid_boundary_link;
		temp1 *= tu->paired_tus * tu->nlanes;
		temp1_fp = drm_fixp_from_fraction(tu->n_symbols - temp1 - temp, tu->nlanes);
		tu->last_partial_lclk = fixp2int_ceil(temp1_fp);

		tu->tu_active_cycles =
			(int)((tu->n_tus_per_lane * tu->tu_size) + tu->last_partial_lclk);
		tu->post_tu_hw_pipe_delay = 4 /*BS_on_the_link*/ + 1 /*BE_next_ren*/;
		temp = tu->pre_tu_hw_pipe_delay + tu->delay_start_link_lclk +
		       tu->tu_active_cycles + tu->post_tu_hw_pipe_delay;

		if (tu->fec_en == 1) {
			if (tu->nlanes == 1) {
				temp1_fp = drm_fixp_from_fraction(temp, 500);
				tu->parity_symbols = fixp2int_ceil(temp1_fp) * 12 + 1;
			} else {
				temp1_fp = drm_fixp_from_fraction(temp, 250);
				tu->parity_symbols = fixp2int_ceil(temp1_fp) * 6 + 1;
			}
		} else { /* no fec BW impact */
			tu->parity_symbols = 0;
		}

		tu->link_config_hactive_time = temp + tu->parity_symbols;

		if (tu->resolution_line_time >= tu->link_config_hactive_time + 1 /*margin*/)
			tu->hbp_delayStartCheck = 1;
		else
			tu->hbp_delayStartCheck = 0;
	} else {
		compare_result_3 = _tu_param_compare(tu->hbp_time_fp, tu->delay_start_time_fp);
		if (compare_result_3 < 2)
			tu->hbp_delayStartCheck = 1;
		else
			tu->hbp_delayStartCheck = 0;
	}

	compare_result_1 = _tu_param_compare(tu->n_n_err_fp, tu->diff_abs_fp);
	if (compare_result_1 == 2)
		compare_result_1 = 1;
	else
		compare_result_1 = 0;

	compare_result_2 = _tu_param_compare(tu->n_n_err_fp, tu->err_fp);
	if (compare_result_2 == 2)
		compare_result_2 = 1;
	else
		compare_result_2 = 0;

	if (((tu->even_distribution == 1) ||
	     ((tu->even_distribution_BF == 0) && (tu->even_distribution_legacy == 0))) &&
	    tu->n_err_fp >= 0 && tu->n_n_err_fp >= 0 && compare_result_2 &&
	    (compare_result_1 || (tu->min_hblank_violated == 1)) &&
	    (tu->new_valid_boundary_link - 1) > 0 && (tu->hbp_delayStartCheck == 1) &&
	    (tu->delay_start_link_tmp <= 1023)) {
		tu->upper_boundary_count = tu->i_upper_boundary_count;
		tu->lower_boundary_count = tu->i_lower_boundary_count;
		tu->err_fp = tu->n_n_err_fp;
		tu->boundary_moderation_en = true;
		tu->tu_size_desired = tu->tu_size;
		tu->valid_boundary_link = tu->new_valid_boundary_link;
		tu->effective_valid_recorded_fp = tu->effective_valid_fp;
		tu->even_distribution_BF = 1;
		tu->delay_start_link = tu->delay_start_link_tmp;
	} else if (tu->boundary_mod_lower_err == 0) {
		compare_result_1 = _tu_param_compare(tu->n_n_err_fp, tu->diff_abs_fp);
		if (compare_result_1 == 2)
			tu->boundary_mod_lower_err = 1;
	}
}

static void _dp_calc_boundary(struct tu_algo_data *tu)
{
	s64 temp1_fp = 0, temp2_fp = 0;

	do {
		tu->err_fp = drm_fixp_from_fraction(1000, 1);

		temp1_fp = drm_fixp_div(tu->lclk_fp, tu->pclk_fp);
		temp2_fp = drm_fixp_from_fraction(tu->delay_start_link_extra_pixclk, 1);
		temp1_fp = drm_fixp_mul(temp2_fp, temp1_fp);
		tu->extra_buffer_margin = fixp2int_ceil(temp1_fp);

		temp1_fp = drm_fixp_from_fraction(tu->bpp, 8);
		temp1_fp = drm_fixp_mul(tu->lwidth_fp, temp1_fp);
		tu->n_symbols = fixp2int_ceil(temp1_fp);

		for (tu->tu_size = 32; tu->tu_size <= 64; tu->tu_size++) {
			for (tu->i_upper_boundary_count = 1; tu->i_upper_boundary_count <= 15;
			     tu->i_upper_boundary_count++) {
				for (tu->i_lower_boundary_count = 1;
				     tu->i_lower_boundary_count <= 15;
				     tu->i_lower_boundary_count++) {
					_tu_valid_boundary_calc(tu);
				}
			}
		}
		tu->delay_start_link_extra_pixclk--;
	} while (!tu->boundary_moderation_en && tu->boundary_mod_lower_err == 1 &&
		 tu->delay_start_link_extra_pixclk != 0 &&
		 ((tu->second_loop_set == 0 && tu->rb2 == 1) || tu->rb2 == 0));
}

static void _dp_calc_extra_bytes(struct tu_algo_data *tu)
{
	s64 temp1_fp = 0, temp2_fp = 0;

	temp1_fp = drm_fixp_from_fraction(tu->tu_size_desired, 1);
	temp2_fp = drm_fixp_mul(tu->original_ratio_fp, temp1_fp);
	temp1_fp = drm_fixp_from_fraction(tu->valid_boundary_link, 1);

	temp2_fp = fixp_subtract(temp1_fp, temp2_fp);

	if ((uint32_t)temp2_fp <= DP_TU_FP_EDGE) {
		tu->extra_bytes = 0;
		printk(BIOS_DEBUG, "extra_bytes set to 0\n");
	} else {
		temp1_fp = drm_fixp_from_fraction(tu->n_tus + 1, 1);
		temp2_fp = drm_fixp_mul(temp1_fp, temp2_fp);
		tu->extra_bytes = fixp2int_ceil(temp2_fp);
	}

	temp1_fp = drm_fixp_from_fraction(tu->extra_bytes, 1);
	temp2_fp = drm_fixp_from_fraction(8, tu->bpp);
	temp1_fp = drm_fixp_mul(temp1_fp, temp2_fp);
	tu->extra_pclk_cycles = fixp2int_ceil(temp1_fp);

	temp1_fp = drm_fixp_div(tu->lclk_fp, tu->pclk_fp);
	temp2_fp = drm_fixp_from_fraction(tu->extra_pclk_cycles, 1);
	temp1_fp = drm_fixp_mul(temp2_fp, temp1_fp);
	tu->extra_pclk_cycles_in_link_clk = fixp2int_ceil(temp1_fp);
}

static void dp_tu_update_timings(struct dp_tu_calc_input *in, struct tu_algo_data *tu)
{
	int nlanes = in->nlanes;
	int dsc_num_slices = in->num_of_dsc_slices;
	int dsc_num_bytes = 0;
	int numerator;
	s64 pclk_dsc_fp;
	s64 dwidth_dsc_fp;
	s64 hbp_dsc_fp;
	s64 overhead_dsc_fp;
	int tot_num_eoc_symbols = 0;
	int tot_num_hor_bytes = 0;
	int tot_num_dummy_bytes = 0;
	int dwidth_dsc_bytes = 0;
	int eoc_bytes = 0;
	s64 tot_num_hor_bytes_frac_fp = 0;
	s64 temp1_fp, temp2_fp, temp3_fp;

	tu->lclk_fp = drm_fixp_from_fraction(in->lclk, 1);
	tu->orig_lclk_fp = tu->lclk_fp;
	tu->pclk_fp = drm_fixp_from_fraction(in->pclk_khz, 1000);
	tu->orig_pclk_fp = tu->pclk_fp;
	tu->lwidth = in->hactive;
	tu->hbp_relative_to_pclk = in->hporch;
	tu->nlanes = in->nlanes;
	tu->bpp = in->bpp;
	tu->pixelEnc = in->pixel_enc;
	tu->dsc_en = in->dsc_en;
	tu->fec_en = in->fec_en;
	tu->async_en = in->async_en;
	tu->lwidth_fp = drm_fixp_from_fraction(in->hactive, 1);
	tu->orig_lwidth = in->hactive;
	tu->hbp_relative_to_pclk_fp = drm_fixp_from_fraction(in->hporch, 1);
	tu->orig_hbp = in->hporch;
	tu->rb2 = (in->hporch < 160) ? 1 : 0;
	tu->comp_bpp = in->comp_bpp;
	tu->ppc_div_factor = in->ppc_div_factor;

	if (!in->comp_bpp)
		tu->comp_bpp = 8;

	if (in->ppc_div_factor != 4)
		tu->ppc_div_factor = 2;

	if (tu->pixelEnc == 420) {
		temp1_fp = drm_fixp_from_fraction(2, 1);
		tu->pclk_fp = drm_fixp_div(tu->pclk_fp, temp1_fp);
		tu->lwidth_fp = drm_fixp_div(tu->lwidth_fp, temp1_fp);
		tu->hbp_relative_to_pclk_fp =
			drm_fixp_div(tu->hbp_relative_to_pclk_fp, temp1_fp);
	}

	if (tu->pixelEnc == 422) {
		switch (tu->bpp) {
		case 24:
			tu->bpp = 16;
			tu->bpc = 8;
			break;
		case 30:
			tu->bpp = 20;
			tu->bpc = 10;
			break;
		default:
			tu->bpp = 16;
			tu->bpc = 8;
			break;
		}
	} else
		tu->bpc = tu->bpp / 3;

	if (!in->dsc_en)
		goto fec_check;

	tu->bpp = 24; /* hardcode to 24 if DSC is enabled. */

	temp1_fp = drm_fixp_from_fraction(in->compress_ratio, 100);
	temp2_fp = drm_fixp_from_fraction(in->bpp, 1);
	temp3_fp = drm_fixp_div(temp2_fp, temp1_fp);
	temp2_fp = drm_fixp_mul(tu->lwidth_fp, temp3_fp);

	temp1_fp = drm_fixp_from_fraction(8, 1);
	temp3_fp = drm_fixp_div(temp2_fp, temp1_fp);

	numerator = drm_fixp2int(temp3_fp);

	dsc_num_bytes = numerator / dsc_num_slices;
	eoc_bytes = dsc_num_bytes % nlanes;
	tot_num_eoc_symbols = nlanes * dsc_num_slices;
	tot_num_hor_bytes = dsc_num_bytes * dsc_num_slices;
	tot_num_dummy_bytes = (nlanes - eoc_bytes) * dsc_num_slices;

	temp1_fp = drm_fixp_from_fraction(8, 1);
	temp2_fp = drm_fixp_from_fraction(tu->orig_lwidth, 1);
	temp3_fp = drm_fixp_div(tu->comp_bpp, temp1_fp);
	tot_num_hor_bytes_frac_fp = drm_fixp_mul(temp3_fp, temp2_fp);

	if (dsc_num_bytes == 0)
		printk(BIOS_DEBUG, "incorrect no of bytes per slice=%d\n", dsc_num_bytes);

	dwidth_dsc_bytes = (tot_num_hor_bytes + tot_num_eoc_symbols +
			    (eoc_bytes == 0 ? 0 : tot_num_dummy_bytes));

	temp1_fp = drm_fixp_from_fraction(dwidth_dsc_bytes, 1);
	overhead_dsc_fp = drm_fixp_div(temp1_fp, tot_num_hor_bytes_frac_fp);
	printk(BIOS_DEBUG, "%lld\n", overhead_dsc_fp);

	dwidth_dsc_fp = drm_fixp_from_fraction(dwidth_dsc_bytes, 3);

	temp2_fp = drm_fixp_mul(tu->pclk_fp, dwidth_dsc_fp);
	temp1_fp = drm_fixp_div(temp2_fp, tu->lwidth_fp);
	pclk_dsc_fp = temp1_fp;

	temp1_fp = drm_fixp_div(pclk_dsc_fp, tu->pclk_fp);
	temp2_fp = drm_fixp_mul(tu->hbp_relative_to_pclk_fp, temp1_fp);
	hbp_dsc_fp = temp2_fp;

	/* output */
	tu->pclk_fp = pclk_dsc_fp;
	tu->lwidth_fp = dwidth_dsc_fp;
	tu->hbp_relative_to_pclk_fp = hbp_dsc_fp;

fec_check:
	if (in->fec_en) {
		temp1_fp = drm_fixp_from_fraction(976, 1000); /* 0.976 */
		tu->lclk_fp = drm_fixp_mul(tu->lclk_fp, temp1_fp);
	}
}

static void dp_tu_calculate(struct dp_tu_calc_input *in)
{
	struct tu_algo_data tu;
	int compare_result_1, compare_result_2;
	u64 temp = 0, temp1;
	s64 temp_fp = 0, temp1_fp = 0, temp2_fp = 0;

	s64 LCLK_FAST_SKEW_fp = drm_fixp_from_fraction(6, 10000); /* 0.0006 */
	s64 RATIO_SCALE_fp = drm_fixp_from_fraction(1001, 1000);

	u8 DP_BRUTE_FORCE = 1;
	s64 BRUTE_FORCE_THRESHOLD_fp = drm_fixp_from_fraction(1, 10); /* 0.1 */
	uint32_t EXTRA_PIXCLK_CYCLE_DELAY = 4;
	s64 HBLANK_MARGIN = drm_fixp_from_fraction(4, 1);
	s64 HBLANK_MARGIN_EXTRA = 0;

	memset(&tu, 0, sizeof(tu));

	dp_tu_update_timings(in, &tu);

	tu.err_fp = drm_fixp_from_fraction(1000, 1); /* 1000 */

	temp1_fp = drm_fixp_from_fraction(4, 1);
	temp2_fp = drm_fixp_mul(temp1_fp, tu.lclk_fp);
	temp_fp = drm_fixp_div(temp2_fp, tu.pclk_fp);
	tu.extra_buffer_margin = fixp2int_ceil(temp_fp);

	if (in->compress_ratio == 375 && tu.bpp == 30)
		temp1_fp = drm_fixp_from_fraction(24, 8);
	else
		temp1_fp = drm_fixp_from_fraction(tu.bpp, 8);

	temp2_fp = drm_fixp_mul(tu.pclk_fp, temp1_fp);
	temp1_fp = drm_fixp_from_fraction(tu.nlanes, 1);
	temp2_fp = drm_fixp_div(temp2_fp, temp1_fp);
	tu.ratio_fp = drm_fixp_div(temp2_fp, tu.lclk_fp);

	tu.original_ratio_fp = tu.ratio_fp;
	tu.boundary_moderation_en = false;
	tu.upper_boundary_count = 0;
	tu.lower_boundary_count = 0;
	tu.i_upper_boundary_count = 0;
	tu.i_lower_boundary_count = 0;
	tu.valid_lower_boundary_link = 0;
	tu.even_distribution_BF = 0;
	tu.even_distribution_legacy = 0;
	tu.even_distribution = 0;
	tu.hbp_delayStartCheck = 0;
	tu.pre_tu_hw_pipe_delay = 0;
	tu.post_tu_hw_pipe_delay = 0;
	tu.link_config_hactive_time = 0;
	tu.delay_start_link_lclk = 0;
	tu.tu_active_cycles = 0;
	tu.resolution_line_time = 0;
	tu.last_partial_lclk = 0;
	tu.delay_start_time_fp = 0;
	tu.second_loop_set = 0;

	tu.err_fp = drm_fixp_from_fraction(1000, 1);
	tu.n_err_fp = 0;
	tu.n_n_err_fp = 0;

	temp = drm_fixp2int(tu.lwidth_fp);
	if ((((u32)temp % tu.nlanes) != 0) &&
	    (_tu_param_compare(tu.ratio_fp, DRM_FIXED_ONE) == 2) && (tu.dsc_en == 0)) {
		tu.ratio_fp = drm_fixp_mul(tu.ratio_fp, RATIO_SCALE_fp);
	}

	if (_tu_param_compare(tu.ratio_fp, DRM_FIXED_ONE) == 1)
		tu.ratio_fp = DRM_FIXED_ONE;

	if (HBLANK_MARGIN_EXTRA != 0) {
		HBLANK_MARGIN += HBLANK_MARGIN_EXTRA;
		printk(BIOS_DEBUG, "increased HBLANK_MARGIN to %lld. (PLUS%lld)\n",
		       HBLANK_MARGIN, HBLANK_MARGIN_EXTRA);
	}

	for (tu.tu_size = 32; tu.tu_size <= 64; tu.tu_size++) {
		temp1_fp = drm_fixp_from_fraction(tu.tu_size, 1);
		temp2_fp = drm_fixp_mul(tu.ratio_fp, temp1_fp);
		temp = fixp2int_ceil(temp2_fp);
		temp1_fp = drm_fixp_from_fraction(temp, 1);
		tu.n_err_fp = fixp_subtract(temp1_fp, temp2_fp);

		if (tu.n_err_fp < tu.err_fp) {
			tu.err_fp = tu.n_err_fp;
			tu.tu_size_desired = tu.tu_size;
		}
	}

	tu.tu_size_minus1 = tu.tu_size_desired - 1;

	temp1_fp = drm_fixp_from_fraction(tu.tu_size_desired, 1);
	temp2_fp = drm_fixp_mul(tu.ratio_fp, temp1_fp);
	tu.valid_boundary_link = fixp2int_ceil(temp2_fp);

	temp1_fp = drm_fixp_from_fraction(tu.bpp, 8);
	temp2_fp = tu.lwidth_fp;
	temp2_fp = drm_fixp_mul(temp2_fp, temp1_fp);

	temp1_fp = drm_fixp_from_fraction(tu.valid_boundary_link, 1);
	temp2_fp = drm_fixp_div(temp2_fp, temp1_fp);
	tu.n_tus = drm_fixp2int(temp2_fp);
	if ((temp2_fp & 0xFFFFFFFF) > 0xFFFFF000)
		tu.n_tus += 1;

	tu.even_distribution_legacy = tu.n_tus % tu.nlanes == 0 ? 1 : 0;

	printk(BIOS_DEBUG, "n_sym= %d, n_tus= %d\n", tu.valid_boundary_link, tu.n_tus);

	_dp_calc_extra_bytes(&tu);

	tu.filler_size = tu.tu_size_desired - tu.valid_boundary_link;

	temp1_fp = drm_fixp_from_fraction(tu.tu_size_desired, 1);
	tu.ratio_by_tu_fp = drm_fixp_mul(tu.ratio_fp, temp1_fp);

	tu.delay_start_link =
		tu.extra_pclk_cycles_in_link_clk + tu.filler_size + tu.extra_buffer_margin;

	tu.resulting_valid_fp = drm_fixp_from_fraction(tu.valid_boundary_link, 1);

	temp1_fp = drm_fixp_from_fraction(tu.tu_size_desired, 1);
	temp2_fp = drm_fixp_div(tu.resulting_valid_fp, temp1_fp);
	tu.TU_ratio_err_fp = temp2_fp - tu.original_ratio_fp;

	temp1_fp = drm_fixp_from_fraction((tu.hbp_relative_to_pclk - HBLANK_MARGIN), 1);
	tu.hbp_time_fp = drm_fixp_div(temp1_fp, tu.pclk_fp);

	temp1_fp = drm_fixp_from_fraction(tu.delay_start_link, 1);
	tu.delay_start_time_fp = drm_fixp_div(temp1_fp, tu.lclk_fp);

	compare_result_1 = _tu_param_compare(tu.hbp_time_fp, tu.delay_start_time_fp);
	if (compare_result_1 == 2) /* hbp_time_fp < delay_start_time_fp */
		tu.min_hblank_violated = 1;

	tu.hactive_time_fp = drm_fixp_div(tu.lwidth_fp, tu.pclk_fp);

	compare_result_2 = _tu_param_compare(tu.hactive_time_fp, tu.delay_start_time_fp);
	if (compare_result_2 == 2)
		tu.min_hblank_violated = 1;

	/* brute force */

	tu.delay_start_link_extra_pixclk = EXTRA_PIXCLK_CYCLE_DELAY;
	tu.diff_abs_fp = tu.resulting_valid_fp - tu.ratio_by_tu_fp;

	temp = drm_fixp2int(tu.diff_abs_fp);
	if (!temp && tu.diff_abs_fp <= 0xffff)
		tu.diff_abs_fp = 0;

	/* if(diff_abs < 0) diff_abs *= -1 */
	if (tu.diff_abs_fp < 0)
		tu.diff_abs_fp = drm_fixp_mul(tu.diff_abs_fp, -1);

	tu.boundary_mod_lower_err = 0;

	temp1_fp = drm_fixp_div(tu.orig_lclk_fp, tu.orig_pclk_fp);

	temp2_fp = drm_fixp_from_fraction(tu.orig_lwidth + tu.orig_hbp, tu.ppc_div_factor);

	temp_fp = drm_fixp_mul(temp1_fp, temp2_fp);
	tu.resolution_line_time = drm_fixp2int(temp_fp);
	tu.pre_tu_hw_pipe_delay = fixp2int_ceil(temp1_fp) + 2 /*cdc fifo write jitter+2*/ +
				  3 /*pre-delay start cycles*/
				  + 3 /*post-delay start cycles*/ + 1 /*BE on the link*/;
	tu.post_tu_hw_pipe_delay = 4 /*BS_on_the_link*/ + 1 /*BE_next_ren*/;

	temp1_fp = drm_fixp_from_fraction(tu.bpp, 8);
	temp1_fp = drm_fixp_mul(tu.lwidth_fp, temp1_fp);
	tu.n_symbols = fixp2int_ceil(temp1_fp);

	if (tu.rb2) {
		temp1_fp = drm_fixp_mul(tu.delay_start_time_fp, tu.lclk_fp);
		tu.delay_start_link_lclk = fixp2int_ceil(temp1_fp);

		tu.new_valid_boundary_link = tu.valid_boundary_link;
		tu.i_upper_boundary_count = 1;
		tu.i_lower_boundary_count = 0;

		temp1 = tu.i_upper_boundary_count * tu.new_valid_boundary_link;
		temp1 += tu.i_lower_boundary_count * (tu.new_valid_boundary_link - 1);
		temp = tu.i_upper_boundary_count + tu.i_lower_boundary_count;
		tu.average_valid2_fp = drm_fixp_from_fraction(temp1, temp);

		temp1_fp = drm_fixp_from_fraction(tu.bpp, 8);
		temp1_fp = drm_fixp_mul(tu.lwidth_fp, temp1_fp);
		temp2_fp = drm_fixp_div(temp1_fp, tu.average_valid2_fp);
		tu.n_tus = drm_fixp2int(temp2_fp);

		tu.n_tus_per_lane = tu.n_tus / tu.nlanes;
		tu.paired_tus = (int)((tu.n_tus_per_lane) / temp);

		tu.remainder_tus = tu.n_tus_per_lane - tu.paired_tus * temp;

		if (tu.remainder_tus > tu.i_upper_boundary_count) {
			temp = (tu.remainder_tus - tu.i_upper_boundary_count) *
			       (tu.new_valid_boundary_link - 1);
			temp += (tu.i_upper_boundary_count * tu.new_valid_boundary_link);
			temp *= tu.nlanes;
		} else {
			temp = tu.nlanes * tu.remainder_tus * tu.new_valid_boundary_link;
		}

		temp1 = tu.i_lower_boundary_count * (tu.new_valid_boundary_link - 1);
		temp1 += tu.i_upper_boundary_count * tu.new_valid_boundary_link;
		temp1 *= tu.paired_tus * tu.nlanes;
		temp1_fp = drm_fixp_from_fraction(tu.n_symbols - temp1 - temp, tu.nlanes);
		tu.last_partial_lclk = fixp2int_ceil(temp1_fp);

		tu.tu_active_cycles =
			(int)((tu.n_tus_per_lane * tu.tu_size) + tu.last_partial_lclk);

		temp = tu.pre_tu_hw_pipe_delay + tu.delay_start_link_lclk +
		       tu.tu_active_cycles + tu.post_tu_hw_pipe_delay;

		if (tu.fec_en == 1) {
			if (tu.nlanes == 1) {
				temp1_fp = drm_fixp_from_fraction(temp, 500);
				tu.parity_symbols = fixp2int_ceil(temp1_fp) * 12 + 1;
			} else {
				temp1_fp = drm_fixp_from_fraction(temp, 250);
				tu.parity_symbols = fixp2int_ceil(temp1_fp) * 6 + 1;
			}
		} else { /* no fec BW impact */
			tu.parity_symbols = 0;
		}

		tu.link_config_hactive_time = temp + tu.parity_symbols;

		if (tu.link_config_hactive_time + 1 /*margin*/ >= tu.resolution_line_time)
			tu.min_hblank_violated = 1;
	}

	tu.delay_start_time_fp = 0;

	if ((tu.diff_abs_fp != 0 &&
	     ((tu.diff_abs_fp > BRUTE_FORCE_THRESHOLD_fp) ||
	      (tu.even_distribution_legacy == 0) || (DP_BRUTE_FORCE == 1))) ||
	    (tu.min_hblank_violated == 1)) {
		_dp_calc_boundary(&tu);

		if (tu.boundary_moderation_en) {
			temp1_fp = drm_fixp_from_fraction(
				(tu.upper_boundary_count * tu.valid_boundary_link +
				 tu.lower_boundary_count * (tu.valid_boundary_link - 1)),
				1);
			temp2_fp = drm_fixp_from_fraction(
				(tu.upper_boundary_count + tu.lower_boundary_count), 1);

			tu.resulting_valid_fp = drm_fixp_div(temp1_fp, temp2_fp);

			temp1_fp = drm_fixp_from_fraction(tu.tu_size_desired, 1);
			tu.ratio_by_tu_fp = drm_fixp_mul(tu.original_ratio_fp, temp1_fp);

			tu.valid_lower_boundary_link = tu.valid_boundary_link - 1;

			temp1_fp = drm_fixp_from_fraction(tu.bpp, 8);
			temp1_fp = drm_fixp_mul(tu.lwidth_fp, temp1_fp);
			temp2_fp = drm_fixp_div(temp1_fp, tu.resulting_valid_fp);
			tu.n_tus = drm_fixp2int(temp2_fp);

			tu.tu_size_minus1 = tu.tu_size_desired - 1;
			tu.even_distribution_BF = 1;

			temp1_fp = drm_fixp_from_fraction(tu.tu_size_desired, 1);
			temp2_fp = drm_fixp_div(tu.resulting_valid_fp, temp1_fp);
			tu.TU_ratio_err_fp = temp2_fp - tu.original_ratio_fp;
		}
	}

	if (tu.async_en) {
		temp2_fp = drm_fixp_mul(LCLK_FAST_SKEW_fp, tu.lwidth_fp);
		temp = fixp2int_ceil(temp2_fp);

		temp1_fp = drm_fixp_from_fraction(tu.nlanes, 1);
		temp2_fp = drm_fixp_mul(tu.original_ratio_fp, temp1_fp);
		temp1_fp = drm_fixp_from_fraction(tu.bpp, 8);
		temp2_fp = drm_fixp_div(temp1_fp, temp2_fp);
		temp1_fp = drm_fixp_from_fraction(temp, 1);
		temp2_fp = drm_fixp_mul(temp1_fp, temp2_fp);
		temp = drm_fixp2int(temp2_fp);

		tu.delay_start_link += (int)temp;
	}

	temp1_fp = drm_fixp_from_fraction(tu.delay_start_link, 1);
	tu.delay_start_time_fp = drm_fixp_div(temp1_fp, tu.lclk_fp);

	/* OUTPUTS */
	struct dp_tu_calc_output tu_table;
	tu_table.valid_boundary_link = tu.valid_boundary_link;
	tu_table.delay_start_link = tu.delay_start_link;
	tu_table.boundary_moderation_en = tu.boundary_moderation_en;
	tu_table.valid_lower_boundary_link = tu.valid_lower_boundary_link;
	tu_table.upper_boundary_count = tu.upper_boundary_count;
	tu_table.lower_boundary_count = tu.lower_boundary_count;
	tu_table.tu_size_minus1 = tu.tu_size_minus1;

	printk(BIOS_DEBUG, "TU: %d %d %d %d %d %d %d\n", tu_table.valid_boundary_link,
	       tu_table.delay_start_link, tu_table.boundary_moderation_en,
	       tu_table.valid_lower_boundary_link, tu_table.upper_boundary_count,
	       tu_table.lower_boundary_count, tu_table.tu_size_minus1);

	write32(&edp_lclk->valid_boundary,
		tu_table.delay_start_link << 16 | tu_table.valid_boundary_link);

	write32(&edp_lclk->tu, tu_table.tu_size_minus1);

	write32(&edp_lclk->valid_boundary2, tu_table.boundary_moderation_en |
						    tu_table.valid_lower_boundary_link << 1 |
						    tu_table.upper_boundary_count << 16 |
						    tu_table.lower_boundary_count << 20);
}

void edp_ctrl_config_TU(struct edp_ctrl *ctrl, struct edid *edid)
{
	struct dp_tu_calc_input inp;

	/* link clock (MHz) */
	inp.lclk = ctrl->link_rate_khz / 1000;

	/* pixel clock (kHz) */
	inp.pclk_khz = edid->mode.pixel_clock;

	/* horizontal timing */
	inp.hactive = edid->mode.ha;
	inp.hporch = edid->mode.hbl;

	/* lanes */
	inp.nlanes = ctrl->lane_cnt;

	/* bits per pixel from EDID */
	inp.bpp = edid->panel_bits_per_pixel;

	/* Defaults unless advertised */
	inp.pixel_enc = 444;
	inp.dsc_en = 0;
	inp.fec_en = 0;
	inp.async_en = 0;
	inp.compress_ratio = 0;
	inp.num_of_dsc_slices = 0;
	inp.comp_bpp = 0;
	inp.ppc_div_factor = 1;

	dp_tu_calculate(&inp);
}
