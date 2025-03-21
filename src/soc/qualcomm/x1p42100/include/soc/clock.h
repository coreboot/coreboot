/* SPDX-License-Identifier: GPL-2.0-only */

#include <soc/addressmap.h>
#include <types.h>
#include <soc/clock_common.h>

#ifndef __SOC_QUALCOMM_X1P42100_CLOCK_H__
#define __SOC_QUALCOMM_X1P42100_CLOCK_H__

#define SRC_XO_HZ		(38400 * KHz)

/* TODO: update as per datasheet */
void clock_configure_qspi(uint32_t hz);
void clock_enable_qup(int qup);
void clock_configure_dfsr(int qup);

/* Subsystem Reset */
static struct aoss *const aoss = (void *)AOSS_CC_BASE;

#endif	// __SOC_QUALCOMM_X1P42100_CLOCK_H__
