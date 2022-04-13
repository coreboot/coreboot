/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef SOC_INTEL_COMMON_BLOCK_FAST_SPI_DEF_H
#define SOC_INTEL_COMMON_BLOCK_FAST_SPI_DEF_H

/* PCI configuration registers */

#define SPI_BIOS_CONTROL		0xdc

/* Extended Bios Support Registers */
#define SPI_CFG_BAR1			0xe0	/* SPI BAR1 MMIO */

/* Bit definitions for BIOS_CONTROL */
#define SPI_BIOS_CONTROL_WPD			(1 << 0)
#define SPI_BIOS_CONTROL_LOCK_ENABLE		(1 << 1)
#define SPI_BIOS_CONTROL_CACHE_DISABLE		(1 << 2)
#define SPI_BIOS_CONTROL_PREFETCH_ENABLE	(1 << 3)
#define SPI_BIOS_CONTROL_EISS			(1 << 5)
#define SPI_BIOS_CONTROL_BILD			(1 << 7)
#define SPI_BIOS_CONTROL_SYNC_SS		(1 << 8)
#define SPI_BIOS_CONTROL_EXT_BIOS_ENABLE	(1 << 27)
#define SPI_BIOS_CONTROL_EXT_BIOS_LOCK_ENABLE	(1 << 28)
#define SPI_BIOS_CONTROL_EXT_BIOS_LIMIT(x)	((x) & ~(0xfff))

/* Register offsets from the MMIO region base (PCI_BASE_ADDRESS_0) */

#define SPIBAR_BFPREG			0x00
#define SPIBAR_HSFSTS_CTL		0x04
#define SPIBAR_FADDR			0x08
#define SPIBAR_DLOCK			0x0c
#define SPIBAR_FDATA(n)			(0x10 + ((n) & 0xf) * 4)
#define SPIBAR_FPR_BASE			0x84
#define SPIBAR_FPR(n)			(0x84 + (4 * (n)))
#define SPIBAR_PREOP			0xa4
#define SPIBAR_OPTYPE			0xa6
#define SPIBAR_OPMENU_LOWER		0xa8
#define SPIBAR_OPMENU_UPPER		0xac
#define SPIBAR_FDOC			0xb4
#define SPIBAR_FDOD			0xb8
#define SPIBAR_PTINX			0xcc
#define SPIBAR_PTDATA			0xd0

/* Bit definitions for BFPREG (0x00) register */
#define SPIBAR_BFPREG_PRB_MASK		0x7fff
#define SPIBAR_BFPREG_PRL_SHIFT		16
#define SPIBAR_BFPREG_PRL_MASK		(0x7fff << SPIBAR_BFPREG_PRL_SHIFT)
#define SPIBAR_BFPREG_SBRS		(1 << 31)

/* Bit definitions for HSFSTS_CTL (0x04) register */
#define SPIBAR_HSFSTS_FDBC_MASK		(0x3f << 24)
#define SPIBAR_HSFSTS_FDBC(n)		(((n) << 24) & SPIBAR_HSFSTS_FDBC_MASK)
#define SPIBAR_HSFSTS_WET		(1 << 21)
#define SPIBAR_HSFSTS_FCYCLE_MASK	(0xf << 17)
#define SPIBAR_HSFSTS_FCYCLE(cyc)	(((cyc) << 17) & SPIBAR_HSFSTS_FCYCLE_MASK)

/* Supported flash cycle types */
#define SPIBAR_HSFSTS_CYCLE_READ	SPIBAR_HSFSTS_FCYCLE(0)
#define SPIBAR_HSFSTS_CYCLE_WRITE	SPIBAR_HSFSTS_FCYCLE(2)
#define SPIBAR_HSFSTS_CYCLE_4K_ERASE	SPIBAR_HSFSTS_FCYCLE(3)
#define SPIBAR_HSFSTS_CYCLE_64K_ERASE	SPIBAR_HSFSTS_FCYCLE(4)
#define SPIBAR_HSFSTS_CYCLE_RD_STATUS	SPIBAR_HSFSTS_FCYCLE(8)

#define SPIBAR_HSFSTS_FGO		(1 << 16)
#define SPIBAR_HSFSTS_FLOCKDN		(1 << 15)
#define SPIBAR_HSFSTS_FDV		(1 << 14)
#define SPIBAR_HSFSTS_FDOPSS		(1 << 13)
#define SPIBAR_HSFSTS_PRR34_LOCKDN	(1 << 12)
#define SPIBAR_HSFSTS_WRSDIS		(1 << 11)
#define SPIBAR_HSFSTS_SAF_CE		(1 << 8)
#define SPIBAR_HSFSTS_SAF_ACTIVE	(1 << 7)
#define SPIBAR_HSFSTS_SAF_LE		(1 << 6)
#define SPIBAR_HSFSTS_SCIP		(1 << 5)
#define SPIBAR_HSFSTS_SAF_DLE		(1 << 4)
#define SPIBAR_HSFSTS_SAF_ERROR		(1 << 3)
#define SPIBAR_HSFSTS_AEL		(1 << 2)
#define SPIBAR_HSFSTS_FCERR		(1 << 1)
#define SPIBAR_HSFSTS_FDONE		(1 << 0)
#define SPIBAR_HSFSTS_W1C_BITS		0xff

