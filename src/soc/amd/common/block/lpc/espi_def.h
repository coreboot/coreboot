/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_ESPI_DEF_H
#define AMD_BLOCK_ESPI_DEF_H

#define ESPI_DN_TX_HDR0				0x00
#define ESPI_DN_TX_HDR1				0x04
#define ESPI_DN_TX_HDR2				0x08
#define ESPI_DN_TX_DATA				0x0c

#define ESPI_MASTER_CAP				0x2c
#define  ESPI_VW_MAX_SIZE_SHIFT			13
#define  ESPI_VW_MAX_SIZE_MASK			(0x3f << ESPI_VW_MAX_SIZE_SHIFT)

#define ESPI_GLOBAL_CONTROL_0			0x30
#define  ESPI_WAIT_CNT_SHIFT			24
#define  ESPI_WAIT_CNT_MASK			(0x3f << ESPI_WAIT_CNT_SHIFT)
#define  ESPI_WDG_CNT_SHIFT			8
#define  ESPI_WDG_CNT_MASK			(0xffff << ESPI_WDG_CNT_SHIFT)
#define  ESPI_AL_IDLE_TIMER_SHIFT		4
#define  ESPI_AL_IDLE_TIMER_MASK		(0x7 << ESPI_AL_IDLE_TIMER_SHIFT)
#define  ESPI_AL_STOP_EN			(1 << 3)
#define  ESPI_PR_CLKGAT_EN			(1 << 2)
#define  ESPI_WAIT_CHKEN			(1 << 1)
#define  ESPI_WDG_EN				(1 << 0)

#define ESPI_GLOBAL_CONTROL_1			0x34
#define  ESPI_ALERT_ENABLE			(1 << 20) /* Sabrina and later SoCs */
#define  ESPI_RGCMD_INT_MAP_SHIFT		13
#define  ESPI_RGCMD_INT_MAP_MASK		(0x1f << ESPI_RGCMD_INT_MAP_SHIFT)
#define    ESPI_RGCMD_INT(irq)			((irq) << ESPI_RGCMD_INT_MAP_SHIFT)
#define    ESPI_RGCMD_INT_SMI			(0x1f << ESPI_RGCMD_INT_MAP_SHIFT)
#define  ESPI_ERR_INT_MAP_SHIFT			8
#define  ESPI_ERR_INT_MAP_MASK			(0x1f << ESPI_ERR_INT_MAP_SHIFT)
#define    ESPI_ERR_INT(irq)			((irq) << ESPI_ERR_INT_MAP_SHIFT)
#define    ESPI_ERR_INT_SMI			(0x1f << ESPI_ERR_INT_MAP_SHIFT)
#define  ESPI_SUB_DECODE_SLV_SHIFT		3
#define  ESPI_SUB_DECODE_SLV_MASK		(0x3 << ESPI_SUB_DECODE_SLV_SHIFT)
#define  ESPI_SUB_DECODE_EN			(1 << 2)
#define  ESPI_BUS_MASTER_EN			(1 << 1)
#define  ESPI_SW_RST				(1 << 0)

/* bits in ESPI_DECODE 0x40 */
#define  ESPI_DECODE_MMIO_RANGE_EN(range)	(1 << (((range) & 3) + 12))
#define  ESPI_DECODE_IO_RANGE_EN(range)		(1 << (((range) & 3) + 8))

#define ESPI_IO_BASE_REG0			0x44
#define ESPI_IO_BASE_REG1			0x48
#define ESPI_IO_SIZE0				0x4c
#define ESPI_MMIO_BASE_REG0			0x50
#define ESPI_MMIO_BASE_REG1			0x54
#define ESPI_MMIO_BASE_REG2			0x58
#define ESPI_MMIO_BASE_REG3			0x5c
#define ESPI_MMIO_SIZE_REG0			0x60
#define ESPI_MMIO_SIZE_REG1			0x64

#define ESPI_SLAVE0_INT_EN			0x6c
#define ESPI_SLAVE0_INT_STS			0x70
#define  ESPI_STATUS_DNCMD_COMPLETE		(1 << 28)
#define  ESPI_STATUS_NON_FATAL_ERROR		(1 << 6)
#define  ESPI_STATUS_FATAL_ERROR		(1 << 5)
#define  ESPI_STATUS_NO_RESPONSE		(1 << 4)
#define  ESPI_STATUS_CRC_ERR			(1 << 2)
#define  ESPI_STATUS_WAIT_TIMEOUT		(1 << 1)
#define  ESPI_STATUS_BUS_ERROR			(1 << 0)

#define ESPI_RXVW_POLARITY			0xac

#define ESPI_IO_RANGE_BASE_REG(base, range)	((base) + ((range) & 3) * 2)
#define ESPI_IO_RANGE_SIZE_REG(base, range)	((base) + ((range) & 3))
#define ESPI_MMIO_RANGE_BASE_REG(base, range)	((base) + ((range) & 3) * 4)
#define ESPI_MMIO_RANGE_SIZE_REG(base, range)	((base) + ((range) & 3) * 2)

#endif /* AMD_BLOCK_ESPI_DEF_H */
