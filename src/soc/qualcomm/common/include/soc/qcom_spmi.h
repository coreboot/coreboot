/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef __SOC_QCOM_SPMI_H__
#define __SOC_QCOM_SPMI_H__

#include <stddef.h>
#include <stdint.h>

/* Macro to construct SPMI address from slave ID and register offset */
#define SPMI_ADDR(slave, reg) ((slave << 16) | reg)

int spmi_read8(uint32_t addr);
int spmi_write8(uint32_t addr, uint8_t data);

#endif	// __SOC_QCOM_SPMI_H__
