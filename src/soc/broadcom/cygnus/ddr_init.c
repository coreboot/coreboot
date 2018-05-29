/*
* Copyright (C) 2015 Broadcom Corporation
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License as
* published by the Free Software Foundation version 2.
*
* This program is distributed "as is" WITHOUT ANY WARRANTY of any
* kind, whether express or implied; without even the implied warranty
* of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#include <delay.h>
#include <console/console.h>
#include <soc/config.h>
#include <soc/reg_utils.h>

#define DDR_CTL_TYPE_1 1
#define DDR_DRAM_TYPE_DDR3L 31

extern unsigned int ddr_init_tab[];
#ifdef DDR2_SUPPORT
extern unsigned int ddr2_init_tab[];
extern unsigned int ddr2_init_tab_400[];
extern unsigned int ddr2_init_tab_667[];
extern unsigned int ddr2_init_tab_800[];
extern unsigned int ddr2_init_tab_1066[];
extern unsigned int ddr2_mode_reg_tab[];
#endif

#if IS_ENABLED(CONFIG_CYGNUS_DDR333)
#define CYGNUS_DRAM_FREQ 333
extern unsigned int ddr3_init_tab_667[];
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR400)
#define CYGNUS_DRAM_FREQ 400
extern unsigned int ddr3_init_tab_800[];
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR533)
#define CYGNUS_DRAM_FREQ 533
extern unsigned int ddr3_init_tab_1066[];
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR667)
#define CYGNUS_DRAM_FREQ 667
extern unsigned int ddr3_init_tab_1333[];
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR800)
#define CYGNUS_DRAM_FREQ 800
extern unsigned int ddr3_init_tab_1600[];
#endif

#define __udelay udelay

/* Local function prototype */
uint32_t change_ddr_clock(uint32_t clk);
void dump_phy_regs(void);
void ddr_init_regs(unsigned int * tblptr);
void ddr_phy_ctl_regs_ovrd(unsigned int * tblptr);
void ddr_phy_wl_regs_ovrd(unsigned int * tblptr);
int is_ddr_32bit(void);
uint32_t iproc_get_ddr3_clock_mhz(uint32_t unit);
int cygnus_phy_powerup(void);
void ddr_init2(void);
void PRE_SRX(void);

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
void PRE_SRX(void)
{
	uint32_t readvalue = 0;

	// Disable low power receivers:  bit 0 of the byte lane STATIC_PAD_CTL register
	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STATIC_PAD_CTL);
	reg32_write ((volatile uint32_t *)DDR_PHY_CONTROL_REGS_STATIC_PAD_CTL, ( readvalue & ~(1 << DDR_PHY_CONTROL_REGS_STATIC_PAD_CTL__RX_MODE_R)));

	// Turn off ZQ_CAL drivers: bits 0,1, and 17 of the ZQ_CAL register (other bits 0 & 1 are set to 1)
	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_CONTROL_REGS_ZQ_CAL);
	reg32_write ((volatile uint32_t *)DDR_PHY_CONTROL_REGS_ZQ_CAL, ( readvalue & ~(1 << DDR_PHY_CONTROL_REGS_ZQ_CAL__ZQ_IDDQ)));

	// Byte lane 0 power up
	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL);
	reg32_write ((volatile uint32_t *)DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL, ( readvalue & ~(1 << DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL__IDLE)));

	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL);
	reg32_write ((volatile uint32_t *)DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL, ( readvalue & 0xffff800f));

	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL);
	reg32_write ((volatile uint32_t *)DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL, ( readvalue & ~(1 << DDR_PHY_BYTE_LANE_0_IDLE_PAD_CONTROL__IDDQ)));

	// Byte lane 1 power up
	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL);
	reg32_write ((volatile uint32_t *)DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL, ( readvalue & ~(1 << DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL__IDLE)));

	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL);
	reg32_write ((volatile uint32_t *)DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL, ( readvalue & 0xffff800f));

	readvalue = reg32_read ((volatile uint32_t *)DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL);
	reg32_write ((volatile uint32_t *)DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL, ( readvalue & ~(1 << DDR_PHY_BYTE_LANE_1_IDLE_PAD_CONTROL__IDDQ)));

	// Turn on PHY_CONTROL AUTO_OEB C not required
	// Enable byte lane AUTO_DQ_RXENB_MODE: bits 18 and 19 of the byte lane IDLE_PAD_CONTROL C already set 180114c8: 000f000a

	printk(BIOS_INFO, "\n....PLL power up.\n");
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG) & ~(1<<DDR_PHY_CONTROL_REGS_PLL_CONFIG__PWRDN)));

	// PLL out of reset
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG) & ~(1<<DDR_PHY_CONTROL_REGS_PLL_CONFIG__RESET)));
	printk(BIOS_INFO, "\n....poll lock..\n");
	// Poll lock
	readvalue = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_STATUS);
	while ( ( readvalue & 0x1) == 0x0 )
	{
		printk(BIOS_INFO, "\n....DDR_PHY_CONTROL_REGS_PLL_STATUS = %8x..\n",readvalue);
		readvalue = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_STATUS);
	}
	printk(BIOS_INFO, "\n....after while..\n");

	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG) & ~(1<<DDR_PHY_CONTROL_REGS_PLL_CONFIG__RESET_POST_DIV)));

	printk(BIOS_INFO, "\n....remove hold..\n");
	// Remove hold
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_CONFIG) & ~(1<<DDR_PHY_CONTROL_REGS_PLL_CONFIG__HOLD)));
	printk(BIOS_INFO, "\n....restore dac..\n");

	// Restore DAC
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VREF_DAC_CONTROL, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_VREF_DAC_CONTROL) & 0xffff0fff));
	printk(BIOS_INFO, "\n....set iddq bit..\n");

	// Set the iddq bit in the idle control register and select all outputs except cke and rst in the idee select registers.
	//	Do NOT assert any other bits in the idle control register.	(This step can be done during init on power up.)
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL) & ~(1 << DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL__IDDQ)));
	printk(BIOS_INFO, "\n....idle pad enable 0..\n");
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_IDLE_PAD_ENABLE0, 0x0);
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_IDLE_PAD_ENABLE1, 0x0);
	printk(BIOS_INFO, "\n....DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL..\n");
	reg32_write((volatile uint32_t *)DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL, (reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL) & ~(1 << DDR_PHY_CONTROL_REGS_IDLE_PAD_CONTROL__IDLE)));
}

#endif

#if defined(CONFIG_IPROC_DDR_ECC) && !defined(CONFIG_IPROC_P7)
void iproc_ddr_ovrd_ecc_lane(void)
{
	uint32_t val;

#define SET_OVR_STEP(v) ( 0x30000 | ( (v) & 0x3F ) )    /* OVR_FORCE = OVR_EN = 1, OVR_STEP = v */

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_W);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_W, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_P);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_P, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_R_N);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_N, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_W);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_W, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_W);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_W, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_W);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_W, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_W);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_W, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_DM_W);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_DM_W, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_P);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_P, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT0_R_N);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_N, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_P);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_P, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT1_R_N);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_N, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_P);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_P, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT2_R_N);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_N, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_P);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_P, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT3_R_N);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_N, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);


	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE0_BIT_RD_EN);
	val = SET_OVR_STEP(val & 0xff);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_DATA_DLY);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_DATA_DLY, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_READ_CONTROL);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_CONTROL, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_IDLE_PAD_CONTROL);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_IDLE_PAD_CONTROL, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_DRIVE_PAD_CTL);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_DRIVE_PAD_CTL, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	val = reg32_read((volatile uint32_t *)DDR_PHY_WORD_LANE_0_WR_PREAMBLE_MODE);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_WR_PREAMBLE_MODE, val);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);

	__udelay(200);
	reg32_write((volatile uint32_t *)DDR_PHY_ECC_LANE_READ_FIFO_CLEAR, 0x1);
	val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
	__udelay(200);
}

