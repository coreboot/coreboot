/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef CPU_PPC64_SCOM_H
#define CPU_PPC64_SCOM_H

#include <arch/byteorder.h>	// PPC_BIT(), PPC_BITMASK()

// 32b SCOM address:
//
//      8         7         6         5         4         3         2         1
//
//  |       | |       | |    1 1| |1 1 1 1| |1 1 1 1| |2 2 2 2| |2 2 2 2| |2 2 3 3|
//  |0 1 2 3| |4 5 6 7| |8 9 0 1| |2 3 4 5| |6 7 8 9| |0 1 2 3| |4 5 6 7| |8 9 0 1|
//    {A}{     B      }           {   C   }     {    D    }{   E   }{      F      }
//
// A - Is multiCast if bit 1 = 0x1
// B - Contains Chiplet ID (6 bits) [2:7]
// C - Contains Port Number (4 bits) [12:15]
// D - Ring (4 bits) [18:21]
// E - Sat ID (4 bits) [22:25]
// F - Sat Offset (6 bits) [26:31]
//
// For 64b SCOM address all of the fields are shifted 32b to the right:
// A - Is multiCast if bit 33 = 0x1
// B - Contains Chiplet ID (6 bits) [34:39]
// C - Contains Port Number (4 bits) [44:47]
// D - Ring (4 bits) [50:53]
// E - Sat ID (4 bits) [54:57]
// F - Sat Offset (6 bits) [58:63]
// Higher bits specify indirect address

#define XSCOM_ADDR_IND_FLAG		PPC_BIT(0)
#define XSCOM_ADDR_IND_ADDR		PPC_BITMASK(11, 31)
#define XSCOM_ADDR_IND_DATA		PPC_BITMASK(48, 63)

#ifndef __ASSEMBLER__
#include <types.h>
#include <arch/io.h>
#include <cpu/power/spr.h>

// TODO: these are probably specific to POWER9
typedef enum {
	PIB_CHIPLET_ID  = 0x00,     ///< PIB chiplet
	PERV_CHIPLET_ID = 0x01,     ///< TP chiplet
	N0_CHIPLET_ID   = 0x02,     ///< Nest0 (North) chiplet
	N1_CHIPLET_ID   = 0x03,     ///< Nest1 (East) chiplet
	N2_CHIPLET_ID   = 0x04,     ///< Nest2 (South) chiplet
	N3_CHIPLET_ID   = 0x05,     ///< Nest3 (West) chiplet
	XB_CHIPLET_ID   = 0x06,     ///< XBus chiplet
	MC01_CHIPLET_ID = 0x07,     ///< MC01 (West) chiplet
	MC23_CHIPLET_ID = 0x08,     ///< MC23 (East) chiplet
	OB0_CHIPLET_ID  = 0x09,     ///< OBus0 chiplet
	OB1_CHIPLET_ID  = 0x0A,     ///< OBus1 chiplet (Cumulus only)
	OB2_CHIPLET_ID  = 0x0B,     ///< OBus2 chiplet (Cumulus only)
	OB3_CHIPLET_ID  = 0x0C,     ///< OBus3 chiplet
	PCI0_CHIPLET_ID = 0x0D,     ///< PCIe0 chiplet
	PCI1_CHIPLET_ID = 0x0E,     ///< PCIe1 chiplet
	PCI2_CHIPLET_ID = 0x0F,     ///< PCIe2 chiplet
	EP00_CHIPLET_ID = 0x10,     ///< Quad0 chiplet (EX0/1)
	EP01_CHIPLET_ID = 0x11,     ///< Quad1 chiplet (EX2/3)
	EP02_CHIPLET_ID = 0x12,     ///< Quad2 chiplet (EX4/5)
	EP03_CHIPLET_ID = 0x13,     ///< Quad3 chiplet (EX6/7)
	EP04_CHIPLET_ID = 0x14,     ///< Quad4 chiplet (EX8/9)
	EP05_CHIPLET_ID = 0x15,     ///< Quad5 chiplet (EX10/11)
	EC00_CHIPLET_ID = 0x20,     ///< Core0 chiplet (Quad0, EX0, C0)
	EC01_CHIPLET_ID = 0x21,     ///< Core1 chiplet (Quad0, EX0, C1)
	EC02_CHIPLET_ID = 0x22,     ///< Core2 chiplet (Quad0, EX1, C0)
	EC03_CHIPLET_ID = 0x23,     ///< Core3 chiplet (Quad0, EX1, C1)
	EC04_CHIPLET_ID = 0x24,     ///< Core4 chiplet (Quad1, EX2, C0)
	EC05_CHIPLET_ID = 0x25,     ///< Core5 chiplet (Quad1, EX2, C1)
	EC06_CHIPLET_ID = 0x26,     ///< Core6 chiplet (Quad1, EX3, C0)
	EC07_CHIPLET_ID = 0x27,     ///< Core7 chiplet (Quad1, EX3, C1)
	EC08_CHIPLET_ID = 0x28,     ///< Core8 chiplet (Quad2, EX4, C0)
	EC09_CHIPLET_ID = 0x29,     ///< Core9 chiplet (Quad2, EX4, C1)
	EC10_CHIPLET_ID = 0x2A,     ///< Core10 chiplet (Quad2, EX5, C0)
	EC11_CHIPLET_ID = 0x2B,     ///< Core11 chiplet (Quad2, EX5, C1)
	EC12_CHIPLET_ID = 0x2C,     ///< Core12 chiplet (Quad3, EX6, C0)
	EC13_CHIPLET_ID = 0x2D,     ///< Core13 chiplet (Quad3, EX6, C1)
	EC14_CHIPLET_ID = 0x2E,     ///< Core14 chiplet (Quad3, EX7, C0)
	EC15_CHIPLET_ID = 0x2F,     ///< Core15 chiplet (Quad3, EX7, C1)
	EC16_CHIPLET_ID = 0x30,     ///< Core16 chiplet (Quad4, EX8, C0)
	EC17_CHIPLET_ID = 0x31,     ///< Core17 chiplet (Quad4, EX8, C1)
	EC18_CHIPLET_ID = 0x32,     ///< Core18 chiplet (Quad4, EX9, C0)
	EC19_CHIPLET_ID = 0x33,     ///< Core19 chiplet (Quad4, EX9, C1)
	EC20_CHIPLET_ID = 0x34,     ///< Core20 chiplet (Quad5, EX10, C0)
	EC21_CHIPLET_ID = 0x35,     ///< Core21 chiplet (Quad5, EX10, C1)
	EC22_CHIPLET_ID = 0x36,     ///< Core22 chiplet (Quad5, EX11, C0)
	EC23_CHIPLET_ID = 0x37      ///< Core23 chiplet (Quad5, EX11, C1)
} chiplet_id_t;

