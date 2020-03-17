/*
 * This file is part of the coreboot project.
 *
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

#ifndef __PI_PICASSO_NORTHBRIDGE_H__
#define __PI_PICASSO_NORTHBRIDGE_H__

#include <device/device.h>
#include <types.h>

/* D1F1 - HDA Configuration Registers */
#define HDA_DEV_CTRL_STATUS	0x60
#define   HDA_NO_SNOOP_EN	BIT(11)

/* D18F0 - HT Configuration Registers */
#define D18F0_NODE_ID		0x60
#define D18F0_CPU_CNT		0x62 /* BKDG defines as a field in DWORD 0x60 */
# define CPU_CNT_MASK		0x1f /*  CpuCnt + 1 = no. CPUs */
#define HT_INIT_CONTROL		0x6c
# define HTIC_BIOSR_DETECT	((1 << 5) | (1 << 9) | (1 << 10))
# define HTIC_COLD_RST_DET	BIT(4)

/* D18F1 - Address Map Registers */

/* MMIO base and limit */
#define D18F1_MMIO_BASE0_LO	0x80
# define MMIO_WE		(1 << 1)
# define MMIO_RE		(1 << 0)
#define D18F1_MMIO_LIMIT0_LO	0x84
# define MMIO_NP		(1 << 7)
#define D18F1_IO_BASE0_LO	0xc0
#define D18F1_IO_BASE1_LO	0xc8
#define D18F1_IO_BASE2_LO	0xd0
#define D18F1_IO_BASE3_LO	0xd8
#define D18F1_MMIO_BASE7_LO	0xb8
#define D18F1_MMIO_BASELIM0_HI	0x180
#define D18F1_MMIO_BASE8_LO	0x1a0
#define D18F1_MMIO_LIMIT8_LO	0x1a4
#define D18F1_MMIO_BASE11_LO	0x1b8
#define D18F1_MMIO_BASELIM8_HI	0x1c0
#define NB_MMIO_BASE_LO(reg)	((reg) * 2 * sizeof(uint32_t) + (((reg) < 8) \
					? D18F1_MMIO_BASE0_LO \
					: D18F1_MMIO_BASE8_LO \
						- 8 * sizeof(uint64_t)))
#define NB_MMIO_LIMIT_LO(reg)	(NB_MMIO_BASE_LO(reg) + sizeof(uint32_t))
#define NB_MMIO_BASELIM_HI(reg)	((reg) * sizeof(uint32_t) + (((reg) < 8) \
					? D18F1_MMIO_BASELIM0_HI \
					: D18F1_MMIO_BASELIM8_HI \
						- 8 * sizeof(uint32_t)))
/* I/O base and limit */
#define D18F1_IO_BASE0		0xc0
# define IO_WE			(1 << 1)
# define IO_RE			(1 << 0)
#define D18F1_IO_LIMIT0		0xc4
#define NB_IO_BASE(reg)		((reg) * 2 * sizeof(uint32_t) + D18F1_IO_BASE0)
#define NB_IO_LIMIT(reg)	(NB_IO_BASE(reg) + sizeof(uint32_t))

#define D18F1_DRAM_HOLE		0xf0
# define DRAM_HOIST_VALID	(1 << 1)
# define DRAM_HOLE_VALID	(1 << 0)
#define D18F1_VGAEN		0xf4
# define VGA_ADDR_ENABLE	(1 << 0)

/* Bus A D0F5 - Audio Processor */
#define ACP_I2S_PIN_CONFIG	0x1400	/* HDA, Soundwire, I2S */

void amd_initcpuio(void);

void domain_enable_resources(struct device *dev);
void domain_set_resources(struct device *dev);
void fam15_finalize(void *chip_info);
void set_warm_reset_flag(void);
int is_warm_reset(void);

#endif /* __PI_PICASSO_NORTHBRIDGE_H__ */
