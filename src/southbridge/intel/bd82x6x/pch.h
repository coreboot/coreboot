/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_BD82X6X_PCH_H
#define SOUTHBRIDGE_INTEL_BD82X6X_PCH_H

#include <acpi/acpi.h>

/* PCH types */
#define PCH_TYPE_CPT	0x1c /* CougarPoint */
#define PCH_TYPE_PPT	0x1e /* IvyBridge */

/* PCH stepping values for LPC device */
#define PCH_STEP_A0	0
#define PCH_STEP_A1	1
#define PCH_STEP_B0	2
#define PCH_STEP_B1	3
#define PCH_STEP_B2	4
#define PCH_STEP_B3	5

#define SMBUS_SLAVE_ADDR	0x24
/* TODO Make sure these don't get changed by stage2 */
#define DEFAULT_GPIOBASE	0x0480
#define DEFAULT_PMBASE		0x0500

#include <southbridge/intel/common/rcba.h> /* IWYU pragma: export */

#if CONFIG(SOUTHBRIDGE_INTEL_BD82X6X)
#define CROS_GPIO_DEVICE_NAME	"CougarPoint"
#elif CONFIG(SOUTHBRIDGE_INTEL_C216)
#define CROS_GPIO_DEVICE_NAME	"PantherPoint"
#endif

#ifndef __ACPI__

int pch_silicon_revision(void);
int pch_silicon_type(void);
void pch_iobp_update(u32 address, u32 andvalue, u32 orvalue);

void enable_usb_bar(void);

void early_thermal_init(void);
void southbridge_configure_default_intmap(void);
void southbridge_rcba_config(void);
/* Optional mainboard hook to do additional configuration
   on the RCBA config space. It is called after the raminit. */
void mainboard_late_rcba_config(void);
/* Optional mainboard hook to do additional LPC configuration
   or to override what is set up by default. */
void mainboard_pch_lpc_setup(void);
void early_pch_init_native(void);
void early_pch_init(void);
void early_pch_init_native_dmi_pre(void);
void early_pch_init_native_dmi_post(void);

struct southbridge_usb_port
{
	int enabled;
	int current;
	int oc_pin;
};

void pch_enable(struct device *dev);
extern const struct southbridge_usb_port mainboard_usb_ports[14];

void early_usb_init(const struct southbridge_usb_port *portmap);

/* PM I/O Space */
#define UPRWC			0x3c
#define  UPRWC_WR_EN		(1 << 1) /* USB Per-Port Registers Write Enable */

/* PCI Configuration Space (D30:F0): PCI2PCI */
#define PSTS	0x06
#define SMLT	0x1b
#define SECSTS	0x1e
#define INTR	0x3c

#define PCH_EHCI1_DEV		PCI_DEV(0, 0x1d, 0)
#define PCH_EHCI2_DEV		PCI_DEV(0, 0x1a, 0)
#define PCH_ME_DEV		PCI_DEV(0, 0x16, 0)
#define PCH_THERMAL_DEV		PCI_DEV(0, 0x1f, 6)
#define PCH_PCIE_DEV_SLOT	28
#define PCH_IOAPIC_PCI_BUS	250
#define PCH_IOAPIC_PCI_SLOT	31
#define PCH_HPET_PCI_BUS	250
#define PCH_HPET_PCI_SLOT	15

/* PCI Configuration Space (D20:F0): xHCI */
#define PCH_XHCI_DEV		PCI_DEV(0, 0x14, 0)

#define XHCI_PWR_CNTL_STS	0x74

/* xHCI memory base registers */
#define XHCI_PORTSC_x_USB3(port)	(0x4c0 + (port) * 0x10)

/* PCI Configuration Space (D31:F0): LPC */
#define PCH_LPC_DEV		PCI_DEV(0, 0x1f, 0)
#define SERIRQ_CNTL		0x64

