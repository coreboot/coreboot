/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <types.h>
#include <soc/clock_common.h>

#ifndef __SOC_QUALCOMM_X1P42100_CLOCK_H__
#define __SOC_QUALCOMM_X1P42100_CLOCK_H__

#define SRC_XO_HZ		(38400 * KHz)

enum clk_qup {
	QUP_WRAP0_S0,
	QUP_WRAP0_S1,
	QUP_WRAP0_S2,
	QUP_WRAP0_S3,
	QUP_WRAP0_S4,
	QUP_WRAP0_S5,
	QUP_WRAP0_S6,
	QUP_WRAP0_S7,
	QUP_WRAP1_S0,
	QUP_WRAP1_S1,
	QUP_WRAP1_S2,
	QUP_WRAP1_S3,
	QUP_WRAP1_S4,
	QUP_WRAP1_S5,
	QUP_WRAP1_S6,
	QUP_WRAP1_S7,
	QUP_WRAP2_S0,
	QUP_WRAP2_S1,
	QUP_WRAP2_S2,
	QUP_WRAP2_S3,
	QUP_WRAP2_S4,
	QUP_WRAP2_S5,
	QUP_WRAP2_S6,
	QUP_WRAP2_S7,
};

/* TODO: update as per datasheet */
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;

#endif	// __SOC_QUALCOMM_X1P42100_CLOCK_H__
