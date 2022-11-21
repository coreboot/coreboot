/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_LYNXPOINT_PCH_H
#define SOUTHBRIDGE_INTEL_LYNXPOINT_PCH_H

#include <acpi/acpi.h>
#include <southbridge/intel/common/rcba.h> /* IWYU pragma: export */

#define CROS_GPIO_DEVICE_NAME	"LynxPoint"

/*
 * Lynx Point PCH PCI Devices:
 *
 * Bus 0:Device 31:Function 0 LPC Controller1
 * Bus 0:Device 31:Function 2 SATA Controller #1
 * Bus 0:Device 31:Function 3 SMBus Controller
 * Bus 0:Device 31:Function 5 SATA Controller #22
 * Bus 0:Device 31:Function 6 Thermal Subsystem
 * Bus 0:Device 29:Function 03 USB EHCI Controller #1
 * Bus 0:Device 26:Function 03 USB EHCI Controller #2
 * Bus 0:Device 28:Function 0 PCI Express* Port 1
 * Bus 0:Device 28:Function 1 PCI Express Port 2
 * Bus 0:Device 28:Function 2 PCI Express Port 3
 * Bus 0:Device 28:Function 3 PCI Express Port 4
 * Bus 0:Device 28:Function 4 PCI Express Port 5
 * Bus 0:Device 28:Function 5 PCI Express Port 6
 * Bus 0:Device 28:Function 6 PCI Express Port 7
 * Bus 0:Device 28:Function 7 PCI Express Port 8
 * Bus 0:Device 27:Function 0 Intel High Definition Audio Controller
 * Bus 0:Device 25:Function 0 Gigabit Ethernet Controller
 * Bus 0:Device 22:Function 0 Intel Management Engine Interface #1
 * Bus 0:Device 22:Function 1 Intel Management Engine Interface #2
 * Bus 0:Device 22:Function 2 IDE-R
 * Bus 0:Device 22:Function 3 KT
 * Bus 0:Device 20:Function 0 xHCI Controller
*/

/* PCH stepping values for LPC device */
#define LPT_H_STEP_B0		0x02
#define LPT_H_STEP_C0		0x03
#define LPT_H_STEP_C1		0x04
#define LPT_H_STEP_C2		0x05
#define LPT_LP_STEP_B0		0x02
#define LPT_LP_STEP_B1		0x03
#define LPT_LP_STEP_B2		0x04

#define SMBUS_SLAVE_ADDR	0x24

#if CONFIG(INTEL_LYNXPOINT_LP)
#define DEFAULT_PMBASE		0x1000
#define DEFAULT_GPIOBASE	0x1400
#define DEFAULT_GPIOSIZE	0x400
#else
#define DEFAULT_PMBASE		0x500
#define DEFAULT_GPIOBASE	0x480
#define DEFAULT_GPIOSIZE	0x80
#endif

#ifndef __ACPI__

#if CONFIG(INTEL_LYNXPOINT_LP)
#define MAX_USB2_PORTS	10
#define MAX_USB3_PORTS	4
#else
#define MAX_USB2_PORTS	14
#define MAX_USB3_PORTS	6
#endif

/* There are 8 OC pins */
#define USB_OC_PIN_SKIP	8

enum usb2_port_location {
	USB_PORT_SKIP = 0,
	USB_PORT_BACK_PANEL,
	USB_PORT_FRONT_PANEL,
	USB_PORT_DOCK,
	USB_PORT_MINI_PCIE,
	USB_PORT_FLEX,
	USB_PORT_INTERNAL,
};

/*
 * USB port length is in MRC format: binary-coded decimal length in tenths of an inch.
 *   4.2 inches -> 0x0042
 *  12.7 inches -> 0x0127
 */
struct usb2_port_config {
	uint16_t length;
	bool enable;
	unsigned short oc_pin;
	enum usb2_port_location location;
};

struct usb3_port_config {
	bool enable;
	unsigned int oc_pin;
};

/* Mainboard-specific USB configuration */
extern const struct usb2_port_config mainboard_usb2_ports[MAX_USB2_PORTS];
extern const struct usb3_port_config mainboard_usb3_ports[MAX_USB3_PORTS];

static inline int pch_is_lp(void)
{
	return CONFIG(INTEL_LYNXPOINT_LP);
}