uint32_t iproc_read_ecc_syndrome(void)
{
	volatile uint32_t syndrome = 0;
	/* Place uncorrectible as bits 7:0, and correctible as 15:8 */
	syndrome = ((reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) >> 3) & 0x1) |
				(((reg32_read((volatile uint32_t *)DDR_DENALI_CTL_89) >> 5) & 0x1));
	return(syndrome);
}

void iproc_clear_ecc_syndrome(void)
{
	uint32_t val;

	/* Clear the interrupts, bits 6:3 */
	reg32_write((volatile uint32_t *)DDR_DENALI_CTL_213, (1 << 5) | (1<< 3));
	__udelay(1000);
}
#endif

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
uint32_t iproc_get_ddr3_clock_mhz(uint32_t unit)
{
	uint32_t ndiv, mdiv, pdiv, ddrclk, data;

	data = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_PLL_DIVIDERS);

	ndiv = data >> DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__NDIV_INT_R;
	ndiv &= (2^DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__NDIV_INT_WIDTH) - 1;

	pdiv = data >> DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__PDIV_R;
	pdiv &= (2^DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__PDIV_WIDTH) - 1;

	mdiv = data >> DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__MDIV_R;
	mdiv &= (2^DDR_PHY_CONTROL_REGS_PLL_DIVIDERS__MDIV_WIDTH) - 1;

	/* read ndiv pdiv and mdiv */
	ddrclk = (25 * ndiv * 2 * pdiv) / mdiv;
	printk(BIOS_INFO, "%s DDR PHY PLL divisor: ndiv(0x%x) mdiv(0x%x) ddrclk(0x%x)\n", __FUNCTION__, ndiv, mdiv, ddrclk);

	return(ddrclk);
}

#endif

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)

