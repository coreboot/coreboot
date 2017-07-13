/*
 * This file is part of the coreboot project.
 *
 * Copyright 2017 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <compiler.h>
#include <intelblocks/lpss_i2c.h>

#define LPSS_DEBUG BIOS_NEVER

/* I2C Controller MMIO register space */
struct lpss_i2c_regs {
	uint32_t control;
	uint32_t target_addr;
	uint32_t slave_addr;
	uint32_t master_addr;
	uint32_t cmd_data;
	uint32_t ss_scl_hcnt;
	uint32_t ss_scl_lcnt;
	uint32_t fs_scl_hcnt;
	uint32_t fs_scl_lcnt;
	uint32_t hs_scl_hcnt;
	uint32_t hs_scl_lcnt;
	uint32_t intr_stat;
	uint32_t intr_mask;
	uint32_t raw_intr_stat;
	uint32_t rx_thresh;
	uint32_t tx_thresh;
	uint32_t clear_intr;
	uint32_t clear_rx_under_intr;
	uint32_t clear_rx_over_intr;
	uint32_t clear_tx_over_intr;
	uint32_t clear_rd_req_intr;
	uint32_t clear_tx_abrt_intr;
	uint32_t clear_rx_done_intr;
	uint32_t clear_activity_intr;
	uint32_t clear_stop_det_intr;
	uint32_t clear_start_det_intr;
	uint32_t clear_gen_call_intr;
	uint32_t enable;
	uint32_t status;
	uint32_t tx_level;
	uint32_t rx_level;
	uint32_t sda_hold;
	uint32_t tx_abort_source;
	uint32_t slv_data_nak_only;
	uint32_t dma_cr;
	uint32_t dma_tdlr;
	uint32_t dma_rdlr;
	uint32_t sda_setup;
	uint32_t ack_general_call;
	uint32_t enable_status;
	uint32_t fs_spklen;
	uint32_t hs_spklen;
	uint32_t clr_restart_det;
	uint32_t comp_param1;
	uint32_t comp_version;
	uint32_t comp_type;
} __packed;

/* Get I2C controller base address */
uintptr_t lpss_i2c_base_address(unsigned int bus);

/*
 * Initialize this bus controller and set the speed
 * Return value:
 * -1 = failure
 *  0 = success
*/
int lpss_i2c_init(unsigned int bus, const struct lpss_i2c_bus_config *bcfg);

/*
 * Generate speed cofnig based on clock
 * Return value:
 * -1 = failure
 *  0 = success
*/
int lpss_i2c_gen_speed_config(struct lpss_i2c_regs *regs,
					enum i2c_speed speed,
					const struct lpss_i2c_bus_config *bcfg,
					struct lpss_i2c_speed_config *config);
