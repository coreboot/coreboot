/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <commonlib/bsd/clamp.h>
#include <console/console.h>
#include <delay.h>
#include <northbridge/intel/haswell/haswell.h>
#include <timer.h>
#include <types.h>

#include "raminit_native.h"

enum raminit_status io_reset(void)
{
	union mc_init_state_g_reg mc_init_state_g = {
		.raw = mchbar_read32(MC_INIT_STATE_G),
	};
	mc_init_state_g.reset_io = 1;
	mchbar_write32(MC_INIT_STATE_G, mc_init_state_g.raw);
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 2000);
	do {
		mc_init_state_g.raw = mchbar_read32(MC_INIT_STATE_G);
		if (mc_init_state_g.reset_io == 0)
			return RAMINIT_STATUS_SUCCESS;

	} while (!stopwatch_expired(&timer));
	printk(BIOS_ERR, "Timed out waiting for DDR I/O reset to complete\n");
	return RAMINIT_STATUS_POLL_TIMEOUT;
}

enum raminit_status wait_for_first_rcomp(void)
{
	struct stopwatch timer;
	stopwatch_init_msecs_expire(&timer, 2000);
	do {
		if (mchbar_read32(RCOMP_TIMER) & BIT(16))
			return RAMINIT_STATUS_SUCCESS;

	} while (!stopwatch_expired(&timer));
	printk(BIOS_ERR, "Timed out waiting for RCOMP to complete\n");
	return RAMINIT_STATUS_POLL_TIMEOUT;
}

void force_rcomp_and_wait_us(unsigned int usecs)
{
	/* Disable periodic COMP */
	const union pcu_comp_reg m_comp = {
		.comp_disable  = 1,
		.comp_interval = COMP_INT,
		.comp_force    = 1,
	};
	mchbar_write32(M_COMP, m_comp.raw);
	if (usecs) {
		udelay(usecs);
	}
}

static int8_t get_comp(union ddr_data_offset_comp_reg data_offset_comp, enum opt_param optparam)
{
	switch (optparam) {
		case OptWrDS:	return data_offset_comp.drv_up;
		case OptRdOdt:	return data_offset_comp.odt_up;
		case OptSComp:	return data_offset_comp.slew_rate;
		default:	return 0;
	}
}

static void optimise_comp_offset(struct sysinfo *ctrl, const enum opt_param optparam)
{
	const uint8_t reserved_codes = 3;
	const bool update_ctrl = true;

	uint8_t param;
	uint8_t current_comp;
	int32_t min_comp_vref;
	int32_t max_comp_vref;
	int32_t curr_comp_vref;
	switch (optparam) {
	case OptWrDS: {
		const union ddr_data_comp_0_reg data_comp_0 = {
			.raw = mchbar_read32(DDR_DATA_COMP_0),
		};
		current_comp = data_comp_0.rcomp_drv_up;
		curr_comp_vref = ctrl->comp_ctl_0.dq_drv_vref;
		min_comp_vref = -8;
		max_comp_vref = 7;
		param = WrDS;
		break;
	}
	case OptRdOdt: {
		const union ddr_data_comp_1_reg data_comp_1 = {
			.raw = mchbar_read32(DDR_DATA_COMP_1),
		};
		current_comp = data_comp_1.rcomp_odt_up;
		curr_comp_vref = ctrl->comp_ctl_0.dq_odt_vref;
		min_comp_vref = -16;
		max_comp_vref = 15;
		param = RdOdt;
		break;
	}
	case OptSComp: {
		const union ddr_data_comp_0_reg data_comp_0 = {
			.raw = mchbar_read32(DDR_DATA_COMP_0),
		};
		current_comp = data_comp_0.slew_rate_comp;
		/* Mask out the phase/cycle bit */
		curr_comp_vref = ctrl->comp_ctl_1.dq_scomp & ~BIT(4);
		min_comp_vref = 4;
		max_comp_vref = 15;
		param = SCompDq;
		break;
	}
	default:
		die("%s: Invalid optparam: %d\n", __func__, optparam);
	}

	uint8_t comp_codes[NUM_CHANNELS][NUM_LANES] = {0};
	int16_t avg_offset = 0;
	uint8_t num_ch = 0;
	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel)) {
			continue;
		}

		num_ch++;
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			int8_t off = get_comp(ctrl->data_offset_comp[channel][byte], optparam);
			avg_offset += off;
			comp_codes[channel][byte] = current_comp + off;
		}
	}
	if (!num_ch) {
		return;
	}

	avg_offset = DIV_ROUND_CLOSEST(avg_offset, num_ch * ctrl->lanes);
	if (!avg_offset) {
		return;
	}

	const uint8_t start_delta = ABS(avg_offset);
	uint8_t best_vref_off = curr_comp_vref;
	uint8_t min_delta = start_delta;
	uint8_t new_comp = current_comp;
	uint8_t dq_scomp_pc = ctrl->comp_ctl_1.dq_scomp & BIT(4); /* phase/cycle bit */
	int8_t sign = sign_of(avg_offset);
	if (optparam == OptSComp) {
		sign = -sign;
	}
	bool done = false;
	for (uint8_t offset = 1; !done; offset++) {
		int8_t new_comp_vref = curr_comp_vref + sign * offset;
		if (new_comp_vref < min_comp_vref || new_comp_vref > max_comp_vref) {
			done = true;
		}
		if (reserved_codes > new_comp || new_comp > 63 - reserved_codes) {
			done = true;
		}
		if (optparam == OptSComp) {
			if (new_comp_vref >= 16) {
				dq_scomp_pc = 0;
			}
			new_comp_vref = dq_scomp_pc | new_comp_vref;
		}
		if (!done) {
			new_comp = update_comp_global_offset(ctrl, param, new_comp_vref, 0);
			const uint8_t curr_delta = ABS(current_comp + avg_offset - new_comp);
			if (curr_delta >= start_delta) {
				done = true;
			} else if (curr_delta < min_delta) {
				best_vref_off = new_comp_vref;
				min_delta = curr_delta;
				if (min_delta == 0) {
					done = true;
				}
			}
		}
	}
	new_comp = update_comp_global_offset(ctrl, param, best_vref_off, update_ctrl);
	if (best_vref_off == curr_comp_vref) {
		return;
	}

	for (uint8_t channel = 0; channel < NUM_CHANNELS; channel++) {
		if (!does_ch_exist(ctrl, channel)) {
			continue;
		}
		for (uint8_t byte = 0; byte < ctrl->lanes; byte++) {
			update_opt_param_offset(
				ctrl,
				channel,
				0,
				byte,
				optparam,
				comp_codes[channel][byte] - new_comp,
				update_ctrl);
		}
	}
}

enum raminit_status optimise_comp(struct sysinfo *ctrl)
{
	optimise_comp_offset(ctrl, OptWrDS);
	optimise_comp_offset(ctrl, OptRdOdt);
	optimise_comp_offset(ctrl, OptSComp);
	return RAMINIT_STATUS_SUCCESS;
}
