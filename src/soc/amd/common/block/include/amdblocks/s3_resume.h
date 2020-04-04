/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */

#ifndef __AMD_S3_RESUME_H__
#define __AMD_S3_RESUME_H__

#include <stdint.h>
#include <agesa_headers.h>

AGESA_STATUS OemInitResume(S3_DATA_BLOCK *dataBlock);
AGESA_STATUS OemS3LateRestore(S3_DATA_BLOCK *dataBlock);
AGESA_STATUS OemS3Save(S3_DATA_BLOCK *dataBlock);

#endif /* __AMD_S3_RESUME_H__ */
