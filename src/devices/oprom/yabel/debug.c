/******************************************************************************
 * Copyright (c) 2004, 2008 IBM Corporation
 * Copyright (c) 2008, 2009 Pattrick Hueper <phueper@hueper.net>
 * All rights reserved.
 * This program and the accompanying materials
 * are made available under the terms of the BSD License
 * which accompanies this distribution, and is available at
 * http://www.opensource.org/licenses/bsd-license.php
 *
 * Contributors:
 *     IBM Corporation - initial implementation
 *****************************************************************************/

#include "debug.h"

u32 debug_flags = 0;

void
dump(u8 * addr, u32 len)
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
