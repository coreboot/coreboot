/*
 * Copyright (C) 2015 Broadcom Corporation
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
 * Foundation, Inc.
 */

#ifndef __SOC_BROADCOM_CYGNUS_TZ_H__
#define __SOC_BROADCOM_CYGNUS_TZ_H__

#define TZ_STATE_SECURE				0
#define TZ_STATE_NON_SECURE			1

/*
 * Masters (for parameter masters in "tz_set_masters_security")
 */
#define  CYGNUS_M_Pcie0_m0			0x00000003
#define  CYGNUS_M_Pcie1_m0			0x0000000c
#define  CYGNUS_M_cmicd_m0			0x00000030
#define  CYGNUS_M_ext_m0			0x000000c0

#define  CYGNUS_M_ext_m1			(0x00000003 << 8)
#define  CYGNUS_M_sdio_m0			(0x0000000c << 8)
#define  CYGNUS_M_sdio_m1			(0x00000030 << 8)
#define  CYGNUS_M_amac_m0			(0x000000c0 << 8)

#define  CYGNUS_M_amac_m1			(0x00000003 << 16)
#define  CYGNUS_M_usb2h_m0			(0x0000000c << 16)
#define  CYGNUS_M_usb2d_m0			(0x00000030 << 16)
#define  CYGNUS_M_a9jtag_m0			(0x000000c0 << 16)

/*
 * Non virtual slaves (for parameter slave_vector in "tz_set_non_virtual_slaves_security")
 * Note: the order has to match the order in non_virtual_slave_regs[] array from tz.c
 */
#define CYGNUS_S_Ihost_acp			(0x1 << 0)
#define CYGNUS_S_PCIe0_s0			(0x1 << 1)
#define CYGNUS_S_PCIe1_s0			(0x1 << 2)
#define CYGNUS_S_ihost_s0			(0x1 << 3)
#define CYGNUS_S_A9jtag_s0			(0x1 << 4)
#define CYGNUS_S_CMICd_s0			(0x1 << 5)
#define CYGNUS_S_mhost0_s0			(0x1 << 6)
#define CYGNUS_S_mhost1_s0			(0x1 << 7)
#define CYGNUS_S_Crypto_s0			(0x1 << 8)
#define CYGNUS_S_DMU_s0				(0x1 << 9)

/*
 * Peripherals (for parameter slave_vector in "tz_set_periph_security")
 */
#define CYGNUS_periph_APBx_DMU			0x00008000
#define CYGNUS_periph_APBx_cru			0x00004000
#define CYGNUS_periph_APBx_ccg_smbus1		0x00000800
#define CYGNUS_periph_APBx_ccg_gpio		0x00000400
#define CYGNUS_periph_APBx_ccg_watchdog		0x00000200
#define CYGNUS_periph_APBx_ccg_smbus0		0x00000100
#define CYGNUS_periph_APBx_ccg_srab		0x00000080
#define CYGNUS_periph_APBx_ccg_timer3		0x00000040
#define CYGNUS_periph_APBx_ccg_timer2		0x00000020
#define CYGNUS_periph_APBx_ccg_timer1		0x00000010
#define CYGNUS_periph_APBx_ccg_timer0		0x00000008
#define CYGNUS_periph_APBx_ccg_mdio		0x00000004
#define CYGNUS_periph_APBx_ccg_pwm		0x00000002
#define CYGNUS_periph_APBx_icfg			0x00000001

#define CYGNUS_periph_APBy_ccg_spi5		(0x00002000 << 16)
#define CYGNUS_periph_APBy_ccg_spi4		(0x00001000 << 16)
#define CYGNUS_periph_APBy_ccg_spi3		(0x00000800 << 16)
#define CYGNUS_periph_APBy_ccg_spi2		(0x00000400 << 16)
#define CYGNUS_periph_APBy_ccg_spi1		(0x00000200 << 16)
#define CYGNUS_periph_APBy_ccg_spi0		(0x00000100 << 16)
#define CYGNUS_periph_APBy_ccg_uart4		(0x00000010 << 16)
#define CYGNUS_periph_APBy_ccg_uart3		(0x00000008 << 16)
#define CYGNUS_periph_APBy_ccg_uart2		(0x00000004 << 16)
#define CYGNUS_periph_APBy_ccg_uart1		(0x00000002 << 16)
#define CYGNUS_periph_APBy_ccg_uart0		(0x00000001 << 16)

/*
 * Sececure Peripherals (for parameter slave_vector in "tz_set_sec_periphs_security")
 */
