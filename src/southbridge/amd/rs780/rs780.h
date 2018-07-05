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
 */

#ifndef __RS780_H__
#define __RS780_H__

#include <rules.h>
#include <stdint.h>
#include <device/pci_ids.h>
#include "chip.h"
#include "rev.h"

#define NBMISC_INDEX	0x60
#define NBHTIU_INDEX	0x94
#define NBMC_INDEX		0xE8
#define NBPCIE_INDEX	0xE0
#define EXT_CONF_BASE_ADDRESS 0xE0000000
#define	TEMP_MMIO_BASE_ADDRESS	0xC0000000

#define get_nb_rev(dev) pci_read_config8((dev), 0x89)

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

/* The Integrated Info Table */
#define USHORT	u16
#define UCHAR	u8
#define ULONG	u32

typedef struct _ATOM_COMMON_TABLE_HEADER
{
	USHORT usStructureSize;
	UCHAR  ucTableFormatRevision;
	UCHAR  ucTableContentRevision;
}ATOM_COMMON_TABLE_HEADER;

typedef struct _ATOM_INTEGRATED_SYSTEM_INFO_V2
{
	ATOM_COMMON_TABLE_HEADER	sHeader;
	ULONG				ulBootUpEngineClock; //in 10kHz unit
	ULONG				ulReserved1[2]; //must be 0x0 for the reserved
	ULONG				ulBootUpUMAClock; //in 10kHz unit
	ULONG				ulBootUpSidePortClock; //in 10kHz unit
	ULONG				ulMinSidePortClock; //in 10kHz unit
	ULONG				ulReserved2[6]; //must be 0x0 for the reserved
	ULONG				ulSystemConfig;
//[0]=1: PowerExpress mode
//   =0 Non-PowerExpress mode;
//[1]=1: system boots up at AMD overdriven state or user customized mode. In this case, driver will disable other power state in VBIOS table.
//   =0: system boots up at driver control state. Power state depends on VBIOS PP table.
//[2]=1: PWM method is used on NB voltage control.
//   =0: GPIO method is used.
//[3]=1: Only one power state(Performance) will be supported.
//   =0: Number of power states supported is from VBIOS PP table.
//[4]=1: CLMC is supported and enabled on current system.
//   =0: CLMC is not supported or enabled on current system. SBIOS need to support HT link/freq change through ATIF interface.
//[5]=1: Enable CDLW for all driver control power states. Max HT width is from SBIOS, while Min HT width is determined by display requirement.
//   =0: CDLW is disabled. If CLMC is enabled case, Min HT width will be set equal to Max HT width. If CLMC disabled case, Max HT width will be applied.
//[6]=1: High Voltage requested for all power states. In this case, voltage will be forced at 1.1v and VBIOS PP table voltage drop/throttling request will be ignored.
//   =0: Voltage settings is determined by VBIOS PP table.
//[7]=1: Enable CLMC Hybrid Mode. CDLD and CILR will be disabled in this case and we're using legacy C1E. This is workaround for CPU(Griffin) performance issue.
//   =0: Enable regular CLMC mode, CDLD and CILR will be enabled.
//[8]=1: CDLF is supported and enabled by fuse   //CHP 914
//   =0: CDLF is not supported and not enabled by fuses
	ULONG                      ulBootUpReqDisplayVector;
	ULONG                      ulOtherDisplayMisc;
	ULONG                      ulDDISlot1Config;
	ULONG                      ulDDISlot2Config;
	UCHAR                      ucMemoryType; //[3:0]=1:DDR1;=2:DDR2;=3:DDR3.[7:4] is reserved
	UCHAR                      ucUMAChannelNumber;
	UCHAR                      ucDockingPinBit;
	UCHAR                      ucDockingPinPolarity;
	ULONG                      ulDockingPinCFGInfo;
	ULONG                      ulCPUCapInfo;
	USHORT                     usNumberOfCyclesInPeriod; //usNumberOfCyclesInPeriod[15] = 0 - invert waveform
	                                                     //                               1 - non inverted waveform
	USHORT                     usMaxNBVoltage;
	USHORT                     usMinNBVoltage;
	USHORT                     usBootUpNBVoltage;
	ULONG                      ulHTLinkFreq; //in 10Khz
	USHORT                     usMinHTLinkWidth; // if no CLMC, usMinHTLinkWidth should be equal to usMaxHTLinkWidth??
	USHORT                     usMaxHTLinkWidth;
	USHORT                     usUMASyncStartDelay; // will be same as usK8SyncStartDelay on RS690
	USHORT                     usUMADataReturnTime; // will be same as usK8DataReturnTime on RS690
	USHORT                     usLinkStatusZeroTime;
	USHORT                     usReserved;
	ULONG                      ulHighVoltageHTLinkFreq; // in 10Khz
	ULONG                      ulLowVoltageHTLinkFreq; // in 10Khz
	USHORT                     usMaxUpStreamHTLinkWidth;
	USHORT                     usMaxDownStreamHTLinkWidth;
	USHORT                     usMinUpStreamHTLinkWidth;
	USHORT                     usMinDownStreamHTLinkWidth;
	ULONG                      ulReserved3[97]; //must be 0x0
} ATOM_INTEGRATED_SYSTEM_INFO_V2;

