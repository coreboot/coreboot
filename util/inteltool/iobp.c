/* inteltool - dump all registers on an Intel CPU + chipset based system */
/* SPDX-License-Identifier: GPL-2.0-only */

#include <stdio.h>
#include <stdlib.h>
#include "inteltool.h"

#define RCBA16(rcba, x) (*((volatile u16 *)((rcba) + (x))))
#define RCBA32(rcba, x) (*((volatile u32 *)((rcba) + (x))))
/* IO Buffer Programming */
#define IOBPIRI		0x2330
#define IOBPD		0x2334
#define IOBPS		0x2338
#define  IOBPS_READY	0x0001
#define  IOBPS_TX_MASK	0x0006
#define  IOBPS_MASK	0xff00
#define  IOBPS_READ	0x0600
#define  IOBPS_WRITE	0x0700
#define IOBPU		0x233a
#define  IOBPU_MAGIC	0xf000

#define IOBP_RETRY 1000
static inline int iobp_poll(volatile uint8_t *rcba)
{
	for (int try = IOBP_RETRY; try > 0; try--) {
		u16 status = RCBA16(rcba, IOBPS);
		if ((status & IOBPS_READY) == 0)
			return 1;
		// udelay(10);
	}

	printf("IOBP: timeout waiting for transaction to complete\n");
	return 0;
}

static u32 pch_iobp_read(volatile uint8_t *rcba, u32 address)
{
	u16 status;

	if (!iobp_poll(rcba))
		return 0;

	/* Set the address */
	RCBA32(rcba, IOBPIRI) = address;

	/* READ OPCODE */
	status = RCBA16(rcba, IOBPS);
	status &= ~IOBPS_MASK;
	status |= IOBPS_READ;
	RCBA16(rcba, IOBPS) = status;

	/* Undocumented magic */
	RCBA16(rcba, IOBPU) = IOBPU_MAGIC;

	/* Set ready bit */
	status = RCBA16(rcba, IOBPS);
	status |= IOBPS_READY;
	RCBA16(rcba, IOBPS) = status;

	if (!iobp_poll(rcba))
		return 0;

	/* Check for successful transaction */
	status = RCBA16(rcba, IOBPS);
	if (status & IOBPS_TX_MASK) {
		printf("IOBP: read 0x%08x failed\n", address);
		return 0;
	}

	/* Read IOBP data */
	return RCBA32(rcba, IOBPD);
}

struct iobp_register {
	u32 addr;
	const char *name;
};

