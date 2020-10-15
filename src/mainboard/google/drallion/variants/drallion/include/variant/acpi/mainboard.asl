/* SPDX-License-Identifier: GPL-2.0-only */

#define CAM_EN GPP_B11 /* Active low */
#define TS_PD GPP_E7
#define HDMI_PD GPP_E16

/* Method called from PEPD prior to enter s0ix state */
Method (MS0X, 1)
{
	If (Arg0) {
		/* Turn off camera power */
		\_SB.PCI0.STXS (CAM_EN)
		/* Turn off HDMI power */
		\_SB.PCI0.CTXS (HDMI_PD)
	} Else {
		/* Turn on camera power */
		\_SB.PCI0.CTXS (CAM_EN)
		/* Turn on HDMI power */
		\_SB.PCI0.STXS (HDMI_PD)
	}
}

/* Method called from _PTS prior to enter sleep state */
Method (MPTS, 1)
{
	\_SB.PCI0.LPCB.EC0.PTS (Arg0)

	/* Clear touch screen pd pin to avoid leakage */
	\_SB.PCI0.CTXS (TS_PD)
	/* Clear HDMI power to avoid leakage */
	\_SB.PCI0.CTXS (HDMI_PD)
}

/* Method called from _WAK prior to wakeup */
Method (MWAK, 1)
{
	\_SB.PCI0.LPCB.EC0.WAK (Arg0)
}