int cygnus_phy_powerup(void)
{
	int data;
	int count = 15;

	data = reg32_read((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL);

	if (reg32_read((volatile uint32_t *)CRMU_IHOST_POR_WAKEUP_FLAG)==0)
	{
		/* Step 1: POWER ON */
		data = reg32_read((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL);
		data |= 0x8;// assert power ON
		reg32_write((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL, data);

		__udelay(2);

		/* Step 2: POWER OK */
		data |= 0x10;// assert power OK
		reg32_write((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL, data);

		while (count--)
			__udelay(2);

	}
	else
	{
		printk(BIOS_INFO, "DeepSleep wakeup: ddr phy init bypassed 1\n");
	}

	/* Step 3: DFI normal mode */
	data &= ~(0x04);// remove DFI isolation
	reg32_write((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL, data);


	/* Step 4: Enable register access */
	data &= ~(0x02);// remove PHY register isolation
	reg32_write((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL, data);

	data &= ~(0x01);// remove PLL isolation
	reg32_write((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL, data);

	count = 20;
	while (count--)
		__udelay(2);

	if (reg32_read((volatile uint32_t *)CRMU_IHOST_POR_WAKEUP_FLAG)==0)
	{
		/* Step 5: release reset */
		data |= 0x20;// de-assert reset
		reg32_write((volatile uint32_t *)CRMU_DDR_PHY_AON_CTRL, data);
	}
	else
	{
		printk(BIOS_INFO, "DeepSleep wakeup: ddr phy init bypassed 2\n");
	}
	while ((reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x08) != 0x08) {
		//poll DDR_S1_IDM_IO_STATUS__o_phy_pwrup_rsb
	}

	return 0;
}

#endif

uint32_t change_ddr_clock(uint32_t clk)
{
	return(0);
}

void dump_phy_regs(void)
{
	int i;
	printk(BIOS_DEBUG, "\n PHY register dump: Control registers\n");
	for (i = 0; i <= 0x94; i+=4)
	{
		printk(BIOS_DEBUG, "0x%03x,\t0x%08x,\n", i,
			*(volatile uint32_t *)(DDR_PHY_CONTROL_REGS_REVISION + i));
	}

	printk(BIOS_DEBUG, "\n PHY register dump: Wordlane0 registers\n");
	for (i = 0; i <= 0xc5; i+=4)
	{
		printk(BIOS_DEBUG, "0x%03x,\t0x%08x,\n", i,
			*(volatile uint32_t *)(DDR_PHY_BYTE_LANE_0_VDL_CONTROL_WR_DQS_P + i));
	}

	return;
}

void ddr_init_regs(unsigned int * tblptr)
{
	unsigned int offset = *tblptr;
	unsigned int *addr = (unsigned int *)DDR_DENALI_CTL_00;

	while (offset != 0xffffffff) {
		++tblptr;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		addr[offset] = *tblptr;
#else
		addr[offset] = swap_u32(*tblptr);
#endif
		++tblptr;
		offset = *tblptr;
	}
}

void ddr_phy_ctl_regs_ovrd(unsigned int * tblptr)
{
	unsigned int offset = *tblptr;
	unsigned int *addr = (unsigned int *)DDR_PHY_CONTROL_REGS_REVISION;
	unsigned int val;

	while (offset != 0xffffffff) {
		++tblptr;
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
		addr[offset/4] = *tblptr;
#else
		addr[offset/4] = swap_u32(*tblptr);
#endif
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		if (val) ;
		++tblptr;
		offset = *tblptr;
	}
}

void ddr_phy_wl_regs_ovrd(unsigned int * tblptr)
{
}

/*DDR_SHMOO_RELATED_CHANGE*/

#ifdef CONFIG_RUN_DDR_SHMOO
int ReWriteModeRegisters( void )
{
	int nRet = 0;
	int j = 100;

	reg32_clear_bits( (volatile uint32_t *)DDR_DENALI_CTL_89 , 1 << 18 );

	/* Set mode register for MR0, MR1, MR2 and MR3 write for all chip selects */
	reg32_write( (volatile uint32_t *)DDR_DENALI_CTL_43 , (1 << 17) | (1 << 24) | (1 << 25) );

	/* Trigger Mode Register Write(MRW) sequence */
	reg32_set_bits( (volatile uint32_t *)DDR_DENALI_CTL_43 , 1 << 25 );

	do {
		if ( reg32_read( (volatile uint32_t *)DDR_DENALI_CTL_89) & (1 << 18) ) {
			break;
		}
		--j;
	} while ( j );

	if ( j == 0 && (reg32_read( (volatile uint32_t *)DDR_DENALI_CTL_89) & (1 << 18) ) == 0 ) {
		printk(BIOS_ERR, "Error: DRAM mode registers write failed\n");
		nRet = 1;
	};

	return nRet;
}
#endif /* CONFIG_RUN_DDR_SHMOO */


int is_ddr_32bit(void)
{
	int ddr32 = 0;

#if IS_ENABLED(CONFIG_CYGNUS_SHMOO_REUSE_DDR_32BIT)
	ddr32=1;
#endif /* (CONFIG_CYGNUS_SHMOO_REUSE_DDR_32BIT) */

	return ddr32;
}


static uint32_t get_ddr_clock(uint32_t sku_id, int ddr_type)
{
#ifdef CYGNUS_DRAM_FREQ
	return  CYGNUS_DRAM_FREQ;
#else
	#error Please set DDR frequency (CYGNUS_DRAM_FREQ must be set)
#endif
}

#if defined(CONFIG_SHMOO_REUSE) || defined(CONFIG_SHMOO_AND28_REUSE)

#define RAND_MAGIC_1    0x0000444BUL
#define RAND_MAGIC_2    0x88740000UL
#define RAND_MAGIC_3    69069UL
#define RAND_SEED       0x5301beef
#define RAND_SEED_2     ((RAND_SEED << 21) + (RAND_SEED << 14) + (RAND_SEED << 7))
#define RAND_C_INIT     (((RAND_SEED_2 + RAND_MAGIC_1) << 1) + 1)
#define RAND_T_INIT     ((RAND_SEED_2 << (RAND_SEED_2 & 0xF)) + RAND_MAGIC_2)

static int simple_memory_test(void *start, uint32_t len)
{
	register uint32_t rand_c_value, rand_t_value, rand_value;
	register uint32_t i;
	register volatile uint32_t *paddr;

	len /= 4;
	paddr = (volatile uint32_t *)start;
	rand_c_value = RAND_C_INIT;
	rand_t_value = RAND_T_INIT;
	for (i=0; i<len; i++, paddr++) {
		rand_c_value *= RAND_MAGIC_3;
		rand_t_value ^= rand_t_value >> 15;
		rand_t_value ^= rand_t_value << 17;
		rand_value = rand_t_value ^ rand_c_value;
		*paddr = rand_value;
	}

	paddr = (volatile uint32_t *)start;
	rand_c_value = RAND_C_INIT;
	rand_t_value = RAND_T_INIT;
	for (i=0; i<len; i++, paddr++) {
		rand_c_value *= RAND_MAGIC_3;
		rand_t_value ^= rand_t_value >> 15;
		rand_t_value ^= rand_t_value << 17;
		rand_value = rand_t_value ^ rand_c_value;
		if (*paddr != rand_value) {
			return -1;
		}
	}

	return 0;
}

#endif /* CONFIG_SHMOO_REUSE || CONFIG_SHMOO_AND28_REUSE */

#if defined(CONFIG_RUN_DDR_SHMOO2) && defined(CONFIG_SHMOO_REUSE)

#define SHMOO_HEADER_MAGIC      "SHMO"
#define SHMOO_MIN_BLOCK_SIZE    0x10000

static const uint16_t ddr_phy_ctl_regs[] = {
	0x030,
	0x034,
	0x06c
};

static const uint16_t ddr_phy_wl_regs[] = {
	0x000,
	0x004,
	0x008,
	0x00c,
	0x010,
	0x014,
	0x018,
	0x01c,
	0x020,
	0x024,
	0x028,
	0x02c,
	0x030,
	0x034,
	0x038,
	0x03c,
	0x040,
	0x044,
	0x048,
	0x04c,
	0x050,
	0x054,
	0x058,
	0x05c,
	0x060,
	0x064,
	0x068,
	0x06c,
	0x070,
	0x074,
	0x0a4,
	0x0a8,
	0x0ac,
	0x0b0,
	0x0b4,
	0x0b8,
	0x0bc,
	0x0c0,
	0x0c4,
	0x0c8,
	0x0cc,
	0x0d0,
	0x0d4,
	0x0d8,
	0x0dc,
	0x0e0,
	0x0e4,
	0x0e8,
	0x0ec,
	0x0f0,
	0x0f4,
	0x0f8,
	0x0fc,
	0x100,
	0x104,
	0x108,
	0x10c,
	0x110,
	0x114,
	0x118,
	0x11c,
	0x120,
	0x124,
	0x128,
	0x12c,
	0x130,
	0x134,
	0x138,
	0x13c,
	0x140,
	0x144,
	0x148,
	0x14c,
	0x150,
	0x154,
	0x158,
	0x15c,
	0x160,
	0x164,
	0x168,
	0x16c,
	0x1a0,
	0x1a4,
	0x1a8,
	0x1ac,
	0x1b0
};
#if defined(CONFIG_IPROC_DDR_ECC) && !defined(CONFIG_IPROC_P7)
static const uint16_t ddr_phy_eccl_regs[] = {
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_W_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_P_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_R_N_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_W_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_W_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_W_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_W_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_DM_W_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_P_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT0_R_N_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_P_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT1_R_N_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_P_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT2_R_N_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_P_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT3_R_N_BASE,
	DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN_BASE,
	DDR_PHY_ECC_LANE_READ_DATA_DLY_BASE,
	DDR_PHY_ECC_LANE_READ_CONTROL_BASE,
	DDR_PHY_ECC_LANE_IDLE_PAD_CONTROL_BASE,
	DDR_PHY_ECC_LANE_DRIVE_PAD_CTL_BASE,
	DDR_PHY_ECC_LANE_WR_PREAMBLE_MODE_BASE,
};
#endif
#if defined(CONFIG_IPROC_NAND) && defined(CONFIG_ENV_IS_IN_NAND) && CONFIG_ENV_IS_IN_NAND

static int write_shmoo_to_flash(void *buf, int length)
{
	nand_info_t *nand;
	int ret = 0;
	uint32_t offset = CONFIG_SHMOO_REUSE_NAND_OFFSET;
	uint32_t end = offset + CONFIG_SHMOO_REUSE_NAND_RANGE;
	uint32_t blksize;

	/* Get flash handle */
	nand = &nand_info[0];
	if (nand->size < offset || nand->writesize == 0 || nand->erasesize == 0) {
		printk(BIOS_ERR, "Failed to initialize NAND flash for saving Shmoo values!\n");
		return -1;
	}

	/* For NAND with bad blocks, we always erase all blocks in the range */
	{
		nand_erase_options_t opts;
		memset(&opts, 0, sizeof(opts));
		opts.offset = offset / nand->erasesize * nand->erasesize;
		opts.length = (CONFIG_SHMOO_REUSE_NAND_RANGE - 1) / nand->erasesize * nand->erasesize + 1;
		opts.quiet  = 1;
		ret = nand_erase_opts(nand, &opts);
		if (ret) {
			printk(BIOS_ERR, "NAND flash erase failed, error=%d\n", ret);
			return ret;
		}
	}

	/* Write data */
	blksize = nand->erasesize > SHMOO_MIN_BLOCK_SIZE?
	nand->erasesize : SHMOO_MIN_BLOCK_SIZE;
	while (offset < end) {
		if (nand_block_isbad(nand, offset)) {
			offset += blksize;
			continue;
		}
		ret = nand_write(nand, offset, (size_t *)&length, (u_char *)buf);
		if (ret) {
			printk(BIOS_ERR, "NAND flash write failed, error=%d\n", ret);
		}
		break;
	}

	return ret;
}

#elif defined(CONFIG_SPI_FLASH) && defined(CONFIG_ENV_IS_IN_SPI_FLASH) && CONFIG_ENV_IS_IN_SPI_FLASH

static int write_shmoo_to_flash(void *buf, int length)
{
	struct spi_flash flash;
	int erase = 0;
	volatile uint32_t *flptr;
	int i, j, ret = 0;
	uint32_t offset = CONFIG_SHMOO_REUSE_QSPI_OFFSET;

	/* Check if erasing is required */
	flptr = (volatile uint32_t *)(IPROC_QSPI_MEM_BASE + offset / 4 * 4);
	j = (length - 1) / 4 + 1;
	for (i=0; i<j; i++, flptr++) {
		if (*flptr != 0xFFFFFFFF) {
		erase = 1;
		break;
		}
	}

	/* Probe flash */
	if (spi_flash_probe(CONFIG_ENV_SPI_BUS, CONFIG_ENV_SPI_CS, &flash)) {
		printk(BIOS_ERR, "Failed to initialize SPI flash for saving Shmoo values!\n");
		return -1;
	}

	/* Erase if necessary */
	if (erase) {
		ret = spi_flash_erase(
			&flash,
			offset / flash->sector_size * flash->sector_size,
			flash->sector_size
			);
		if (ret) {
			printk(BIOS_ERR, "SPI flash erase failed, error=%d\n", ret);
			return ret;
		}
	}

	/* Write data */
	ret = spi_flash_write(&flash, offset, length, buf);
	if (ret) {
		printk(BIOS_ERR, "SPI flash write failed, error=%d\n", ret);
	}

	return ret;
}

#elif defined(CONFIG_ENV_IS_IN_FLASH)

static int write_shmoo_to_flash(void *buf, int length)
{
	int erase = 0;
	volatile uint32_t *flptr, shmoo_start;
	int i, j, ret = 0;
	uint32_t offset = CONFIG_SHMOO_REUSE_NOR_OFFSET;
	int sect_len;

	/* Check if erasing is required */
	flptr = (volatile uint32_t *)(IPROC_NOR_MEM_BASE + offset / 4 * 4);
	shmoo_start = flptr;
	j = (length - 1) / 4 + 1;
	for (i=0; i<j; i++, flptr++) {
		if (*flptr != 0xFFFFFFFF) {
			erase = 1;
			break;
		}
	}

	sect_len = (((length / 0x20000) + 1)*0x20000 - 1);
	/* Erase if necessary */
	if (erase) {
		ret = flash_sect_erase((ulong)shmoo_start, (ulong)shmoo_start + sect_len);
		if (ret) {
			printk(BIOS_ERR, "NOR flash erase failed, error=%d, start addr: 0x%x, end addr: 0x%x\n",
		                ret, (ulong)shmoo_start, (ulong)shmoo_start + sect_len);
			return ret;
		}
	}

	/* Write data */
	ret = flash_write((char *)buf, (ulong)shmoo_start, length);

	if (ret) {
		printk(BIOS_ERR, "NOR flash write failed, error=%d\n", ret);
	}


	return ret;

}
#else
 #error Flash (SPI or NAND) must be enabled
#endif

/* Return flash pointer; or NULL if validation failed */
static volatile uint32_t *validate_flash_shmoo_values(struct shmoo_signature *psig, int *ppairs)
{
	uint32_t dev_id, sku_id, ddr_type, ddr_clk;
	volatile uint32_t *ptr;
	volatile uint32_t *flptr;
	struct shmoo_signature sig;
	uint32_t checksum, pairs, length;
	uint32_t chksum;
	int offset;
	int i;
	int numpairs = 1;

	if (is_ddr_32bit()) {
		numpairs = 2;
	}

    /* Calculate required length (register/value pair) */
	pairs =
	sizeof(ddr_phy_ctl_regs) / sizeof(ddr_phy_ctl_regs[0]) +
	sizeof(ddr_phy_wl_regs) / sizeof(ddr_phy_wl_regs[0]) * numpairs;
#ifdef CONFIG_IPROC_DDR_ECC
	pairs += sizeof(ddr_phy_eccl_regs) / sizeof(ddr_phy_eccl_regs[0]);
#endif

	if (ppairs != NULL) {
		*ppairs = pairs;
	}

#if defined(CONFIG_ENV_IS_IN_NAND) && CONFIG_ENV_IS_IN_NAND
	/* Read SHMOO data from NAND */
	flptr = (volatile uint32_t *)(IPROC_NAND_MEM_BASE + CONFIG_SHMOO_REUSE_NAND_OFFSET);
	offset = (CONFIG_SHMOO_REUSE_NAND_RANGE - 1) / SHMOO_MIN_BLOCK_SIZE * SHMOO_MIN_BLOCK_SIZE;
#elif defined(CONFIG_ENV_IS_IN_FLASH)
	/* Read SHMOO data from NOR */
	flptr = (volatile uint32_t *)(IPROC_NOR_MEM_BASE + CONFIG_SHMOO_REUSE_NOR_OFFSET);
	offset = 0;
#else
	/* Read SHMOO data from SPI */
	flptr = (volatile uint32_t *)(IPROC_QSPI_MEM_BASE + CONFIG_SHMOO_REUSE_QSPI_OFFSET);
	offset = 0;
#endif

	/* Get chip type and DDR type/clock */
	dev_id = (reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff;
	sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 2) & 0x03;
	ddr_type = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x1;
	ddr_clk = get_ddr_clock(sku_id, ddr_type);

	/* Construct signature */
	memcpy(sig.magic, SHMOO_HEADER_MAGIC, 4);
	sig.dev_id = dev_id;
	sig.sku_id = sku_id;
	sig.ddr_type = ddr_type;
	sig.ddr_clock = ddr_clk;

	/* Provide signature data to caller */
	if (psig) {
		memcpy(psig, &sig, sizeof(sig));
	}

	/* Check signature (in min-blocks from bottom) */
	while (offset >= 0) {
		ptr = flptr + offset;
		if (!shmoo_sigmemcmp(&sig,(void *)ptr)) {
			break;
		}
		offset -= SHMOO_MIN_BLOCK_SIZE;
	}
	if (offset < 0) {
		printk(BIOS_ERR, " Signature mismatch ");
		return NULL;
	}
	ptr += 3;

	/* Verify checksum */
	checksum = *ptr++;
	length = *ptr++;
	if (pairs != length) {
		/* Pair count unmatched */
		printk(BIOS_ERR, " Pair count mismatch pairs %x length %x",pairs, length);
		return NULL;
	}
	chksum = 0;
	for (i=0; i<length * 2; i++, ptr++) {
		chksum += *ptr;
	}
	if (chksum != checksum) {
		printk(BIOS_ERR, " Checksum mismatch cksum: %x checksum:%x",chksum,checksum);
		return NULL;
	}

	return flptr + offset;
}

static int try_restore_shmoo(void)
{
    int invalid = 0;
    struct shmoo_signature sig;
    volatile uint32_t *flptr;
    volatile uint32_t *reg;
    uint32_t val;
    int pairs, i;

    /* Validate values in flash */
    printk(BIOS_INFO, "Validate Shmoo parameters stored in flash ..... ");
    flptr = validate_flash_shmoo_values(&sig, &pairs);
    if (flptr == NULL) {
        printk(BIOS_ERR, "failed\n");
        return 1;
    }
    printk(BIOS_INFO, "OK\n");

    /* Check if user wants to skip restoring and run Shmoo */
    if (CONFIG_SHMOO_REUSE_DELAY_MSECS > 0) {
        char c = 0;
        unsigned long start;
        printk(BIOS_INFO, "Press Ctrl-C to run Shmoo ..... ");
        start = get_timer(0);
        while (get_timer(start) <= CONFIG_SHMOO_REUSE_DELAY_MSECS) {
            if (tstc()) {
                c = getc();
                if (c == 0x03) {
                    printk(BIOS_INFO, "Pressed.\n");
                    printk(BIOS_INFO, "Do you want to run the Shmoo? [y/N] ");
                    for (;;) {
                        c = getc();
                        if (c == 'y' || c == 'Y') {
                            printk(BIOS_INFO, "Y\n");
                            invalid = 1;
                            break;
                        } else if (c == '\r' || c == 'n' || c == 'N') {
                            if (c != '\r')
                                printk(BIOS_INFO, "N\n");
                            break;
                        }
                    }
                    break;
                } else {
                    c = 0;
                }
            }
        }
        if (c == 0)
            printk(BIOS_INFO, "skipped\n");
    }

    if (invalid) {
        return 1;
    }

    /* Restore values from flash */
    printk(BIOS_INFO, "Restoring Shmoo parameters from flash ..... ");
    flptr += 5;
    for (i=0; i<pairs; i++) {
        reg = (uint32_t *)(*flptr++);
        val = (uint32_t *)(*flptr++);
	if ( (((uint32_t)reg >= DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN) && ((uint32_t)reg <= (DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN + 0x114)))
#if IS_ENABLED(CONFIG_CYGNUS_SHMOO_REUSE_DDR_32BIT) || defined(CONFIG_NS_PLUS)
		|| (((uint32_t)reg >= DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_EN) && ((uint32_t)reg <= (DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_EN + 0x114)))
#endif
#ifdef CONFIG_IPROC_DDR_ECC
		|| (((uint32_t)reg >= (DDR_DENALI_CTL_00 + DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_RD_EN_BASE)) && ((uint32_t)reg <= (DDR_DENALI_CTL_00 + DDR_PHY_ECC_LANE_VDL_OVRIDE_BYTE_BIT_RD_EN_BASE)))
#endif
	) {
		val |= (1 << 17); /* Force Override */
	}
        // printk(BIOS_INFO, "Writing 0x%x to 0x%x\n",val,reg);
        reg32_write(reg,val);

        reg32_read(reg); /* Dummy read back */
    }
    printk(BIOS_INFO, "done\n");

    /* Perform memory test to see if the parameters work */
    if (CONFIG_SHMOO_REUSE_MEMTEST_LENGTH > 0 ) {
        printk(BIOS_INFO, "Running simple memory test ..... ");
        i = simple_memory_test(
            (void *)CONFIG_SHMOO_REUSE_MEMTEST_START,
            CONFIG_SHMOO_REUSE_MEMTEST_LENGTH);
        if (i) {
            printk(BIOS_ERR, "failed!\n");
            return 1;
        }
        printk(BIOS_INFO, "OK\n");
    }

    return 0;
}

#define SHMOO_REG_BUFFER_SIZE 100;
static uint32_t shmoo_reg_buffer[SHMOO_REG_BUFFER_SIZE];

void iproc_save_shmoo_values(void)
{
    uint32_t *buffer, *ptr;
    volatile uint32_t *flptr;
    uint32_t reg, val;
    struct shmoo_signature sig;
    int pairs, length;
    uint32_t chksum;
    int i;

    /* Check if flash already contains valid data  */
    flptr = validate_flash_shmoo_values(&sig, &pairs);
    if (flptr != NULL) {
        /* Check if the flash data are the same as current DDR PHY values */
        flptr += 5;
	for (i=0; i<pairs; i++) {
	    reg = *flptr++;
	    val = *flptr++;
            if (val != reg32_read(reg)) {
                break;
            }

	}
	if (i == pairs) {
		/* No difference found; Saving skipped */
	    return;
	}
    }

    /* Calculate size of buffer */
    length = 12 +
        sizeof(uint32_t) * 2 +
        sizeof(uint32_t) * pairs * 2;

    /* Allocate buffer */
	if (length > size(uint32_t) * SHMOO_REG_BUFFER_SIZE) {
        printk(BIOS_INFO, "Error pre-allocated shmoo register buffer is not large enough!\n");
        return;
	}

    buffer = shmoo_reg_buffer;
    ptr = buffer;

    /* Fill signature */
    shmoo_sig2mem(&sig,ptr);
    ptr += 5;

    /* Copy registers and values to buffer */
    chksum = 0;
    for (i=0; i<sizeof(ddr_phy_ctl_regs) / sizeof(ddr_phy_ctl_regs[0]); i++) {
        reg = (uint32_t)DDR_PHY_CONTROL_REGS_REVISION + ddr_phy_ctl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        // val = *(volatile uint32_t *)reg;
        val = reg32_read((volatile uint32_t *)reg);
        *ptr++ = val;
        chksum += val;
    }
    for (i=0; i<sizeof(ddr_phy_wl_regs) / sizeof(ddr_phy_wl_regs[0]); i++) {
        reg = (uint32_t)DDR_PHY_WORD_LANE_0_VDL_OVRIDE_BYTE_RD_EN + ddr_phy_wl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        // val = *(volatile uint32_t *)reg;
        val = reg32_read((volatile uint32_t *)reg);
	*ptr++ = val;
        chksum += val;
    }
#if IS_ENABLED(CONFIG_CYGNUS_SHMOO_REUSE_DDR_32BIT) || defined(CONFIG_NS_PLUS)
	if (is_ddr_32bit()) {
	    for (i=0; i<sizeof(ddr_phy_wl_regs) / sizeof(ddr_phy_wl_regs[0]); i++) {
	        reg = (uint32_t)DDR_PHY_WORD_LANE_1_VDL_OVRIDE_BYTE_RD_EN + ddr_phy_wl_regs[i];
	        *ptr++ = reg;
	        chksum += reg;
	        // val = *(volatile uint32_t *)reg;
		val = reg32_read((volatile uint32_t *)reg);
		*ptr++ = val;
	        chksum += val;
	    }
	}
#endif /* (CONFIG_CYGNUS_SHMOO_REUSE_DDR_32BIT || defined(CONFIG_NS_PLUS)) */
#ifdef CONFIG_IPROC_DDR_ECC
    for (i=0; i<sizeof(ddr_phy_eccl_regs) / sizeof(ddr_phy_eccl_regs[0]); i++) {
        reg = (uint32_t)DDR_DENALI_CTL_00 + ddr_phy_eccl_regs[i];
        *ptr++ = reg;
        chksum += reg;
        // val = *(volatile uint32_t *)reg;
        val = reg32_read((volatile uint32_t *)reg);
	*ptr++ = val;
        chksum += val;
    }
#endif

    /* Fill checksum and length */
    buffer[3] = chksum;
    buffer[4] = pairs;

    /* Write to flash */
    printk(BIOS_INFO, "Writing Shmoo values into flash .....\n");
    i = write_shmoo_to_flash(buffer, length);

    /* Free buffer */
//    free(buffer);
}

#endif /* CONFIG_RUN_DDR_SHMOO2 && CONFIG_SHMOO_REUSE */

#include "soc/ddr_bist.h"
#include "soc/shmoo_and28/shmoo_and28.h"

#ifdef CONFIG_IPROC_DDR_ECC
static int clear_ddr(uint32_t offset, uint32_t size)
{
	unsigned long start;
	unsigned int i, val;

    reg32_write((uint32_t *)DDR_BistConfig,reg32_read((uint32_t *)DDR_BistConfig) & ~0x1);

    for ( i = 0; i < 1000; i++);

#if !defined(CONFIG_IPROC_P7)
	reg32_write((volatile uint32_t *)DDR_DENALI_CTL_213, 0x00FFFFFF);
#endif

	reg32_write((volatile uint32_t *)DDR_BistConfig, 0x00000002);
	reg32_write((volatile uint32_t *)DDR_BistConfig, 0x00000003);
	reg32_write((volatile uint32_t *)DDR_BistConfig, 0x0000C003);
	reg32_write((volatile uint32_t *)DDR_BistGeneralConfigurations, 0x00000020);

	val =  255 << DDR_BistConfigurations__WriteWeight_R |
		  0 << DDR_BistConfigurations__ReadWeight_R |
		  1 << DDR_BistConfigurations__ConsAddr8Banks;

	reg32_write((volatile uint32_t *)DDR_BistConfigurations, val);


	reg32_write((volatile uint32_t *)DDR_BistStartAddress, offset);
	reg32_write((volatile uint32_t *)DDR_BistEndAddress, (1 << DDR_BistEndAddress__BistEndAddress_WIDTH) - 1);
	reg32_write((volatile uint32_t *)DDR_BistNumberOfActions, (size + 31) / 32);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord0, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord1, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord2, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord3, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord4, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord5, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord6, 0);
	reg32_write((volatile uint32_t *)DDR_BistPatternWord7, 0);

	reg32_set_bits((volatile uint32_t *)DDR_BistConfigurations, 1 << DDR_BistConfigurations__IndWrRdAddrMode);

	reg32_set_bits((volatile uint32_t *)DDR_BistConfigurations, 1 << DDR_BistConfigurations__BistEn);

	start = get_timer(0);
	while (get_timer(start) <= 10000) {
		if (reg32_read((volatile uint32_t *)DDR_BistStatuses) & (1 << DDR_BistStatuses__BistFinished))
			break;
	}
	/* Clear BIST_EN bit */
	reg32_clear_bits((volatile uint32_t *)DDR_BistConfigurations, 1 << DDR_BistConfigurations__BistEn);

	if ((get_timer(start) <= 10000)  &&
	   (!reg32_read((volatile uint32_t *)DDR_BistErrorOccurred)))
	{
		printk(BIOS_INFO, "clear_ddr: OK\n");
		return(0);
	}
	printk(BIOS_INFO, "clear_ddr: Failed: 0x%lx\n", get_timer(start));
	if (reg32_read((volatile uint32_t *)DDR_BistErrorOccurred))
		printk(BIOS_ERR, "clear_ddr: Error occurred\n");
	return(1);
}
#endif /* CONFIG_IPROC_DDR_ECC */

#if defined(CONFIG_SHMOO_AND28_REUSE)
extern void restore_shmoo_config(and28_shmoo_config_param_t *shmoo_control_para);
#endif

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
static int simple_ddr_crc32_check(void)
{
	return 0;
	register uint32_t crc_mcu = 0;
	register uint32_t crc = 0, offset = 0;
	register volatile uint32_t *buf = (uint32_t *)0x60000000;
	register uint32_t len = 0x00100000;//in word

	printk(BIOS_INFO, "Checking simple DDR CRC, word start 0x%p, len 0x%08x...\n", buf, len);

	for (offset=0; offset<len; offset++)
	{
		crc ^= *buf++;
	}

	crc_mcu = reg32_read((volatile uint32_t *)0x03012A00);

	if (crc != crc_mcu)
	{
		printk(BIOS_ERR, "DDR CRC NOT match, old=0x%08x, new=0x%08x!\n", crc_mcu, crc);
		return -1;
	}
	else
	{
		printk(BIOS_INFO, "DDR CRC 0x%08x, match!\n", crc);
		return 0;
	}
}
#endif

void ddr_init2(void)
{
	int i;
	volatile unsigned int val;
	int ddr_type;
	uint32_t status, sku_id, ddr_clk, dev_id = 0;
	uint32_t unit = 0;
	uint32_t skip_shmoo = 0;
#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	uint32_t pwrctli0 = reg32_read((volatile uint32_t *)IHOST_SCU_POWER_STATUS)  & 0x3;
	skip_shmoo = reg32_read((volatile uint32_t *)CRMU_IHOST_POR_WAKEUP_FLAG) & 0x1;

	if (pwrctli0==2)
	{
		goto wakeup;
	}
	else if (pwrctli0==3)
	{
		skip_shmoo = 1;
		reg32_write((volatile uint32_t *)IHOST_GTIM_GLOB_CTRL, reg32_read((volatile uint32_t *)IHOST_GTIM_GLOB_CTRL)| 0x1);
	}
#endif	/* IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS) */

	dev_id = dev_id;
#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	and28_shmoo_dram_info_t sdi;
	and28_shmoo_config_param_t config_param;
#endif

#if !IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	dev_id = (reg32_read((volatile uint32_t *)ChipcommonA_ChipID)) & 0x0000ffff;
#else
    dev_id = 0x5800;
    cygnus_phy_powerup();
#endif

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 8) & 0x0f;
#else
	sku_id = (reg32_read((volatile uint32_t *)ROM_S0_IDM_IO_STATUS) >> 2) & 0x03;
#endif
	/* See if it is KATANA2, KATANA2 doesn't have right chip ID in ChipcommonA_ChipID */
	if (((sku_id & 0xfff0) == 0xa450) || ((sku_id & 0xfff0) == 0xb450) || sku_id == 0xb248) {
		dev_id = 56450; /* KATANA2 */
	}

	printk(BIOS_INFO, "DEV ID = 0x%x\n", dev_id);

	printk(BIOS_INFO, "SKU ID = 0x%x\n", sku_id);

#if defined(CONFIG_IPROC_P7)
	val = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x3;
	if (val == 0) {
		ddr_type = 1;
	} else if (val == 1) {
		ddr_type = 2;
	} else {
		printk(BIOS_ERR, "Unsupported DDR type: %d\n", val);
		goto done;
	}
    printk(BIOS_INFO, "DDR type: DDR%d\n", (ddr_type == 1)? 3 : 4);
#elif IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
    ddr_type = 1;
#else
	ddr_type = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS) & 0x1;
	printk(BIOS_INFO, "DDR type: DDR%d\n", (ddr_type) ? 3 : 2);
#endif /* defined(CONFIG_IPROC_P7) */

	ddr_clk = get_ddr_clock(sku_id, ddr_type);
	printk(BIOS_INFO, "MEMC 0 DDR speed = %dMHz\n", ddr_clk);

	status = change_ddr_clock(ddr_clk);
	if (status) {
		printk(BIOS_INFO, "CRU LCPLL configuratioin failed\n");
		goto done;
	}

#if defined(CONFIG_IPROC_P7)
	val = reg32_read((volatile uint32_t *)CRU_ddrphy_pwr_ctrl);

	/* assert power ON */
	val |= 1 << CRU_ddrphy_pwr_ctrl__i_pwronin_phy;
	reg32_write((volatile uint32_t *)CRU_ddrphy_pwr_ctrl, val);

	/* assert power OK */
	__udelay(10);
	val |= 1 << CRU_ddrphy_pwr_ctrl__i_pwrokin_phy;
	reg32_write((volatile uint32_t *)CRU_ddrphy_pwr_ctrl, val);

	/* remove DFI isolation */
	__udelay(150);
	val &= ~(1 << CRU_ddrphy_pwr_ctrl__i_iso_phy_dfi);
	reg32_write((volatile uint32_t *)CRU_ddrphy_pwr_ctrl, val);

	/* remove PHY register isolation */
	val &= ~(1 << CRU_ddrphy_pwr_ctrl__i_iso_phy_regs);
	reg32_write((volatile uint32_t *)CRU_ddrphy_pwr_ctrl, val);

	/* remove PLL isolation */
	val &= ~(1 << CRU_ddrphy_pwr_ctrl__i_iso_phy_pll);
	reg32_write((volatile uint32_t *)CRU_ddrphy_pwr_ctrl, val);

	/* de-assert reset */
	__udelay(200);
	val |= 1 << CRU_ddrphy_pwr_ctrl__i_hw_reset_n;
	reg32_write((volatile uint32_t *)CRU_ddrphy_pwr_ctrl, val);

	/* Wait for PHY power up */
	for (i=0; i < 0x19000; i++) {
		val = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS);
		if ((val & (1 << DDR_S1_IDM_IO_STATUS__o_phy_pwrup_rsb)))
			break;
	}
	if (i == 0x19000) {
		printk(BIOS_ERR, "DDR PHY not power up\n");
		goto done;
	}
#endif /* defined(CONFIG_IPROC_P7) */

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS) || defined(CONFIG_IPROC_P7)
	/* Get the DDR S1 and S2 out of reset */
	reg32_write((volatile uint32_t *)DDR_S1_IDM_RESET_CONTROL, 0);
	reg32_write((volatile uint32_t *)DDR_S2_IDM_RESET_CONTROL, 0);

	__udelay(1000);
	reg32_write((volatile uint32_t *)DDR_S0_IDM_RESET_CONTROL, 0);
	/* Set the ddr_ck to 400 MHz, 2x memc clock */
	reg32_write_masked((volatile uint32_t *)DDR_S1_IDM_IO_CONTROL_DIRECT, 0xfff << 16, /*ddr_clk*/ 0x190 << 16);

	if (pwrctli0==3)
	{
		printk(BIOS_INFO, "\n PRE_SRX call\n");
		PRE_SRX();
	}
#else
    reg32_write((volatile uint32_t *)DDR_S1_IDM_RESET_CONTROL, 0);
    reg32_write((volatile uint32_t *)DDR_S2_IDM_RESET_CONTROL, 0);
    /* Set the ddr_ck to 400 MHz, 2x memc clock */
    reg32_write_masked((volatile uint32_t *)DDR_S1_IDM_IO_CONTROL_DIRECT, 0xfff << 16, /*ddr_clk*/ 0x190 << 16);
#endif /* IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS) || defined(CONFIG_IPROC_P7) */

#if defined(CONFIG_IPROC_P7)
	if (is_ddr_32bit()) {
		reg32_write_masked(
			(volatile uint32_t *)DDR_S2_IDM_IO_CONTROL_DIRECT,
				1 << DDR_S2_IDM_IO_CONTROL_DIRECT__mode_32b,
				1 << DDR_S2_IDM_IO_CONTROL_DIRECT__mode_32b
				);
	}

	/* Wait for PHY ready */
	for (i=0; i < 0x19000; i++) {
		val = reg32_read((volatile uint32_t *)DDR_S1_IDM_IO_STATUS);
		if ((val & (1 << DDR_S1_IDM_IO_STATUS__o_phy_ready)))
			break; /* DDR PHY is up */
	}

	if (i == 0x19000) {
		printk(BIOS_ERR, "DDR PLL not locked\n");
		goto done;
	}

	/* Get the DDR S0 out of reset */
	reg32_write((volatile uint32_t *)DDR_S0_IDM_RESET_CONTROL, 0);
#endif /* defined(CONFIG_IPROC_P7) */

	/* Wait for DDR PHY up */
	for (i=0; i < 0x19000; i++) {
		val = reg32_read((volatile uint32_t *)DDR_PHY_CONTROL_REGS_REVISION);
		if ( val != 0) {
            printk(BIOS_INFO, "PHY revision version: 0x%08x\n", val);
			break; /* DDR PHY is up */
        }
	}

	if (i == 0x19000) {
		printk(BIOS_ERR, "DDR PHY is not up\n");
		return;
	}

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	if (!skip_shmoo)
	{
		printk(BIOS_INFO, "ddr_init2: Calling soc_and28_shmoo_dram_info_set\n");
		/* Cygnus clock speed:
		*
		*    clock       rate
		*    400         800
		*    533         1066
		*    667         1333
		*    800         1600
		*/
		sdi.data_rate_mbps = (ddr_clk == 667) ? 1333 : ((ddr_clk == 333) ? 667 : (ddr_clk << 1));
		sdi.ref_clk_mhz = 50;
		sdi.ctl_type = DDR_CTL_TYPE_1;
		sdi.dram_type = DDR_DRAM_TYPE_DDR3L;
		sdi.dram_bitmap = 0x00000001;
		sdi.interface_bitwidth = SDI_INTERFACE_BITWIDTH;
		sdi.num_columns = SDI_NUM_COLUMNS;
		sdi.num_rows = SDI_NUM_ROWS;
		sdi.num_banks = SDI_NUM_BANKS;
		sdi.refi = 7800;
		sdi.command_parity_latency = 0;
		sdi.sim_system_mode = 0;
		printk(BIOS_INFO, "ddr_init2: Calling soc_and28_shmoo_dram_info_set\n");
		soc_and28_shmoo_dram_info_set(unit, &sdi);
	}
	else
	{
		printk(BIOS_INFO, "DeepSleep wakeup: ddr init bypassed 1\n");
	}
#else
#error "DRAM config is not set"
#endif

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	if (!skip_shmoo)
	{
		printk(BIOS_INFO, "ddr_init2: Calling soc_and28_shmoo_phy_init\n");
		if (soc_and28_shmoo_phy_init(unit, 0) != SOC_E_NONE) {

			printk(BIOS_ERR, "DDR PHY initialization failed\n");
			goto done;
		}
	}
	else
	{
		printk(BIOS_INFO, "DeepSleep wakeup: ddr init bypassed 2\n");
	}
#endif

#ifdef CONFIG_RUN_DDR_SHMOO
	printk(BIOS_DEBUG, "PHY register dump after DDR PHY init\n");
	dump_phy_regs();
#endif

	printk(BIOS_INFO, "Programming controller register\n");
	ddr_init_regs(ddr_init_tab);

    ddr_type = 1;
	if (ddr_type) {
		/* DDR3 */
		switch(ddr_clk) {
#if IS_ENABLED(CONFIG_CYGNUS_DDR333)
			case 333:
				ddr_init_regs(ddr3_init_tab_667);
				break;
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR400)
			case 400:
				ddr_init_regs(ddr3_init_tab_800);
				break;
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR533)
			case 533:
				ddr_init_regs(ddr3_init_tab_1066);
				break;
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR667)
			case 667:
				ddr_init_regs(ddr3_init_tab_1333);
				break;
#endif
#if IS_ENABLED(CONFIG_CYGNUS_DDR800)
			case 800:
				ddr_init_regs(ddr3_init_tab_1600);
				break;
#endif
		}
	}

