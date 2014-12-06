/*
 * This file is part of the coreboot project.
 *
 * Copyright (C) 2014 The Linux Foundation. All rights reserved.
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

#include <arch/io.h>
#include <soc/gsbi.h>
#include <soc/gpio.h>

//TODO: To be implemented as part of the iomap.
static int gsbi_base[] = {
	0x12440000, /*GSBI1*/
	0x12480000, /*GSBI2*/
	0x16200000, /*GSBI3*/
	0x16300000, /*GSBI4*/
	0x1A200000, /*GSBI5*/
	0x16500000, /*GSBI6*/
	0x16600000  /*GSBI7*/
};

#define QUP_APPS_ADDR(N, os)	((void *)((0x009029C8+os)+(32*(N-1))))
#define GSBI_HCLK_CTL(N)	((void *)(0x009029C0 + (32*(N-1))))
#define GSBI_RESET(N)		((void *)(0x009029DC + (32*(N-1))))
#define GSBI_CTL(N)		((void *)(gsbi_base[N-1]))

#define GSBI_APPS_MD_OFFSET	0x0
#define GSBI_APPS_NS_OFFSET	0x4
#define GSBI_APPS_MAX_OFFSET	0xff

gsbi_return_t gsbi_init(gsbi_id_t gsbi_id, gsbi_protocol_t protocol)
{
	unsigned i = 0;
	unsigned qup_apps_ini[] = {
		GSBI_APPS_NS_OFFSET,            0xf80b43,
		GSBI_APPS_NS_OFFSET,            0xfc095b,
		GSBI_APPS_NS_OFFSET,            0xfc015b,
		GSBI_APPS_NS_OFFSET,            0xfc005b,
		GSBI_APPS_NS_OFFSET,            0xA05,
		GSBI_APPS_NS_OFFSET,            0x185,
		GSBI_APPS_MD_OFFSET,            0x100fb,
		GSBI_APPS_NS_OFFSET,            0xA05,
		GSBI_APPS_NS_OFFSET,            0xfc015b,
		GSBI_APPS_NS_OFFSET,            0xfc015b,
		GSBI_APPS_NS_OFFSET,            0xfc095b,
		GSBI_APPS_NS_OFFSET,            0xfc0b5b,
		GSBI_APPS_MAX_OFFSET,           0x0
	};

	gsbi_return_t ret = GSBI_SUCCESS;

	writel(0, GSBI_RESET(gsbi_id));

	if (gsbi_init_board(gsbi_id)) {
		ret = GSBI_UNSUPPORTED;
		goto bail_out;
	}

	/*Select i2c protocol*/
	writel((2 << 4), GSBI_CTL(gsbi_id));

	//TODO: Make use of clock API when available instead of the hardcoding.
	/* Clock set to 24Mhz */
	for (i = 0; GSBI_APPS_MAX_OFFSET != qup_apps_ini[i]; i += 2)
		writel(qup_apps_ini[i+1],
		       QUP_APPS_ADDR(gsbi_id, qup_apps_ini[i]));

	writel(((1 << 6)|(1 << 4)), GSBI_HCLK_CTL(gsbi_id));

bail_out:
	return ret;
}
