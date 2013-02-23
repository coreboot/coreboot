/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2010 Advanced Micro Devices, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef __SR5650_H__
#define __SR5650_H__

#include <stdint.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "rev.h"

typedef struct __PCIE_CFG__ {
	u16 Config;
	u8 ResetReleaseDelay;
	u8 Gfx0Width;
	u8 Gfx1Width;
	u8 GfxPayload;
	u8 GppPayload;
	u16 PortDetect;
	u8 PortHp;		/* hot plug */
	u16 DbgConfig;
	u32 DbgConfig2;
	u8 GfxLx;
	u8 GppLx;
	u8 NBSBLx;
	u8 PortSlotInit;
	u8 Gfx0Pwr;
	u8 Gfx1Pwr;
	u8 GppPwr;
} PCIE_CFG;

/* PCIE config flags */
#define	PCIE_DUALSLOT_CONFIG		(1 << 0)
#define	PCIE_OVERCLOCK_ENABLE		(1 << 1)
#define	PCIE_GPP_CLK_GATING		(1 << 2)
#define	PCIE_ENABLE_STATIC_DEV_REMAP	(1 << 3)
#define	PCIE_OFF_UNUSED_GFX_LANES	(1 << 4)
#define	PCIE_OFF_UNUSED_GPP_LANES	(1 << 5)
#define	PCIE_DISABLE_HIDE_UNUSED_PORTS	(1 << 7)
#define	PCIE_GFX_CLK_GATING		(1 << 11)
#define	PCIE_GFX_COMPLIANCE		(1 << 14)
#define	PCIE_GPP_COMPLIANCE		(1 << 15)

/* -------------------- ----------------------
* NBMISCIND
 ------------------- -----------------------*/
#define	PCIE_LINK_CFG			0x8
#define	PCIE_NBCFG_REG7			0x37
#define	STRAPS_OUTPUT_MUX_7		0x67
#define	STRAPS_OUTPUT_MUX_A		0x6a

/* -------------------- ----------------------
* PCIEIND
 ------------------- -----------------------*/
#define	PCIE_CI_CNTL			0x20
#define	PCIE_LC_LINK_WIDTH		0xa2
#define PCIE_LC_STATE0			0xa5
#define	PCIE_VC0_RESOURCE_STATUS	0x12a	/* 16bit read only */

#define	PCIE_CORE_INDEX_SB		(0x05 << 16) /* see rpr 4.3.2.2, bdg 2.1 */
#define	PCIE_CORE_INDEX_GPP1		(0x04 << 16)
#define	PCIE_CORE_INDEX_GPP2		(0x06 << 16)
#define	PCIE_CORE_INDEX_GPP1_GPP2	(0x00 << 16)
#define	PCIE_CORE_INDEX_GPP3a		(0x07 << 16)
#define	PCIE_CORE_INDEX_GPP3b		(0x03 << 16)

/* contents of PCIE_VC0_RESOURCE_STATUS */
#define	VC_NEGOTIATION_PENDING		(1 << 1)

#define	LC_STATE_RECONFIG_GPPSB		0x10

/* ------------------------------------------------
* Global variable
* ------------------------------------------------- */
extern PCIE_CFG AtiPcieCfg;

/* ----------------- export funtions ----------------- */
u32 nbpcie_p_read_index(device_t dev, u32 index);
void nbpcie_p_write_index(device_t dev, u32 index, u32 data);
u32 nbpcie_ind_read_index(device_t nb_dev, u32 index);
void nbpcie_ind_write_index(device_t nb_dev, u32 index, u32 data);
u32 pci_ext_read_config32(device_t nb_dev, device_t dev, u32 reg);
void pci_ext_write_config32(device_t nb_dev, device_t dev, u32 reg, u32 mask, u32 val);
void sr5650_set_tom(device_t nb_dev);

void ProgK8TempMmioBase(u8 in_out, u32 pcie_base_add, u32 mmio_base_add);
void enable_pcie_bar3(device_t nb_dev);
void disable_pcie_bar3(device_t nb_dev);

void enable_sr5650_dev8(void);
void sr5650_htinit(void);
void sr5650_early_setup(void);
void sr5650_before_pci_init(void);
void sr5650_enable(device_t dev);
void sr5650_gpp_sb_init(device_t nb_dev, device_t dev, u32 port);
void sr5650_gfx_init(device_t nb_dev, device_t dev, u32 port);
void avoid_lpc_dma_deadlock(device_t nb_dev, device_t sb_dev);
void config_gpp_core(device_t nb_dev, device_t sb_dev);
void PcieReleasePortTraining(device_t nb_dev, device_t dev, u32 port);
u8 PcieTrainPort(device_t nb_dev, device_t dev, u32 port);
void pcie_config_misc_clk(device_t nb_dev);
void fam10_optimization(void);
void sr5650_disable_pcie_bridge(void);
u32 get_vid_did(device_t dev);
void sr5650_nb_pci_table(device_t nb_dev);
void init_gen2(device_t nb_dev, device_t dev, u8 port);
void sr56x0_lock_hwinitreg(void);
#endif /* SR5650_H */
