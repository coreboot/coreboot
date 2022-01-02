/* SPDX-License-Identifier: GPL-2.0-only */

/* ======== General PnP configuration functions ======= */

/*
 * Controlled by the following preprocessor defines:
 * PNP_ENTER_MAGIC_1ST	If defined, specifies the first magic byte
 *			used to enter config mode.
 * PNP_ENTER_MAGIC_2ND	If defined, specifies the second magic byte
 *			used to enter config mode.
 * PNP_ENTER_MAGIC_3RD	If defined, specifies the third magic byte
 *			used to enter config mode.
 * PNP_ENTER_MAGIC_4TH	If defined, specifies the fourth magic byte
 *			used to enter config mode.
 * PNP_EXIT_MAGIC_1ST	If defined, specifies the first magic byte
 *			used to exit config mode.
 * PNP_EXIT_SPECIAL_REG	If defined, specifies a special register plus
 * PNP_EXIT_SPECIAL_VAL	a value to be written there to exit config mode.
 */


/*
 * Mutex for accesses to the configuration ports (prolog and
 * epilog commands are used, so synchronization is useful)
 */
Mutex(CONF_MODE_MUTEX, 1)

/*
 * Enter configuration mode (and acquire mutex)
 * Method must be run before accessing the configuration region.
 * Parameter is the LDN which should be accessed. Values >= 0xFF mean
 * no LDN switch should be done.
 */
Method (ENTER_CONFIG_MODE, 1)
{
	Acquire (CONF_MODE_MUTEX, 0xFFFF)
#ifdef PNP_ENTER_MAGIC_1ST
	Store (PNP_ENTER_MAGIC_1ST, PNP_ADDR_REG)
#ifdef PNP_ENTER_MAGIC_2ND
	Store (PNP_ENTER_MAGIC_2ND, PNP_ADDR_REG)
#ifdef PNP_ENTER_MAGIC_3RD
	Store (PNP_ENTER_MAGIC_3RD, PNP_ADDR_REG)
#ifdef PNP_ENTER_MAGIC_4TH
	Store (PNP_ENTER_MAGIC_4TH, PNP_ADDR_REG)
#endif
#endif
#endif
#endif
	If (Arg0 < PNP_NO_LDN_CHANGE) {
		Store(Arg0, PNP_LOGICAL_DEVICE)
	}
}

/*
 * Exit configuration mode (i.e. release mutex)
 * Method must be run after accessing the configuration region.
 */
Method (EXIT_CONFIG_MODE)
{
#ifdef PNP_EXIT_MAGIC_1ST
	Store (PNP_EXIT_MAGIC_1ST, PNP_ADDR_REG)
#endif
#if defined(PNP_EXIT_SPECIAL_REG) && defined(PNP_EXIT_SPECIAL_VAL)
	Store (PNP_EXIT_SPECIAL_VAL, PNP_EXIT_SPECIAL_REG)
#endif
	Release (CONF_MODE_MUTEX)
}

/*
 * Just change the LDN. Make sure that you are in config mode (or
 * have otherwise acquired CONF_MODE_MUTEX), when calling.
 */
Method (SWITCH_LDN, 1)
{
	Store(Arg0, PNP_LOGICAL_DEVICE)
}
