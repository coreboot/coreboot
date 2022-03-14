/* SPDX-License-Identifier: GPL-2.0-only */

/* Create EFI equivalent datatype in coreboot based on UEFI specification */
#ifndef __EFI_DATATYPE_H__
#define __EFI_DATATYPE_H__
#include <Base.h>
#include <Uefi/UefiBaseType.h>

#if CONFIG_UDK_VERSION >= CONFIG_UDK_2017_VERSION
#include <Pi/PiPeiCis.h>

/* Data structure for EFI_PEI_SERVICE. */
typedef EFI_PEI_SERVICES efi_pei_services;
/*  Structure that describes information about a logical CPU. */
typedef EFI_PROCESSOR_INFORMATION efi_processor_information;
#endif

/* Basic Data types */
/* 8-byte unsigned value. */
typedef UINT64 efi_uint64_t;
/* 8-byte signed value. */
typedef INT64 efi_int64_t;
/* 4-byte unsigned value. */
typedef UINT32 efi_uint32_t;
/* 4-byte signed value. */
typedef INT32 efi_int32_t;
/* 2-byte unsigned value. */
typedef UINT16 efi_uint16_t;
/* 2-byte Character. */
typedef CHAR16 efi_char16_t;
/* 2-byte signed value. */
typedef INT16 efi_int16_t;
/* Logical Boolean. */
typedef BOOLEAN efi_boolean_t;
/* 1-byte unsigned value. */
typedef UINT8 efi_uint8_t;
/* 1-byte Character */
typedef CHAR8 efi_char8_t;
/* 1-byte signed value */
typedef INT8 efi_int8_t;
/* Unsigned value of native width. */
typedef UINTN efi_uintn_t;
/* Signed value of native width. */
typedef INTN efi_intn_t;
/* Status codes common to all execution phases */
typedef EFI_STATUS efi_return_status_t;
/* Data structure for EFI_PHYSICAL_ADDRESS */
typedef EFI_PHYSICAL_ADDRESS efi_physical_address;

/*
 * The function prototype for invoking a function on an
 * Application Processor.
 */
typedef
void
(EFIAPI *efi_ap_procedure)(void *buffer);

#endif