/* PCIE config flags */
#define	PCIE_DUALSLOT_CONFIG			(1 << 0)
#define	PCIE_OVERCLOCK_ENABLE		(1 << 1)
#define	PCIE_GPP_CLK_GATING			(1 << 2)
#define	PCIE_ENABLE_STATIC_DEV_REMAP	(1 << 3)
#define	PCIE_OFF_UNUSED_GFX_LANES		(1 << 4)
#define	PCIE_OFF_UNUSED_GPP_LANES		(1 << 5)
#define	PCIE_DISABLE_HIDE_UNUSED_PORTS	(1 << 7)
#define	PCIE_GFX_CLK_GATING			(1 << 11)
#define	PCIE_GFX_COMPLIANCE			(1 << 14)
#define	PCIE_GPP_COMPLIANCE			(1 << 15)

/* -------------------- ----------------------
* NBMISCIND
 ------------------- -----------------------*/
#define	PCIE_LINK_CFG			0x8
#define	PCIE_NBCFG_REG7		0x37
#define	STRAPS_OUTPUT_MUX_7		0x67
#define	STRAPS_OUTPUT_MUX_A		0x6a

/* -------------------- ----------------------
* PCIEIND
 ------------------- -----------------------*/
#define	PCIE_CI_CNTL			0x20
#define	PCIE_LC_LINK_WIDTH		0xa2
#define   PCIE_LC_STATE0			0xa5
#define	PCIE_VC0_RESOURCE_STATUS	0x12a	/* 16bit read only */

#define	PCIE_CORE_INDEX_GFX		(0x00 << 16) /* see 5.2.2 */
#define	PCIE_CORE_INDEX_GPPSB		(0x01 << 16)
#define	PCIE_CORE_INDEX_GPP		(0x02 << 16)
#define	PCIE_CORE_INDEX_BRDCST		(0x03 << 16)

/* contents of PCIE_NBCFG_REG7 */
#define   RECONFIG_GPPSB_EN			(1 << 12)
#define	RECONFIG_GPPSB_GPPSB			(1 << 14)
#define   RECONFIG_GPPSB_LINK_CONFIG		(1 << 15)
#define	RECONFIG_GPPSB_ATOMIC_RESET		(1 << 17)

/* contents of PCIE_VC0_RESOURCE_STATUS */
#define	VC_NEGOTIATION_PENDING		(1 << 1)

#define	LC_STATE_RECONFIG_GPPSB		0x10

/* ------------------------------------------------
* Global variable
* ------------------------------------------------- */
extern PCIE_CFG AtiPcieCfg;

#if ENV_RAMSTAGE
/* ----------------- export functions ----------------- */
u32 nbmisc_read_index(struct device * nb_dev, u32 index);
void nbmisc_write_index(struct device * nb_dev, u32 index, u32 data);
u32 nbpcie_p_read_index(struct device * dev, u32 index);
void nbpcie_p_write_index(struct device * dev, u32 index, u32 data);
u32 nbpcie_ind_read_index(struct device * nb_dev, u32 index);
void nbpcie_ind_write_index(struct device * nb_dev, u32 index, u32 data);
u32 htiu_read_index(struct device * nb_dev, u32 index);
void htiu_write_index(struct device * nb_dev, u32 index, u32 data);
u32 nbmc_read_index(struct device * nb_dev, u32 index);
void nbmc_write_index(struct device * nb_dev, u32 index, u32 data);

u32 pci_ext_read_config32(struct device *nb_dev, struct device *dev, u32 reg);
void pci_ext_write_config32(struct device *nb_dev, struct device *dev, u32 reg, u32 mask, u32 val);

void set_nbcfg_enable_bits(struct device * nb_dev, u32 reg_pos, u32 mask, u32 val);
void set_nbcfg_enable_bits_8(struct device * nb_dev, u32 reg_pos, u8 mask, u8 val);
void set_nbmc_enable_bits(struct device * nb_dev, u32 reg_pos, u32 mask, u32 val);
void set_htiu_enable_bits(struct device * nb_dev, u32 reg_pos, u32 mask, u32 val);

void set_nbmisc_enable_bits(struct device * nb_dev, u32 reg_pos, u32 mask, u32 val);

void set_pcie_enable_bits(struct device *dev, u32 reg_pos, u32 mask, u32 val);
void rs780_set_tom(struct device *nb_dev);

void ProgK8TempMmioBase(u8 in_out, u32 pcie_base_add, u32 mmio_base_add);
void enable_pcie_bar3(struct device *nb_dev);
void disable_pcie_bar3(struct device *nb_dev);

void rs780_enable(struct device *dev);
void rs780_gpp_sb_init(struct device *nb_dev, struct device *dev, u32 port);
void rs780_gfx_init(struct device *nb_dev, struct device *dev, u32 port);
void avoid_lpc_dma_deadlock(struct device *nb_dev, struct device *sb_dev);
void config_gpp_core(struct device *nb_dev, struct device *sb_dev);
void PcieReleasePortTraining(struct device *nb_dev, struct device *dev, u32 port);
u8 PcieTrainPort(struct device *nb_dev, struct device *dev, u32 port);

void pcie_hide_unused_ports(struct device *nb_dev);

#endif

u32 extractbit(u32 data, int bit_number);
u32 extractbits(u32 source, int lsb, int msb);
int cpuidFamily(void);
int is_family0Fh(void);
int is_family10h(void);

void enable_rs780_dev8(void);
void rs780_early_setup(void);
void rs780_htinit(void);
int is_dev3_present(void);
void set_pcie_reset(void);
void set_pcie_dereset(void);

#endif /* __RS780_H__ */
