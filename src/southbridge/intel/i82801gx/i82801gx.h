/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_I82801GX_I82801GX_H
#define SOUTHBRIDGE_INTEL_I82801GX_I82801GX_H

/* TODO Make sure these don't get changed by stage2 */
#define DEFAULT_GPIOBASE	0x0480
#define DEFAULT_PMBASE		0x0500

#include <southbridge/intel/common/rcba.h> /* IWYU pragma: export */

#ifndef __ACPI__
#define DEBUG_PERIODIC_SMIS 0

#include <device/device.h>
void i82801gx_enable(struct device *dev);

void i82801gx_lpc_setup(void);
void i82801gx_setup_bars(void);
void i82801gx_early_init(void);

void ich7_setup_cir(void);

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

/* PCI Configuration Space (D30:F0): PCI2PCI */
#define PSTS	0x06
#define SMLT	0x1b
#define SECSTS	0x1e
#define INTR	0x3c

#define ICH_PCIE_DEV_SLOT	28

/* PCI Configuration Space (D31:F0): LPC */

#define SERIRQ_CNTL		0x64

#define GEN_PMCON_1		0xa0
#define GEN_PMCON_2		0xa2
#define GEN_PMCON_3		0xa4

#define GPIO_ROUT		0xb8
#define   GPI_DISABLE		0x00
#define   GPI_IS_SMI		0x01
#define   GPI_IS_SCI		0x02
#define   GPI_IS_NMI		0x03

#define FDVCT			0xe4
#define   PCIE_4_PORTS_MAX	(1 << 7)
#define   AHCI_UNSUPPORTED	(1 << 3)

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

#define ACPI_CNTL		0x44
#define   ACPI_EN		(1 << 7)
#define BIOS_CNTL		0xDC
#define GPIO_BASE		0x48 /* LPC GPIO Base Address Register */
#define GPIO_CNTL		0x4C /* LPC GPIO Control Register */
#define   GPIO_EN		(1 << 4)

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63
#define PIRQE_ROUT		0x68
#define PIRQF_ROUT		0x69
#define PIRQG_ROUT		0x6A
#define PIRQH_ROUT		0x6B

#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define   CNF2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define   CNF1_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define   MC_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define   KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define   GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define   GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define   FDD_LPC_EN		(1 << 3)  /* LPC_IO_DEC[12] */
#define   LPT_LPC_EN		(1 << 2)  /* LPC_IO_DEC[9:8] */
#define   COMB_LPC_EN		(1 << 1)  /* LPC_IO_DEC[6:4] */
#define   COMA_LPC_EN		(1 << 0)  /* LPC_IO_DEC[2:0] */

#define GEN1_DEC		0x84
#define GEN2_DEC		0x88
#define GEN3_DEC		0x8c
#define GEN4_DEC		0x90

/* PCI Configuration Space (D31:F1): IDE */
#define INTR_LN			0x3c
#define IDE_TIM_PRI		0x40	/* IDE timings, primary */
#define   IDE_DECODE_ENABLE	(1 << 15)
#define   IDE_SITRE		(1 << 14)
#define   IDE_ISP_5_CLOCKS	(0 << 12)
#define   IDE_ISP_4_CLOCKS	(1 << 12)
#define   IDE_ISP_3_CLOCKS	(2 << 12)
#define   IDE_RCT_4_CLOCKS	(0 <<  8)
#define   IDE_RCT_3_CLOCKS	(1 <<  8)
#define   IDE_RCT_2_CLOCKS	(2 <<  8)
#define   IDE_RCT_1_CLOCKS	(3 <<  8)
#define   IDE_DTE1		(1 <<  7)
#define   IDE_PPE1		(1 <<  6)
#define   IDE_IE1		(1 <<  5)
#define   IDE_TIME1		(1 <<  4)
#define   IDE_DTE0		(1 <<  3)
#define   IDE_PPE0		(1 <<  2)
#define   IDE_IE0		(1 <<  1)
#define   IDE_TIME0		(1 <<  0)
#define IDE_TIM_SEC		0x42	/* IDE timings, secondary */

#define IDE_SDMA_CNT		0x48	/* Synchronous DMA control */
#define   IDE_SSDE1		(1 <<  3)
#define   IDE_SSDE0		(1 <<  2)
#define   IDE_PSDE1		(1 <<  1)
#define   IDE_PSDE0		(1 <<  0)

#define IDE_SDMA_TIM		0x4a

#define IDE_CONFIG		0x54	/* IDE I/O Configuration Register */
#define   SIG_MODE_SEC_NORMAL	(0 << 18)
#define   SIG_MODE_SEC_TRISTATE	(1 << 18)
#define   SIG_MODE_SEC_DRIVELOW	(2 << 18)
#define   SIG_MODE_PRI_NORMAL	(0 << 16)
#define   SIG_MODE_PRI_TRISTATE	(1 << 16)
#define   SIG_MODE_PRI_DRIVELOW	(2 << 16)
#define   FAST_SCB1		(1 << 15)
#define   FAST_SCB0		(1 << 14)
#define   FAST_PCB1		(1 << 13)
#define   FAST_PCB0		(1 << 12)
#define   SCB1			(1 <<  3)
#define   SCB0			(1 <<  2)
#define   PCB1			(1 <<  1)
#define   PCB0			(1 <<  0)

