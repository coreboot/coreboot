/* SPDX-License-Identifier: GPL-2.0-only */

/*
 * These definitions are used to describe memory modules physical layout
 */

#ifndef SOC_DMI_INFO_H
#define SOC_DMI_INFO_H

#define AGESA_STRUCT_SOCKET_COUNT         1   ///< Number of sockets in AGESA FSP DMI T17 table
#define AGESA_STRUCT_CHANNELS_PER_SOCKET  4   ///< Channels per socket in AGESA FSP DMI T17 table
#define AGESA_STRUCT_DIMMS_PER_CHANNEL    4   ///< DIMMs per channel in AGESA FSP DMI T17 table
#define AGESA_STRUCT_PART_NUMBER_SIZE     21

#endif /* SOC_DMI_INFO_H */