#if IS_ENABLED(CONFIG_CYGNUS_DDR_AUTO_SELF_REFRESH_ENABLE)
#if (DDR_AUTO_SELF_REFRESH_IDLE_COUNT > 0) & (DDR_AUTO_SELF_REFRESH_IDLE_COUNT <= 0xff)
	/* Enable auto self-refresh */
	reg32_set_bits((unsigned int *)DDR_DENALI_CTL_57,
		0x2 << DDR_DENALI_CTL_57__LP_AUTO_EXIT_EN_R |
		0x2 << DDR_DENALI_CTL_57__LP_AUTO_ENTRY_EN_R );

	reg32_set_bits((unsigned int *)DDR_DENALI_CTL_58,
		DDR_AUTO_SELF_REFRESH_IDLE_COUNT << DDR_DENALI_CTL_58__LP_AUTO_SR_IDLE_R);
#else
	#error DDR_AUTO_SELF_REFRESH_IDLE_COUNT out of range
#endif
#else
	/* Disable auto-self refresh */
	reg32_clear_bits((unsigned int *)DDR_DENALI_CTL_57,
		0x2 << DDR_DENALI_CTL_57__LP_AUTO_EXIT_EN_R |
		0x2 << DDR_DENALI_CTL_57__LP_AUTO_ENTRY_EN_R );
	reg32_clear_bits((unsigned int *)DDR_DENALI_CTL_58,
		0xff << DDR_DENALI_CTL_58__LP_AUTO_SR_IDLE_R );
