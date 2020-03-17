/*
 * This file is part of the coreboot project.
 *
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

/* SMBus I/O bits. */
#define SMBHSTSTAT 0x0
#define HST_HBSY (1 << 0)
#define HST_INTR (1 << 1)
#define HST_DERR (1 << 2)
#define HST_BERR (1 << 3)
#define HST_BYTE_DONE_STS (1 << 7)
#define HST_HSTS_ALL 0xFF
#define SMBHSTCTL 0x2
#define HST_LAST_BYTE (1 << 5)
#define HST_START (1 << 6)
#define HST_CMD_IIC_READ 0x18
#define HST_READ 0x01 // RW
#define SMBHSTCMD 0x3
#define SMBXMITADD 0x4
#define SMBHSTDAT0 0x5
#define SMBHSTDAT1 0x6
#define SMBBLKDAT 0x7
#define SMBTRNSADD 0x9
#define SMBSLVDATA 0xa
#define SMBHSTAUXC 0xd
#define SMLINK_PIN_CTL 0xe
#define SMBUS_PIN_CTL 0xf

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
