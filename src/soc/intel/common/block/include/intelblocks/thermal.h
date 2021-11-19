/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_COMMON_BLOCK_THERMAL_H_
#define _SOC_INTEL_COMMON_BLOCK_THERMAL_H_

/* Catastrophic Trip Point Enable */
#define PMC_PWRM_THERMAL_CTEN			0x150c
/* Policy Lock-Down Bit */
#define  PMC_PWRM_THERMAL_CTEN_CTENLOCK		(1 << 31)
/* Catastrophic Power-Down Enable */
#define  PMC_PWRM_THERMAL_CTEN_CPDEN		(1 << 0)
/* EC Thermal Sensor Reporting Enable */
#define PMC_PWRM_THERMAL_ECRPTEN		0x1510
/* Lock-Down Bit */
#define  PMC_PWRM_THERMAL_ECRPTEN_ECRPTENLOCK	(1 << 31)
/* Enable PMC to EC Temp Reporting */
#define  PMC_PWRM_THERMAL_ECRPTEN_EN_RPT	(1 << 0)
/* Throttle Levels */
#define PMC_PWRM_THERMAL_TL			0x1520
/* TL LOCK */
#define  PMC_PWRM_THERMAL_TL_TLLOCK		(1 << 31)
/* TT Enable */
#define  PMC_PWRM_THERMAL_TL_TTEN		(1 << 29)
/* Throttle Levels Enable */
#define PMC_PWRM_THERMAL_TLEN			0x1528
/* TLENLOCK */
#define  PMC_PWRM_THERMAL_TLEN_TLENLOCK		(1 << 31)
/* PCH Hot Level Control */
#define PMC_PWRM_THERMAL_PHLC			0x1540
/* PHL Lock */
#define  PMC_PWRM_THERMAL_PHLC_PHLCLOCK		(1 << 31)

/* Enable thermal sensor power management */
void pch_thermal_configuration(void);

#endif
