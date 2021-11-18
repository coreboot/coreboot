/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _DENVERTON_NS_SMBUS_H_
#define _DENVERTON_NS_SMBUS_H_

/* PCI Configuration Space (D31:F4): SMBus */
#define SMB_BASE 0x20
#define HOSTC 0x40
#define HST_EN (1 << 0)
#define HOSTC_SMI_EN (1 << 1)
#define HOSTC_I2C_EN (1 << 2)

/* SMBUS TCO base address. */
#define TCOBASE 0x50
#define MASK_TCOBASE 0xffe0
#define TCOCTL 0x54
#define TCOBASE_EN (1 << 8)
#define TCOBASE_LOCK (1 << 0)

#define SMBUS_SLAVE_ADDR		0x44

/*
 * SMBus Private Config Registers (PID:SMB)
 */
#define PCR_SMBUS_TCOCFG 0x00	/* TCO Configuration register */
#define PCR_SMBUS_TCOCFG_IE (1 << 7) /* TCO IRQ Enable */
#define PCR_SMBUS_TCOCFG_IS 7	/* TCO IRQ Select */
#define PCR_SMBUS_TCOCFG_IRQ_9 0x00
#define PCR_SMBUS_TCOCFG_IRQ_10 0x01
#define PCR_SMBUS_TCOCFG_IRQ_11 0x02
#define PCR_SMBUS_TCOCFG_IRQ_20 0x04   /* only if APIC enabled */
#define PCR_SMBUS_TCOCFG_IRQ_21 0x05   /* only if APIC enabled */
#define PCR_SMBUS_TCOCFG_IRQ_22 0x06   /* only if APIC enabled */
#define PCR_SMBUS_TCOCFG_IRQ_23 0x07   /* only if APIC enabled */
#define PCR_SMBUS_SMBTM 0x04	   /* SMBus Test Mode */
#define PCR_SMBUS_SMBTM_SMBCT (1 << 1) /* SMBus Counter */
#define PCR_SMBUS_SMBTM_SMBDG (1 << 0) /* SMBus Deglitch */
#define PCR_SMBUS_SCTM 0x08	    /* Short Counter Test Mode */
#define PCR_SMBUS_SCTM_SSU (1 << 31)   /* Simulation Speed-Up */
#define PCR_SMBUS_GC 0x0C	      /* General Control */
#define PCR_SMBUS_GC_FD (1 << 0)       /* Function Disable */
#define PCR_SMBUS_GC_NR (1 << 1)       /* No Reboot */
#define PCR_SMBUS_GC_SMBSCGE (1 << 2)  /* SMB Static Clock Gating Enable */
#define PCR_SMBUS_PCE 0x10	     /* Power Control Enable */
#define PCR_SMBUS_PCE_HAE (1 << 5)     /* Hardware Autonomous Enable */

#endif /* _DENVERTON_NS_SMBUS_H_ */