/* PCH platform types, safe for MRC consumption */
enum pch_platform_type {
	PCH_TYPE_MOBILE	 = 0,
	PCH_TYPE_DESKTOP = 1, /* or server */
	PCH_TYPE_ULT	 = 5,
};

void usb_ehci_sleep_prepare(pci_devfn_t dev, u8 slp_typ);
void usb_ehci_disable(pci_devfn_t dev);
void usb_xhci_sleep_prepare(pci_devfn_t dev, u8 slp_typ);
void usb_xhci_route_all(void);

enum pch_platform_type get_pch_platform_type(void);
int pch_silicon_revision(void);
int pch_silicon_id(void);
u16 get_pmbase(void);
u16 get_gpiobase(void);

/* Power Management register handling in pmutil.c */
/* PM1_CNT */
void enable_pm1_control(u32 mask);
void disable_pm1_control(u32 mask);
/* PM1 */
u16 clear_pm1_status(void);
void enable_pm1(u16 events);
u32 clear_smi_status(void);
/* SMI */
void enable_smi(u32 mask);
void disable_smi(u32 mask);
/* ALT_GP_SMI */
u32 clear_alt_smi_status(void);
void enable_alt_smi(u32 mask);
/* TCO */
u32 clear_tco_status(void);
void enable_tco_sci(void);
/* GPE0 */
u32 clear_gpe_status(void);
void clear_gpe_enable(void);
void enable_all_gpe(u32 set1, u32 set2, u32 set3, u32 set4);
void disable_all_gpe(void);
void enable_gpe(u32 mask);
void disable_gpe(u32 mask);

void pch_enable(struct device *dev);
void pch_disable_devfn(struct device *dev);
void pch_log_state(void);
void acpi_create_serialio_ssdt(acpi_header_t *ssdt);

void enable_usb_bar(void);
void early_pch_init(void);
void pch_enable_lpc(void);
void uart_bootblock_init(void);
void mainboard_config_superio(void);
void mainboard_config_rcba(void);

#define MAINBOARD_POWER_OFF	0
#define MAINBOARD_POWER_ON	1
#define MAINBOARD_POWER_KEEP	2

/* PCI Configuration Space (D30:F0): PCI2PCI */
#define PSTS	0x06
#define SMLT	0x1b
#define SECSTS	0x1e
#define INTR	0x3c

/* Power Management Control and Status */
#define PCH_PCS			0x84
#define  PCH_PCS_PS_D3HOT	3

/* SerialIO */
#define PCH_DEVFN_SDMA		PCI_DEVFN(0x15, 0)
#define PCH_DEVFN_I2C0		PCI_DEVFN(0x15, 1)
#define PCH_DEVFN_I2C1		PCI_DEVFN(0x15, 2)
#define PCH_DEVFN_SPI0		PCI_DEVFN(0x15, 3)
#define PCH_DEVFN_SPI1		PCI_DEVFN(0x15, 4)
#define PCH_DEVFN_UART0		PCI_DEVFN(0x15, 5)
#define PCH_DEVFN_UART1		PCI_DEVFN(0x15, 6)

#define PCH_DEVFN_SDIO		PCI_DEVFN(0x17, 0)

#define PCH_EHCI1_DEV		PCI_DEV(0, 0x1d, 0)
#define PCH_EHCI2_DEV		PCI_DEV(0, 0x1a, 0)
#define PCH_XHCI_DEV		PCI_DEV(0, 0x14, 0)
#define PCH_ME_DEV		PCI_DEV(0, 0x16, 0)
#define PCH_PCIE_DEV_SLOT	28

/* PCI Configuration Space (D31:F0): LPC */
#define PCH_LPC_DEV		PCI_DEV(0, 0x1f, 0)
#define SERIRQ_CNTL		0x64

