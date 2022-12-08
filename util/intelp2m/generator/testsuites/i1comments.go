package testsuites

const ReferenceI1Comments = `
	/* ------- GPIO Community 0 ------- */

	/* ------- GPIO Group GPP_A ------- */
	PAD_CFG_NF(GPP_A0, NONE, PLTRST, NF1),	/* RCIN# */
	PAD_CFG_NF(GPP_A1, UP_20K, PLTRST, NF1),	/* LAD0 */
	PAD_CFG_NF(GPP_A5, NONE, PLTRST, NF1),	/* LFRAME# */
	PAD_CFG_NF(GPP_A13, NONE, DEEP, NF1),	/* SUSWARN#/SUSPWRDNACK */
	PAD_CFG_GPI_TRIG_OWN(GPP_A23, NONE, PLTRST, OFF, ACPI),	/* GPIO */

	/* ------- GPIO Group GPP_B ------- */
	/* GPP_C1 - RESERVED */
	PAD_CFG_GPI_TRIG_OWN(GPP_B0, NONE, PLTRST, OFF, ACPI),	/* GPIO */
	PAD_CFG_NF(GPP_B23, DN_20K, PLTRST, NF2),	/* PCHHOT# */

	/* ------- GPIO Community 1 ------- */

	/* ------- GPIO Group GPP_C ------- */
	PAD_CFG_NF(GPP_C0, NONE, DEEP, NF1),	/* SMBCLK */
	PAD_CFG_GPI_TRIG_OWN(GPP_C5, NONE, PLTRST, OFF, ACPI),	/* GPIO */
	/* GPP_C6 - RESERVED */
	/* GPP_C7 - RESERVED */
	PAD_CFG_NF(GPP_C22, NONE, PLTRST, NF1),	/* UART2_RTS# */

	/* ------- GPIO Group GPP_D ------- */

	/* ------- GPIO Group GPP_E ------- */
	PAD_CFG_NF(GPP_E0, UP_20K, PLTRST, NF1),	/* SATAXPCIE0 */

	/* ------- GPIO Group GPP_G ------- */
	PAD_CFG_NF(GPP_G19, NONE, PLTRST, NF1),	/* SMI# */

	/* ------- GPIO Community 2 ------- */

	/* -------- GPIO Group GPD -------- */
	PAD_CFG_NF(GPD9, NONE, PWROK, NF1),	/* SLP_WLAN# */

	/* ------- GPIO Community 3 ------- */

	/* ------- GPIO Group GPP_I ------- */
	PAD_CFG_NF(GPP_I0, NONE, PLTRST, NF1),	/* DDPB_HPD0 */
	PAD_CFG_NF(GPP_I1, NONE, PLTRST, NF1),	/* DDPC_HPD1 */
	PAD_CFG_NF(GPP_I2, NONE, PLTRST, NF1),	/* DDPD_HPD2 */
`
