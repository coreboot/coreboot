/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2017 Google, Inc.
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

/* Do not use header guards on this file */

/* This header should be included *BEFORE* any guards in the AGESA header */
#if !defined AGESA_HEADERS_ARE_WRAPPED && !ENV_LIBAGESA
#error AGESA headers should be included by agesawrapper.h or agesa_headers.h
#endif