#define GEN_PMCON_1		0xa0
#define  SMI_LOCK		(1 << 4)
#define GEN_PMCON_2		0xa2
#define  SYSTEM_RESET_STS	(1 << 4)
#define  THERMTRIP_STS		(1 << 3)
#define  SYSPWR_FLR		(1 << 1)
#define  PWROK_FLR		(1 << 0)
#define GEN_PMCON_3		0xa4
#define  SUS_PWR_FLR		(1 << 14)
#define  GEN_RST_STS		(1 << 9)
#define  RTC_BATTERY_DEAD	(1 << 2)
#define  PWR_FLR		(1 << 1)
#define  SLEEP_AFTER_POWER_FAIL	(1 << 0)
#define PMIR			0xac
#define  PMIR_CF9LOCK		(1 << 31)
#define  PMIR_CF9GR		(1 << 20)

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
#define  CNF2_LPC_EN		(1 << 13) /* 0x4e/0x4f */
#define  CNF1_LPC_EN		(1 << 12) /* 0x2e/0x2f */
#define  MC_LPC_EN		(1 << 11) /* 0x62/0x66 */
#define  KBC_LPC_EN		(1 << 10) /* 0x60/0x64 */
#define  GAMEH_LPC_EN		(1 << 9)  /* 0x208/0x20f */
#define  GAMEL_LPC_EN		(1 << 8)  /* 0x200/0x207 */
#define  FDD_LPC_EN		(1 << 3)  /* LPC_IO_DEC[12] */
#define  LPT_LPC_EN		(1 << 2)  /* LPC_IO_DEC[9:8] */
#define  COMB_LPC_EN		(1 << 1)  /* LPC_IO_DEC[6:4] */
#define  COMA_LPC_EN		(1 << 0)  /* LPC_IO_DEC[2:0] */
#define LPC_IBDF		0x6C /* I/O APIC bus/dev/fn */
#define LPC_HnBDF(n)		(0x70 + (n) * 2) /* HPET n bus/dev/fn */
#define LPC_GEN1_DEC		0x84 /* LPC IF Generic Decode Range 1 */
#define LPC_GEN2_DEC		0x88 /* LPC IF Generic Decode Range 2 */
#define LPC_GEN3_DEC		0x8c /* LPC IF Generic Decode Range 3 */
#define LPC_GEN4_DEC		0x90 /* LPC IF Generic Decode Range 4 */
#define LGMR			0x98 /* LPC Generic Memory Range */

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
#define SATA_IOBP_SP0DTLE_DATA	0xea002750
#define SATA_IOBP_SP0DTLE_EDGE	0xea002754
#define SATA_IOBP_SP1DTLE_DATA	0xea002550
#define SATA_IOBP_SP1DTLE_EDGE	0xea002554

#define SATA_DTLE_MASK		0xF
#define SATA_DTLE_DATA_SHIFT	24
#define SATA_DTLE_EDGE_SHIFT	16

/* EHCI PCI Registers */
#define EHCI_PWR_CTL_STS	0x54
#define  PWR_CTL_SET_MASK	0x3
#define  PWR_CTL_SET_D0		0x0
#define  PWR_CTL_SET_D3		0x3
#define  PWR_CTL_ENABLE_PME	(1 << 8)
#define  PWR_CTL_STATUS_PME	(1 << 15)

/* EHCI Memory Registers */
#define EHCI_USB_CMD		0x20
#define  EHCI_USB_CMD_RUN	(1 << 0)
#define  EHCI_USB_CMD_PSE	(1 << 4)
#define  EHCI_USB_CMD_ASE	(1 << 5)
#define EHCI_PORTSC(port)	(0x64 + (port) * 4)
#define  EHCI_PORTSC_ENABLED	(1 << 2)
#define  EHCI_PORTSC_SUSPEND	(1 << 7)

/* XHCI PCI Registers */
#define XHCI_PWR_CTL_STS	0x74
#define XHCI_USB2PR		0xd0
#define XHCI_USB2PRM		0xd4
#define  XHCI_USB2PR_HCSEL	0x7fff
#define XHCI_USB3PR		0xd8
#define  XHCI_USB3PR_SSEN	0x3f
#define XHCI_USB3PRM		0xdc
#define XHCI_USB3FUS		0xe0
#define  XHCI_USB3FUS_SS_MASK	3
#define  XHCI_USB3FUS_SS_SHIFT	3
#define XHCI_USB3PDO		0xe8