#define CYGNUS_sec_periph_APBz_sotp		0x00000020
#define CYGNUS_sec_periph_APBz_tzpc		0x00000010
#define CYGNUS_sec_periph_APBz_ccg_wdog		0x00000008
#define CYGNUS_sec_periph_APBz_ccs_rng		0x00000004
#define CYGNUS_sec_periph_APBz_ccs_pka		0x00000002
#define CYGNUS_sec_periph_APBz_ccs_gpio		0x00000001

/*
 * Cfg Slaves (for parameter slave_vector in "tz_set_cfg_slaves_security")
 */
#define CYGNUS_cfg_slv_APBr_mhost0		0x00004000
#define CYGNUS_cfg_slv_APBr_dma_s1		0x00000200
#define CYGNUS_cfg_slv_APBr_dma_s0		0x00000100
#define CYGNUS_cfg_slv_APBr_acpal		0x00000040
#define CYGNUS_cfg_slv_APBr_smau		0x00000020
#define CYGNUS_cfg_slv_APBr_pcie1		0x00000008
#define CYGNUS_cfg_slv_APBr_pcie0		0x00000004
#define CYGNUS_cfg_slv_APBr_ddr			0x00000001

#define CYGNUS_cfg_slv_APBs_sram		(0x00004000 << 16)
#define CYGNUS_cfg_slv_APBs_usb2d		(0x00001000 << 16)
#define CYGNUS_cfg_slv_APBs_amac1		(0x00000400 << 16)
#define CYGNUS_cfg_slv_APBs_usb2h_apb2		(0x00000200 << 16)
#define CYGNUS_cfg_slv_APBs_usb2h_apb1		(0x00000100 << 16)
#define CYGNUS_cfg_slv_APBs_qspi		(0x00000080 << 16)
#define CYGNUS_cfg_slv_APBs_nand		(0x00000040 << 16)
#define CYGNUS_cfg_slv_APBs_nor			(0x00000020 << 16)
#define CYGNUS_cfg_slv_APBs_sdio1		(0x00000008 << 16)
#define CYGNUS_cfg_slv_APBs_amac0		(0x00000004 << 16)
#define CYGNUS_cfg_slv_APBs_sdio0		(0x00000002 << 16)

/*
 * Wrapper1 (for parameter wrapper1 in "tz_set_wrapper_security")
 */
#define CYGNUS_APB_W1_wrp_master_ihost_m0	0x00000001
#define CYGNUS_APB_W1_wrp_master_pcie0_m0	0x00000002
#define CYGNUS_APB_W1_wrp_master_pcie1_m0	0x00000004
#define CYGNUS_APB_W1_wrp_master_cmicd_m0	0x00000008
#define CYGNUS_APB_W1_wrp_master_mhost0_m0	0x00000010
#define CYGNUS_APB_W1_wrp_master_mhost1_m0	0x00000020
#define CYGNUS_APB_W1_wrp_slave_ihost_acp	0x00000040
#define CYGNUS_APB_W1_wrp_slave_ihost_s0	0x00000080
#define CYGNUS_APB_W1_wrp_slave_pcie0_s0	0x00000100
#define CYGNUS_APB_W1_wrp_slave_pcie1_s0	0x00000200
#define CYGNUS_APB_W1_wrp_slave_cmicd_s0	0x00000400
#define CYGNUS_APB_W1_wrp_slave_smau_s0		0x00000800
#define CYGNUS_APB_W1_wrp_slave_mhost0_s0	0x00001000
#define CYGNUS_APB_W1_wrp_slave_mhost1_s0	0x00002000
#define CYGNUS_APB_W1_wrp_slave_apbr		0x00004000
#define CYGNUS_APB_W1_wrp_master_dma_m0		0x00008000

/*
 * Wrapper2 (for parameter wrapper2 in "tz_set_wrapper_security")
 */
#define CYGNUS_APB_W2_wrp_ds_ds_2		0x00000008
#define CYGNUS_APB_W2_sdrom			0x00000004
#define CYGNUS_APB_W2_wrp_ds_ds_1		0x00000002
#define CYGNUS_APB_W2_wrp_ds_ds_0		0x00000001

/*
 * Wrapper3 (for parameter wrapper3 in "tz_set_wrapper_security")
 */
