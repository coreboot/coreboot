/* SPDX-License-Identifier: GPL-2.0-only */

Method (MPTS, 1, NotSerialized)
{
#if CONFIG(BOARD_STARLABS_STARBOOK_TGL)
	If (Arg0 == 0x03) {
		\_SB.PCI0.CTXS (GPP_D16)
	}
#endif

	RPTS (Arg0)
}

Method (MWAK, 1, NotSerialized)
{
	RWAK (Arg0)
}
