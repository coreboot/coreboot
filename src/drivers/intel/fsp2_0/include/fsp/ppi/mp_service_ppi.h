/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef MP_SERVICE_PPI_H
#define MP_SERVICE_PPI_H

/*
 * This file to implement MP_SERVICES_PPI for Intel FSP to use.
 * More details about this PPI can be found here :
 * http://github.com/tianocore/edk2/blob/master/MdePkg/Include/Ppi/MpServices.h
 */
#include <efi/efi_datatype.h>
#include <fsp/soc_binding.h>

/*
 * SOC must call this function to get required EFI_PEI_MP_SERVICES_PPI
 * structure.
 */
efi_pei_mp_services_ppi *mp_fill_ppi_services_data(void);

#endif	/* MP_SERVICE_PPI_H */
