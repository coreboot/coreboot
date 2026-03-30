/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOUTHBRIDGE_INTEL_I82801HX_I82801HX_H
#define SOUTHBRIDGE_INTEL_I82801HX_I82801HX_H

#include <southbridge/intel/common/rcba.h> /* IWYU pragma: export */

#define DEFAULT_TBAR ((u8 *)0xfed1b000)

/* PCI Devices */
#define D31F0 PCI_DEV(0, 31, 0) /* LPC Bridge */
#define D31F1 PCI_DEV(0, 31, 1) /* IDE */
#define D31F2 PCI_DEV(0, 31, 2) /* SATA */
#define D31F3 PCI_DEV(0, 31, 3) /* SMBus */
#define D31F5 PCI_DEV(0, 31, 5) /* SATA #2 */
#define D31F6 PCI_DEV(0, 31, 6) /* Thermal */
#define D29F0 PCI_DEV(0, 29, 0) /* USB UHCI #1 */
#define D29F1 PCI_DEV(0, 29, 1) /* USB UHCI #2 */
#define D29F2 PCI_DEV(0, 29, 2) /* USB UHCI #3 */
#define D29F7 PCI_DEV(0, 29, 7) /* USB EHCI #1 */
#define D26F0 PCI_DEV(0, 26, 0) /* USB UHCI #4 */
#define D26F1 PCI_DEV(0, 26, 1) /* USB UHCI #5 */
#define D26F7 PCI_DEV(0, 26, 7) /* USB EHCI #2 */
#define D27F0 PCI_DEV(0, 27, 0) /* HD Audio */
#define D28F0 PCI_DEV(0, 28, 0) /* PCIe Root Port #1 */
#define D28F1 PCI_DEV(0, 28, 1) /* PCIe Root Port #2 */
#define D28F2 PCI_DEV(0, 28, 2) /* PCIe Root Port #3 */
#define D28F3 PCI_DEV(0, 28, 3) /* PCIe Root Port #4 */
#define D30F0 PCI_DEV(0, 30, 0) /* PCI Bridge */

/* ================================================================== */
/* D31:F0 LPC Bridge Registers                                        */
/* ================================================================== */

#define D31F0_PMBASE         0x40
#define D31F0_ACPI_CNTL      0x44
#define D31F0_GPIO_BASE      0x48
#define D31F0_GPIO_CNTL      0x4c
#define D31F0_PIRQA_ROUT     0x60
#define D31F0_PIRQB_ROUT     0x61
#define D31F0_PIRQC_ROUT     0x62
#define D31F0_PIRQD_ROUT     0x63
#define D31F0_SERIRQ_CNTL    0x64
#define D31F0_PIRQE_ROUT     0x68
#define D31F0_PIRQF_ROUT     0x69
#define D31F0_PIRQG_ROUT     0x6a
#define D31F0_PIRQH_ROUT     0x6b
#define D31F0_LPC_IODEC      0x80
#define D31F0_LPC_EN         0x82
#define D31F0_GEN1_DEC       0x84
#define D31F0_GEN2_DEC       0x88
#define D31F0_GEN3_DEC       0x8c
#define D31F0_GEN4_DEC       0x90
#define D31F0_GEN_PMCON_1    0xa0
#define D31F0_GEN_PMCON_2    0xa2
#define D31F0_GEN_PMCON_3    0xa4
#define D31F0_C5_EXIT_TIMING 0xa8
#define D31F0_C5_ENABLE      0xa9
#define D31F0_C4TIMING_CNT   0xaa
#define D31F0_PMIR           0xac /* Power Management Initialization Register */
#define D31F0_GPIO_ROUT      0xb8
#define D31F0_RCBA           0xf0

