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

#ifndef SOC_INTEL_COMMON_BLOCK_PCR_H
#define SOC_INTEL_COMMON_BLOCK_PCR_H

/* Port Id lives in bits 23:16 and register offset lives in 15:0 of address. */
#define PCR_PORTID_SHIFT	16

#if !defined(__ACPI__)
#include <stdint.h>

uint32_t pcr_read32(uint8_t pid, uint16_t offset);
uint16_t pcr_read16(uint8_t pid, uint16_t offset);
uint8_t pcr_read8(uint8_t pid, uint16_t offset);
void pcr_write32(uint8_t pid, uint16_t offset, uint32_t indata);
void pcr_write16(uint8_t pid, uint16_t offset, uint16_t indata);
void pcr_write8(uint8_t pid, uint16_t offset, uint8_t indata);
void pcr_rmw32(uint8_t pid, uint16_t offset, uint32_t anddata,
		uint32_t ordata);
void pcr_rmw16(uint8_t pid, uint16_t offset, uint16_t anddata,
		uint16_t ordata);
void pcr_rmw8(uint8_t pid, uint16_t offset, uint8_t anddata,
		uint8_t ordata);
void pcr_or32(uint8_t pid, uint16_t offset, uint32_t ordata);
void pcr_or16(uint8_t pid, uint16_t offset, uint16_t ordata);
void pcr_or8(uint8_t pid, uint16_t offset, uint8_t ordata);

/* Get the starting address of the port's registers. */
void *pcr_reg_address(uint8_t pid, uint16_t offset);
#endif /* if !defined(__ACPI__) */

#endif	/* SOC_INTEL_COMMON_BLOCK_PCR_H */
