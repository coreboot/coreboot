/*
 * This file is part of the coreboot project.
 *
 * Copyright 2013 Google Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; version 2 of
 * the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef INTEL_COMMON_PMUTIL_H
#define INTEL_COMMON_PMUTIL_H

#define D31F0_PMBASE		0x40
#define D31F0_GEN_PMCON_3	0xa4
#define D31F0_GPIO_ROUT		0xb8
#define  GPI_DISABLE		0x00
#define  GPI_IS_SMI		0x01
#define  GPI_IS_SCI		0x02
#define  GPI_IS_NMI		0x03


#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

#ifndef CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL
#define CONFIG_MAINBOARD_POWER_ON_AFTER_POWER_FAIL MAINBOARD_POWER_ON
#endif

#define PM1_STS		0x00
#define   WAK_STS	(1 << 15)
#define   PCIEXPWAK_STS	(1 << 14)
#define   PRBTNOR_STS	(1 << 11)
#define   RTC_STS	(1 << 10)
#define   PWRBTN_STS	(1 << 8)
#define   GBL_STS	(1 << 5)
#define   BM_STS	(1 << 4)
#define   TMROF_STS	(1 << 0)
#define PM1_EN		0x02
#define   PCIEXPWAK_DIS	(1 << 14)
#define   RTC_EN	(1 << 10)
#define   PWRBTN_EN	(1 << 8)
#define   GBL_EN	(1 << 5)
#define   TMROF_EN	(1 << 0)
#define PM1_CNT		0x04
#define   GBL_RLS	(1 << 2)
#define   BM_RLD	(1 << 1)
#define   SCI_EN	(1 << 0)
#define PM1_TMR		0x08
#define PROC_CNT	0x10
#define LV2		0x14
#define LV3		0x15
#define LV4		0x16
#define PM2_CNT		0x50 // mobile only
#define GPE0_STS	0x20
#define   PME_B0_STS	(1 << 13)
#define   PME_STS	(1 << 11)
#define   BATLOW_STS	(1 << 10)
#define   PCI_EXP_STS	(1 << 9)
#define   RI_STS	(1 << 8)
#define   SMB_WAK_STS	(1 << 7)
#define   TCOSCI_STS	(1 << 6)
#define   SWGPE_STS	(1 << 2)
#define   HOT_PLUG_STS	(1 << 1)
#define GPE0_EN		0x28
#define   PME_B0_EN	(1 << 13)
#define   PME_EN	(1 << 11)
#define   TCOSCI_EN	(1 << 6)
#define SMI_EN		0x30
#define   INTEL_USB2_EN	 (1 << 18) // Intel-Specific USB2 SMI logic
#define   LEGACY_USB2_EN (1 << 17) // Legacy USB2 SMI logic
#define   PERIODIC_EN	 (1 << 14) // SMI on PERIODIC_STS in SMI_STS
#define   TCO_EN	 (1 << 13) // Enable TCO Logic (BIOSWE et al)
#define   MCSMI_EN	 (1 << 11) // Trap microcontroller range access
#define   BIOS_RLS	 (1 <<  7) // asserts SCI on bit set
#define   SWSMI_TMR_EN	 (1 <<  6) // start software smi timer on bit set
#define   APMC_EN	 (1 <<  5) // Writes to APM_CNT cause SMI#
#define   SLP_SMI_EN	 (1 <<  4) // Write to SLP_EN in PM1_CNT asserts SMI#
#define   LEGACY_USB_EN  (1 <<  3) // Legacy USB circuit SMI logic
#define   BIOS_EN	 (1 <<  2) // Assert SMI# on setting GBL_RLS bit
#define   EOS		 (1 <<  1) // End of SMI (deassert SMI#)
#define   GBL_SMI_EN	 (1 <<  0) // SMI# generation at all?
#define SMI_STS		0x34
#define ALT_GP_SMI_EN	0x38
#define ALT_GP_SMI_STS	0x3a
#define GPE_CNTL	0x42
#define DEVACT_STS	0x44
#define SS_CNT		0x50
#define C3_RES		0x54
#define TCO1_STS	0x64
#define   DMISCI_STS	(1 << 9)
#define   BOOT_STS	(1 << 18)
#define TCO2_STS	0x66
#define TCO1_CNT	0x68
#define   TCO_LOCK	(1 << 12)
#define TCO2_CNT	0x6a

u16 get_pmbase(void);

u16 reset_pm1_status(void);
void dump_pm1_status(u16 pm1_sts);
void dump_tco_status(u32 tco_sts);
u32 reset_tco_status(void);
void dump_gpe0_status(u64 gpe0_sts);
u64 reset_gpe0_status(void);
void dump_smi_status(u32 smi_sts);
u32 reset_smi_status(void);
void gpe0_mask(u32 clr, u32 set);
void alt_gpi_mask(u16 clr, u16 set);
void smi_set_eos(void);
void dump_alt_gp_smi_status(u16 alt_gp_smi_sts);
u16 reset_alt_gp_smi_status(void);
void southbridge_smm_xhci_sleep(u8 slp_type);
void gpi_route_interrupt(u8 gpi, u8 mode);
void southbridge_gate_memory_reset(void);
void southbridge_update_gnvs(u8 apm_cnt, int *smm_done);
void southbridge_finalize_all(void);
void southbridge_smi_monitor(void);
em64t101_smm_state_save_area_t *smi_apmc_find_state_save(u8 cmd);

#endif /*INTEL_COMMON_PMUTIL_H */
