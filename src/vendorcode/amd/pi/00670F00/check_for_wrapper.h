/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

/* Do not use header guards on this file */

/* This header should be included *BEFORE* any guards in the AGESA header */
#if !defined AGESA_HEADERS_ARE_WRAPPED && !ENV_LIBAGESA
#error AGESA headers should be included by agesawrapper.h or agesa_headers.h
#endif
