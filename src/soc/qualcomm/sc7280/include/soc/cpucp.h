/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_QUALCOMM_SC7280_CPUCP_H__
#define _SOC_QUALCOMM_SC7280_CPUCP_H__

#include <soc/addressmap.h>

struct epsstop_epss_top {
	uint32_t access_override;
	uint32_t global_enable;
	uint32_t trace_bus_ctrl;
	uint32_t debug_bus_ctrl;
	uint32_t muc_hang_det_ctrl;
	uint32_t muc_hang_irq_sts;
	uint32_t muc_hang_count_threshold;
	uint32_t muc_hang_count_sts;
	uint32_t muc_hang_det_sts;
	uint32_t l3_voting_en;
};

struct epssfast_epss_fast {
	uint32_t epss_muc_clk_ctrl;
	uint32_t muc_rvbar;
	uint32_t muc_rvbar_ctrl;
	uint32_t muc_non_secure_dmem_start_addr;
	uint32_t muc_non_secure_dmem_end_addr;
	uint32_t reserved_1[2];
	uint32_t cpr_data_fifo[4];
	uint32_t reserved_2[4];
	uint32_t pll_data_fifo[4];
	uint32_t reserved_3[4];
	uint32_t gfmux_data_fifo_1[4];
	uint32_t cpu_pcu_spare_irq_status;
	uint32_t cpu_pcu_spare_irq_clr;
	uint32_t cpu_pcu_spare_wait_event;
	uint32_t seq_mem[256];
};

static struct epsstop_epss_top *const epss_top = (void *)EPSSTOP_EPSS_TOP;
static struct epssfast_epss_fast *const epss_fast = (void *)EPSSFAST_BASE_ADDR;

void cpucp_fw_load_reset(void);
void cpucp_prepare(void);

#endif  // _SOC_QUALCOMM_SC7280_CPUCP_H__
