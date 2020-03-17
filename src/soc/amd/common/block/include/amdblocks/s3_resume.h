/*
 * This file is part of the coreboot project.
 *
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

#ifndef __AMD_S3_RESUME_H__
#define __AMD_S3_RESUME_H__

#include <stdint.h>
#include <agesa_headers.h>

AGESA_STATUS OemInitResume(S3_DATA_BLOCK *dataBlock);
AGESA_STATUS OemS3LateRestore(S3_DATA_BLOCK *dataBlock);
AGESA_STATUS OemS3Save(S3_DATA_BLOCK *dataBlock);

#endif /* __AMD_S3_RESUME_H__ */