#endif

	/* Start the DDR */
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_00, 0x01);

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	if (!skip_shmoo)
	{
		while (!(reg32_read((volatile uint32_t *)DDR_DENALI_CTL_175) & 0x100));
		printk(BIOS_INFO, "ddr_init2: MemC initialization complete\n");

		reg32_set_bits((unsigned int *)DDR_DENALI_CTL_177, 0x00100);
		reg32_write((unsigned int *)DDR_BistConfig, 0x00000002);
		reg32_write((unsigned int *)DDR_BistConfig, 0x00000003);
		reg32_write((unsigned int *)DDR_BistConfig, 0x0000C003);
		reg32_write((unsigned int *)DDR_BistGeneralConfigurations, 0x00000020);

		printk(BIOS_INFO, "ddr_init2: Calling soc_and28_shmoo_ctl\n");
  #if defined(CONFIG_SHMOO_AND28_REUSE)
		if (is_shmoo_data_valid()) {
			restore_shmoo_config(&config_param);
			soc_and28_shmoo_ctl(unit, 0, SHMOO_AND28_SHMOO_RSVP, 0, 1, SHMOO_AND28_ACTION_RESTORE, &config_param);

    #if defined(CONFIG_SHMOO_REUSE_MEMTEST_LENGTH)
			/* Perform memory test to see if the stored SHMMO values work */
			if (CONFIG_SHMOO_REUSE_MEMTEST_LENGTH > 0) {
				/* Release DDR to AXI for memory testing */
				reg32_clear_bits((volatile uint32_t *)DDR_BistConfig, 1 << DDR_BistConfig__axi_port_sel);

				printk(BIOS_INFO, "Running simple memory test ..... ");
				i = simple_memory_test(
					(void *)CONFIG_SHMOO_REUSE_MEMTEST_START,
					CONFIG_SHMOO_REUSE_MEMTEST_LENGTH);
				if (i) {
					printk(BIOS_ERR, "failed!\n");

					/* Connect DDR controller to BIST for SHMOO */
					reg32_set_bits((volatile uint32_t *)DDR_BistConfig, 1 << DDR_BistConfig__axi_port_sel);

					/* Perform full SHMOO since stored values don't work */
					soc_and28_shmoo_ctl(unit, 0, SHMOO_AND28_SHMOO_RSVP, 0, 1, SHMOO_AND28_ACTION_RUN, &config_param);
				} else {
					printk(BIOS_INFO, "OK\n");
				}
			}
    #endif /* defined(CONFIG_SHMOO_REUSE_MEMTEST_LENGTH) */

		} else {
			soc_and28_shmoo_ctl(unit, 0, SHMOO_AND28_SHMOO_RSVP, 0, 1, SHMOO_AND28_ACTION_RUN, &config_param);
		}
  #else
		soc_and28_shmoo_ctl(unit, 0, SHMOO_AND28_SHMOO_RSVP, 0, 1, SHMOO_AND28_ACTION_RUN, &config_param);
  #endif /* CONFIG_SHMOO_AND28_REUSE */
	}
