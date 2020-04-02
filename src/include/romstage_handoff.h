/* SPDX-License-Identifier: GPL-2.0-only */
/* This file is part of the coreboot project. */
#ifndef ROMSTAGE_HANDOFF_H
#define ROMSTAGE_HANDOFF_H

/* Returns 0 if initialized. Else < 0 if handoff structure not added. */
int romstage_handoff_init(int is_s3_resume);

/* Return 1 if resuming or 0 if not. */
int romstage_handoff_is_resume(void);

#endif /* ROMSTAGE_HANDOFF_H */