/* XHCI Memory Registers */
#define XHCI_USB3_PORTSC(port)	((pch_is_lp() ? 0x510 : 0x570) + ((port) * 0x10))
#define  XHCI_USB3_PORTSC_CHST	(0x7f << 17)
#define  XHCI_USB3_PORTSC_WCE	(1 << 25)	/* Wake on Connect */
#define  XHCI_USB3_PORTSC_WDE	(1 << 26)	/* Wake on Disconnect */
#define  XHCI_USB3_PORTSC_WOE	(1 << 27)	/* Wake on Overcurrent */
#define  XHCI_USB3_PORTSC_WRC	(1 << 19)	/* Warm Reset Complete */
#define  XHCI_USB3_PORTSC_LWS	(1 << 16)	/* Link Write Strobe */
#define  XHCI_USB3_PORTSC_PED	(1 << 1)	/* Port Enabled/Disabled */
#define  XHCI_USB3_PORTSC_WPR	(1 << 31)	/* Warm Port Reset */
#define  XHCI_USB3_PORTSC_PLS	(0xf << 5)	/* Port Link State */
#define   XHCI_PLSR_DISABLED	(4 << 5)	/* Port is disabled */
#define   XHCI_PLSR_RXDETECT	(5 << 5)	/* Port is disconnected */
#define   XHCI_PLSR_POLLING	(7 << 5)	/* Port is polling */
#define   XHCI_PLSW_ENABLE	(5 << 5)	/* Transition from disabled */

/* Serial IO IOBP Registers */
#define SIO_IOBP_PORTCTRL0	0xcb000000	/* SDIO D23:F0 */
#define  SIO_IOBP_PORTCTRL0_ACPI_IRQ_EN		(1 << 5)
#define  SIO_IOBP_PORTCTRL0_PCI_CONF_DIS	(1 << 4)
#define SIO_IOBP_PORTCTRL1	0xcb000014	/* SDIO D23:F0 */
#define  SIO_IOBP_PORTCTRL1_SNOOP_SELECT(x)	(((x) & 3) << 13)
#define SIO_IOBP_GPIODF		0xcb000154
#define  SIO_IOBP_GPIODF_SDIO_IDLE_DET_EN	(1 << 4)
#define  SIO_IOBP_GPIODF_DMA_IDLE_DET_EN	(1 << 3)
#define  SIO_IOBP_GPIODF_UART_IDLE_DET_EN	(1 << 2)
#define  SIO_IOBP_GPIODF_I2C_IDLE_DET_EN	(1 << 1)
#define  SIO_IOBP_GPIODF_SPI_IDLE_DET_EN	(1 << 0)
#define SIO_IOBP_PORTCTRL2	0xcb000240	/* DMA D21:F0 */
#define SIO_IOBP_PORTCTRL3	0xcb000248	/* I2C0 D21:F1 */
#define SIO_IOBP_PORTCTRL4	0xcb000250	/* I2C1 D21:F2 */
#define SIO_IOBP_PORTCTRL5	0xcb000258	/* SPI0 D21:F3 */
#define SIO_IOBP_PORTCTRL6	0xcb000260	/* SPI1 D21:F4 */
#define SIO_IOBP_PORTCTRL7	0xcb000268	/* UART0 D21:F5 */
#define SIO_IOBP_PORTCTRL8	0xcb000270	/* UART1 D21:F6 */
#define SIO_IOBP_PORTCTRLX(x)	(0xcb000240 + ((x) * 8))
/* PORTCTRL 2-8 have the same layout */
#define  SIO_IOBP_PORTCTRL_ACPI_IRQ_EN		(1 << 21)
#define  SIO_IOBP_PORTCTRL_PCI_CONF_DIS		(1 << 20)
#define  SIO_IOBP_PORTCTRL_SNOOP_SELECT(x)	(((x) & 3) << 18)
#define  SIO_IOBP_PORTCTRL_INT_PIN(x)		(((x) & 0xf) << 2)
#define  SIO_IOBP_PORTCTRL_PM_CAP_PRSNT		(1 << 1)
#define SIO_IOBP_FUNCDIS0	0xce00aa07	/* DMA D21:F0 */
#define SIO_IOBP_FUNCDIS1	0xce00aa47	/* I2C0 D21:F1 */
#define SIO_IOBP_FUNCDIS2	0xce00aa87	/* I2C1 D21:F2 */
#define SIO_IOBP_FUNCDIS3	0xce00aac7	/* SPI0 D21:F3 */
#define SIO_IOBP_FUNCDIS4	0xce00ab07	/* SPI1 D21:F4 */
#define SIO_IOBP_FUNCDIS5	0xce00ab47	/* UART0 D21:F5 */
#define SIO_IOBP_FUNCDIS6	0xce00ab87	/* UART1 D21:F6 */
#define SIO_IOBP_FUNCDIS7	0xce00ae07	/* SDIO D23:F0 */
#define  SIO_IOBP_FUNCDIS_DIS			(1 << 8)

