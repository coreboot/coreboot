/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _BAYTRAIL_PCIE_H_
#define _BAYTRAIL_PCIE_H_

/* PCIe root port config space registers. */
#define XCAP			0x40
# define SI			(1 << 24)
#define DCAP			0x44
# define MPS_MASK		0x7
#define DCTL_DSTS		0x48
# define URE			(1 << 3)
# define FEE			(1 << 2)
# define NFE			(1 << 1)
# define CEE			(1 << 0)
#define LCAP			0x4c
# define L1EXIT_SHIFT		15
# define L1EXIT_MASK		(0x7 << L1EXIT_SHIFT)
#define LCTL			0x50
# define CCC			(1 << 6)
# define RL			(1 << 5)
# define LD			(1 << 4)
#define LSTS			0x52
#define SLCAP			0x54
# define SLN_SHIFT		19
# define SLS_SHIFT		15
# define SLV_SHIFT		7
# define HPC			(1 << 6)
# define HPS			(1 << 5)
#define SLCTL_SLSTS		0x58
# define PDS			(1 << 22)
#define DCAP2			0x64
# define OBFFS			(0x3 << 18)
# define LTRMS			(1 << 11)
#define DSTS2			0x68
# define OBFFEN			(3 << 13)
# define LTRME			(1 << 10)
# define CTD			(1 <<  4)
#define CHCFG			0xd0
# define UPSD			(1 << 24)
# define UNRS			(1 << 15)
# define UPRS			(1 << 14)
#define MPC2			0xd4
# define IPF			(1 << 11)
# define LSTP			(1 <<  6)
# define EOIFD			(1 <<  1)
#define MPC			0xd8
# define CCEL_SHIFT		15
# define CCEL_MASK		(0x7 << CCEL_SHIFT)
#define RPPGEN			0xe0
# define RPSCGEN		(1 << 15)
# define LCLKREQEN		(1 << 13)
# define BBCLKREQEN		(1 << 12)
# define SRDLCGEN		(1 << 11)
# define SRDBCGEN		(1 << 10)
# define RPDLCGEN		(1 << 9)
# define RPDBCGEN		(1 << 8)
#define PWRCTL			0xe8
# define RPL1SQPOL		(1 << 1)
# define RPDTSQPOL		(1 << 0)
#define PHYCTL2_IOSFBCTL	0xf4
# define PLL_OFF_EN		(1 << 8)
# define TDFT			(3 << 14)
# define TXCFGCHWAIT		(3 << 12)
# define SIID			(3 << 26)
#define STRPFUSECFG		0xfc
# define LANECFG_SHIFT		14
# define LANECFG_MASK		(0x3 << LANECFG_SHIFT)
#define AERCH			0x100
#define NFTS			0x314
#define L0SC			0x318
#define CFG2			0x320
# define CSREN			(1 << 22)
# define LATGC_SHIFT		6
# define LATGC_MASK		(0x7 << LATGC_SHIFT)
#define PCIEDBG			0x324
# define SPCE			(1 << 5)
#define PCIESTS1		0x328
#define PCIEALC			0x338
#define RTP			0x33c
#define PHYCTL4			0x408
# define SQDIS			(1 << 27)

#define PCIE_ROOT_PORT_COUNT	4
#endif /* _BAYTRAIL_PCIE_H_ */
