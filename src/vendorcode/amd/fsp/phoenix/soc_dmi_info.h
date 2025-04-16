/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * These definitions are used to describe memory modules physical layout
 */

#ifndef SOC_DMI_INFO_H
#define SOC_DMI_INFO_H

#define AGESA_STRUCT_SOCKET_COUNT         4   ///< Number of sockets in AGESA FSP DMI T17 table
#define AGESA_STRUCT_CHANNELS_PER_SOCKET  12  ///< Channels per socket in AGESA FSP DMI T17 table
#define AGESA_STRUCT_DIMMS_PER_CHANNEL    2   ///< DIMMs per channel in AGESA FSP DMI T17 table
#define AGESA_STRUCT_PART_NUMBER_SIZE     21

#define SMBIOS_3_2_3_3_SUPPORT            1

#endif /* SOC_DMI_INFO_H */