/* Serial IO Devices */
#define SIO_ID_SDMA		0 /* D21:F0 */
#define SIO_ID_I2C0		1 /* D21:F1 */
#define SIO_ID_I2C1		2 /* D21:F2 */
#define SIO_ID_SPI0		3 /* D21:F3 */
#define SIO_ID_SPI1		4 /* D21:F4 */
#define SIO_ID_UART0		5 /* D21:F5 */
#define SIO_ID_UART1		6 /* D21:F6 */
#define SIO_ID_SDIO		7 /* D23:F0 */

#define SIO_REG_PPR_CLOCK		0x800
#define  SIO_REG_PPR_CLOCK_EN		 (1 << 0)
#define  SIO_REG_PPR_CLOCK_UPDATE	 (1 << 31)
#define  SIO_REG_PPR_CLOCK_M_DIV	 0x25a
#define  SIO_REG_PPR_CLOCK_N_DIV	 0x7fff
#define SIO_REG_PPR_RST			0x804
#define  SIO_REG_PPR_RST_ASSERT		 0x3
#define SIO_REG_PPR_GEN			0x808
#define  SIO_REG_PPR_GEN_LTR_MODE_MASK	 (1 << 2)
#define  SIO_REG_PPR_GEN_VOLTAGE_MASK	 (1 << 3)
#define  SIO_REG_PPR_GEN_VOLTAGE(x)	 (((x) & 1) << 3)
#define SIO_REG_AUTO_LTR		0x814

#define SIO_REG_SDIO_PPR_GEN		0x1008
#define SIO_REG_SDIO_PPR_SW_LTR		0x1010
#define SIO_REG_SDIO_PPR_CMD12		0x3c
#define  SIO_REG_SDIO_PPR_CMD12_B30	 (1 << 30)

#define SIO_PIN_INTA 1 /* IRQ5 in ACPI mode */
#define SIO_PIN_INTB 2 /* IRQ6 in ACPI mode */
#define SIO_PIN_INTC 3 /* IRQ7 in ACPI mode */
#define SIO_PIN_INTD 4 /* IRQ13 in ACPI mode */

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

#define RPC		0x0400	/* 32bit */
#define RPFN		0x0404	/* 32bit */

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

/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_READY	0x0001
#define  IOBPS_TX_MASK	0x0006
#define  IOBPS_MASK     0xff00
#define  IOBPS_READ     0x0600
#define  IOBPS_WRITE	0x0700
#define IOBPU		0x233a
#define  IOBPU_MAGIC	0xf000
#define  IOBP_PCICFG_READ	0x0400
#define  IOBP_PCICFG_WRITE	0x0500

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
#define D20IP		0x3128	/* 32bit */
#define D20IP_XHCI	0	/* XHCI Pin */
#define D31IR		0x3140	/* 16bit */
#define D30IR		0x3142	/* 16bit */
#define D29IR		0x3144	/* 16bit */
#define D28IR		0x3146	/* 16bit */
#define D27IR		0x3148	/* 16bit */
#define D26IR		0x314c	/* 16bit */
#define D25IR		0x3150	/* 16bit */
#define D23IR		0x3158	/* 16bit */
#define D22IR		0x315c	/* 16bit */
#define D20IR		0x3160	/* 16bit */
#define D21IR		0x3164	/* 16bit */
#define D19IR		0x3168	/* 16bit */
#define ACPIIRQEN	0x31e0	/* 32bit */
#define OIC		0x31fe	/* 16bit */
#define PMSYNC_CONFIG	0x33c4	/* 32bit */
#define PMSYNC_CONFIG2	0x33cc	/* 32bit */
#define SOFT_RESET_CTRL 0x38f4
#define SOFT_RESET_DATA 0x38f8