#define GEN_PMCON_1		0xa0
#define GEN_PMCON_2		0xa2
#define GEN_PMCON_3		0xa4
#define GEN_PMCON_LOCK		0xa6
#define ETR3			0xac
#define  ETR3_CWORWRE		(1 << 18)
#define  ETR3_CF9GR		(1 << 20)
#define  ETR3_CF9LOCK		(1 << 31)

/* GEN_PMCON_3 bits */
#define RTC_BATTERY_DEAD	(1 << 2)
#define RTC_POWER_FAILED	(1 << 1)
#define SLEEP_AFTER_POWER_FAIL	(1 << 0)

#define PMBASE			0x40
#define ACPI_CNTL		0x44
#define   ACPI_EN		(1 << 7)
#define BIOS_CNTL		0xDC
#define GPIO_BASE		0x48 /* LPC GPIO Base Address Register */
#define GPIO_CNTL		0x4C /* LPC GPIO Control Register */

#define GPIO_ROUT		0xb8
#define   GPI_DISABLE		0x00
#define   GPI_IS_SMI		0x01
#define   GPI_IS_SCI		0x02
#define   GPI_IS_NMI		0x03

#define PIRQA_ROUT		0x60
#define PIRQB_ROUT		0x61
#define PIRQC_ROUT		0x62
#define PIRQD_ROUT		0x63
#define PIRQE_ROUT		0x68
#define PIRQF_ROUT		0x69
#define PIRQG_ROUT		0x6A
#define PIRQH_ROUT		0x6B

#define LPC_IBDF		0x6C /* I/O APIC bus/dev/fn */
#define LPC_HnBDF(n)		(0x70 + n * 2) /* HPET n bus/dev/fn */

#define LPC_IO_DEC		0x80 /* IO Decode Ranges Register */
#define LPC_EN			0x82 /* LPC IF Enables Register */
#define  CNF2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define  CNF1_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define  MC_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define  KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define  GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define  GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define  FDD_LPC_EN		(1 << 3)  /* LPC_IO_DEC[12] */
#define  LPT_LPC_EN		(1 << 2)  /* LPC_IO_DEC[9:8] */
#define  COMB_LPC_EN		(1 << 1)  /* LPC_IO_DEC[6:4] */
#define  COMA_LPC_EN		(1 << 0)  /* LPC_IO_DEC[3:2] */
#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8c /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */
#define LGMR			0x98 /* LPC Generic Memory Range */
#define BIOS_DEC_EN1		0xd8 /* BIOS Decode Enable */

/* PCI Configuration Space (D31:F2): SATA */
#define PCH_SATA_DEV		PCI_DEV(0, 0x1f, 2)
#define PCH_SATA2_DEV		PCI_DEV(0, 0x1f, 5)
#define IDE_TIM_PRI		0x40	/* IDE timings, primary */
#define   IDE_DECODE_ENABLE	(1 << 15)
#define IDE_TIM_SEC		0x42	/* IDE timings, secondary */

#define SATA_SIRI		0xa0 /* SATA Indexed Register Index */
#define SATA_SIRD		0xa4 /* SATA Indexed Register Data */
#define SATA_SP			0xd0 /* Scratchpad */

/* SATA IOBP Registers */
#define SATA_IOBP_SP0G3IR	0xea000151
#define SATA_IOBP_SP1G3IR	0xea000051

/* PCI Configuration Space (D31:F3): SMBus */
#define PCH_SMBUS_DEV		PCI_DEV(0, 0x1f, 3)
#define SMB_BASE		0x20
#define HOSTC			0x40

/* HOSTC bits */
#define I2C_EN			(1 << 2)
#define SMB_SMI_EN		(1 << 1)
#define HST_EN			(1 << 0)

/* Southbridge IO BARs */

#define GPIOBASE		0x48

#define PMBASE		0x40

#define CIR0		0x0050	/* 32bit */
#define  TCLOCKDN	(1u << 31)

#define RPC		0x0400	/* 32bit */
#define RPFN		0x0404	/* 32bit */

