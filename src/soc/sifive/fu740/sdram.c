/* SPDX-License-Identifier: Apache-2.0 */
/* SPDX-License-Identifier: GPL-2.0-or-later */
/* See the file LICENSE for further information */

#ifndef _SIFIVE_SDRAM_H
#define _SIFIVE_SDRAM_H

#include <console/console.h>
#include <device/mmio.h>
#include <soc/addressmap.h>
#include <soc/sdram.h>
#include <stdint.h>
#include <stddef.h>

#define DRAM_CLASS_OFFSET                 8
#define DRAM_CLASS_DDR4                   0xA
#define OPTIMAL_RMODW_EN_OFFSET           0
#define DISABLE_RD_INTERLEAVE_OFFSET      16
#define OUT_OF_RANGE_FLAG                 (1 << 1)
#define MULTIPLE_OUT_OF_RANGE_FLAG        (1 << 2)
#define PORT_COMMAND_CHANNEL_ERROR_FLAG   (1 << 7)
#define MC_INIT_COMPLETE_FLAG             (1 << 8) // Memory Controller init complete
#define LEVELING_OPERATION_COMPLETED_FLAG (1 << 22)
#define DFI_PHY_WRLELV_MODE_OFFSET        24
#define DFI_PHY_RDLVL_MODE_OFFSET         24
#define DFI_PHY_RDLVL_GATE_MODE_OFFSET    0
#define VREF_EN_OFFSET                    24
#define PORT_ADDR_PROTECTION_EN_OFFSET    0
#define AXI0_ADDRESS_RANGE_ENABLE_OFFSET  8
#define AXI0_RANGE_PROT_BITS_0_OFFSET     24
#define RDLVL_EN_OFFSET                   16
#define RDLVL_GATE_EN_OFFSET              24
#define WRLVL_EN_OFFSET                   0

#define PHY_RX_CAL_DQ0_0_OFFSET           0
#define PHY_RX_CAL_DQ1_0_OFFSET           16

// reference:  fu740-c000 manual chapter 32: DDR Subsystem
// Cahpter 32.2: Memory Map
#define FU740_DDRCTRL         0x100b0000
#define FU740_DDRPHY          0x100b2000
#define FU740_PHYSICAL_FILTER 0x100b8000  // formerly called DDRBUSBLOCKER (FU540)
#define FU740_DDRMGMT         0x100c0000

static void phy_reset(u32 *ddrphyreg, const u32 *physettings)
{
	for (int i = 1152; i <= 1214; i++)
		write32(&ddrphyreg[i], physettings[i]);

	for (int i = 0; i <= 1151; i++)
		write32(&ddrphyreg[i], physettings[i]);
}

static void ux00ddr_writeregmap(u32 *ahbregaddr, const u32 *ctlsettings, const u32 *physettings)
{
	u32 *ddrctlreg = (u32 *) ahbregaddr;
	u32 *ddrphyreg = ((u32 *) ahbregaddr) + (0x2000 / sizeof(u32)); //TODO use FU740_DDRPHY instead

	for (int i = 0; i <= 264; i++)
		write32((void *)&ddrctlreg[i], ctlsettings[i]);

	phy_reset(ddrphyreg, physettings);
}

static void ux00ddr_start(u32 *ahbregaddr, u64 *filteraddr, uint64_t ddrend)
{
	// start calibration and training operation
	setbits32(ahbregaddr, 0x1);

	// wait for memory initialization complete
	// bit 8 of INT_STATUS (DENALI_CTL_132) 0x210
	while (!(read32(&ahbregaddr[132]) & MC_INIT_COMPLETE_FLAG))
		;

	// Disable the BusBlocker in front of the controller AXI slave ports
	write64(filteraddr, 0x0f00000000000000UL | (ddrend >> 2));
//                             ^ RWX + TOR
}

static void ux00ddr_mask_mc_init_complete_interrupt(u32 *ahbregaddr)
{
	// Mask off Bit 8 of Interrupt Status
	// Bit [8] The MC initialization has been completed
	setbits32(&ahbregaddr[136], MC_INIT_COMPLETE_FLAG);
}

static void ux00ddr_mask_outofrange_interrupts(u32 *ahbregaddr)
{
	// Mask off Bit 8, Bit 2 and Bit 1 of Interrupt Status
	// Bit [2] Multiple accesses outside the defined PHYSICAL memory space have occurred
	// Bit [1] A memory access outside the defined PHYSICAL memory space has occurred
	setbits32(&ahbregaddr[136], OUT_OF_RANGE_FLAG | MULTIPLE_OUT_OF_RANGE_FLAG);
}

static void ux00ddr_mask_port_command_error_interrupt(u32 *ahbregaddr)
{
	// Mask off Bit 7 of Interrupt Status
	// Bit [7] An error occurred on the port command channel
	setbits32(&ahbregaddr[136], PORT_COMMAND_CHANNEL_ERROR_FLAG);
}

static void ux00ddr_mask_leveling_completed_interrupt(u32 *ahbregaddr)
{
	// Mask off Bit 22 of Interrupt Status
	// Bit [22] The leveling operation has completed
	setbits32(&ahbregaddr[136], LEVELING_OPERATION_COMPLETED_FLAG);
}

static void ux00ddr_setuprangeprotection(u32 *ahbregaddr, size_t size)
{
	write32(&ahbregaddr[209], 0x0);
	u32 size_16Kblocks = ((size >> 14) & 0x7FFFFF) - 1;
	write32(&ahbregaddr[210], size_16Kblocks);
	write32(&ahbregaddr[212], 0x0);
	write32(&ahbregaddr[214], 0x0);
	write32(&ahbregaddr[216], 0x0);
	setbits32(&ahbregaddr[224], (0x3  <<  AXI0_RANGE_PROT_BITS_0_OFFSET));
	write32(&ahbregaddr[225], 0xFFFFFFFF);
	setbits32(&ahbregaddr[208], (1  <<  AXI0_ADDRESS_RANGE_ENABLE_OFFSET));
	setbits32(&ahbregaddr[208], (1  <<  PORT_ADDR_PROTECTION_EN_OFFSET));
}

