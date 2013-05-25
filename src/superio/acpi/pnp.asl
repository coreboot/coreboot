/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 secunet Security Networks AG
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SUPERIO_ACPI_PNP_DEFS_ASL
#define SUPERIO_ACPI_PNP_DEFS_ASL

#define _SUPERIO_ID(name, ldn) name ## ldn
#define SUPERIO_ID(name, ldn) _SUPERIO_ID(name, ldn)

#define STRINGIFY(x) #x
#define EXPAND_AND_STRINGIFY(x) STRINGIFY(x)
#define SUPERIO_UID(name, ldn) \
	EXPAND_AND_STRINGIFY(SUPERIO_CHIP_NAME-SUPERIO_ID(name, ldn))
#define SUPERIO_NAME(name) EXPAND_AND_STRINGIFY(SUPERIO_CHIP_NAME name)

/* Some longer identifiers for readability */
#define PNP_ADDR_REG		ADDR
#define PNP_DATA_REG		DATA
#define PNP_LOGICAL_DEVICE	LDN
#define PNP_DEVICE_ACTIVE	ACTR
#define PNP_IO0_HIGH_BYTE	IO0H
#define PNP_IO0_LOW_BYTE	IO0L
#define PNP_IO1_HIGH_BYTE	IO1H
#define PNP_IO1_LOW_BYTE	IO1L
#define PNP_IRQ0		IRQ0
#define PNP_IRQ1		IRQ1
#define PNP_DMA0		DMA0

#define CONFIG_MODE_MUTEX	CMMX
#define ENTER_CONFIG_MODE	ENCM
#define EXIT_CONFIG_MODE	EXCM
#define SWITCH_LDN		SWLD
#define PNP_NO_LDN_CHANGE	0xff

/* Values for ACPI's _STA method */
#define DEVICE_NOT_PRESENT	0x00
#define DEVICE_PRESENT_ACTIVE	0x0f
#define DEVICE_PRESENT_INACTIVE	0x0d


/* ================== Generic Method bodies ================= */

#define PNP_GENERIC_STA(LDN) \
	ENTER_CONFIG_MODE (LDN)\
	  If (PNP_DEVICE_ACTIVE) {\
		Store (DEVICE_PRESENT_ACTIVE, Local0)\
	  }\
	  Else\
	  {\
		Store (DEVICE_PRESENT_INACTIVE, Local0)\
	  }\
	EXIT_CONFIG_MODE ()\
	Return (Local0)\

#define PNP_GENERIC_DIS(LDN) \
	ENTER_CONFIG_MODE (LDN)\
	  Store (Zero, PNP_DEVICE_ACTIVE)\
	EXIT_CONFIG_MODE ()\


/*
 * Current power state (returns the chip's state)
 */
#define PNP_DEFAULT_PSC \
	Store(^^_PSC (), Local0)\
	Return (Local0)

/*
 * Current power state (returns the chip's state, if it's in
 * power saving mode, 1 if this LDN is in power saving mode,
 * 0 else)
 *
 * PM_REG	Identifier of a register which powers down the device
 * PM_LDN	The logical device number to access the PM_REG
 *		bit
 */
#define PNP_GENERIC_PSC(PM_REG, PM_LDN) \
	Store(^^_PSC (), Local0)\
	If (Local0) { Return (Local0) }\
	ENTER_CONFIG_MODE (PM_LDN)\
	  Store (PM_REG, Local0)\
	EXIT_CONFIG_MODE ()\
	If (Local0) { Return (1) }\
	Else { Return (0) }\

/* Disable power saving mode */
#define PNP_GENERIC_PS0(PM_REG, PM_LDN) \
	ENTER_CONFIG_MODE (PM_LDN)\
	  Store (Zero, PM_REG)\
	EXIT_CONFIG_MODE ()

/* Enable power saving mode */
#define PNP_GENERIC_PS1(PM_REG, PM_LDN) \
	ENTER_CONFIG_MODE (PM_LDN)\
	  Store (One, PM_REG)\
	EXIT_CONFIG_MODE ()


/* ==================== Resource helpers ==================== */

#define PNP_READ_IO(IO_FROM, RESOURCE_TEMPLATE, IO_TAG) \
	CreateWordField (RESOURCE_TEMPLATE, IO_TAG._MIN, IO_TAG##I)\
	CreateWordField (RESOURCE_TEMPLATE, IO_TAG._MAX, IO_TAG##A)\
	Or (ShiftLeft (IO_FROM##_HIGH_BYTE, 8), IO_FROM##_LOW_BYTE, Local0)\
	Store (Local0, IO_TAG##I)\
	Store (Local0, IO_TAG##A)

#define PNP_READ_IRQ(IRQ_FROM, RESOURCE_TEMPLATE, IRQ_TAG) \
	CreateWordField (RESOURCE_TEMPLATE, IRQ_TAG._INT, IRQ_TAG##W)\
	ShiftLeft (One, IRQ_FROM, IRQ_TAG##W)

#define PNP_READ_DMA(DMA_FROM, RESOURCE_TEMPLATE, DMA_TAG) \
	CreateWordField (RESOURCE_TEMPLATE, DMA_TAG._DMA, DMA_TAG##W)\
	ShiftLeft (One, DMA_FROM, DMA_TAG##W)

#define PNP_WRITE_IO(IO_TO, RESOURCE, IO_TAG) \
	CreateWordField (RESOURCE, IO_TAG._MIN, IO_TAG##I)\
	Store (And(IO_TAG##I, 0xff), IO_TO##_LOW_BYTE)\
	Store (ShiftRight(IO_TAG##I, 8), IO_TO##_HIGH_BYTE)

#define PNP_WRITE_IRQ(IRQ_TO, RESOURCE, IRQ_TAG) \
	CreateWordField (RESOURCE, IRQ_TAG._INT, IRQ_TAG##W)\
	Subtract (FindSetLeftBit (IRQ_TAG##W), 1, IRQ_TO)

#define PNP_WRITE_DMA(DMA_TO, RESOURCE, DMA_TAG) \
	CreateWordField (RESOURCE, DMA_TAG._DMA, DMA_TAG##W)\
	Subtract (FindSetLeftBit (DMA_TAG##W), 1, DMA_TO)

#endif
