/***********************license start***********************************
* Copyright (c) 2003-2017  Cavium Inc. (support@cavium.com). All rights
* reserved.
*
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*
*   * Neither the name of Cavium Inc. nor the names of
*     its contributors may be used to endorse or promote products
*     derived from this software without specific prior written
*     permission.
*
* This Software, including technical data, may be subject to U.S. export
* control laws, including the U.S. Export Administration Act and its
* associated regulations, and may be subject to export or import
* regulations in other countries.
*
* TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED "AS IS"
* AND WITH ALL FAULTS AND CAVIUM INC. MAKES NO PROMISES, REPRESENTATIONS OR
* WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR OTHERWISE, WITH RESPECT
* TO THE SOFTWARE, INCLUDING ITS CONDITION, ITS CONFORMITY TO ANY
* REPRESENTATION OR DESCRIPTION, OR THE EXISTENCE OF ANY LATENT OR PATENT
* DEFECTS, AND CAVIUM SPECIFICALLY DISCLAIMS ALL IMPLIED (IF ANY) WARRANTIES
* OF TITLE, MERCHANTABILITY, NONINFRINGEMENT, FITNESS FOR A PARTICULAR
* PURPOSE, LACK OF VIRUSES, ACCURACY OR COMPLETENESS, QUIET ENJOYMENT,
* QUIET POSSESSION OR CORRESPONDENCE TO DESCRIPTION. THE ENTIRE  RISK
* ARISING OUT OF USE OR PERFORMANCE OF THE SOFTWARE LIES WITH YOU.
***********************license end**************************************/
#include <bdk.h>
#include "dram-internal.h"

const char* lookup_env_parameter(const char *format, ...)
{
    const char *s;
    unsigned long value;
    va_list args;
    char buffer[64];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer)-1, format, args);
    buffer[sizeof(buffer)-1] = '\0';
    va_end(args);

    if ((s = getenv(buffer)) != NULL)
    {
        value = strtoul(s, NULL, 0);
        error_print("Parameter found in environment: %s = \"%s\" 0x%lx (%ld)\n",
		    buffer, s, value, value);
    }
    return s;
}

const char* lookup_env_parameter_ull(const char *format, ...)
{
    const char *s;
    unsigned long long value;
    va_list args;
    char buffer[64];

    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer)-1, format, args);
    buffer[sizeof(buffer)-1] = '\0';
    va_end(args);

    if ((s = getenv(buffer)) != NULL)
    {
        value = strtoull(s, NULL, 0);
        error_print("Parameter found in environment: %s = 0x%016llx\n",
		    buffer, value);
    }
    return s;
}

