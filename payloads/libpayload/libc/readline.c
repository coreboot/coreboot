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

static char *readline_buffer;
static int readline_bufferlen;

/**
 * Read a line from the terminal and return it.
 *
 * This readline implementation is rather simple, but it does more than the
 * original readline() because it allows us to have a pre-filled buffer.
 * To pre-fill the buffer, use the getline() function.
 *
 * @param prompt A prompt to display on the line.
 * @return A pointer to the input string.
 */
char *readline(const char *prompt)
{
	char *buffer;
	int current, ch, nonspace_seen;

	if (!readline_buffer || !readline_bufferlen) {
#define READLINE_BUFFERSIZE	256
		readline_buffer = malloc(READLINE_BUFFERSIZE);
		if (!readline_buffer)
			return NULL;
		readline_bufferlen = READLINE_BUFFERSIZE;
		memset(readline_buffer, 0, readline_bufferlen);
	}

	buffer = readline_buffer;

	/* print prompt */
	if (prompt) {
		current = 0;
		while (prompt[current]) {
			putchar(prompt[current]);
			current++;
		}
	}

	/* print existing buffer, if there is one */
	current = 0;
	while (buffer[current]) {
		putchar(buffer[current]);
		current++;
	}

	while (1) {
		ch = getchar();
		switch (ch) {
		case '\r':
		case '\n':
			/* newline */
			putchar('\n');
			goto out;
		case '\b':
		case '\x7f':
			/* backspace */
			if (current > 0) {
				putchar('\b');
				putchar(' ');
				putchar('\b');
				current--;
			}
			break;
		case 'W' & 0x1f:	/* CTRL-W */
			/* word erase */
			nonspace_seen = 0;
			while (current) {
				if (buffer[current - 1] != ' ')
					nonspace_seen = 1;
				putchar('\b');
				putchar(' ');
				putchar('\b');
				current--;
				if (nonspace_seen && (current < readline_bufferlen - 1)
				    && (current > 0) && (buffer[current - 1] == ' '))
					break;
			}
			break;
		case 'U' & 0x1f:	/* CTRL-U */
			/* line erase */
			while (current) {
				putchar('\b');
				putchar(' ');
				putchar('\b');
				current--;
			}
			current = 0;
			break;
		default:
			/* all other characters */

			/* ignore control characters */
			if (ch < 0x20)
				break;

			/* ignore unprintables */
			if (ch >= 0x7f)
				break;

			if (current + 1 < readline_bufferlen) {
				/* print new character */
				putchar(ch);
				/* and add it to the array */
				buffer[current] = ch;
				current++;
			}
		}
	}

out:
	if (current >= readline_bufferlen)
		current = readline_bufferlen - 1;
	buffer[current] = '\0';

	return buffer;
}

/**
 * Read a line from the input and store it in a buffer.
 *
 * This function allows the user to pass a predefined buffer to readline().
 * The buffer may be filled with a default value which will be displayed by
 * readline() and can be edited as normal.
 * The final input string returned by readline() will be returned in
 * the buffer and the function will return the length of the string.
 *
 * @param buffer Pointer to a buffer to store the line in.
 * @param len Length of the buffer.
 * @return The final length of the string.
 */
int getline(char *buffer, int len)
{
	readline_buffer = buffer;
	readline_bufferlen = len;
	readline(NULL);

	return strlen(buffer);
}
