/*
 * This file is part of the depthcharge project.
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

#ifndef __GSBI_TYPES_H__
#define __GSBI_TYPES_H__

typedef enum {
	GSBI_ID_1 = 1,
	GSBI_ID_2,
	GSBI_ID_3,
	GSBI_ID_4,
	GSBI_ID_5,
	GSBI_ID_6,
	GSBI_ID_7,
} gsbi_id_t;

typedef enum {
	GSBI_SUCCESS = 0,
	GSBI_ID_ERROR,
	GSBI_ERROR,
	GSBI_UNSUPPORTED
} gsbi_return_t;

typedef enum {
	GSBI_PROTO_I2C_UIM = 1,
	GSBI_PROTO_I2C_ONLY,
	GSBI_PROTO_SPI_ONLY,
	GSBI_PROTO_UART_FLOW_CTL,
	GSBI_PROTO_UIM,
	GSBI_PROTO_I2C_UART,
} gsbi_protocol_t;

gsbi_return_t gsbi_init(gsbi_id_t gsbi_id, gsbi_protocol_t protocol);
#endif
