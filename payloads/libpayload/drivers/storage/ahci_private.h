/*
 *
 * Copyright (C) 2012 secunet Security Networks AG
 * Copyright (C) 2013 Edward O'Callaghan <eocallaghan@alterapraxis.com>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _AHCI_PRIVATE_H
#define _AHCI_PRIVATE_H

#include <stdint.h>
#include <storage/ata.h>
#include <storage/atapi.h>

typedef volatile struct {
	u64 cmdlist_base;
	u64 frameinfo_base;
	u32 intr_status;
	u32 intr_enable;
	u32 cmd_stat;
	u32 _reserved0;
	u32 taskfile_data;
	u32 signature;
	u32 sata_status;
	u32 sata_control;
	u32 sata_error;
	u32 sata_active;
	u32 cmd_issue;
	u32 sata_notify;
	u32 fis_based_switch;
	u32 _reserved1[11];
	u32 _vendor[4];
} hba_port_t;

#define HBA_PxIS_TFES	(1 << 30)	/* TFES - Task File Error Status */
#define HBA_PxIS_HBFS	(1 << 29)	/* HBFS - Host Bus Fatal Error Status */
#define HBA_PxIS_HBDS	(1 << 28)	/* HBDS - Host Bus Data Error Status */
#define HBA_PxIS_IFS	(1 << 27)	/* IFS - Interface Fatal Error Status */
#define HBA_PxIS_FATAL	(HBA_PxIS_TFES | HBA_PxIS_HBFS | \
				HBA_PxIS_HBDS | HBA_PxIS_IFS)
#define HBA_PxIS_PCS	(1 <<  6)	/* PCS - Port Connect Change Status */

#define HBA_PxCMD_ICC_SHIFT	28
#define HBA_PxCMD_ICC_MASK	(0xf << HBA_PxCMD_ICC_SHIFT)
#define HBA_PxCMD_ICC_ACTIVE	(0x1 << HBA_PxCMD_ICC_SHIFT)
#define HBA_PxCMD_CR		(1 << 15) /* CR - Command list Running */
#define HBA_PxCMD_FR		(1 << 14) /* FR - FIS receive Running */
#define HBA_PxCMD_FRE		(1 <<  4) /* FRE - FIS Receive Enable */
#define HBA_PxCMD_SUD		(1 <<  1) /* SUD - Spin-Up Device */
#define HBA_PxCMD_ST		(1 <<  0) /* ST - Start (command processing) */

#define HBA_PxTFD_BSY		(1 <<  7)
#define HBA_PxTFD_DRQ		(1 <<  3)

#define HBA_PxSSTS_IPM_SHIFT		8
#define HBA_PxSSTS_IPM_MASK		(0xf << HBA_PxSSTS_IPM_SHIFT)
#define HBA_PxSSTS_IPM_ACTIVE		(1 << HBA_PxSSTS_IPM_SHIFT)
#define HBA_PxSSTS_DET_SHIFT		0
#define HBA_PxSSTS_DET_MASK		(0xf << HBA_PxSSTS_DET_SHIFT)
#define HBA_PxSSTS_DET_ESTABLISHED	(3 << HBA_PxSSTS_DET_SHIFT)

#define HBA_PxSCTL_DET_SHIFT		0
#define HBA_PxSCTL_DET_MASK		(0xf << HBA_PxSCTL_DET_SHIFT)
#define HBA_PxSCTL_DET_COMRESET		(0x1 << HBA_PxSCTL_DET_SHIFT)

#define	HBA_PxSIG_ATA	0x00000101 /* SATA drive */
#define	HBA_PxSIG_ATAPI	0xeb140101 /* SATAPI drive */
#define	HBA_PxSIG_SEMB	0xc33c0101 /* Enclosure management bridge */
#define	HBA_PxSIG_PM	0x96690101 /* Port multiplier */

typedef volatile struct {
	u32 caps;
	u32 global_ctrl;
	u32 intr_status;
	u32 ports_impl;
	u32 version;
	u32 ccc_ctrl;		/* CCC - Command Completion Coalescing */
	u32 ccc_ports;
	u32 em_location;	/* EM - Enclosure Management */
	u32 em_ctrl;
	u32 ext_caps;
	u32 handoff_ctrl_stat;
	u32 _reserved0[13];
	u32 _reserved_nvmchi[16];
	u32 _vendor[24];
	hba_port_t ports[32];
} hba_ctrl_t;

#define HBA_CAPS_SSS		(1 << 27) /* SSS - Supports Staggered Spin-up */
#define HBA_CAPS_NCS_SHIFT	8	/* NCS - Number of Command Slots */
#define HBA_CAPS_NCS_MASK	(0x1f << HBA_CAPS_NCS_SHIFT)