#define DIR_ROUTE(a,b,c,d) \
  (((d) << DIR_IDR) | ((c) << DIR_ICR) | \
  ((b) << DIR_IBR) | ((a) << DIR_IAR))

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
#define PCH_DISABLE_ALWAYS	(1 << 0)
#define PCH_DISABLE_ADSPD	(1 << 1)
#define PCH_DISABLE_SATA1	(1 << 2)
#define PCH_DISABLE_SMBUS	(1 << 3)
#define PCH_DISABLE_HD_AUDIO	(1 << 4)
#define PCH_DISABLE_EHCI2	(1 << 13)
#define PCH_DISABLE_LPC		(1 << 14)
#define PCH_DISABLE_EHCI1	(1 << 15)
#define PCH_DISABLE_PCIE(x)	(1 << (16 + (x)))
#define PCH_DISABLE_THERMAL	(1 << 24)
#define PCH_DISABLE_SATA2	(1 << 25)
#define PCH_DISABLE_XHCI	(1 << 27)

/* Function Disable 2 RCBA 0x3428 */
#define PCH_DISABLE_KT		(1 << 4)
#define PCH_DISABLE_IDER	(1 << 3)
#define PCH_DISABLE_MEI2	(1 << 2)
#define PCH_DISABLE_MEI1	(1 << 1)
#define PCH_ENABLE_DBDF		(1 << 0)

#define PCH_IOAPIC_PCI_BUS	250
#define PCH_IOAPIC_PCI_SLOT	31
#define PCH_HPET_PCI_BUS	250
#define PCH_HPET_PCI_SLOT	15

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
#define GPE0_STS_2	0x24
#define GPE0_EN		0x28
#define   PME_B0_EN	(1 << 13)
#define   PME_EN	(1 << 11)
#define   TCOSCI_EN	(1 << 6)
#define GPE0_EN_2	0x2c
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

#if CONFIG(TCO_SPACE_NOT_YET_SPLIT)
#define TCO1_STS	0x64
#define   DMISCI_STS	(1 << 9)
#define TCO2_STS	0x66
#define   TCO2_STS_SECOND_TO	(1 << 1)
#endif

#define ALT_GP_SMI_EN2	0x5c
#define ALT_GP_SMI_STS2	0x5e

/* Lynxpoint LP */
#define LP_GPE0_STS_1	0x80	/* GPIO 0-31 */
#define LP_GPE0_STS_2	0x84	/* GPIO 32-63 */
#define LP_GPE0_STS_3	0x88	/* GPIO 64-94 */
#define LP_GPE0_STS_4	0x8c	/* Standard GPE */
#define LP_GPE0_EN_1	0x90
#define LP_GPE0_EN_2	0x94
#define LP_GPE0_EN_3	0x98
#define LP_GPE0_EN_4	0x9c

/*
 * SPI Opcode Menu setup for SPIBAR lockdown
 * should support most common flash chips.
 */

#define SPIBAR_OFFSET 0x3800
#define SPIBAR8(x) RCBA8((x) + SPIBAR_OFFSET)
#define SPIBAR16(x) RCBA16((x) + SPIBAR_OFFSET)
#define SPIBAR32(x) RCBA32((x) + SPIBAR_OFFSET)

/* Registers within the SPIBAR */
#define SSFC 0x91
#define FDOC 0xb0
#define FDOD 0xb4

#define SPIBAR_HSFS                 0x3804   /* SPI hardware sequence status */
#define  SPIBAR_HSFS_SCIP           (1 << 5) /* SPI Cycle In Progress */
#define  SPIBAR_HSFS_AEL            (1 << 2) /* SPI Access Error Log */
#define  SPIBAR_HSFS_FCERR          (1 << 1) /* SPI Flash Cycle Error */
#define  SPIBAR_HSFS_FDONE          (1 << 0) /* SPI Flash Cycle Done */
#define SPIBAR_HSFC                 0x3806   /* SPI hardware sequence control */
#define  SPIBAR_HSFC_BYTE_COUNT(c)  ((((c) - 1) & 0x3f) << 8)
#define  SPIBAR_HSFC_CYCLE_READ     (0 << 1) /* Read cycle */
#define  SPIBAR_HSFC_CYCLE_WRITE    (2 << 1) /* Write cycle */
#define  SPIBAR_HSFC_CYCLE_ERASE    (3 << 1) /* Erase cycle */
#define  SPIBAR_HSFC_GO             (1 << 0) /* GO: start SPI transaction */
#define SPIBAR_FADDR                0x3808   /* SPI flash address */
#define SPIBAR_FDATA(n)             (0x3810 + (4 * (n))) /* SPI flash data */

#endif /* __ACPI__ */
#endif /* SOUTHBRIDGE_INTEL_LYNXPOINT_PCH_H */