/* Bit definitions for FADDR (0x08) register */
#define SPIBAR_FADDR_MASK		0x7ffffff

/* Bit definitions for DLOCK (0x0c) register */
#define SPIBAR_DLOCK_PR0LOCKDN		(1 << 8)
#define SPIBAR_DLOCK_PR1LOCKDN		(1 << 9)
#define SPIBAR_DLOCK_PR2LOCKDN		(1 << 10)
#define SPIBAR_DLOCK_PR3LOCKDN		(1 << 11)
#define SPIBAR_DLOCK_PR4LOCKDN		(1 << 12)

/* Maximum bytes of data that can fit in FDATAn (0x10) registers */
#define SPIBAR_FDATA_FIFO_SIZE		0x40

/* Bit definitions for FDOC (0xb4) register */
#define SPIBAR_FDOC_COMPONENT		(1 << 12)
#define SPIBAR_FDOC_FDSI_1		(1 << 2)

/* Flash Descriptor Component Section - Component 0 Density Bit Settings */
#define FLCOMP_C0DEN_MASK		0xf
#define FLCOMP_C0DEN_8MB		4
#define FLCOMP_C0DEN_16MB		5
#define FLCOMP_C0DEN_32MB		6

/* Bit definitions for FPRn (0x84 + (4 * n)) registers */
#define SPIBAR_FPR_WPE			(1 << 31) /* Flash Write protected */
#define SPIBAR_FPR_MAX			5

/* Programmable values for OPMENU_LOWER(0xa8) & OPMENU_UPPER(0xac) register */
#define SPI_OPMENU_0			0x01 /* WRSR: Write Status Register */
#define SPI_OPTYPE_0			0x01 /* Write, no address */
#define SPI_OPMENU_1			0x02 /* BYPR: Byte Program */
#define SPI_OPTYPE_1			0x03 /* Write, address required */
#define SPI_OPMENU_2			0x03 /* READ: Read Data */
#define SPI_OPTYPE_2			0x02 /* Read, address required */
#define SPI_OPMENU_3			0x05 /* RDSR: Read Status Register */
#define SPI_OPTYPE_3			0x00 /* Read, no address */
#define SPI_OPMENU_4			0x20 /* SE20: Sector Erase 0x20 */
#define SPI_OPTYPE_4			0x03 /* Write, address required */
#define SPI_OPMENU_5			0x9f /* RDID: Read ID */
#define SPI_OPTYPE_5			0x00 /* Read, no address */
#define SPI_OPMENU_6			0xd8 /* BED8: Block Erase 0xd8 */
#define SPI_OPTYPE_6			0x03 /* Write, address required */
#define SPI_OPMENU_7			0x0b /* FAST: Fast Read */
#define SPI_OPTYPE_7			0x02 /* Read, address required */
#define SPI_OPMENU_UPPER ((SPI_OPMENU_7 << 24) | (SPI_OPMENU_6 << 16) | \
			  (SPI_OPMENU_5 << 8) | SPI_OPMENU_4)
#define SPI_OPMENU_LOWER ((SPI_OPMENU_3 << 24) | (SPI_OPMENU_2 << 16) | \
			  (SPI_OPMENU_1 << 8) | SPI_OPMENU_0)
#define SPI_OPTYPE ((SPI_OPTYPE_7 << 14) | (SPI_OPTYPE_6 << 12) | \
		    (SPI_OPTYPE_5 << 10) | (SPI_OPTYPE_4 << 8)  | \
		    (SPI_OPTYPE_3 << 6) | (SPI_OPTYPE_2 << 4)   | \
		    (SPI_OPTYPE_1 << 2) | (SPI_OPTYPE_0))
#define SPI_OPPREFIX ((0x50 << 8) | 0x06) /* EWSR and WREN */

/* Bit definitions for PTINX (0xcc) register */
#define SPIBAR_PTINX_COMP_0		(0 << 14)
#define SPIBAR_PTINX_COMP_1		(1 << 14)
#define SPIBAR_PTINX_HORD_SFDP		(0 << 12)
#define SPIBAR_PTINX_HORD_PARAM		(1 << 12)
#define SPIBAR_PTINX_HORD_JEDEC		(2 << 12)
#define SPIBAR_PTINX_IDX_MASK		0xffc

/* Register Offsets of BIOS Flash Program Registers */
#define SPIBAR_RESET_LOCK		0xf0
#define SPIBAR_RESET_CTRL		0xf4
#define SPIBAR_RESET_DATA		0xf8

/* Programmable values of Bit0 (SSL) of Set STRAP MSG LOCK (0xF0) Register */
#define SPIBAR_RESET_LOCK_DISABLE	0 /* Set_Strap Lock(SSL) Bit 0 = 0 */
#define SPIBAR_RESET_LOCK_ENABLE	1 /* Set_Strap Lock(SSL) Bit 0 = 1 */

/* Programmable values of Bit0(SSMS) of Set STRAP MSG Control (0xF4) Register*/
#define SPIBAR_RESET_CTRL_SSMC		1 /* Set_Strap Mux Select(SSMS) Bit=1*/

#define SPIBAR_HWSEQ_XFER_TIMEOUT_MS	5000

void *fast_spi_get_bar(void);

#endif	/* SOC_INTEL_COMMON_BLOCK_FAST_SPI_DEF_H */
