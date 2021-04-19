/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __ACPI_ACPI_OSC_H__
#define __ACPI_ACPI_OSC_H__

/* _OSC (Operating System Capabilities) */
#define  OSC_CDW1_QUERY			(1 << 0)
#define  OSC_CDW1_UNKNOWN_FAILURE	(1 << 1)
#define  OSC_CDW1_UNRECOGNIZED_UUID	(1 << 2)
#define  OSC_CDW1_UNRECOGNIZED_REVISION	(1 << 3)
#define  OSC_CDW1_CAPABILITIES_MASKED	(1 << 4)

/* Platform-Wide \_SB._OSC */
#define OSC_SB_UUID			"0811b06e-4a27-44f9-8d60-3cbbc22e7b48"

#endif /* __ACPI_ACPI_OSC_H__ */
