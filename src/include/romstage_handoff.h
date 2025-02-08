/* SPDX-License-Identifier: GPL-2.0-only */
#ifndef ROMSTAGE_HANDOFF_H
#define ROMSTAGE_HANDOFF_H

#include <stdbool.h>

/* Returns 0 if initialized. Else < 0 if handoff structure not added. */
int romstage_handoff_init(int is_s3_resume);

bool romstage_handoff_is_resume(void);

#endif /* ROMSTAGE_HANDOFF_H */
