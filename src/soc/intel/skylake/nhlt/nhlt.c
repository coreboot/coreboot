/*
 * This file is part of the coreboot project.
 *
 * Copyright 2015 Google, Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <cbmem.h>
#include <soc/acpi.h>
#include <soc/nhlt.h>

#define NHLT_VID 0x8086
#define NHLT_DID_DMIC 0xae20
#define NHLT_DID_BT 0xae30
#define NHLT_DID_SSP 0xae34

struct nhlt_endpoint *nhlt_soc_add_endpoint(struct nhlt *nhlt, int soc_hwintf,
						int soc_devtype, int dir)
{
	int nhlt_link_type;
	int nhlt_dev_type;
	uint16_t did;
	struct nhlt_endpoint *endp;

	/* Check link type and device type. */
	switch (soc_hwintf) {
	case AUDIO_LINK_SSP0:
	case AUDIO_LINK_SSP1:
		/* Only I2S devices on SSP0 and SSP1. */
		if (soc_devtype != AUDIO_DEV_I2S)
			return NULL;
		nhlt_link_type = NHLT_LINK_SSP;
		break;
	case AUDIO_LINK_SSP2:
		/* Only Bluetooth devices on SSP2. */
		if (soc_devtype != AUDIO_DEV_BT)
			return NULL;
		nhlt_link_type = NHLT_LINK_SSP;
		break;
	case AUDIO_LINK_DMIC:
		/* Only DMIC devices on DMIC links. */
		if (soc_devtype != AUDIO_DEV_DMIC)
			return NULL;
		nhlt_link_type = NHLT_LINK_PDM;
		break;
	default:
		return NULL;
	}

	switch (soc_devtype) {
	case AUDIO_DEV_I2S:
		nhlt_dev_type = NHLT_SSP_DEV_I2S;
		did = NHLT_DID_SSP;
		break;
	case AUDIO_DEV_DMIC:
		nhlt_dev_type = NHLT_PDM_DEV;
		did = NHLT_DID_DMIC;
		break;
	case AUDIO_DEV_BT:
		nhlt_dev_type = NHLT_SSP_DEV_BT;
		did = NHLT_DID_BT;
		break;
	default:
		return NULL;
	}

	endp = nhlt_add_endpoint(nhlt, nhlt_link_type, nhlt_dev_type, dir,
				NHLT_VID, did);

	if (endp == NULL)
		return NULL;

	/* Virtual bus id of SSP links are the hardware port ids proper. */
	if (nhlt_link_type == NHLT_LINK_SSP)
		endp->virtual_bus_id = soc_hwintf;

	return endp;
}

uintptr_t nhlt_soc_serialize(struct nhlt *nhlt, uintptr_t acpi_addr)
{
	return nhlt_soc_serialize_oem_overrides(nhlt, acpi_addr, NULL, NULL);
}

uintptr_t nhlt_soc_serialize_oem_overrides(struct nhlt *nhlt,
	uintptr_t acpi_addr, const char *oem_id, const char *oem_table_id)
{
	global_nvs_t *gnvs;

	gnvs = cbmem_find(CBMEM_ID_ACPI_GNVS);

	if (gnvs == NULL)
		return acpi_addr;

	/* Update NHLT GNVS Data */
	gnvs->nhla = (uintptr_t)acpi_addr;
	gnvs->nhll = nhlt_current_size(nhlt);

	return nhlt_serialize_oem_overrides(nhlt, acpi_addr,
						oem_id, oem_table_id);
}