void reset_scom_engine(void);

uint64_t read_scom_direct(uint64_t reg_address);
void write_scom_direct(uint64_t reg_address, uint64_t data);

uint64_t read_scom_indirect(uint64_t reg_address);
void write_scom_indirect(uint64_t reg_address, uint64_t data);

static inline void write_scom(uint64_t addr, uint64_t data)
{
	if (addr & XSCOM_ADDR_IND_FLAG)
		write_scom_indirect(addr, data);
	else
		write_scom_direct(addr, data);
}

static inline uint64_t read_scom(uint64_t addr)
{
	if (addr & XSCOM_ADDR_IND_FLAG)
		return read_scom_indirect(addr);
	else
		return read_scom_direct(addr);
}

static inline void scom_and_or(uint64_t addr, uint64_t and, uint64_t or)
{
	uint64_t data = read_scom(addr);
	write_scom(addr, (data & and) | or);
}

static inline void scom_and(uint64_t addr, uint64_t and)
{
	scom_and_or(addr, and, 0);
}

static inline void scom_or(uint64_t addr, uint64_t or)
{
	scom_and_or(addr, ~0, or);
}

static inline void write_scom_for_chiplet(chiplet_id_t chiplet, uint64_t addr, uint64_t data)
{
	addr &= ~PPC_BITMASK(34, 39);
	addr |= ((chiplet & 0x3F) << 24);
	write_scom(addr, data);
}

static inline uint64_t read_scom_for_chiplet(chiplet_id_t chiplet, uint64_t addr)
{
	addr &= ~PPC_BITMASK(34, 39);
	addr |= ((chiplet & 0x3F) << 24);
	return read_scom(addr);
}

static inline void scom_and_or_for_chiplet(chiplet_id_t chiplet, uint64_t addr,
					   uint64_t and, uint64_t or)
{
	uint64_t data = read_scom_for_chiplet(chiplet, addr);
	write_scom_for_chiplet(chiplet, addr, (data & and) | or);
}

static inline void scom_and_for_chiplet(chiplet_id_t chiplet, uint64_t addr, uint64_t and)
{
	scom_and_or_for_chiplet(chiplet, addr, and, 0);
}

static inline void scom_or_for_chiplet(chiplet_id_t chiplet, uint64_t addr, uint64_t or)
{
	scom_and_or_for_chiplet(chiplet, addr, ~0, or);
}

#endif /* __ASSEMBLER__ */
#endif /* CPU_PPC64_SCOM_H */