/* GEN_PMCON_1 bits */
#define GEN_PMCON_1_SMI_LOCK        (1 << 4)
#define GEN_PMCON_1_CLKRUN_EN       (1 << 2)
#define GEN_PMCON_1_SS_EN           (1 << 3) /* SpeedStep Enable */
#define GEN_PMCON_1_CPUSLP_EN       (1 << 5)
#define GEN_PMCON_1_C4ONC3          (1 << 7)
#define GEN_PMCON_1_BIOS_PCI_EXP_EN (1 << 10)
#define GEN_PMCON_1_C5_ENABLE       (1 << 11)

/* GEN_PMCON_2 bits */
#define GEN_PMCON_2_DRAM_INIT (1 << 7) /* DRAM init scratchpad bit (DISB) */
#define GEN_PMCON_2_W1C_BITS  0x19     /* Write 1 to clear bits 0,3,4 */

/* GEN_PMCON_3 bits */
#define GEN_PMCON_3_SLP_S4_STRETCH (1 << 3)
#define RTC_BATTERY_DEAD           (1 << 2)
#define RTC_POWER_FAILED           (1 << 1)
#define SLEEP_AFTER_POWER_FAIL     (1 << 0)
#define GEN_PMCON_3_SLP_S4_MAW     (3 << 4) /* Minimum assertion width */

/* PMIR bits (D31F0 + 0xAC) */
#define PMIR_USB_TRANSIENT_DISCONNECT (3 << 8)
#define PMIR_CF9GR                    (1 << 20)
#define PMIR_CF9LOCK                  (1 << 31)

/* LPC_EN bits */
#define LPC_EN_CNF2  (1 << 13) /* 0x4E/0x4F decode */
#define LPC_EN_CNF1  (1 << 12) /* 0x2E/0x2F decode */
#define LPC_EN_MC    (1 << 11) /* 0x62/0x66 decode (EC) */
#define LPC_EN_KBC   (1 << 10) /* 0x60/0x64 decode (KBC) */
#define LPC_EN_GAMEL (1 << 8)
#define LPC_EN_COMB  (1 << 1)
#define LPC_EN_COMA  (1 << 0)

/* ================================================================== */
/* D31:F1 IDE Registers (PATA controller, PCI ID 0x2850)              */
/* ================================================================== */

/* Standard IDE timing registers - same layout as ICH7 (i82801gx) */
#define IDE_TIM_PRI       0x40 /* Primary channel timing */
#define IDE_TIM_SEC       0x42 /* Secondary channel timing */
#define IDE_DECODE_ENABLE (1 << 15)
#define IDE_SITRE         (1 << 14)
#define IDE_ISP_5_CLOCKS  (0 << 12)
#define IDE_ISP_4_CLOCKS  (1 << 12)
#define IDE_ISP_3_CLOCKS  (2 << 12)
#define IDE_RCT_4_CLOCKS  (0 << 8)
#define IDE_RCT_3_CLOCKS  (1 << 8)
#define IDE_RCT_2_CLOCKS  (2 << 8)
#define IDE_RCT_1_CLOCKS  (3 << 8)
#define IDE_DTE1          (1 << 7)
#define IDE_PPE1          (1 << 6)
#define IDE_IE1           (1 << 5)
#define IDE_TIME1         (1 << 4)
#define IDE_DTE0          (1 << 3)
#define IDE_PPE0          (1 << 2)
#define IDE_IE0           (1 << 1)
#define IDE_TIME0         (1 << 0)

/* IDE I/O Configuration Register - controls DMA/signal modes */
#define IDE_CONFIG            0x54
#define SIG_MODE_SEC_NORMAL   (0 << 18)
#define SIG_MODE_SEC_TRISTATE (1 << 18)
#define SIG_MODE_SEC_DRIVELOW (2 << 18)
#define SIG_MODE_PRI_NORMAL   (0 << 16)
#define SIG_MODE_PRI_TRISTATE (1 << 16)
#define SIG_MODE_PRI_DRIVELOW (2 << 16)
#define FAST_SCB1             (1 << 15)
#define FAST_SCB0             (1 << 14)
#define FAST_PCB1             (1 << 13)
#define FAST_PCB0             (1 << 12)
#define SCB1                  (1 << 3)
#define SCB0                  (1 << 2)
#define PCB1                  (1 << 1)
#define PCB0                  (1 << 0)

