/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_I82801IX_I82801IX_H
#define SOUTHBRIDGE_INTEL_I82801IX_I82801IX_H

#define DEFAULT_TBAR		((u8 *)0xfed1b000)

#include <southbridge/intel/common/rcba.h> /* IWYU pragma: export */

#if CONFIG(BOARD_EMULATION_QEMU_X86_Q35)
/*
 * Qemu has the fw_cfg interface at 0x510.  Move the pmbase to a
 * non-conflicting address.  No need to worry about speedstep, it
 * is not supported by qemu and isn't enabled in the qemu config.
 */
# define DEFAULT_PMBASE		0x00000600
#else
# define DEFAULT_PMBASE		0x00000500 /* Speedstep code has this hardcoded, too. */
#endif
#define DEFAULT_TCOBASE		(DEFAULT_PMBASE + 0x60)
#define DEFAULT_GPIOBASE	0x00000580

#define APM_CNT		0xb2

#define GP_IO_USE_SEL	0x00
#define GP_IO_SEL	0x04
#define GP_LVL		0x0c
#define GPO_BLINK	0x18
#define GPI_INV		0x2c
#define GP_IO_USE_SEL2	0x30
#define GP_IO_SEL2	0x34
#define GP_LVL2		0x38

#define DEBUG_PERIODIC_SMIS	0

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

/* D31:F0 LPC bridge */
#define D31F0_ACPI_CNTL		0x44
#define D31F0_GPIO_BASE		0x48
#define D31F0_GPIO_CNTL		0x4c
#define D31F0_PIRQA_ROUT	0x60
#define D31F0_PIRQB_ROUT	0x61
#define D31F0_PIRQC_ROUT	0x62
#define D31F0_PIRQD_ROUT	0x63
#define D31F0_SERIRQ_CNTL	0x64
#define D31F0_PIRQE_ROUT	0x68
#define D31F0_PIRQF_ROUT	0x69
#define D31F0_PIRQG_ROUT	0x6a
#define D31F0_PIRQH_ROUT	0x6b
#define D31F0_LPC_IODEC		0x80
#define D31F0_LPC_EN		0x82
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
#define D31F0_GEN1_DEC		0x84
#define D31F0_GEN2_DEC		0x88
#define D31F0_GEN3_DEC		0x8c
#define D31F0_GEN4_DEC		0x90
#define D31F0_C5_EXIT_TIMING	0xa8
#define D31F0_CxSTATE_CNF	0xa9
#define D31F0_C4TIMING_CNT	0xaa

/* D31:F2 SATA */
#define D31F2_IDE_TIM_PRI	0x40
#define D31F2_IDE_TIM_SEC	0x42
#define D31F2_SIDX		0xa0
#define D31F2_SDAT		0xa4

/* D30:F0 PCI-to-PCI bridge */
#define D30F0_SMLT		0x1b

/* D28:F0-5 PCIe root ports */
#define D28Fx_XCAP		0x42
#define D28Fx_SLCAP		0x54

/* PCI Configuration Space (D31:F3): SMBus */
#define SMB_BASE		0x20
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

#define RCBA_V0CTL		0x0014
#define RCBA_V1CAP		0x001c
#define RCBA_V1CTL		0x0020
#define RCBA_V1STS		0x0026
#define RCBA_PAT		0x0030
#define RCBA_CIR1		0x0088
#define RCBA_ESD		0x0104
#define RCBA_ULD		0x0110
#define RCBA_ULBA		0x0118
#define RCBA_LCAP		0x01a4
#define RCBA_LCTL		0x01a8
#define RCBA_LSTS		0x01aa
#define RCBA_CIR2		0x01f4
#define RCBA_CIR3		0x01fc
#define RCBA_BCR		0x0220
#define RCBA_DMIC		0x0234
#define RCBA_RPFN		0x0238
#define RCBA_CIR13		0x0f20
#define RCBA_CIR5		0x1d40
#define RCBA_DMC		0x2010
#define RCBA_CIR6		0x2024
#define RCBA_CIR7		0x2034
#define RCBA_HPTC		0x3404
#define GCS			0x3410
#define RCBA_BUC		0x3414
#define RCBA_FD			0x3418 /* Function Disable, see below. */
#define RCBA_CG			0x341c
#define RCBA_FDSW		0x3420
#define RCBA_CIR8		0x3430
#define RCBA_CIR9		0x350c
#define RCBA_CIR10		0x352c
#define RCBA_MAP		0x35f0 /* UHCI controller #6 remapping */

#define BUC_LAND	(1 <<  5) /* LAN */
#define FD_SAD2		(1 << 25) /* SATA #2 */
#define FD_TTD		(1 << 24) /* Thermal Throttle */
#define FD_PE6D		(1 << 21) /* PCIe root port 6 */
#define FD_PE5D		(1 << 20) /* PCIe root port 5 */
#define FD_PE4D		(1 << 19) /* PCIe root port 4 */
#define FD_PE3D		(1 << 18) /* PCIe root port 3 */
#define FD_PE2D		(1 << 17) /* PCIe root port 2 */
#define FD_PE1D		(1 << 16) /* PCIe root port 1 */
#define FD_EHCI1D	(1 << 15) /* EHCI #1 */
#define FD_LBD		(1 << 14) /* LPC bridge */
#define FD_EHCI2D	(1 << 13) /* EHCI #2 */
#define FD_U5D		(1 << 12) /* UHCI #5 */
#define FD_U4D		(1 << 11) /* UHCI #4 */
#define FD_U3D		(1 << 10) /* UHCI #3 */
#define FD_U2D		(1 <<  9) /* UHCI #2 */
#define FD_U1D		(1 <<  8) /* UHCI #1 */
#define FD_U6D		(1 <<  7) /* UHCI #6 */
#define FD_HDAD		(1 <<  4) /* HD audio */
#define FD_SD		(1 <<  3) /* SMBus */
#define FD_SAD1		(1 <<  2) /* SATA #1 */

#ifndef __ACPI__
#ifndef __ASSEMBLER__

#include <device/pci_ops.h>

static inline int lpc_is_mobile(const u16 devid)
{
	return (devid == 0x2917) || (devid == 0x2919);
}
#define LPC_IS_MOBILE(dev) lpc_is_mobile(pci_read_config16(dev, PCI_DEVICE_ID))

void aseg_smm_lock(void);

void i82801ix_early_init(void);
void i82801ix_lpc_setup(void);
void i82801ix_dmi_setup(void);
void i82801ix_dmi_poll_vc1(void);

#endif
#endif

#endif
