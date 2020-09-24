/* SPDX-License-Identifier: GPL-2.0-only */

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
	Local0 = GPIORXSTATE_MASK & (VAL0 >> GPIORXSTATE_SHIFT)

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
	Local0 = GPIOTXSTATE_MASK & VAL0

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
	VAL0 |= GPIOTXSTATE_MASK
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
	VAL0 &= ~GPIOTXSTATE_MASK
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
	Local0 = ~GPIOPADMODE_MASK & VAL0
	Arg1 = (Arg1 << GPIOPADMODE_SHIFT) & GPIOPADMODE_MASK
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
		VAL0 &= ~GPIOTXBUFDIS_MASK
	} ElseIf (Arg1 == 0){
		VAL0 |= GPIOTXBUFDIS_MASK
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
		VAL0 &= ~GPIORXBUFDIS_MASK
	} ElseIf (Arg1 == 0){
		VAL0 |= GPIORXBUFDIS_MASK
	}
}
