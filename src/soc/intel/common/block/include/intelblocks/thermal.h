/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _SOC_INTEL_COMMON_BLOCK_THERMAL_H_
#define _SOC_INTEL_COMMON_BLOCK_THERMAL_H_

#define MAX_TRIP_TEMP 205
/* This is the safest default Trip Temp value */
#define DEFAULT_TRIP_TEMP 50

#if CONFIG(SOC_INTEL_COMMON_BLOCK_THERMAL_PCI_DEV)
  /* Trip Point Temp = (LTT / 2 - 50 degree C) */
  #define GET_LTT_VALUE(x) (((x) + 50) * (2))
#elif CONFIG(SOC_INTEL_COMMON_BLOCK_THERMAL_BEHIND_PMC)
  /*
   * Trip Point = T2L | T1L | T0L where T2L > T1L > T0L
   * T2L = Bit 28:20
   * T1L = Bit 18:10
   * T0L = Bit 8:0
   */
  #define GET_LTT_VALUE(x) (((x) + 10) << 20 | ((x) + 5) << 10 | (x))
#else
  #error <Undefined: GET_LTT_VALUE macro>
#endif

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

/* Get PCH Thermal Trip from common chip config */
uint8_t get_thermal_trip_temp(void);
/* PCH Low Temp Threshold (LTT) */
uint32_t pch_get_ltt_value(void);
/* Enable thermal sensor power management */
void pch_thermal_configuration(void);

#endif