#define CIR2		0x900	/* 16bit */
#define CIR3		0x1100	/* 16bit */
#define UPDCR		0x1114	/* 32bit */

/* Root Port configuratinon space hide */
#define RPFN_HIDE(port)         (1 << (((port) * 4) + 3))
/* Get the function number assigned to a Root Port */
#define RPFN_FNGET(reg,port)    (((reg) >> ((port) * 4)) & 7)
/* Set the function number for a Root Port */
#define RPFN_FNSET(port,func)   (((func) & 7) << ((port) * 4))
/* Root Port function number mask */
#define RPFN_FNMASK(port)       (7 << ((port) * 4))

#define TRSR		0x1e00	/*  8bit */
#define TRCR		0x1e10	/* 64bit */
#define TWDR		0x1e18	/* 64bit */

#define IOTR0		0x1e80	/* 64bit */
#define IOTR1		0x1e88	/* 64bit */
#define IOTR2		0x1e90	/* 64bit */
#define IOTR3		0x1e98	/* 64bit */

#define  VCNEGPND	2

#define TCTL		0x3000	/*  8bit */

#define NOINT		0
#define INTA		1
#define INTB		2
#define INTC		3
#define INTD		4

#define DIR_IDR		12	/* Interrupt D Pin Offset */
#define DIR_ICR		8	/* Interrupt C Pin Offset */
#define DIR_IBR		4	/* Interrupt B Pin Offset */
#define DIR_IAR		0	/* Interrupt A Pin Offset */

#define PIRQA		0
#define PIRQB		1
#define PIRQC		2
#define PIRQD		3
#define PIRQE		4
#define PIRQF		5
#define PIRQG		6
#define PIRQH		7

/* DMI control */
#define V0CTL		0x2014	/* 32bit */
#define V0STS		0x201a	/* 16bit */
#define V1CTL		0x2020	/* 32bit */
#define V1STS		0x2026	/* 16bit */
#define CIR31		0x2030	/* 32bit */
#define CIR32		0x2040	/* 32bit */
#define CIR1		0x2088	/* 32bit */
#define REC		0x20ac	/* 32bit */
#define LCAP		0x21a4	/* 32bit */
#define LCTL		0x21a8	/* 16bit */
#define LSTS		0x21aa	/* 16bit */
#define DLCTL2		0x21b0	/* 16bit */
#define DMIC		0x2234	/* 32bit */
#define CIR30		0x2238	/* 32bit */
#define CIR5		0x228c	/* 32bit */
#define DMC		0x2304	/* 32bit */
#define CIR6		0x2314	/* 32bit */
#define CIR9		0x2320	/* 32bit */
#define DMC2		0x2324	/* 32bit - name guessed */

/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_RW_BX    ((1 << 9)|(1 << 10))
#define  IOBPS_WRITE_AX	((1 << 9)|(1 << 10))
#define  IOBPS_READ_AX	((1 << 8)|(1 << 9)|(1 << 10))