static const struct iobp_register lynxpoint_iobp_registers[] = {
	/* SATA Electrical Control Register */
	{0xea002488, "SECRT88P0"},
	{0xea00248c, "SECRT8CP0"},
	{0xea002490, "SECRT90P0"},
	{0xea002498, "SECRT98P0"},
	{0xea00251c, "SECRR1CP0"},
	{0xea002550, "SECRR50P0"},
	{0xea002554, "SECRR54P0"},
	{0xea002558, "SECRR58P0"},

	{0xea002688, "SECRT88P1"},
	{0xea00268c, "SECRT8CP1"},
	{0xea002690, "SECRT90P1"},
	{0xea002698, "SECRT98P1"},
	{0xea00271c, "SECRR1CP1"},
	{0xea002750, "SECRR50P1"},
	{0xea002754, "SECRR54P1"},
	{0xea002758, "SECRR58P1"},

	{0xea000888, "SECRT88P2"},
	{0xea00088c, "SECRT8CP2"},
	{0xea000890, "SECRT90P2"},
	{0xea000898, "SECRT98P2"},
	{0xea00091c, "SECRR1CP2"},
	{0xea000950, "SECRR50P2"},
	{0xea000954, "SECRR54P2"},
	{0xea000958, "SECRR58P2"},

	{0xea000a88, "SECRT88P3"},
	{0xea000a8c, "SECRT8CP3"},
	{0xea000a90, "SECRT90P3"},
	{0xea000a98, "SECRT98P3"},
	{0xea000b1c, "SECRR1CP3"},
	{0xea000b50, "SECRR50P3"},
	{0xea000b54, "SECRR54P3"},
	{0xea000b58, "SECRR58P3"},

	{0xea002088, "SECRT88P4"},
	{0xea00208c, "SECRT8CP4"},
	{0xea002090, "SECRT90P4"},
	{0xea002098, "SECRT98P4"},
	{0xea00211c, "SECRR1CP4"},
	{0xea002150, "SECRR50P4"},
	{0xea002154, "SECRR54P4"},
	{0xea002158, "SECRR58P4"},

	{0xea002288, "SECRT88P5"},
	{0xea00228c, "SECRT8CP5"},
	{0xea002290, "SECRT90P5"},
	{0xea002298, "SECRT98P5"},
	{0xea00231c, "SECRR1CP5"},
	{0xea002350, "SECRR50P5"},
	{0xea002354, "SECRR54P5"},
	{0xea002358, "SECRR58P5"},

	{0xea008100, "SECRF00"},
	{0xea008104, "SECRF04"},

	/* USB 2.0 Electrical Control Register */
	{0xe5004100, "U2ECRP01"},
	{0xe5004200, "U2ECRP02"},
	{0xe5004300, "U2ECRP03"},
	{0xe5004400, "U2ECRP04"},
	{0xe5004500, "U2ECRP05"},
	{0xe5004600, "U2ECRP06"},
	{0xe5004700, "U2ECRP07"},
	{0xe5004800, "U2ECRP08"},
	{0xe5004900, "U2ECRP09"},
	{0xe5004a00, "U2ECRP10"},
	{0xe5004b00, "U2ECRP11"},
	{0xe5004c00, "U2ECRP12"},
	{0xe5004d00, "U2ECRP13"},
	{0xe5004e00, "U2ECRP14"},

	/* IOBP related to USB 3.0 ports */
	/* port 1 */
	{0xe900175c, ""},
	{0xe9001760, ""},
	{0xe9001768, ""},
	{0xe9001770, ""},
	{0xe90017cc, ""},
	/* port 2 */
	{0xe900155c, ""},
	{0xe9001560, ""},
	{0xe9001568, ""},
	{0xe9001570, ""},
	{0xe90015cc, ""},
	/* port 3 */
	{0xe9002f5c, ""},
	{0xe9002f60, ""},
	{0xe9002f68, ""},
	{0xe9002f70, ""},
	{0xe9002fcc, ""},
	/* port 4 */
	{0xe9002d5c, ""},
	{0xe9002d60, ""},
	{0xe9002d68, ""},
	{0xe9002d70, ""},
	{0xe9002dcc, ""},
	/* port 5 */
	{0xe900335c, ""},
	{0xe9003360, ""},
	{0xe9003368, ""},
	{0xe9003370, ""},
	{0xe90033cc, ""},
	/* port 6 */
	{0xe900315c, ""},
	{0xe9003160, ""},
	{0xe9003168, ""},
	{0xe9003170, ""},
	{0xe90031cc, ""},
};

