/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

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
	And (PAD_CFG0_RX_STATE, ShiftRight (VAL0, PAD_CFG0_RX_STATE_BIT), Local0)

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
	And (PAD_CFG0_TX_STATE, VAL0, Local0)

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
	Or (PAD_CFG0_TX_STATE, VAL0, VAL0)
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
	And (Not (PAD_CFG0_TX_STATE), VAL0, VAL0)
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
	Store (VAL0, Local0)
	And (Not (PAD_CFG0_MODE_MASK), Local0, Local0)
	And (ShiftLeft (Arg1, PAD_CFG0_MODE_SHIFT, Arg1), PAD_CFG0_MODE_MASK, Arg1)
	Or (Local0, Arg1, VAL0)
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

	If (LEqual (Arg1, 1)) {
		And (Not (PAD_CFG0_TX_DISABLE), VAL0, VAL0)
	} ElseIf (LEqual (Arg1, 0)){
		Or (PAD_CFG0_TX_DISABLE, VAL0, VAL0)
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

	If (LEqual (Arg1, 1)) {
		And (Not (PAD_CFG0_RX_DISABLE), VAL0, VAL0)
	} ElseIf (LEqual (Arg1, 0)){
		Or (PAD_CFG0_RX_DISABLE, VAL0, VAL0)
	}
}
