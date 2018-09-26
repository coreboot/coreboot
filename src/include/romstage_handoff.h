/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2013 Google LLC
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
#ifndef ROMSTAGE_HANDOFF_H
#define ROMSTAGE_HANDOFF_H

/* Returns 0 if initialized. Else < 0 if handoff structure not added. */
int romstage_handoff_init(int is_s3_resume);

/* Return 1 if resuming or 0 if not. */
int romstage_handoff_is_resume(void);

#endif /* ROMSTAGE_HANDOFF_H */
