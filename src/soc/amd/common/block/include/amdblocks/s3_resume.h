/* SPDX-License-Identifier: GPL-2.0-only */

#ifndef AMD_BLOCK_S3_RESUME_H
#define AMD_BLOCK_S3_RESUME_H

#include <agesa_headers.h>

AGESA_STATUS OemInitResume(S3_DATA_BLOCK *dataBlock);
AGESA_STATUS OemS3LateRestore(S3_DATA_BLOCK *dataBlock);
AGESA_STATUS OemS3Save(S3_DATA_BLOCK *dataBlock);

#endif /* AMD_BLOCK_S3_RESUME_H */
