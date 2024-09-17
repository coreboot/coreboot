/* SPDX-License-Identifier: GPL-2.0-only */

/* Create EFI equivalent datatype in coreboot based on UEFI specification */
#ifndef __EFI_DATATYPE_H__
#define __EFI_DATATYPE_H__

/*
 * EDK2 EFIAPI macro definition relies on compiler flags such as __GNUC__ which
 * is not working well when included by coreboot. While it has no side-effect on
 * i386 because the C calling convention used by coreboot and FSP are the same,
 * it breaks on x86_64 because FSP/UEFI uses the Microsoft x64 calling
 * convention while coreboot uses the System V AMD64 ABI.
 *
 * Fortunately, EDK2 header allows to override EFIAPI.
 */
#if CONFIG(PLATFORM_USES_FSP1_1) || CONFIG(PLATFORM_USES_FSP2_X86_32)
#define EFIAPI __attribute__((regparm(0)))
#else
#define EFIAPI __attribute__((__ms_abi__))
#endif

#include <Base.h>
#include <Uefi/UefiBaseType.h>

#if CONFIG_UDK_VERSION >= 2017
#include <Guid/StatusCodeDataTypeId.h>
#include <IndustryStandard/Bmp.h>
#include <Pi/PiPeiCis.h>
#include <Pi/PiStatusCode.h>
#include <Protocol/GraphicsOutput.h>
#include <Protocol/MpService.h>

/* Data structure for EFI_PEI_SERVICE. */
typedef EFI_PEI_SERVICES efi_pei_services;
/*  Structure that describes information about a logical CPU. */
typedef EFI_PROCESSOR_INFORMATION efi_processor_information;
/* Status code type definition */
typedef EFI_STATUS_CODE_TYPE efi_status_code_type_t;
/* Status value type definition */
typedef EFI_STATUS_CODE_VALUE efi_status_code_value_t;
/* Status data type definition */
typedef EFI_STATUS_CODE_DATA efi_status_code_data_t;
/* Status string data type definition */
typedef EFI_STATUS_CODE_STRING_DATA efi_status_code_string_data;
/* Data structure for EFI_GRAPHICS_OUTPUT_BLT_PIXEL. */
typedef EFI_GRAPHICS_OUTPUT_BLT_PIXEL efi_graphics_output_blt_pixel;
/* Data structure for BMP_IMAGE_HEADER. */
typedef BMP_IMAGE_HEADER efi_bmp_image_header;
/* Data structure for BMP_COLOR_MAP; . */
typedef BMP_COLOR_MAP efi_bmp_color_map;
#endif

/* EFIAPI calling convention */
#define __efiapi EFIAPI

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
#if CONFIG(PLATFORM_USES_FSP2_X86_32)
typedef UINT32 efi_return_status_t;
#else
typedef UINT64 efi_return_status_t;
#endif
/* Data structure for EFI_PHYSICAL_ADDRESS */
typedef EFI_PHYSICAL_ADDRESS efi_physical_address;
/* 128-bit buffer containing a unique identifier value */
typedef EFI_GUID efi_guid_t;

/*
 * The function prototype for invoking a function on an
 * Application Processor.
 */
typedef
void
(__efiapi *efi_ap_procedure)(void *buffer);

#endif
