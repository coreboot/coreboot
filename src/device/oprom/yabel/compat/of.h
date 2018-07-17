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


#ifndef OF_H
#define OF_H
#define p32 int
#define p32cast (int) (unsigned long) (void *)

#define phandle_t p32
#define ihandle_t p32

typedef struct
{
	unsigned int serv;
	int nargs;
	int nrets;
	unsigned int args[16];
} of_arg_t;


phandle_t of_finddevice (const char *);
phandle_t of_peer(phandle_t);
phandle_t of_child(phandle_t);
phandle_t of_parent(phandle_t);
int of_getprop(phandle_t, const char *, void *, int);
void *of_call_method_3(const char *, ihandle_t, int);


ihandle_t of_open(const char *);
void of_close(ihandle_t);
int of_read(ihandle_t, void *, int);
int of_write(ihandle_t, void *, int);
int of_seek(ihandle_t, int, int);

void *of_claim(void *, unsigned int, unsigned int);
void of_release(void *, unsigned int);

int of_yield(void);
void *of_set_callback(void *);

int vpd_read(unsigned int, unsigned int, char *);
int vpd_write(unsigned int, unsigned int, char *);
int write_mm_log(char *, unsigned int, unsigned short);

#endif
