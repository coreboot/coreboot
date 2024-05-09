/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef _FSP2_0_SOC_BINDING_H_
#define _FSP2_0_SOC_BINDING_H_

#include <stddef.h>

#pragma pack(push)

#include <efi/efi_datatype.h>

/**
 * These includes are required to include headers that are missing in
 * the FSP headers. Import order matter for the correct PiHob definition
 * to be found.
 */
#if CONFIG_UDK_VERSION >= CONFIG_UDK_2017_VERSION
#include <PiPei.h>
#include <Ppi/MpServices.h>
#include <Uefi/UefiMultiPhase.h>
#include <Pi/PiBootMode.h>
#include <Pi/PiHob.h>
#endif

/*
 * This file is a implementation specific header. i.e. different
 * FSP implementations for different chipsets.
 */
#include <FspmUpd.h>
#include <FspsUpd.h>
#if CONFIG(MRC_CACHE_USING_MRC_VERSION)
#include <FspProducerDataHeader.h>
#endif
#if CONFIG(DISPLAY_FSP_VERSION_INFO)
#include <FirmwareVersionInfoHob.h>
#elif CONFIG(DISPLAY_FSP_VERSION_INFO_2)
#include <FirmwareVersionInfo.h>
#endif

#if CONFIG(ENABLE_FSP_ERROR_INFO)
#include <FspErrorInfoHob.h>
#endif

#pragma pack(pop)

#endif
