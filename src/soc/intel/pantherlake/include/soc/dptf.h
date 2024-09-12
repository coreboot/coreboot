/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_PANTHERLAKE_DPTF_H_
#define _SOC_PANTHERLAKE_DPTF_H_

/*
 * Below is a list of unique ACPI Device IDs for thermal and DPTF
 * (Dynamic Platform and Thermal Framework)
 */
/* DPTF ACPI Device ID */
#define DPTF_DPTF_DEVICE	"INTC10D4"
/* Generic ACPI Device ID for TSR0/1/2/3 and charger */
#define DPTF_GEN_DEVICE		"INTC10D5"
/* Fan ACPI Device ID */
#define DPTF_FAN_DEVICE		"INTC10D6"
/* TPCH ACPI Device ID */
#define DPTF_TPCH_DEVICE	"INTC10D7"
/* TPWR ACPI Device ID */
#define DPTF_TPWR_DEVICE	"INTC10D8"
/* BAT1 ACPI Device ID */
#define DPTF_BAT1_DEVICE	"INTC10D9"

#endif
