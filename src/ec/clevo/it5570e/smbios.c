/* SPDX-License-Identifier: GPL-2.0-or-later */

#include <string.h>
#include <smbios.h>
#include "commands.h"

void smbios_ec_revision(uint8_t *ec_major_revision, uint8_t *ec_minor_revision)
{
	char *version, *major, *minor;

	version = ec_read_fw_version(); /* 1.XX.YY */
	major = &version[2];
	minor = &version[5];

	*ec_major_revision = skip_atoi(&major);
	*ec_minor_revision = skip_atoi(&minor);
}