#endif
	else
	{
		printk(BIOS_INFO, "DeepSleep wakeup: ddr init bypassed 3\n");
	}

#if defined(CONFIG_IPROC_P7) && defined(CONFIG_IPROC_DDR_ECC)
	printk(BIOS_INFO, "Enabling DDR ECC correcting and reporting\n");

	/* Clear DDR ECC interrupts if any */
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_177,
		DDR_DENALI_CTL_177_ECC_MASK);

	/* Disable auto corruption */
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_148,
		1 << DDR_DENALI_CTL_148__ECC_DISABLE_W_UC_ERR);

	/* Enable ECC correction and reporting */
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_146,
		1 << DDR_DENALI_CTL_146__ECC_EN);

	/* Initialize DDR so that uninitialized reads won't report ecc error */
	clear_ddr(0, CONFIG_PHYS_SDRAM_1_SIZE);
#elif defined(CONFIG_IPROC_DDR_ECC)
	printk(BIOS_INFO, "Enabling DDR ECC reporting\n");
	/* Clear DDR interrupts if any */
	*(unsigned int *)(DDR_DENALI_CTL_213) = 0x00FFFFFF;
	__udelay(1000);
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_67, 0x01); //Disable auto correction
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_66, 0x01); //Enable ECC

	clear_ddr(0, CONFIG_PHYS_SDRAM_1_SIZE);
	printk(BIOS_INFO, "Enabling DDR ECC correction\n");
	reg32_set_bits((volatile uint32_t *)DDR_DENALI_CTL_66, 1 << 1); //Enable ECC correction
