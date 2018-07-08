/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2008, 2009 Pattrick Hueper <phueper@hueper.net>
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

#include "debug.h"

u32 debug_flags = 0;

void
dump(u8 *addr, u32 len)
{
	printf("\n%s(%p, %x):\n", __func__, addr, len);
	while (len) {
		unsigned int tmpCnt = len;
		unsigned char x;
		if (tmpCnt > 8)
			tmpCnt = 8;
		printf("\n%p: ", addr);
		// print hex
		while (tmpCnt--) {
			set_ci();
			x = *addr++;
			clr_ci();
			printf("%02x ", x);
		}
		tmpCnt = len;
		if (tmpCnt > 8)
			tmpCnt = 8;
		len -= tmpCnt;
		//reset addr ptr to print ascii
		addr = addr - tmpCnt;
		// print ascii
		while (tmpCnt--) {
			set_ci();
			x = *addr++;
			clr_ci();
			if ((x < 32) || (x >= 127)) {
				//non-printable char
				x = '.';
			}
			printf("%c", x);
		}
	}
	printf("\n");
}
