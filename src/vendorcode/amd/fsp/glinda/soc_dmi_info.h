/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * These definitions are used to describe memory modules physical layout
 * TODO: Update for Glinda
 */

#ifndef SOC_DMI_INFO_H
#define SOC_DMI_INFO_H

#define AGESA_STRUCT_SOCKET_COUNT         2   ///< Number of sockets in AGESA FSP DMI T17 table
#define AGESA_STRUCT_CHANNELS_PER_SOCKET  8   ///< Channels per socket in AGESA FSP DMI T17 table
#define AGESA_STRUCT_DIMMS_PER_CHANNEL    4   ///< DIMMs per channel in AGESA FSP DMI T17 table
#define AGESA_STRUCT_PART_NUMBER_SIZE     31

#define SMBIOS_3_2_3_3_SUPPORT            1

#endif /* SOC_DMI_INFO_H */