#define HBA_CAPS_DECODE_NCS(caps)	(((caps & HBA_CAPS_NCS_MASK)	\
					  >> HBA_CAPS_NCS_SHIFT)	\
					 + 1)

#define HBA_CTRL_AHCI_EN	(1 << 31)
#define HBA_CTRL_INTR_EN	(1 <<  1)
#define HBA_CTRL_RESET		(1 <<  0)

typedef volatile struct {
	u8 dma_setup_fis[28];
	u8 _reserved0[4];
	u8 pio_setup_fis[20];
	u8 _reserved1[12];
	u8 d2h_register_fis[20];
	u8 _reserved2[4];
	u8 set_device_bits_fis[8];
	u8 unknown_fis[64];
	u8 _reserved3[96];
} rcvd_fis_t;

typedef volatile struct {
	u16 cmd;
	u16 prdt_length;
	u32 prd_bytes;
	u64 cmdtable_base;
	u8 _reserved[16];
} cmd_t;

#define CMD_PMP_SHIFT	12		/* PMP - Port Multiplier Port */
#define CMD_PMP_MASK	(0xf << CMD_PMP_SHIFT)
#define CMD_PMP(x)	((x << CMD_PMP_SHIFT) & CMD_PMP_MASK)
#define CMD_CBuROK	(1 << 10)	/* C - Clear Busy upon R_OK */
#define CMD_BIST	(1 <<  9)	/* B - BIST - Built-In Selft Test */
#define CMD_RESET	(1 <<  8)	/* R - Reset */
#define CMD_PREFETCH	(1 <<  7)	/* P - Prefetch (PRDTs or ATAPI cmd) */
#define CMD_WRITE	(1 <<  6)	/* W - Write (host to device) */
#define CMD_ATAPI	(1 <<  5)	/* A - ATAPI cmd */
#define CMD_CFL_SHIFT	0		/* CFL - Command FIS Length */
#define CMD_CFL_MASK	(0xf << CMD_CFL_SHIFT)
#define CMD_CFL(x)	((((x) >> 2) << CMD_CFL_SHIFT) & CMD_CFL_MASK)

typedef volatile struct {
	u8 fis[64];
	u8 atapi_cmd[16];
	u8 _reserved0[48];
	struct {
		u64 data_base;
		u32 _reserved0;
		u32 flags;
	} prdt[8]; /* Can be up to 65,535 prds,
		      but implementation needs multiple of 128 bytes. */
} cmdtable_t;

#define BYTES_PER_PRD_SHIFT	20
#define BYTES_PER_PRD		(4 << 20)

enum {
	FIS_HOST_TO_DEVICE	= 0x27,
};
#define FIS_H2D_CMD	(1 << 7)
#define FIS_H2D_FIS_LEN	20
#define FIS_H2D_DEV_LBA	(1 << 6)

#define PRD_TABLE_I		(1 << 31) /* I - Interrupt on Completion */
#define PRD_TABLE_BYTES_MASK	0x3fffff
#define PRD_TABLE_BYTES(x)	(((x) - 1) & PRD_TABLE_BYTES_MASK)

typedef struct {
	union {
		ata_dev_t ata_dev;
		atapi_dev_t atapi_dev;
	};

	hba_ctrl_t *ctrl;
	hba_port_t *port;

	cmd_t *cmdlist;
	cmdtable_t *cmdtable;
	rcvd_fis_t *rcvd_fis;

	u8 *buf, *user_buf;
	int write_back;
	size_t buflen;
} ahci_dev_t;

/*
 * ahci_common.c
 */
int ahci_cmdengine_start(hba_port_t *const port);

int ahci_cmdengine_stop(hba_port_t *const port);

ssize_t ahci_cmdslot_exec(ahci_dev_t *const dev);

size_t ahci_cmdslot_prepare(ahci_dev_t *const dev,
		   u8 *const user_buf, size_t buf_len,
		   const int out);

int ahci_identify_device(ata_dev_t *const ata_dev, u8 *const buf);

int ahci_error_recovery(ahci_dev_t *const dev, const u32 intr_status);

/*
 * ahci_atapi.c
 */
ssize_t ahci_packet_read_cmd(atapi_dev_t *const _dev,
		    const u8 *const cmd, const size_t cmdlen,
		    u8 *const buf, const size_t buflen);

/*
 * ahci_ata.c
 */
ssize_t ahci_ata_read_sectors(ata_dev_t *const ata_dev,
		     const lba_t start, size_t count,
		     u8 *const buf);

#endif /* _AHCI_PRIVATE_H */
