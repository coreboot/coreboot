/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef DPTF_CPU_DEVICE
#define DPTF_CPU_DEVICE		TCPU
#endif

#ifndef DPTF_CPU_ADDR
#define DPTF_CPU_ADDR		0x00040000
#endif

#ifndef DPTF_CPU_PASSIVE
#define DPTF_CPU_PASSIVE	80
#endif

#ifndef DPTF_CPU_CRITICAL
#define DPTF_CPU_CRITICAL	90
#endif

#ifndef DPTF_CPU_ACTIVE_AC0
#define DPTF_CPU_ACTIVE_AC0	90
#endif

#ifndef DPTF_CPU_ACTIVE_AC1
#define DPTF_CPU_ACTIVE_AC1	80
#endif

#ifndef DPTF_CPU_ACTIVE_AC2
#define DPTF_CPU_ACTIVE_AC2	70
#endif

#ifndef DPTF_CPU_ACTIVE_AC3
#define DPTF_CPU_ACTIVE_AC3	60
#endif

#ifndef DPTF_CPU_ACTIVE_AC4
#define DPTF_CPU_ACTIVE_AC4	50
#endif

/* DPTF ACPI Device ID */
#ifndef DPTF_DPTF_DEVICE
#define DPTF_DPTF_DEVICE	"INT3400"
#endif

/* Generic ACPI Device ID for TSR0/1/2/3 and charger */
#ifndef DPTF_GEN_DEVICE
#define DPTF_GEN_DEVICE		"INT3403"
#endif

/* Fan ACPI Device ID */
#ifndef DPTF_FAN_DEVICE
#define DPTF_FAN_DEVICE		"INT3404"
#endif
