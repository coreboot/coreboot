/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AM335X_MMC_H
#define AM335X_MMC_H

#include <commonlib/sd_mmc_ctrlr.h>

#define MMCHS0_BASE 0x48060000

struct am335x_mmc {
	uint8_t res1[0x110];
	uint32_t sysconfig;
	uint32_t sysstatus;
	uint8_t res2[0x14];
	uint32_t con;
	uint32_t pwcnt;
	uint32_t dll;
	uint8_t res3[0xcc];
	uint32_t blk;
	uint32_t arg;
	uint32_t cmd;
	uint32_t rsp10;
	uint32_t rsp32;
	uint32_t rsp54;
	uint32_t rsp76;
	uint32_t data;
	uint32_t pstate;
	uint32_t hctl;
	uint32_t sysctl;
	uint32_t stat;
	uint32_t ie;
	uint8_t res4[0x4];
	uint32_t ac12;
	uint32_t capa;
	uint32_t capa2;
	uint8_t res5[0xc];
	uint32_t admaes;
	uint32_t admasal;
} __packed;

struct am335x_mmc_host {
	struct sd_mmc_ctrlr sd_mmc_ctrlr;
	struct am335x_mmc *reg;
	uint32_t sd_clock_hz;
};

int am335x_mmc_init_storage(struct am335x_mmc_host *mmc_host);

#endif