/* ================================================================== */
/* D31:F2 SATA Registers                                              */
/* ================================================================== */

#define D31F2_IDE_TIM_PRI 0x40
#define D31F2_IDE_TIM_SEC 0x42
#define D31F2_SATA_MAP    0x90
#define D31F2_SATA_PCS    0x92
#define D31F2_SIR         0x94
#define D31F2_SIDX        0xa0
#define D31F2_SDAT        0xa4

/* ================================================================== */
/* D30:F0 PCI-to-PCI Bridge Registers                                 */
/* ================================================================== */

#define D30F0_SMLT 0x1b /* Secondary ID Register */

/* ================================================================== */
/* RCBA Register Offsets (base = 0xFED1C000)                          */
/* ================================================================== */

#define RCBA_V0CTL 0x0014
#define RCBA_V1CAP 0x001c
#define RCBA_V1CTL 0x0020
#define RCBA_V1STS 0x0026
#define RCBA_PAT   0x0030 /* 64 bytes */
#define RCBA_CIR1  0x0088
#define RCBA_ESD   0x0104
#define RCBA_ULD   0x0110
#define RCBA_ULBA  0x0118
#define RCBA_LCAP  0x01a4
#define RCBA_LCTL  0x01a8
#define RCBA_LSTS  0x01aa
#define RCBA_CIR2  0x01f4
#define RCBA_CIR3  0x01fc
#define RCBA_CIR4  0x0200
#define RCBA_BCR   0x0220
#define RCBA_DMIC  0x0234
#define RCBA_RPFN  0x0238
#define RCBA_CIR13 0x0f20
#define RCBA_CIR5  0x1d40
#define RCBA_DMC   0x2010
#define RCBA_CIR6  0x2024
#define RCBA_CIR7  0x2034

/* Interrupt pin values for DxxIP registers */
#define NOINT 0
#define INTA  1
#define INTB  2
#define INTC  3
#define INTD  4

/* PIRQ values for DxxIR registers */
#define PIRQA 0
#define PIRQB 1
#define PIRQC 2
#define PIRQD 3
#define PIRQE 4
#define PIRQF 5
#define PIRQG 6
#define PIRQH 7

/* DxxIR nibble offsets */
#define DIR_IDR 12 /* Interrupt D Pin Offset */
#define DIR_ICR 8  /* Interrupt C Pin Offset */
#define DIR_IBR 4  /* Interrupt B Pin Offset */
#define DIR_IAR 0  /* Interrupt A Pin Offset */

#define DIR_ROUTE(x, a, b, c, d) \
	RCBA16(x) = (((d) << DIR_IDR) | ((c) << DIR_ICR) | ((b) << DIR_IBR) | ((a) << DIR_IAR))

/* Interrupt routing - DxxIP (pin assignment) */
#define D31IP      0x3100 /* 32-bit */
#define D31IP_TTIP 24     /* Thermal Throttle Pin (D31:F6) */
#define D31IP_SMIP 12     /* SMBus Pin (D31:F3) */
#define D31IP_SIP  8      /* SATA Pin (D31:F2) */
#define D31IP_IDEP 4      /* IDE/PATA Pin (D31:F1) */
#define D30IP      0x3104 /* 32-bit */
#define D30IP_PIP  0      /* PCI Bridge Pin (D30:F0) */
#define D29IP      0x3108 /* 32-bit */
#define D29IP_E1P  28     /* EHCI #1 Pin (D29:F7) */
#define D29IP_U3P  8      /* UHCI #3 Pin (D29:F2) */
#define D29IP_U2P  4      /* UHCI #2 Pin (D29:F1) */
#define D29IP_U1P  0      /* UHCI #1 Pin (D29:F0) */
#define D28IP      0x310c /* 32-bit */
#define D28IP_P6IP 20     /* PCIe Port 6 (D28:F5) */
#define D28IP_P5IP 16     /* PCIe Port 5 (D28:F4) */
#define D28IP_P4IP 12     /* PCIe Port 4 (D28:F3) */
#define D28IP_P3IP 8      /* PCIe Port 3 (D28:F2) */
#define D28IP_P2IP 4      /* PCIe Port 2 (D28:F1) */
#define D28IP_P1IP 0      /* PCIe Port 1 (D28:F0) */
#define D27IP      0x3110 /* 32-bit */
#define D27IP_ZIP  0      /* HD Audio Pin (D27:F0) */
#define D26IP      0x3114 /* 32-bit */
#define D26IP_E2P  28     /* EHCI #2 Pin (D26:F7) */
#define D26IP_U5P  4      /* UHCI #5 Pin (D26:F1) */
#define D26IP_U4P  0      /* UHCI #4 Pin (D26:F0) */
#define D25IP      0x3118 /* 32-bit */
#define D25IP_LIP  0      /* GbE LAN Pin (D25:F0) */

