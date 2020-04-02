/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef EC_GOOGLE_WILCO_SMM_H
#define EC_GOOGLE_WILCO_SMM_H

/* Handlers for SMI events */

/**
 * wilco_ec_smi_sleep
 *
 * Perform EC actions before suspend or power-off.
 *
 * @slp_type:	Requested sleep type
 */
void wilco_ec_smi_sleep(int slp_type);

/**
 * wilco_ec_smi_apmc
 *
 * Perform EC actions on ACPI OS commands.
 * This is used when the OS enables or disables ACPI.
 *
 * @apmc:	OS command
 */
void wilco_ec_smi_apmc(int apmc);

/**
 * wilco_ec_smi_espi
 *
 * Perform EC actions on EC eSPI SMI interrupt.
 */
void wilco_ec_smi_espi(void);

#endif /* EC_GOOGLE_WILCO_SMM_H */