#define D31IP		0x3100	/* 32bit */
#define D31IP_TTIP	24	/* Thermal Throttle Pin */
#define D31IP_SIP2	20	/* SATA Pin 2 */
#define D31IP_SMIP	12	/* SMBUS Pin */
#define D31IP_SIP	8	/* SATA Pin */
#define D30IP		0x3104	/* 32bit */
#define D30IP_PIP	0	/* PCI Bridge Pin */
#define D29IP		0x3108	/* 32bit */
#define D29IP_E1P	0	/* EHCI #1 Pin */
#define D28IP		0x310c	/* 32bit */
#define D28IP_P8IP	28	/* PCI Express Port 8 */
#define D28IP_P7IP	24	/* PCI Express Port 7 */
#define D28IP_P6IP	20	/* PCI Express Port 6 */
#define D28IP_P5IP	16	/* PCI Express Port 5 */
#define D28IP_P4IP	12	/* PCI Express Port 4 */
#define D28IP_P3IP	8	/* PCI Express Port 3 */
#define D28IP_P2IP	4	/* PCI Express Port 2 */
#define D28IP_P1IP	0	/* PCI Express Port 1 */
#define D27IP		0x3110	/* 32bit */
#define D27IP_ZIP	0	/* HD Audio Pin */
#define D26IP		0x3114	/* 32bit */
#define D26IP_E2P	0	/* EHCI #2 Pin */
#define D25IP		0x3118	/* 32bit */
#define D25IP_LIP	0	/* GbE LAN Pin */
#define D22IP		0x3124	/* 32bit */
#define D22IP_KTIP	12	/* KT Pin */
#define D22IP_IDERIP	8	/* IDE-R Pin */
#define D22IP_MEI2IP	4	/* MEI #2 Pin */
#define D22IP_MEI1IP	0	/* MEI #1 Pin */
#define D20IP		0x3128  /* 32bit */
#define D20IP_XHCIIP	0
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define D26IR		0x314c	/* 16bit */
#define D25IR		0x3150	/* 16bit */
#define D22IR		0x315c	/* 16bit */
#define D20IR		0x3160	/* 16bit */
#define OIC		0x31fe	/* 16bit */
#define SOFT_RESET_CTRL 0x38f4
#define SOFT_RESET_DATA 0x38f8

#define DIR_ROUTE(x,a,b,c,d) \
  RCBA16(x) = (((d) << DIR_IDR) | ((c) << DIR_ICR) | \
               ((b) << DIR_IBR) | ((a) << DIR_IAR))

#define PRSTS		0x3310	/* 32bit */
#define CIR7		0x3314	/* 32bit */
#define PM_CFG		0x3318	/* 32bit */
#define CIR8		0x3324	/* 32bit */
#define CIR10		0x3340	/* 32bit */
#define CIR11		0x3344	/* 32bit */
#define CIR12		0x3360	/* 32bit */
#define CIR14		0x3368	/* 32bit */
#define CIR15		0x3378	/* 32bit */
#define CIR13		0x337c	/* 32bit */
#define CIR16		0x3388	/* 32bit */
#define CIR18		0x3390	/* 32bit */
#define CIR17		0x33a0	/* 32bit */
#define CIR23		0x33b0	/* 32bit */
#define CIR19		0x33c0	/* 32bit */
#define PMSYNC_CFG	0x33c8	/* 32bit */
#define CIR20		0x33cc	/* 32bit */
#define CIR21		0x33d0	/* 32bit */
#define CIR22		0x33d4	/* 32bit */

#define RC		0x3400	/* 32bit */
#define HPTC		0x3404	/* 32bit */
#define GCS		0x3410	/* 32bit */
#define BUC		0x3414	/* 32bit */
#define PCH_DISABLE_GBE		(1 << 5)
#define FD		0x3418	/* 32bit */
#define DISPBDF		0x3424  /* 16bit */
#define FD2		0x3428	/* 32bit */
#define CG		0x341c	/* 32bit */

/* Function Disable 1 RCBA 0x3418 */
#define PCH_DISABLE_ALWAYS	((1 << 0)|(1 << 26))
#define PCH_DISABLE_P2P		(1 << 1)
#define PCH_DISABLE_SATA1	(1 << 2)
#define PCH_DISABLE_SMBUS	(1 << 3)
#define PCH_DISABLE_HD_AUDIO	(1 << 4)
#define PCH_DISABLE_EHCI2	(1 << 13)
#define PCH_DISABLE_LPC		(1 << 14)
#define PCH_DISABLE_EHCI1	(1 << 15)
#define PCH_DISABLE_PCIE(x)	(1 << (16 + x))
#define PCH_DISABLE_THERMAL	(1 << 24)
#define PCH_DISABLE_SATA2	(1 << 25)
#define PCH_DISABLE_XHCI	(1 << 27)