/* PCI Configuration Space (D31:F3): SMBus */
#define SMB_BASE		0x20
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* Southbridge IO BARs */

#define GPIOBASE		0x48

#define PMBASE		0x40

#define VCH		0x0000	/* 32bit */
#define VCAP1		0x0004	/* 32bit */
#define VCAP2		0x0008	/* 32bit */
#define PVC		0x000c	/* 16bit */
#define PVS		0x000e	/* 16bit */

#define V0CAP		0x0010	/* 32bit */
#define V0CTL		0x0014	/* 32bit */
#define V0STS		0x001a	/* 16bit */

#define V1CAP		0x001c	/* 32bit */
#define V1CTL		0x0020	/* 32bit */
#define V1STS		0x0026	/* 16bit */

#define RCTCL		0x0100	/* 32bit */
#define ESD		0x0104	/* 32bit */
#define ULD		0x0110	/* 32bit */
#define ULBA		0x0118	/* 64bit */

#define RP1D		0x0120	/* 32bit */
#define RP1BA		0x0128	/* 64bit */
#define RP2D		0x0130	/* 32bit */
#define RP2BA		0x0138	/* 64bit */
#define RP3D		0x0140	/* 32bit */
#define RP3BA		0x0148	/* 64bit */
#define RP4D		0x0150	/* 32bit */
#define RP4BA		0x0158	/* 64bit */
#define HDD		0x0160	/* 32bit */
#define HDBA		0x0168	/* 64bit */
#define RP5D		0x0170	/* 32bit */
#define RP5BA		0x0178	/* 64bit */
#define RP6D		0x0180	/* 32bit */
#define RP6BA		0x0188	/* 64bit */

#define ILCL		0x01a0	/* 32bit */
#define LCAP		0x01a4	/* 32bit */
#define LCTL		0x01a8	/* 16bit */
#define LSTS		0x01aa	/* 16bit */

#define RPC		0x0224	/* 32bit */
#define RPFN		0x0238	/* 32bit */

/* Get the function number assigned to a Root Port */
#define RPFN_FNGET(reg, port)	(((reg) >> ((port) * 4)) & 7)
/* Set the function number for a Root Port */
#define RPFN_FNSET(port, func)	(((func) & 7) << ((port) * 4))
/* Root Port function number mask */
#define RPFN_FNMASK(port)	(7 << ((port) * 4))

#define TRSR		0x1e00	/*  8bit */
#define TRCR		0x1e10	/* 64bit */
#define TWDR		0x1e18	/* 64bit */

#define IOTR0		0x1e80	/* 64bit */
#define IOTR1		0x1e88	/* 64bit */
#define IOTR2		0x1e90	/* 64bit */
#define IOTR3		0x1e98	/* 64bit */

#define TCTL		0x3000	/*  8bit */

#define D31IP		0x3100	/* 32bit */
#define D30IP		0x3104	/* 32bit */
#define D29IP		0x3108	/* 32bit */
#define D28IP		0x310c	/* 32bit */
#define D27IP		0x3110	/* 32bit */
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define OIC		0x31ff	/*  8bit */

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define FD		0x3418	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable (FD) register values.
 * Setting a bit disables the corresponding
 * feature.
 * Not all features might be disabled on
 * all chipsets. Esp. ICH-7U is picky.
 */
#define ICH_DISABLE_PCIE(x)	(1 << (16 + (x)))
#define FD_EHCI		(1 << 15)
#define FD_LPCB		(1 << 14)

/* UHCI must be disabled from 4 downwards.
 * If UHCI controllers get disabled, EHCI
 * must know about it, too! */
#define ICH_DISABLE_UHCI(x)	(1 << (8 + (x)))

#define FD_INTLAN	(1 <<  7)
#define FD_ACMOD	(1 <<  6)
#define FD_ACAUD	(1 <<  5)
#define FD_HDAUD	(1 <<  4)
#define FD_SMBUS	(1 <<  3)
#define FD_SATA		(1 <<  2)
#define FD_PATA		(1 <<  1)

/* ICH7 PMBASE */
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
#define PM2_CNT		0x20 // mobile only
#define GPE0_STS	0x28
#define   USB4_STS	(1 << 14)
#define   PME_B0_STS	(1 << 13)
#define   USB3_STS	(1 << 12)
#define   PME_STS	(1 << 11)
#define   BATLOW_STS	(1 << 10)
#define   PCI_EXP_STS	(1 << 9)
#define   RI_STS	(1 << 8)
#define   SMB_WAK_STS	(1 << 7)
#define   TCOSCI_STS	(1 << 6)
#define   AC97_STS	(1 << 5)
#define   USB2_STS	(1 << 4)
#define   USB1_STS	(1 << 3)
#define   SWGPE_STS	(1 << 2)
#define   HOT_PLUG_STS	(1 << 1)
#define   THRM_STS	(1 << 0)
#define GPE0_EN		0x2c
#define   PME_B0_EN	(1 << 13)
#define   PME_EN	(1 << 11)
#define SMI_EN		0x30
#define   EL_SMI_EN	 (1 << 25) // Intel Quick Resume Technology
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
#define TCO1_CNT	0x68

#endif /* __ACPI__ */
#endif				/* SOUTHBRIDGE_INTEL_I82801GX_I82801GX_H */
