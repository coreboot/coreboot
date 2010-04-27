/*
 * This file is part of the libpayload project.
 *
 * Copyright (C) 2008 coresystems GmbH
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/**
 * @file libc/readline.c
 * Simple readline implementation
 */

#include <libpayload.h>
#include <getopt.h>

/* We don't want to waste malloc on this, so we live with a small
 * fixed size array
 */
char *string_argv[MAX_ARGS];
int string_argc;

/**
 * Take a string and make char *argv[] and int argc from it.
 *
 * This function allows the user to use getopt on an arbitrary string.
 *
 * global variables valid after a successful run of string_to_args():
 *   string_argc pointer to number of arguments
 *   string_argv pointer to argument list.
 *
 * @param caller to be used as argv[0] (may be NULL to ignore)
 * @param string to process
 * @return 0 if no error occured.
 */
int string_to_args(char *caller, char *string)
{
	int i = 0;

	if (caller)
		string_argv[i++] = caller;

	if (*string)
		string_argv[i++] = string;

	/* Terminate if the string ends */
	while (string && *string) {
		/* whitespace occured? */
		if ((*string == ' ') || (*string == '\t')) {
			/* skip all whitespace (and null it) */
			while (*string == ' ' || *string == '\t')
				*string++ = 0;
			/* if our ugly static array is big enough, store
			 * argument to string_argv[]
			 */
			if (i < MAX_ARGS)
				string_argv[i++] = string;
		}
		string++;
	}

	/* prevent array from overflowing */
	string_argc = (i <= MAX_ARGS) ? i : MAX_ARGS;

	/* and return whether there was an overflow */
	return (i <= MAX_ARGS) ? 0 : 1;
}
