/* SPDX-License-Identifier: GPL-2.0-only */
#include <intelblocks/gpio_defs.h>

/*
 * Get GPIO Value
 * Arg0 - GPIO Number
 */
Method (GRXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Local0 = (PAD_CFG0_RX_STATE & VAL0) >> PAD_CFG0_RX_STATE_BIT

	Return (Local0)
}

/*
 * Get GPIO Tx Value
 * Arg0 - GPIO Number
 */
Method (GTXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Local0 = PAD_CFG0_TX_STATE & VAL0

	Return (Local0)
}

/*
 * Set GPIO Tx Value
 * Arg0 - GPIO Number
 */
Method (STXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	VAL0 |= PAD_CFG0_TX_STATE
}

/*
 * Clear GPIO Tx Value
 * Arg0 - GPIO Number
 */
Method (CTXS, 1, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	VAL0 &= ~PAD_CFG0_TX_STATE
}

/*
 * Set Pad mode
 * Arg0 - GPIO Number
 * Arg1 - Pad mode
 *     0 = GPIO control pad
 *     1 = Native Function 1
 *     2 = Native Function 2
 *     3 = Native Function 3
 */
Method (GPMO, 2, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}
	Local0 = ~PAD_CFG0_MODE_MASK & VAL0
	Arg1 = (Arg1 << PAD_CFG0_MODE_SHIFT) & PAD_CFG0_MODE_MASK
	VAL0 = Local0 | Arg1
}

/*
 * Enable/Disable Tx buffer
 * Arg0 - GPIO Number
 * Arg1 - TxBuffer state
 *     0 = Disable Tx Buffer
 *     1 = Enable Tx Buffer
 */
Method (GTXE, 2, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}

	If (Arg1 == 1) {
		VAL0 &= ~PAD_CFG0_TX_DISABLE
	} ElseIf (Arg1 == 0){
		VAL0 |= PAD_CFG0_TX_DISABLE
	}
}

/*
 * Enable/Disable Rx buffer
 * Arg0 - GPIO Number
 * Arg1 - RxBuffer state
 *     0 = Disable Rx Buffer
 *     1 = Enable Rx Buffer
 */
Method (GRXE, 2, Serialized)
{
	OperationRegion (PREG, SystemMemory, GADD (Arg0), 4)
	Field (PREG, AnyAcc, NoLock, Preserve)
	{
		VAL0, 32
	}

	If (Arg1 == 1) {
		VAL0 &= ~PAD_CFG0_RX_DISABLE
	} ElseIf (Arg1 == 0){
		VAL0 |= PAD_CFG0_RX_DISABLE
	}
}