#endif /* defined(CONFIG_IPROC_P7) && defined(CONFIG_IPROC_DDR_ECC) */

	/* Release DDR slave port to AXI */
	reg32_clear_bits((volatile uint32_t *)DDR_BistConfig, 1 << DDR_BistConfig__axi_port_sel);
	printk(BIOS_INFO, "DDR Interface Ready\n");

	//dump_phy_regs();

#if IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS)
	/* SRX */
	if (skip_shmoo)
	{
		// Enter Self refresh (dummy) , to keep Denali happy
		reg32_write((unsigned int *)DDR_DENALI_CTL_56, 0x0a050505);

		__udelay(200);
		printk(BIOS_INFO, "\nDDR self refresh exit\n");

		// Assert DFI request from PHY to mask any interaction with MEMC
		reg32_write((unsigned int *)DDR_PHY_CONTROL_REGS_DFI_CNTRL, 0xe0);
		reg32_write((unsigned int *)DDR_PHY_CONTROL_REGS_DFI_CNTRL, 0);

		// Exit Self refresh
		reg32_write((unsigned int *)DDR_DENALI_CTL_56, 0x09050505);
	}

	/* Clear iHOST flag */
	reg32_write((unsigned int *)CRMU_IHOST_POR_WAKEUP_FLAG, 0x0);
	printk(BIOS_INFO, "IHOST POR WAKEUP FLAG cleared\n");

//	iproc_dump_ddr_regs();

	if (pwrctli0==0)
		goto done;

wakeup:
	printk(BIOS_INFO, "Wakeup from %s\n", pwrctli0==2 ? "SLEEP":"DEEPSLEEP");

	if (pwrctli0==3)
	{
		__udelay(10000);
		if (simple_ddr_crc32_check()<0)
		{
			printk(BIOS_INFO, "Die...\n");
			while (1);
		}
	}

	/* CRMU_IHOST_SW_PERSISTENT_REG4 = 0x03024c64 */
	asm(
		"movw	r3, #0x4c64\n"
		"movt	r3, #0x0302\n"
		"ldr 	r5, [r3]\n"
		"mov	lr, #0\n"
		"mov	pc, r5\n");
#endif /* IS_ENABLED(CONFIG_SOC_BROADCOM_CYGNUS) */

done:
	/* Reclaim everything we have previously allocated for temporary usage. */
//	free_heap();
	return;
}