static const struct iobp_register lynxpoint_lp_iobp_registers[] = {
	/* SATA Electrical Control Register */
	{0xea002688, "SECRT88P0"},
	{0xea00268c, "SECRT8CP0"},
	{0xea002690, "SECRT90P0"},
	{0xea002698, "SECRT98P0"},
	{0xea00271c, "SECRR1CP0"},
	{0xea002750, "SECRR50P0"},
	{0xea002754, "SECRR54P0"},
	{0xea002758, "SECRR58P0"},

	{0xea002488, "SECRT88P1"},
	{0xea00248c, "SECRT8CP1"},
	{0xea002490, "SECRT90P1"},
	{0xea002498, "SECRT98P1"},
	{0xea00251c, "SECRR1CP1"},
	{0xea002550, "SECRR50P1"},
	{0xea002554, "SECRR54P1"},
	{0xea002558, "SECRR58P1"},

	{0xea002288, "SECRT88P2"},
	{0xea00228c, "SECRT8CP2"},
	{0xea002290, "SECRT90P2"},
	{0xea002298, "SECRT98P2"},
	{0xea00231c, "SECRR1CP2"},
	{0xea002350, "SECRR50P2"},
	{0xea002354, "SECRR54P2"},
	{0xea002358, "SECRR58P2"},

	{0xea002088, "SECRT88P3"},
	{0xea00208c, "SECRT8CP3"},
	{0xea002090, "SECRT90P3"},
	{0xea002098, "SECRT98P3"},
	{0xea00211c, "SECRR1CP3"},
	{0xea002150, "SECRR50P3"},
	{0xea002154, "SECRR54P3"},
	{0xea002158, "SECRR58P3"},

	{0xea008100, "SECRF00"},
	{0xea008104, "SECRF04"},

	/* USB 2.0 Electrical Control Register, 8 for -U SoC and 10 for Core-M */
	{0xe5004100, "U2ECRP01"},
	{0xe5004200, "U2ECRP02"},
	{0xe5004300, "U2ECRP03"},
	{0xe5004400, "U2ECRP04"},
	{0xe5004500, "U2ECRP05"},
	{0xe5004600, "U2ECRP06"},
	{0xe5004700, "U2ECRP07"},
	{0xe5004800, "U2ECRP08"},
	{0xe5004900, "U2ECRP09"},
	{0xe5004a00, "U2ECRP10"},

	/* IOBP related to USB 3.0 ports */
	/* port 1 */
	{0xe900215c, ""},
	{0xe9002160, ""},
	{0xe9002168, ""},
	{0xe9002170, ""},
	{0xe90021cc, ""},
	/* port 2 */
	{0xe900235c, ""},
	{0xe9002360, ""},
	{0xe9002368, ""},
	{0xe9002370, ""},
	{0xe90023cc, ""},
	/* port 3 */
	{0xe900255c, ""},
	{0xe9002560, ""},
	{0xe9002568, ""},
	{0xe9002570, ""},
	{0xe90025cc, ""},
	/* port 4 */
	{0xe900275c, ""},
	{0xe9002760, ""},
	{0xe9002768, ""},
	{0xe9002770, ""},
	{0xe90027cc, ""},
};

void print_iobp(struct pci_dev *sb, volatile uint8_t *rcba)
{
	const struct iobp_register *iobp_registers = NULL;
	size_t iobp_size = 0;

	switch (sb->device_id) {
	case PCI_DEVICE_ID_INTEL_C8_MOBILE:
	case PCI_DEVICE_ID_INTEL_C8_DESKTOP:
	case PCI_DEVICE_ID_INTEL_Z87:
	case PCI_DEVICE_ID_INTEL_Z85:
	case PCI_DEVICE_ID_INTEL_HM86:
	case PCI_DEVICE_ID_INTEL_H87:
	case PCI_DEVICE_ID_INTEL_HM87:
	case PCI_DEVICE_ID_INTEL_Q85:
	case PCI_DEVICE_ID_INTEL_Q87:
	case PCI_DEVICE_ID_INTEL_QM87:
	case PCI_DEVICE_ID_INTEL_B85:
	case PCI_DEVICE_ID_INTEL_C222:
	case PCI_DEVICE_ID_INTEL_C224:
	case PCI_DEVICE_ID_INTEL_C226:
	case PCI_DEVICE_ID_INTEL_H81:
	case PCI_DEVICE_ID_INTEL_C9_MOBILE:
	case PCI_DEVICE_ID_INTEL_C9_DESKTOP:
	case PCI_DEVICE_ID_INTEL_HM97:
	case PCI_DEVICE_ID_INTEL_Z97:
	case PCI_DEVICE_ID_INTEL_H97:
		iobp_registers = lynxpoint_iobp_registers;
		iobp_size = ARRAY_SIZE(lynxpoint_iobp_registers);
		break;
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_FULL:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_LYNXPOINT_LP_BASE:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP_PREM:
	case PCI_DEVICE_ID_INTEL_WILDCATPOINT_LP:
		iobp_registers = lynxpoint_lp_iobp_registers;
		iobp_size = ARRAY_SIZE(lynxpoint_lp_iobp_registers);
		break;
	default:
		break;
	}

	if (iobp_size == 0)
		return;

	printf("\n============= IOBP ==============\n\n");

	for (size_t i = 0; i < iobp_size; i++) {
		u32 address = iobp_registers[i].addr;
		const char *name = iobp_registers[i].name;
		u32 v = pch_iobp_read(rcba, address);
		printf("0x%08x: 0x%08x (%s)\n", address, v, name);
	}
}
