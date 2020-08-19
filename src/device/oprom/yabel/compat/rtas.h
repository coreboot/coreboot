/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * Redistributions of source code must retain the above copyright
 *   notice, this list of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer
 *   in the documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#ifndef RTAS_H
#define RTAS_H

#include "of.h"

typedef struct dtime {
	unsigned int year;
	unsigned int month;
	unsigned int day;
	unsigned int hour;
	unsigned int minute;
	unsigned int second;
	unsigned int nano;
} dtime;

typedef void (*thread_t) (int);

int rtas_token(const char *);
int rtas_call(int, int, int, int *, ...);
void rtas_init(void);
int rtas_pci_config_read (long long, int, int, int, int);
int rtas_pci_config_write (long long, int, int, int, int, int);
int rtas_set_time_of_day(dtime *);
int rtas_get_time_of_day(dtime *);
int rtas_ibm_update_flash_64(long long, long long);
int rtas_ibm_update_flash_64_and_reboot(long long, long long);
int rtas_system_reboot(void);
int rtas_start_cpu (int, thread_t, int);
int rtas_stop_self (void);
int rtas_ibm_manage_flash(int);

#endif
