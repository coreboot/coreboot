/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DPTF_H_
#define _SOC_DPTF_H_

/* Below are the unique ACPI Device IDs for thermal/dptf on Meteor Lake SoC. */
/* DPTF ACPI Device ID */
#define DPTF_DPTF_DEVICE	"INTC1042"
/* Generic ACPI Device ID for TSR0/1/2/3 and charger */
#define DPTF_GEN_DEVICE		"INTC1062"
/* Fan ACPI Device ID */
#define DPTF_FAN_DEVICE		"INTC1063"
/* TPCH ACPI Device ID */
#define DPTF_TPCH_DEVICE	"INTC1064"
/* TPWR ACPI Device ID */
#define DPTF_TPWR_DEVICE	"INTC1065"
/* BAT1 ACPI Device ID */
#define DPTF_BAT1_DEVICE	"INTC1066"

#endif /* _SOC_DPTF_H_ */