#define CYGNUS_APB_W3_wrp_master_amac_m1	0x00002000
#define CYGNUS_APB_W3_wrp_ds_ds_3		0x00001000
#define CYGNUS_APB_W3_wrp_slave_apbs		0x00000800
#define CYGNUS_APB_W3_wrp_slave_sram_s0		0x00000400
#define CYGNUS_APB_W3_wrp_master_sdio_m1	0x00000200
#define CYGNUS_APB_W3_wrp_slave_a9jtag_s0	0x00000100
#define CYGNUS_APB_W3_wrp_slave_crypto_s0	0x00000080
#define CYGNUS_APB_W3_wrp_master_usb2h_m0	0x00000040
#define CYGNUS_APB_W3_wrp_master_a9jtag_m0	0x00000020
#define CYGNUS_APB_W3_wrp_master_ext_m1		0x00000010
#define CYGNUS_APB_W3_wrp_master_sdio_m0	0x00000008
#define CYGNUS_APB_W3_wrp_master_ext_m0		0x00000004
#define CYGNUS_APB_W3_wrp_master_usb2d_m0	0x00000002
#define CYGNUS_APB_W3_wrp_master_amac_m0	0x00000001

/*
 * Wrapper4 (for parameter wrapper4 in "tz_set_wrapper_security")
 */
#define CYGNUS_APB_W4_wrp_slave_ext_s1		0x00000040
#define CYGNUS_APB_W4_wrp_slave_dmu_s0		0x00000020
#define CYGNUS_APB_W4_wrp_slave_ext_s0		0x00000010
#define CYGNUS_APB_W4_wrp_master_dmu_m0		0x00000008
#define CYGNUS_APB_W4_wrp_slave_apbz		0x00000004
#define CYGNUS_APB_W4_wrp_slave_apby		0x00000002
#define CYGNUS_APB_W4_wrp_slave_apbx		0x00000001

/*
 * Ext Slaves (for parameter slave_vector in "tz_set_ext_slaves_security")
 */
#define CYGNUS_AXIIC_ext_s0_slave_0	0x00000001	/* LCD */
#define CYGNUS_AXIIC_ext_s0_slave_1	0x00000002	/* Camera */
#define CYGNUS_AXIIC_ext_s0_slave_2	0x00000004
#define CYGNUS_AXIIC_ext_s0_slave_3	0x00000008	/* Smartcard */
#define CYGNUS_AXIIC_ext_s0_slave_4	0x00000010	/* S-GPIO */
#define CYGNUS_AXIIC_ext_s0_slave_5	0x00000020	/* GPIO */
#define CYGNUS_AXIIC_ext_s0_slave_6	0x00000040	/* ADC_APB */
#define CYGNUS_AXIIC_ext_s0_slave_7	0x00000080
#define CYGNUS_AXIIC_ext_s0_slave_8	0x00000100	/* GFX_S0 */
#define CYGNUS_AXIIC_ext_s0_slave_9	0x00000200	/* MIPI */
#define CYGNUS_AXIIC_ext_s0_slave_10	0x00000400	/* ASIU_TOP */
#define CYGNUS_AXIIC_ext_s0_slave_11	0x00000800	/* D1W */
#define CYGNUS_AXIIC_ext_s0_slave_12	0x00001000	/* Keypad */
#define CYGNUS_AXIIC_ext_s0_slave_13	0x00002000	/* Keypad */
#define CYGNUS_AXIIC_ext_s0_slave_14	0x00004000	/* Audio */
#define CYGNUS_AXIIC_ext_s0_slave_15	0x00008000

#define CYGNUS_AXIIC_ext_s1_slave_0	(0x00000001 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_1	(0x00000002 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_2	(0x00000004 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_3	(0x00000008 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_4	(0x00000010 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_5	(0x00000020 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_6	(0x00000040 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_7	(0x00000080 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_8	(0x00000100 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_9	(0x00000200 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_10	(0x00000400 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_11	(0x00000800 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_12	(0x00001000 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_13	(0x00002000 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_14	(0x00004000 << 16)
#define CYGNUS_AXIIC_ext_s1_slave_15	(0x00008000 << 16)

void tz_set_masters_security(uint32_t masters, uint32_t ns_bit);
void tz_set_wrapper_security(uint32_t wrapper1, uint32_t wrapper2,
			     uint32_t wrapper3, uint32_t wrapper4,
			     uint32_t ns_bit);
void tz_set_cfg_slaves_security(uint32_t slave_vector, uint32_t ns_bit);
void tz_set_sec_periphs_security(uint32_t slave_vector, uint32_t ns_bit);
void tz_set_periph_security(uint32_t slave_vector, uint32_t ns_bit);
void tz_set_non_virtual_slaves_security(uint32_t slave_vector, uint32_t ns_bit);
void tz_set_sram_sec_region(uint32_t r0size);
void tz_set_ext_slaves_security(uint32_t slave_vector, uint32_t ns_bit);

#endif
