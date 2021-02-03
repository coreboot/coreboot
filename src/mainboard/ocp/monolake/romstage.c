/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google Inc.
 * Copyright (C) 2015 Intel Corp.
 * Copyright (C) 2019 Wiwynn Corp.
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

#include <stddef.h>
#include <soc/romstage.h>
#include <drivers/intel/fsp1_0/fsp_util.h>
#include <drivers/vpd/vpd.h>
#include <device/pci_ops.h>
#include <soc/pci_devs.h>
#include <soc/lpc.h>
#include <soc/gpio.h>


/* Define the strings for UPD variables that could be customized */
#define FSP_VAR_HYPERTHREADING "HyperThreading"

static const struct gpio_config gpio_tables[] = {
	/* PU_BMBUSY_N */
	{0, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* SKU_BDE_ID1 */
	{1, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_BDXDE_ERR0_LVT3_N */
	{2, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_BDXDE_ERR1_LVT3_N */
	{3, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_CPU2PCH_THROT_LVT3 */
	{4, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_BDXDE_CATERR_LVT3_N */
	{5, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* SKU_BDE_ID2 */
	{6, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* REV_BDE_ID0 */
	{7, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* RQ_BMC_PCH_NMI_NOA1_CLK */
	{8, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_USB_OC_5_N */
	{9, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_USB_OC_6_N */
	{10, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_SMBALERT_N */
	{11, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* RQ_IBMC_PCH_SMI_LPC_N */
	{12, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{13, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_LVC3_RISER1_ID4_N_PU */
	{14, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PD_P1V2_VDDQ_SEL_N */
	{15, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_CPU_THROTTLE_N */
	{16, GPIO_MODE_GPIO, GPIO_OUTPUT, GPIO_OUT_LEVEL_HIGH, 0, 0},
	/* SKU_BDE_ID0 */
	{17, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_SRC1CLKRQB */
	{18, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* RST_PCIE_PCH_N */
	{19, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* SMI_BMC_N_R */
	{20, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* M_SATA0GP */
	{21, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* SGPIO_SATA_CLOCK_R */
	{22, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* TP */
	{23, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FAST_THROTTLE_N_R */
	{24, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* BMC_READY_N */
	{25, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* TP */
	{26, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_CPLD */
	{27, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_BDXDE_ME_DRIVE_N */
	{28, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* H_BDXDE_PROCHOT_DISABLE */
	{29, GPIO_MODE_GPIO, GPIO_OUTPUT, GPIO_OUT_LEVEL_HIGH, 0, 0},
	/* SUSPWRDNACK */
	{30, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* SMB_INA230_ALRT_N */
	{31, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* TP */
	{32, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PD_DMI_RX_TERMINATION */
	{33, GPIO_MODE_GPIO, GPIO_OUTPUT, GPIO_OUT_LEVEL_HIGH, 0, 0},
	/* NC */
	{34, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NMI_BDE_R */
	{35, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* FM_BIOS_ADV_FUNCTIONS */
	{36, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_ADR_TRIGGER_N */
	{37, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* SGPIO_SATA_LOAD_R */
	{38, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* SGPIO_SATA_DATAOUT0_R */
	{39, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* FM_USB_OC_1_N */
	{40, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_USB_OC_2_N */
	{41, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_USB_OC_3_N */
	{42, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_USB_OC_4_N */
	{43, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* TP */
	{44, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* TP */
	{45, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_BIOS_POST_CMPLT_N */
	{46, GPIO_MODE_GPIO, GPIO_OUTPUT, GPIO_OUT_LEVEL_HIGH, 0, 0},
	/* NC */
	{47, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_SGPIO_SATA_DATAOUT1 */
	{48, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* FM_XDP_PCH_OBSDATA */
	{49, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_GSXCLK */
	{50, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_GSXDOUT */
	{51, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PD_CPUSV */
	{52, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PD_GSXDIN */
	{53, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_GSXSREST_N */
	{54, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_BIOS_RCVR_BOOT_J2 */
	{55, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{56, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_ME_RCVR_N */
	{57, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* SMB_SML1_3V3SB_CLK	 */
	{58, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* FM_USB_OC_0_N */
	{59, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* SMB_SML0_3V3SB_ALERT */
	{60, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* SLP_SUS_STAT_N */
	{61, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* CLK_CPLD_SUSCLK_R */
	{62, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* NC */
	{63, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{64, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{65, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{66, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{67, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* REV_BDE_ID1 */
	{68, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* TPM_PRSNT_N */
	{69, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{70, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{71, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PU_BATLOW_N */
	{72, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* NC */
	{73, GPIO_MODE_GPIO, GPIO_INPUT, 0, 0, 0},
	/* PCHHOT_CPU_N */
	{74, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	/* SMB_SML1_3V3SB_DAT */
	{75, GPIO_MODE_NATIVE, 0, 0, 0, 0},
	{0xff, GPIO_LIST_END, 0, 0, 0, 0},
};

/**
 * /brief mainboard call for setup that needs to be done before fsp init
 *
 */
void early_mainboard_romstage_entry(void)
{

}

/**
 * /brief mainboard call for setup that needs to be done after fsp init
 *
 */
void late_mainboard_romstage_entry(void)
{
	// IPMI through BIC
	pci_write_config32(PCI_DEV(0, LPC_DEV, LPC_FUNC), LPC_GEN2_DEC,
			   0x0c0ca1);

	// Initialize GPIOs
	init_gpios(gpio_tables);
}

/*
 * This function sets up global variable to store VPD binary blob info,
 * and use settings in the binary blob to configure UPD.
 */
static void board_configure_upd(UPD_DATA_REGION *UpdData)
{
	u8 val;

	if (vpd_get_bool(FSP_VAR_HYPERTHREADING, VPD_RW, &val))
		UpdData->HyperThreading = val;
}

/**
 * /brief customize fsp parameters, use data stored in VPD binary blob
 * to configure FSP UPD variables.
 */
void romstage_fsp_rt_buffer_callback(FSP_INIT_RT_BUFFER *FspRtBuffer)
{
	UPD_DATA_REGION *UpdData = FspRtBuffer->Common.UpdDataRgnPtr;

	if (CONFIG(VPD))
		board_configure_upd(UpdData);
}