/* DxxIR - interrupt route (PIRQ mapping) */
#define D31IR 0x3140 /* 16-bit */
#define D30IR 0x3142 /* 16-bit */
#define D29IR 0x3144 /* 16-bit */
#define D28IR 0x3146 /* 16-bit */
#define D27IR 0x3148 /* 16-bit */
#define D26IR 0x314c /* 16-bit */
#define D25IR 0x3150 /* 16-bit */

#define OIC 0x31ff /* 8-bit: IOAPIC control */

/* CIR (Chipset Initialization Registers) - Intel-required */
#define RCBA_CIR8  0x3430
#define RCBA_CIR9  0x350c
#define RCBA_CIR10 0x352c

/* I/O Trap registers */
#define IOTR0 0x1e80 /* 64bit */
#define IOTR1 0x1e88 /* 64bit */
#define IOTR2 0x1e90 /* 64bit */
#define IOTR3 0x1e98 /* 64bit */

/* Misc config */
#define RCBA_RC   0x3400   /* Root Complex config */
#define RCBA_HPTC 0x3404   /* HPET Configuration */
#define RCBA_GCS  0x3410   /* General Control and Status */
#define RCBA_BUC  0x3414   /* Backed Up Control */
#define RCBA_FD   0x3418   /* Function Disable */
#define RCBA_CG   0x341c   /* Clock Gating */
#define RCBA_FDSW 0x3420   /* Function Disable SUS Well */
#define FDSW_LAND (1 << 0) /* LAN Disable */
#define RCBA_MAP  0x35f0   /* UHCI controller remap (R/WO lock) */

/* Function Disable bits (RCBA_FD) */
#define FD_SAD2   (1 << 25) /* SATA #2 */
#define FD_TTD    (1 << 24) /* Thermal Throttle */
#define FD_PE6D   (1 << 21)
#define FD_PE5D   (1 << 20)
#define FD_PE4D   (1 << 19)
#define FD_PE3D   (1 << 18)
#define FD_PE2D   (1 << 17)
#define FD_PE1D   (1 << 16)
#define FD_EHCI1D (1 << 15)
#define FD_LBD    (1 << 14) /* LPC Bridge */
#define FD_EHCI2D (1 << 13)
#define FD_U5D    (1 << 12)
#define FD_U4D    (1 << 11)
#define FD_U3D    (1 << 10)
#define FD_U2D    (1 << 9)
#define FD_U1D    (1 << 8)
#define FD_HDAD   (1 << 4) /* HD Audio */
#define FD_SD     (1 << 3) /* SMBus */
#define FD_SAD1   (1 << 2) /* SATA #1 */

/* EHCI PCI config registers */
#define EHCI_USB_RELNUM    0x60 /* USB Release Number */
#define EHCI_USB_LEGCTLSTS 0x6c /* Legacy Support/Control */
#define EHCI_ACCESS_CNTL   0x80 /* Access Control (bit 0 = write enable) */
#define EHCI_INTEL_FCREG   0xfc /* Intel-specific config (must program) */