static void ux00ddr_disableaxireadinterleave(u32 *ahbregaddr)
{
	setbits32(&ahbregaddr[120], (1 << DISABLE_RD_INTERLEAVE_OFFSET));
}

static void ux00ddr_disableoptimalrmodw(u32 *ahbregaddr)
{
	clrbits32(&ahbregaddr[21], (1 << OPTIMAL_RMODW_EN_OFFSET));
}

static void ux00ddr_enablewriteleveling(u32 *ahbregaddr)
{
	setbits32(&ahbregaddr[170], (1 << WRLVL_EN_OFFSET) | (1 << DFI_PHY_WRLELV_MODE_OFFSET));
}

static void ux00ddr_enablereadleveling(u32 *ahbregaddr)
{
	setbits32(&ahbregaddr[181], (1 << DFI_PHY_RDLVL_MODE_OFFSET));
	setbits32(&ahbregaddr[260], (1 << RDLVL_EN_OFFSET));
}

static void ux00ddr_enablereadlevelinggate(u32 *ahbregaddr)
{
	setbits32(&ahbregaddr[260], (1 << RDLVL_GATE_EN_OFFSET));
	setbits32(&ahbregaddr[182], (1 << DFI_PHY_RDLVL_GATE_MODE_OFFSET));
}

static void ux00ddr_enablevreftraining(u32 *ahbregaddr)
{
	setbits32(&ahbregaddr[184], (1 << VREF_EN_OFFSET));
}

static u32 ux00ddr_getdramclass(u32 *ahbregaddr)
{
	return ((read32(ahbregaddr) >> DRAM_CLASS_OFFSET) & 0xF);
}

static void ux00ddr_phy_fixup(void *ahbregaddr)
{
	void *ddrphyreg = ahbregaddr + 0x2000;

	// bitmask of failed lanes
	uint64_t fails = 0;
	u32 slicebase = 0;
	u32 dq = 0;

	// check errata condition
	for (u32 slice = 0; slice < 8; slice++) {
		u32 regbase = slicebase + 34;
		for (u32 reg = 0 ; reg < 4; reg++) {
			u32 updownreg = read32(ddrphyreg + ((regbase+reg) << 2));
			for (u32 bit = 0; bit < 2; bit++) {
				u32 phy_rx_cal_dqn_0_offset;

				if (bit == 0)
					phy_rx_cal_dqn_0_offset = PHY_RX_CAL_DQ0_0_OFFSET;
				else
					phy_rx_cal_dqn_0_offset = PHY_RX_CAL_DQ1_0_OFFSET;

				u32 down = (updownreg >>  phy_rx_cal_dqn_0_offset)      & 0x3F;
				u32 up =   (updownreg >> (phy_rx_cal_dqn_0_offset + 6)) & 0x3F;

				uint8_t failc0 = ((down == 0) && (up == 0x3F));
				uint8_t failc1 = ((up == 0) && (down == 0x3F));

				// print error message on failure
				if (failc0 || failc1) {
					fails |= (1 << dq);

					char slicelsc = '0';
					char slicemsc = '0';
					slicelsc += (dq % 10);
					slicemsc += (dq / 10);
					printk(BIOS_ERR, "S %c%c%c\n", slicelsc, slicemsc, failc0 ? 'U' : 'D');
				}
				dq++;
			}
		}
		slicebase += 128;
	}
	if (fails)
		printk(BIOS_ERR, "DDR error in fixing up: %llx\n", fails);
}

extern const u32 denali_ddr_phy_data[1215];
extern const u32 denali_ddr_ctl_data[265];

void sdram_init(size_t dram_size)
{
	u32 *ddrctrl = (u32 *)FU740_DDRCTRL;
	u64 *ddr_physical_filter = (u64 *)FU740_PHYSICAL_FILTER;
	ux00ddr_writeregmap(ddrctrl, denali_ddr_ctl_data, denali_ddr_phy_data);
	ux00ddr_disableaxireadinterleave(ddrctrl);

	ux00ddr_disableoptimalrmodw(ddrctrl);

	ux00ddr_enablewriteleveling(ddrctrl);
	ux00ddr_enablereadleveling(ddrctrl);
	ux00ddr_enablereadlevelinggate(ddrctrl);
	if (ux00ddr_getdramclass(ddrctrl) == DRAM_CLASS_DDR4)
		ux00ddr_enablevreftraining(ddrctrl);

	// mask off interrupts for leveling completion
	ux00ddr_mask_leveling_completed_interrupt(ddrctrl);

	ux00ddr_mask_mc_init_complete_interrupt(ddrctrl);
	ux00ddr_mask_outofrange_interrupts(ddrctrl);
	ux00ddr_setuprangeprotection(ddrctrl, dram_size);
	ux00ddr_mask_port_command_error_interrupt(ddrctrl);

	ux00ddr_start(ddrctrl, ddr_physical_filter, FU740_DRAM + dram_size);

	ux00ddr_phy_fixup(ddrctrl);
}

// sdram_init MUST be called before sdram_size
size_t sdram_size(void)
{
	u64 devicepmp0 = read64((u64 *)FU740_PHYSICAL_FILTER);
	return ((devicepmp0 & 0xFFFFFFFFFFFFFF) << 2) - FU740_DRAM;
}

#endif
