/* SPDX-License-Identifier: GPL-2.0-only */

External(\TBTS, MethodObj)

Method(MPTS, 1, Serialized) {
	If (CondRefOf(\TBTS)) {
		\TBTS()
	}
}