/* Function Disable 2 RCBA 0x3428 */
#define PCH_DISABLE_KT		(1 << 4)
#define PCH_DISABLE_IDER	(1 << 3)
#define PCH_DISABLE_MEI2	(1 << 2)
#define PCH_DISABLE_MEI1	(1 << 1)
#define PCH_ENABLE_DBDF		(1 << 0)

/* USB Initialization Registers[13:0] */
#define USBIR0		0x3500	/* 32bit */
#define USBIR1		0x3504	/* 32bit */
#define USBIR2		0x3508	/* 32bit */
#define USBIR3		0x350c	/* 32bit */
#define USBIR4		0x3510	/* 32bit */
#define USBIR5		0x3514	/* 32bit */
#define USBIR6		0x3518	/* 32bit */
#define USBIR7		0x351c	/* 32bit */
#define USBIR8		0x3520	/* 32bit */
#define USBIR9		0x3524	/* 32bit */
#define USBIR10		0x3528	/* 32bit */
#define USBIR11		0x352c	/* 32bit */
#define USBIR12		0x3530	/* 32bit */
#define USBIR13		0x3534	/* 32bit */

/* Miscellaneous Control Register */
#define MISCCTL		0x3590	/* 32bit */
/* USB Port Disable Override */
#define USBPDO		0x359c	/* 32bit */
/* USB Overcurrent MAP Register */
#define USBOCM1		0x35a0	/* 32bit */
#define USBOCM2		0x35a4	/* 32bit */
/* Rate Matching Hub Wake Control Register */
#define RMHWKCTL	0x35b0	/* 32bit */

#define CIR24		0x3a28	/* 32bit */
#define CIR25		0x3a2c	/* 32bit */
#define CIR26		0x3a6c	/* 32bit */
#define CIR27		0x3a80	/* 32bit */
#define CIR28		0x3a84	/* 32bit */
#define CIR29		0x3a88	/* 32bit */

/* XHCI USB 3.0 */
#define XOCM		0xc0	/* 32bit */
#define XUSB2PRM	0xd4	/* 32bit */
#define USB3PRM		0xdc	/* 32bit */

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
#define PM2_CNT		0x50 // mobile only
#define C3_RES		0x54

#define TCO1_STS	0x64
#define   TCO1_TIMEOUT	(1 << 3)
#define   DMISCI_STS	(1 << 9)
#define TCO2_STS	0x66
#define   SECOND_TO_STS	(1 << 1)
#define TCO1_CNT	0x68
#define   TCO_TMR_HLT	(1 << 11)
#define   TCO_LOCK	(1 << 12)
#define TCO2_CNT	0x6a

#define SPIBAR_HSFS                 0x3804   /* SPI hardware sequence status */
#define  SPIBAR_HSFS_SCIP           (1 << 5) /* SPI Cycle In Progress */
#define  SPIBAR_HSFS_AEL            (1 << 2) /* SPI Access Error Log */
#define  SPIBAR_HSFS_FCERR          (1 << 1) /* SPI Flash Cycle Error */
#define  SPIBAR_HSFS_FDONE          (1 << 0) /* SPI Flash Cycle Done */
#define SPIBAR_HSFC                 0x3806   /* SPI hardware sequence control */
#define  SPIBAR_HSFC_BYTE_COUNT(c)  (((c - 1) & 0x3f) << 8)
#define  SPIBAR_HSFC_CYCLE_READ     (0 << 1) /* Read cycle */
#define  SPIBAR_HSFC_CYCLE_WRITE    (2 << 1) /* Write cycle */
#define  SPIBAR_HSFC_CYCLE_ERASE    (3 << 1) /* Erase cycle */
#define  SPIBAR_HSFC_GO             (1 << 0) /* GO: start SPI transaction */
#define SPIBAR_FADDR                0x3808   /* SPI flash address */
#define SPIBAR_FDATA(n)             (0x3810 + (4 * n)) /* SPI flash data */

#endif /* __ACPI__ */
#endif				/* SOUTHBRIDGE_INTEL_BD82X6X_PCH_H */
