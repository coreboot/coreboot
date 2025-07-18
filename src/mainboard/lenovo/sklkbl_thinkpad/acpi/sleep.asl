/* SPDX-License-Identifier: GPL-2.0-only */

/* Prepare discrete TBT controller before sleep */
Method(MPTS, 1, Serialized) {
	If (CondRefOf(\TBTS)) {
		\TBTS()
	}
}
