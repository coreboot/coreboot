/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef __SUPERIO_COMMON_CHIP_H__
#define __SUPERIO_COMMON_CHIP_H__

struct superio_common_config {
	/* FIXME: Add enter conf/exit conf codes here for SSDT generation */
	/* Rarely, boards may have more than one SuperIO.  Give each SuperIO a
	 * unique name for ACPI.  The default is 'SIO0'. */
	char acpi_name[5];
};

#endif /* __SUPERIO_COMMON_CHIP_H__ */