/* EHCI MMIO registers (at BAR + CAPLENGTH, CAPLENGTH typically 0x20) */
#define EHCI_CAPLENGTH    0x00
#define EHCI_USBCMD       0x20     /* USB Command (BAR + 0x20 for Intel) */
#define EHCI_USBCMD_RS    (1 << 0) /* Run/Stop */
#define EHCI_USBCMD_HCRST (1 << 1) /* Host Controller Reset */

/* IOAPIC */
#define IO_APIC_ADDR 0xfec00000
#define OIC_AEN      (1 << 0) /* APIC Enable */

/* HPET */
#define HPET_BASE_ADDR 0xfed00000
#define HPTC_HPTE      (1 << 7) /* HPET Address Enable */
#define HPTC_HPAS_MASK (3 << 0) /* HPET Address Select */

/* ================================================================== */
/* D28:Fx PCIe Root Port Registers                                    */
/* ================================================================== */

/* Standard PCI/PCIe config registers used on D28:Fx */
#define D28Fx_CMD      0x04  /* PCI Command */
#define D28Fx_CLS      0x0c  /* Cache Line Size */
#define D28Fx_BCTRL    0x3e  /* Bridge Control */
#define D28Fx_XCAP     0x42  /* PCI Express Capabilities */
#define D28Fx_LCAP     0x4c  /* Link Capabilities (R/WO for ASPM) */
#define D28Fx_LCTL     0x50  /* Link Control (ASPM bits [1:0]) */
#define D28Fx_SLCAP    0x54  /* Slot Capabilities */
#define D28Fx_VC0RCTL  0x114 /* VC0 Resource Control */
#define D28Fx_CTTOMASK 0x148 /* Completion Timeout Mask */
#define D28Fx_CEMASK   0x154 /* Correctable Error Mask (R/WO lock) */

/* D28:Fx extended config */
#define D28Fx_IOXAPIC     0xd8 /* IO xAPIC + hotplug control */
#define D28Fx_BBCLKG      0xe1 /* Backbone Clock Gating */
#define D28Fx_ASPM_MOBILE 0xe8 /* Mobile ASPM config */

/* D28:Fx CIR (Chipset Init) registers */
#define D28Fx_CIR_300 0x300 /* CIR: bit 21 must set, bits 17:16 = link disable */
#define D28Fx_CIR_324 0x324 /* CIR: must write 0x40 */

/* SLCAP field positions */
#define D28_SLCAP_SLOTNUM_SHIFT 19
#define D28_SLCAP_SCALE_SHIFT   16
#define D28_SLCAP_POWER_SHIFT   7

/* ================================================================== */
/* I/O Base Addresses                                                 */
/* ================================================================== */

#define DEFAULT_PMBASE   0x0500
#define DEFAULT_GPIOBASE 0x0580

#define DEFAULT_TCOBASE (DEFAULT_PMBASE + 0x60)

/* ================================================================== */
/* MMIO Base Addresses                                                */
/* ================================================================== */

#define RCBA_BASE 0xfed1c000

/* ================================================================== */
/* GPIO Register Offsets (from GPIO I/O base)                         */
/* ================================================================== */

#define GP_IO_USE_SEL  0x00
#define GP_IO_SEL      0x04
#define GP_LVL         0x0c
#define GPO_BLINK      0x18
#define GPI_INV        0x2c
#define GP_IO_USE_SEL2 0x30
#define GP_IO_SEL2     0x34
#define GP_LVL2        0x38
#define GP_IO_USE_SEL3 0x40
#define GP_IO_SEL3     0x44
#define GP_LVL3        0x48

/* ACPI / PM / TCO registers are defined in southbridge/intel/common/pmutil.h
 * (shared across all ICH southbridges). ICH8 does NOT have 64 GPE events
 * and uses TCO_SPACE_NOT_YET_SPLIT (selected in Kconfig).
 */

/* ================================================================== */
/* i8259 PIC I/O Ports                                                */
/* ================================================================== */

