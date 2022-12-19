/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_DPTF_H_
#define _SOC_DPTF_H_

/* Below are the unique ACPI Device IDs for thermal/dptf on Alder Lake SoC. */
/* DPTF ACPI Device ID */
#define DPTF_DPTF_DEVICE	"INTC1041"
/* Generic ACPI Device ID for TSR0/1/2/3 and charger */
#define DPTF_GEN_DEVICE		"INTC1046"
/* Fan ACPI Device ID */
#define DPTF_FAN_DEVICE		"INTC1048"
/* TPCH ACPI Device ID */
#define DPTF_TPCH_DEVICE	"INTC1049"
/* TPWR ACPI Device ID */
#define DPTF_TPWR_DEVICE	"INTC1060"
/* BAT1 ACPI Device ID */
#define DPTF_BAT1_DEVICE	"INTC1061"

#endif /* _SOC_DPTF_H_ */
