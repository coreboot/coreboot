/* SPDX-License-Identifier: GPL-2.0-or-later */

#ifndef SOC_INTEL_COMMON_CSE_TELEMETRY_H
#define SOC_INTEL_COMMON_CSE_TELEMETRY_H

#if CONFIG(SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY_V1)
#include "cse_telemetry_v1.h"
#elif CONFIG(SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY_V2)
#include "cse_telemetry_v2.h"
#elif CONFIG(SOC_INTEL_CSE_PRE_CPU_RESET_TELEMETRY_V3)
#include "cse_telemetry_v3.h"
#endif

#endif // SOC_INTEL_COMMON_CSE_TELEMETRY_H