#define MASTER_PIC_ICW1 0x20
#define SLAVE_PIC_ICW1  0xa0
#define ICW_SELECT      (1 << 4)
#define IC4             (1 << 0)

#define MASTER_PIC_ICW2   0x21
#define SLAVE_PIC_ICW2    0xa1
#define INT_VECTOR_MASTER 0x20 /* IRQ0-7 -> INT 0x20-0x27 */
#define INT_VECTOR_SLAVE  0x28 /* IRQ8-15 -> INT 0x28-0x2F */

#define MASTER_PIC_ICW3 0x21
#define CASCADED_PIC    (1 << 2) /* IRQ2 is cascade */
#define SLAVE_PIC_ICW3  0xa1
#define SLAVE_ID        0x02

#define MASTER_PIC_OCW1     0x21 /* IRQ mask register (master) */
#define SLAVE_PIC_OCW1      0xa1 /* IRQ mask register (slave) */
#define ALL_IRQS            0xff
#define IRQ2_MASK           (1 << 2)
#define MICROPROCESSOR_MODE (1 << 0)

/* Edge/Level Control Registers */
#define ELCR1 0x4d0
#define ELCR2 0x4d1

/* ================================================================== */
/* ISA DMA Controller I/O Ports                                       */
/* ================================================================== */

#define DMA1_RESET_REG   0x0D /* Master Clear (slave DMA) */
#define DMA2_RESET_REG   0xDA /* Master Clear (master DMA) */
#define DMA2_MODE_REG    0xD6 /* Mode register (master DMA) */
#define DMA2_MASK_REG    0xD4 /* Single-channel mask (master DMA) */
#define DMA_MODE_CASCADE 0xC0 /* Pass thru DREQ->HRQ, DACK<-HLDA */

/* CMOS/RTC defines are in <pc80/mc146818rtc.h>.
 * i8259 and ISA DMA prototypes are in <pc80/i8259.h> and <pc80/isa-dma.h>.
 */

#ifndef __ACPI__

#include <device/pci_ids.h>
#include <device/pci_ops.h>
#include <stdint.h>
#include <device/pci_type.h>

static inline int lpc_is_mobile(const uint16_t devid)
{
	return (devid == PCI_DID_INTEL_82801HBM_LPC) || (devid == PCI_DID_INTEL_82801HEM_LPC);
}
#define LPC_IS_MOBILE(dev) lpc_is_mobile(pci_read_config16(dev, PCI_DEVICE_ID))

/* ================================================================== */
/* RCBA MMIO access helpers                                           */
/* ================================================================== */

#define rcba_read8(reg)        read8p(RCBA_BASE + (reg))
#define rcba_read16(reg)       read16p(RCBA_BASE + (reg))
#define rcba_read32(reg)       read32p(RCBA_BASE + (reg))
#define rcba_write8(reg, val)  write8p(RCBA_BASE + (reg), (val))
#define rcba_write16(reg, val) write16p(RCBA_BASE + (reg), (val))
#define rcba_write32(reg, val) write32p(RCBA_BASE + (reg), (val))

/* ================================================================== */
/* Function Prototypes                                                */
/* ==================================================================*/

/* early_init.c - pre-RAM southbridge setup */
void i82801hx_early_init(void);
void i82801hx_lpc_setup(void);
void i82801hx_setup_gpio(void);
int i82801hx_detect_s3_resume(void);

/* dmi_setup.c - DMI link configuration */
void i82801hx_dmi_setup(void);
void i82801hx_dmi_poll_vc1(void);

/* lpc.c - post-RAM LPC/IOAPIC init */
void i82801hx_enable_ioapic(void);

/* pcie.c - PCIe root port init, clock gating, RPFN */
void i82801hx_pcie_init(void);

/* early_rcba.c - default interrupt routing */
void southbridge_configure_default_intmap(void);
/* Optional mainboard hook for additional RCBA configuration */
void mainboard_late_rcba_config(void);

#endif /* !__ACPI__ */

#endif /* SOUTHBRIDGE_INTEL_I82801HX_I82801HX_H */
